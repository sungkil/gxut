#include "gxobjparser.h"

// common local attributes
const char* __GX_MTLPARSER_CPP_TIMESTAMP__ = __TIMESTAMP__;

// local variables
static path mtl_dir;
static bool b_dirty = false;

//*************************************
namespace mtl {
//*************************************

struct item_t
{
	string key;
	vector<string> tokens; // tokens excluding key

	// attributes
	uint crc = 0; // for maps
	bool b_map_type = false;

	item_t( const string& _key, const char* first_token=nullptr );
	void clear(){ key.clear(); tokens.clear(); crc=0; b_map_type=false; }
	bool empty() const { return tokens.empty(); }
	size_t size() const { return tokens.size(); }
	string& front() { return tokens.front(); }
	string& back() { return tokens.back(); }

	string str() const { return tokens.empty()?"":(key.empty()?string():(key+" "))+join(tokens); }
	const char* token( int index=0 ) const { return tokens[index].c_str(); }
	float value( int index=0 ) const { return float(fast::atof(tokens[index].c_str())); }
	int	valuei( int index=0 ) const { return int(fast::atoi(tokens[index].c_str())); }

	bool is_map_type() const { return b_map_type; }
	path map_path();
	bool make_canonical_relative_path();
};

struct section_t
{
	string			name;
	vector<item_t>	items;

	section_t(){ items.reserve(256); }
	section_t( const string& _name ):name(_name){ items.reserve(64); }

	bool empty() const { return name.empty()||items.empty(); }
	auto begin(){ return items.begin(); }
	auto end(){ return items.end(); }
	auto maps(){ vector<item_t*> v;for(auto& t:items){if(t.is_map_type())v.emplace_back(&t);} return v; }

	item_t* find( const char* key ) const { for(auto& t:items){ if(!items.empty()&&stricmp(t.key.c_str(),key)==0) return (item_t*)&t; } return nullptr; }
	int find_index( const char* key ) const	{ for( int k=0, kn=int(items.size()); k<kn; k++ ){ if(items[k].empty()) continue; if(stricmp(items[k].key.c_str(),key)==0) return k; } return -1; }
	item_t* add_norm( string token )
	{
		if(find("norm")) printf( "%s(%s): norm already exists\n", __func__, token.c_str() );
		item_t t("norm",token.c_str());
		if(items.empty()) return &items.emplace_back(t);
		int i=find_index("bump"), offset=i<0?int(items.size())-(items.back().empty()?1:0):i+1;
		return &(*items.emplace(items.begin()+offset,t));
	}
};

//*************************************
} // end namespace mtl
//*************************************

mtl::item_t::item_t( const string& _key, const char* first_token ):key(_key)
{
	if(first_token&&*first_token) tokens.emplace_back(first_token);
	static nocase::set<string> map_keys = {"bump","refl","norm","pc","pcr","aniso","anisor"};
	b_map_type = (key.size()>4&&strnicmp(key.c_str(),"map_",4)==0)||map_keys.find(key)!=map_keys.end();
}

path mtl::item_t::map_path()
{
	if(tokens.empty()) return "";
	auto& b=tokens.back(); if(::path p(b);p.is_absolute()){b=p.is_subdir(mtl_dir)?p.relative(mtl_dir).c_str():p.name();}
	return mtl_dir+tokens.back();
}

bool mtl::item_t::make_canonical_relative_path()
{
	auto &b=tokens.back(), b0=b;
	for( int k=0;k<8&&strstr(b.c_str(),"\\\\");k++) b=str_replace(b.c_str(),"\\\\","\\");	// remove double backslash
	for( int k=0;k<8&&strstr(b.c_str(),"//");k++) b=str_replace(b.c_str(),"//","/");		// remove double slash
	::path p(b=path(b).to_backslash().canonical().c_str());
	if(!p.is_relative()) b=p.is_subdir(mtl_dir)?p.relative(mtl_dir).c_str():p.name();
	return stricmp(b0.c_str(),b.c_str())!=0; // return change exists
}

static bool save_mtl( path file_path, const vector<mtl::section_t>& sections, float opt_time )
{
	auto mtime0 = file_path.mtime();
	FILE* fp = file_path.fopen("w"); if(!fp){ printf("%s(): failed to open %s\n", __func__, file_path.slash() ); return false; }
	printf( "[%s] optimization ... ", file_path.name() );

	for( int k=0, kn=int(sections.size()); k<kn; k++ )
	{
		auto& section = sections[k];
		if(!section.name.empty()) fprintf(fp,"newmtl %s\n", section.name.c_str() );
		for( auto& t : section.items )
		{
			string r = std::move(t.str());
			if(!r.empty()) fprintf( fp, "%s\n", r.c_str() );
		}
		if(k==0||(!section.name.empty()&&k<kn-1)) fprintf(fp,"\n");
	}
	fclose(fp);
	file_path.utime(mtime0); // keep time stamp
	printf( "completed in %.2f ms\n", opt_time );

	return true;
}

vector<mtl::section_t> parse_mtl( path file_path )
{
	vector<mtl::section_t> v; v.reserve(1024);

	if(!file_path.exists()){ printf("%s(): %s not exists\n", __func__, file_path.c_str() ); return v; }
	FILE* fp = file_path.fopen("r"); if(!fp){ printf("%s(): unable to open %s\n", __func__, file_path.c_str()); return v; }
	
	// fill no-name header entry
	v.emplace_back(mtl::section_t());

	char buff[4096]={};
	auto is_empty_line = []( const char* s )->bool { char c0=s[0]; if(!c0||c0=='#'||strncmp(s,"Wavefront",9)==0) return true; char c1=s[1]; return !c1; };
	for( uint k=0; fgets(buff,4096,fp)&&k<65536; k++ )
	{
		const char* b = rtrim(buff); if(!b||!*b) continue;
		if(is_empty_line(b)) // add comments
		{
			if(*b)
			{
				if(!v.back().name.empty()) v.emplace_back(mtl::section_t()); 
				v.back().items.emplace_back(mtl::item_t("",b));
			}
			continue;
		}

		vector<string> vs = std::move(explode(b));
		if(vs.size()<2){ b_dirty=true; continue; } // cull no-value lines
		const string& key = vs[0];
		
		if(stricmp(key.c_str(),"newmtl")==0)
		{
			v.emplace_back(mtl::section_t(vs[1]));
		}
		else
		{
			mtl::item_t t(key); for(size_t j=1;j<vs.size();j++) t.tokens.emplace_back(vs[j]);
			if(t.is_map_type()&&t.make_canonical_relative_path()) b_dirty=true;
			v.back().items.emplace_back(std::move(t)); // add split tokens
		}
	}
	fclose(fp);

	// remove map_bump when identical bump exists
	for( auto& m : v )
	{
		int i=m.find_index("map_bump"), b=m.find_index("bump");
		if(i>=0&&b>=0){ b_dirty=true; m.items.erase(m.items.begin()+i); } // remove only when both exists
	}

	return v;
}

static material_impl create_light( uint ID )
{
	material_impl m(ID);
	strcpy(m.name,"light");
	m.color = vec4(1.0f,1.0f,1.0f,1.0f);
	m.metal = 0.0f;
	m.bsdf = BSDF_EMISSIVE;
	return m;
}

// forward decl.
#ifdef __GX_MTL_OPTIMIZE__
	float optimize_textures( path file_path, vector<mtl::section_t>& sections );
#endif

//*************************************
namespace mtl {
//*************************************

uint find( vector<material_impl>& materials, const char* name )
{
	for(uint k=0,kn=uint(materials.size());k<kn;k++)
		if(stricmp(name,materials[k].name)==0) return k;
	return -1;
}

void create_default( vector<material_impl>& materials )
{
	materials.emplace_back(material_impl(uint(materials.size())));
	auto& m = materials.back();
	strcpy(m.name,"default");
	m.color = vec4(0.7f,0.7f,0.7f,1.0f);
}

bool load( path file_path, vector<material_impl>& materials, bool with_cache )
{
	gx::timer_t timer;

	// reset common attributes
	mtl_dir = file_path.dir().canonical();
	b_dirty = false; // something changed?

	// pre-parse raw lines
	vector<mtl::section_t> sections = std::move(parse_mtl(file_path)); if(sections.empty()) return false;
	
	// default material for light source (mat_index==0 or emissive>0)
	materials.clear();

	// preprocessing only for fresh loading
	float opt_time = 0;
#ifdef __GX_MTL_OPTIMIZE__
	if(!with_cache) opt_time = optimize_textures( file_path, sections );
#endif

	// tags to bypass
	static nocase::set<string> passtags
	{
		"ka",		// ambient materials
		"map_ks",	// specular map
		"map_ns",	// specular power map
		"disp",		// displacement map
		"tf",		// Transmission filter
		"fr",		// Fresnel reflectance
		"ft",		// Fresnel transmittance
		"Ia",		// ambient light
		"Ir",		// intensity from reflected direction
		"It",		// intensity from transmitted direction
	};
	
	// start loading
	for( auto& section : sections )
	{
		if(section.empty()) continue;

		materials.emplace_back(material_impl(uint(materials.size())));
		auto& m = materials.back();
		strcpy( m.name, section.name.c_str() );
		if(strnicmp(m.name,"light",5)==0) m.bsdf = BSDF_EMISSIVE;

		// preprocess illum
		for(auto& t: section.items)
		{
			string key = tolower(t.key.c_str());
			if(key=="illum")
			{
				int i=t.valuei(); // https://paulbourke.net/dataformats/mtl/
				//if(i==0)		m.bsdf = BSDF_DIFFUSE;					// Color on and Ambient off
				//if(i==1)		m.bsdf = BSDF_DIFFUSE;					// Color on and Ambient on
				if(i==2)		m.bsdf = BSDF_DIFFUSE|BSDF_GLOSS;		// Highlight on
				else if(i==3)	m.bsdf = BSDF_MIRROR;					// Reflection on and Ray trace on
				else if(i==4)	m.bsdf = BSDF_MIRROR|BSDF_DIELECTRIC;	// Transparency: Glass on, Reflection: Ray trace on
				else if(i==5)	m.bsdf = BSDF_MIRROR|BSDF_FRESNEL;		// Reflection: Fresnel on and Ray trace on
				else if(i==6)	m.bsdf = BSDF_DIELECTRIC;				// Transparency: Refraction on, Reflection: Fresnel off and Ray trace on
				else if(i==7)	m.bsdf = BSDF_DIELECTRIC|BSDF_FRESNEL;	// Transparency: Refraction on, Reflection: Fresnel on and Ray trace on
				else if(i==8)	m.bsdf = BSDF_DIFFUSE|BSDF_GLOSS;		// Reflection on and Ray trace off
				else if(i==9)	m.bsdf = BSDF_DIELECTRIC;				// Transparency: Glass on, Reflection: Ray trace off
				//else if(i==10);										// Cast shadows onto invisible surfaces
			}
		}

		vec4 emit=vec4(0);
		for(auto& t: section.items)
		{
			string key = tolower(t.key.c_str());

			if(key.empty()||t.empty()) continue;
			else if(key=="newmtl"); // already processed
			else if(key=="illum"); // already processed
			else if(key=="kd")
			{
				if(t.size()<3){ printf("Kd size < 3\n"); return false; }
				m.color[0] = t.value(0);
				m.color[1] = t.value(1);
				m.color[2] = t.value(2);
				m.color[3] = 1.0f;
			}
			else if(key=="ks"){ m.specular = t.value(); }
			else if(key=="ns") // specular power
			{
				m.beta = t.value();
				if(m.rough>0) m.rough = beta_to_roughness(m.beta); // only for non-reflection
			}
			else if(key=="map_ka"){ m.path["ambient"] = t.token(); }	// ambient occlusion
			else if(key=="map_kd"){ m.path["albedo"] = t.token(); }
			else if(key=="map_d"||key=="map_opacity"){ m.path["alpha"] = t.token(); }
			else if(key=="bump")
			{
				if(m.path.find("bump")==m.path.end()) m.path["bump"] = t.tokens.back(); // only for new entry
			}
			else if(key=="refl")	// reflection map
			{
				if(stristr(t.token(), "metal")) m.path["metal"] = t.token(); // some mtl uses refl for map_pm for legacy compatibility
				else if(!t.empty())	m.rough = 0.0f; 	// ignore the reflection map and use the global env map
				m.bsdf = BSDF_MIRROR;
			}
			else if(key=="ni")
			{
				m.n = t.value(); // refractive index
			}
			else if(key=="d")	m.color.a = t.value();
			else if(key=="tr")
			{
				m.color.a = 1.0f-t.value(); // transparency
				if(m.color.a<1.0f) m.bsdf = BSDF_DIELECTRIC;
			}
			// PBR extensions: http://exocortex.com/blog/extending_wavefront_mtl_to_support_pbr
			else if(key=="ke")
			{
				emit[0] = t.value(0);
				emit[1] = t.value(1);
				emit[2] = t.value(2);
				if(emit.r>0||emit.g>0||emit.b>0)
				{
					emit[3]=1.0f;
					m.bsdf = BSDF_EMISSIVE;
				}
			}
			else if(key=="pr"){ if(m.rough>0) m.rough = t.value(); } // only for non-reflection
			else if(key=="map_ke")
			{
				m.path["emissive"] = t.token();
				m.bsdf = BSDF_EMISSIVE;
			}
			else if(key=="map_pr"){ m.path["rough"] = t.token(); }
			else if(key=="map_pm"){ m.path["metal"] = t.token(); }
			else if(key=="map_ps"){ m.path["sheen"] = t.token(); }
			else if(key=="norm"){ m.path["normal"] = t.token(); }
			else if(key=="pc"){ m.path["clearcoat"] = t.token(); }
			else if(key=="pcr"){ m.path["clearcoatroughness"] = t.token(); }
			else if(key=="aniso"){ m.path["anisotropy"] = t.token(); }
			else if(key=="anisor"){ m.path["anisotropyrotation"] = t.token(); }
			// DirectXMesh toolkit extension
			else if(key=="map_rma"){ m.path["rma"] = t.token(); } // (roughness, metalness, ambient occlusion)
			else if(key=="map_orm"){ m.path["orm"] = t.token(); } // (ambient occlusion, roughness, metalness)
			// unrecognized keys
			else if(passtags.find(key)==passtags.end())
			{
				printf("[%s] %s: unrecognized key: '%s'\n", file_path.name(), section.name.c_str(), key.c_str()); return false;
			}
		}

		// per-material postprocessing
		if(emit.a>0) m.color.rgb = emit.rgb;
		m.metal = clamp(m.color.r/m.specular, 0.0f, 1.0f); // convert to metallic from specular
	}

	// no light is found create light material
	auto light_source_exists = [](auto& materials)->bool { for(auto& m:materials) if(m.bsdf==BSDF_EMISSIVE) return true; return false; };
	if(!light_source_exists(materials))
	{
		for(auto& m:materials) const_cast<uint&>(m.ID)++;
		materials.insert(materials.begin(),create_light(0));
	}

	// update file after bump_as_normal
	if(b_dirty) save_mtl( file_path, sections, opt_time );
	if(!with_cache) printf( "Loading %s ... completed in %.2f ms\n", file_path.name(), timer.end() );
	
	return true;
}

//*************************************
} // namespace mtl
//*************************************

//*************************************
#ifdef __GX_MTL_OPTIMIZE__
//*************************************

#if __has_include(<gximage/gximage.h>)
	#include <gximage/gximage.h>
#endif

static const vector<vec2> halton_samples =
{
	{0.015625f,0.061224f}, {0.515625f,0.204082f}, {0.265625f,0.346939f}, {0.765625f,0.489796f}, {0.140625f,0.632653f}, {0.640625f,0.775510f}, {0.390625f,0.918367f}, {0.890625f,0.081633f},
	{0.078125f,0.224490f}, {0.578125f,0.367347f}, {0.328125f,0.510204f}, {0.828125f,0.653061f}, {0.203125f,0.795918f}, {0.703125f,0.938776f}, {0.453125f,0.102041f}, {0.953125f,0.244898f},
	{0.046875f,0.387755f}, {0.546875f,0.530612f}, {0.296875f,0.673469f}, {0.796875f,0.816326f}, {0.171875f,0.959184f}, {0.671875f,0.122449f}, {0.421875f,0.265306f}, {0.921875f,0.408163f},
	{0.109375f,0.551020f}, {0.609375f,0.693878f}, {0.359375f,0.836735f}, {0.859375f,0.979592f}, {0.234375f,0.142857f}, {0.734375f,0.285714f}, {0.484375f,0.428571f}, {0.984375f,0.571429f},
};

static std::map<uint,string> build_crc_lut( vector<mtl::section_t>& sections )
{
	// scan crc
	logical::map<string,uint> crc_map; // sorted by path
	for( auto& section : sections ) for( auto* t: section.maps() )
	{
		auto it = crc_map.find(t->back()); if(it!=crc_map.end()){ t->crc = it->second; continue; }
		t->crc = crc_map[t->back()] = t->map_path().exists() ? t->map_path().crc() : 0;
	}

	// invert lookup table for crc-unique maps
	std::map<uint,string> lut;
	for( auto& [token,crc] : crc_map ) if(auto it=lut.find(crc); it==lut.end()) lut[crc] = token;

	return lut;
}

static bool is_normal_map( path file_path )
{
	if(file_path.empty()) return false;

	static nocase::map<path,bool> cache;
	if(auto it=cache.find(file_path); it!=cache.end()) return it->second;
	cache[file_path]=false; if(!file_path.exists()) return false;

	image* header=gx::load_image_header(file_path.c_str()); if(!header){ printf("failed to load header of %s\n", file_path.c_str() ); return false; }
	bool early_exit = header->channels!=3||header->width<8||header->height<8; safe_delete(header); if(early_exit) return false;

	// do not force rgb to bump; and use cache
	image* i = gx::load_image(file_path.c_str(),true,false,false); if(!i){ printf("failed to load the bump map %s\n", file_path.c_str() ); return false; }
	int w=i->width, h=i->height;
	int bcount=0, bcount_thresh=int(halton_samples.size()*0.8f);
	for( int k=0, kn=int(halton_samples.size()); k<kn&&bcount<bcount_thresh; k++ )
	{
		const auto& s = halton_samples[k];
		ivec2 tc = ivec2(min(w-1,int((w-1)*s.x)),min(h-1,int((h-1)*s.y)));
		uchar3 c = *i->ptr<uchar3>(tc.y,tc.x);
		if(c.r==c.g&&c.r==c.b) continue;			// skip grayscale
		if(c.b<c.r||c.b<c.g||c.b<127) continue;		// probably wrong
		vec3 n = vec3(c.r,c.g,c.b)/127.5f-1.0f; if(fabs(n.length()-1.0f)>0.3f) continue;	// length around one
		bcount++;
	}

	safe_delete(i);
	return cache[file_path] = bcount<bcount_thresh ? false : true;
}

static bool generate_normal_map( path normal_path, path bump_path, path mtl_path="" )
{
	if(!bump_path.exists()){ printf("%s(): %s not exists\n", __func__, bump_path.name() ); return false; }
	time_t bump_mtime = bump_path.mtime();
	if(normal_path.exists()&&normal_path.mtime()>=bump_mtime) return true;

	gx::timer_t t;

	// test whether the bump map is actually a normal map
	// do not force rgb to bump; and use cache
	image* bump0 = gx::load_image(bump_path.c_str(),true,false,false); if(!bump0){ printf("%s(): failed to load %s\n", __func__, bump_path.name() ); return false; }
	printf( "%sgenerating %s from %s... ",
		!mtl_path.empty()?format("[%s] ", mtl_path.name()):"", normal_path.name(), bump_path.name());

	// convert uchar bumpmap to 1-channel float image
	image* bump = new image( bump0->width, bump0->height, 32, 1 );
	int yn=bump->height, c0=bump0->channels, xn=bump->width;
	for( int y=0; y<yn; y++ )
	{
		uchar* src = bump0->ptr<uchar>(y);
		float* dst = bump->ptr<float>(y);
		for( int x=0; x<xn; x++, src+=c0, dst++ ) dst[0] = float(src[0])/255.0f;
	}

	// create normal image
	image* normal = new image( bump->width, bump->height, 8, 3 );

	// apply sobel filter to the bump
	float bump_scale = min(1000.0f,1.0f/(1.0f+0.000001f));
	uchar *B=bump->data, *N=normal->data;
	int NW=int(normal->stride()), BW=int(bump->stride());
	
	for( int y=0; y<yn; y++ )
	{
		uchar3* dst = (uchar3*)(N+y*NW);
		float* src2 = (float*)(B+((y-1+yn)%yn)*BW);
		float* src1 = (float*)(B+y*BW);
		float* src0 = (float*)(B+((y+1)%yn)*BW);

		for( int x=0; x<xn; x++, dst++ )
		{
			int l=(x-1+xn)%xn, r=(x+1)%xn;
			float nx = -((src2[r]-src2[l]) + (src1[r]-src1[l])*2.0f + (src0[r]-src0[l]));
			float ny = -((src2[l]-src0[l]) + (src2[x]-src0[x])*2.0f + (src2[r]-src0[r]));
			float nz = bump_scale*2.4f;	// scaling
			float nlen = sqrtf(nx*nx+ny*ny+nz*nz);

			// save as RGB
			dst->x = uchar((nx/nlen+1.0f)*127.5f);
			dst->y = uchar((ny/nlen+1.0f)*127.5f);
			dst->z = uchar((nz/nlen+1.0f)*127.5f);
		}
	}

	// first write to the temporary image
	gx::save_image( normal_path.c_str(), normal );
	if(normal_path.exists()) normal_path.utime(bump_mtime);

	safe_delete(bump0);
	safe_delete(bump);
	safe_delete(normal);

	printf( "completed in %.f ms\n", t.end() );
	return true;
}

static path get_normal_path( const path& bump_path, nocase::map<path,path>& bton, nocase::set<path> used_images )
{
	auto it = bton.find( bump_path );
	if(it!=bton.end()&&is_normal_map(it->second)) return it->second;

	path base = bump_path.remove_extension();
	path ext = bump_path.extension();
	
	// remove postfix for bump
	if(base.size()>4&&stristr(substr(base.c_str(),-4),"bump")) base=path(substr(base.c_str(),0,-4))+"norm";
	else if(base.back()==L'b') base.back()=L'n';
	else base+="n";
	
	for( int k=0; k<8; k++, base+="n" )
	{
		path dst = base+"."+ext;
		if(!dst.exists()||used_images.find(dst)==used_images.end()) return dst;
	}
	printf("%s(): unable to find normal_path for %s\n",__func__,bump_path.slash());
	return "";
}

static float optimize_textures( path file_path, vector<mtl::section_t>& sections )
{
	gx::timer_t timer;

	// replace redundant map path
	nocase::set<path> dups; // duplicate images
	auto crc_lut = build_crc_lut( sections );
	for( auto& section : sections )
		for( auto* t : section.maps() )
		{
			if(!t->map_path().exists()) continue;

			auto &src=t->back();
			auto &dst=crc_lut[t->crc]; if(stricmp(src.c_str(),dst.c_str())==0) continue;
			printf( "[%s] replace: %s << %s\n", file_path.name(), src.c_str(), dst.c_str() );
			dups.emplace(t->map_path());
			src = dst;
			b_dirty = true;
		}
	
	// find valid existing images
	nocase::set<path> used_images;
	for( auto& section : sections ) for( auto* t : section.maps() )
	{
		path m=t->map_path(); if(m.exists()){ used_images.insert(m); continue; }
		printf( "[%s] %s.%s: %s not exists\n", file_path.name(), section.name.c_str(), t->key.c_str(), m.name() );
		t->clear(); b_dirty=true;
	}

	// normal map tests
	// 1. clear invalid normal maps
	// 2. add norm entry, when bump-as-normal maps are found
	// 3. find existing bump-to-normal mapping
	nocase::map<path,bool> valid_normals;
	nocase::map<path,path> bton;
	auto test_normal = [&]( mtl::item_t* t )->bool { path m=t->map_path(); if(auto it=valid_normals.find(m);it!=valid_normals.end()) return it->second; return valid_normals[m]=is_normal_map(m); };
	for( auto& section : sections )
	{
		auto* n=section.find("norm"); if(n&&!test_normal(n)){ printf( "[%s] %s: %s is not a normal map\n", file_path.name(), section.name.c_str(), n->map_path().name() ); n->clear(); n=nullptr; b_dirty=true; }
		auto* b=section.find("bump"); if(!n&&b&&test_normal(b)){ n=section.add_norm(b->back()); b_dirty=true; }
		if(b&&n){ auto it=bton.find(b->map_path()); if(it==bton.end()) bton[b->map_path()]=n->map_path(); }
	}

	// auto-generate normal maps
	for( auto& m: sections )
	{
		auto *n=m.find("norm"), *b=m.find("bump"); if(n||!b) continue;
		path norm_path = get_normal_path( b->map_path(), bton, used_images ); if(norm_path.empty()) continue;
		if(!norm_path.exists()&&!generate_normal_map( norm_path, b->map_path(), file_path )) continue;
		n = m.add_norm( norm_path.relative(mtl_dir).c_str() ); if(!n) continue;
		
		used_images.insert(norm_path);
		bton[b->map_path()] = norm_path;
		b_dirty=true;
	}

	// delete non-used images
	for( auto& f : mtl_dir.scan<false>( "jpg;png" ) )
	{
		if(used_images.find(f)!=used_images.end()) continue;
		if(dups.find(f)==dups.end()) // do not delete other non-duplicate non-used images
		{
			if(f.stem()!="index") printf( "[%s] potential redundancy: %s\n", file_path.name(), f.name() );
			continue;
		}
		if(!f.delete_file(true)) continue;
		printf( "[%s] deleting duplicates: %s\n", file_path.name(), f.name() );
	}

	return float(timer.end());
}

//*************************************
#endif // __GX_MTL_OPTIMIZE__
//*************************************