#include <gxut/gxmath.h>
#include <gxut/gxstring.h>
#include <shellapi.h>
#include <gxut/gxfilesystem.h>
#include <gxut/gxos.h>
#include <gxut/gxobjparser.h>
#include <gximage/gximage.h>

const char* __GX_MTLPARSER_CPP_TIMESTAMP__ = __TIMESTAMP__;

struct mtl_item_t
{
	std::string key;
	std::vector<std::string> tokens; // tokens excluding key
	uint	map_crc = 0; // map crc

	mtl_item_t() = default;
	mtl_item_t( const std::string& _key, const char* first_token=nullptr ):key(_key){ if(first_token&&*first_token) tokens.emplace_back(first_token); }

	void clear(){ key.clear(); tokens.clear(); map_crc=0; }
	bool empty() const { return tokens.empty(); }
	size_t size() const { return tokens.size(); }
	std::string& front() { return tokens.front(); }
	std::string& back() { return tokens.back(); }

	std::string str() const { return tokens.empty()?"":(key.empty()?std::string():(key+" "))+join(tokens); }
	const char* token( int index=0 ) const { return tokens[index].c_str(); }
	float value( int index=0 ) const { return float(fast::atof(tokens[index].c_str())); }

	path map_path( const path& dir ) const { return (dir+tokens.back().c_str()).canonical(); }
	bool is_map_type() const { static nocase::set<std::string> map_keys = {"bump","refl","norm","pc","pcr","aniso","anisor"}; return (key.size()>4&&_strnicmp(key.c_str(),"map_",4)==0) || map_keys.find(key)!=map_keys.end(); }
	bool map_make_relative( const path& dir ){ path map_token_path = tokens.back().c_str(); if(!map_token_path.is_absolute()) return false; tokens.back() = map_token_path.is_subdir(dir) ? map_token_path.relative(false,dir).wtoa() : map_token_path.name().wtoa(); return true; }
};

struct mtl_section_t
{
	std::string					name;
	std::vector<mtl_item_t>		items;

	constexpr auto begin(){ return items.begin(); }
	constexpr auto end(){ return items.end(); }

	bool empty() const { return name.empty()||items.empty(); }
	mtl_item_t* find( const char* key ) const { for(auto& t:items){ if(!items.empty()&&_stricmp(t.key.c_str(),key)==0) return (mtl_item_t*)&t; } return nullptr; }
	int find_index( const char* key ) const	{ for( int k=0, kn=int(items.size()); k<kn; k++ ){ if(items[k].empty()) continue; if(_stricmp(items[k].key.c_str(),key)==0) return k; } return -1; }
	void add_norm( std::string token )
	{
		if(find("norm")) printf( "%s(%s): norm already exists\n", __func__, token.c_str() );
		mtl_item_t t("norm",token.c_str());
		if(items.empty()){ items.emplace_back(t); return; }
		int i = find_index("bump");
		if(i<0) items.insert(items.begin()+items.size()-(items.back().empty()?1:0),t);
		else	items.insert(items.begin()+i+1,t);
	}
};

static const std::vector<vec2> halton_samples =
{
	{0.015625f,0.061224f}, {0.515625f,0.204082f}, {0.265625f,0.346939f}, {0.765625f,0.489796f}, {0.140625f,0.632653f}, {0.640625f,0.775510f}, {0.390625f,0.918367f}, {0.890625f,0.081633f},
	{0.078125f,0.224490f}, {0.578125f,0.367347f}, {0.328125f,0.510204f}, {0.828125f,0.653061f}, {0.203125f,0.795918f}, {0.703125f,0.938776f}, {0.453125f,0.102041f}, {0.953125f,0.244898f},
	{0.046875f,0.387755f}, {0.546875f,0.530612f}, {0.296875f,0.673469f}, {0.796875f,0.816326f}, {0.171875f,0.959184f}, {0.671875f,0.122449f}, {0.421875f,0.265306f}, {0.921875f,0.408163f},
	{0.109375f,0.551020f}, {0.609375f,0.693878f}, {0.359375f,0.836735f}, {0.859375f,0.979592f}, {0.234375f,0.142857f}, {0.734375f,0.285714f}, {0.484375f,0.428571f}, {0.984375f,0.571429f},
};

static bool is_normal_map( path file_path )
{
	static nocase::map<path,bool> cache;
	if(auto it=cache.find(file_path); it!=cache.end()) return it->second;

	cache[file_path]=false;
	image* header=gx::load_image_header(file_path); if(!header){ printf("failed to load header of %s\n", file_path.wtoa() ); return false; }
	bool early_exit = header->channels!=3||header->width<64||header->height<64; gx::release_image_header(&header);
	if(early_exit) return false;

	// do not force rgb to bump; and use cache
	image* i = gx::load_image(file_path,true,false,false); if(!i){ printf("failed to load the bump map %s\n", wtoa(file_path) ); return false; }
	int w=i->width, h=i->height;
	int bcount=0, bcount_thresh=int(halton_samples.size()*0.8f);
	for( int k=0, kn=int(halton_samples.size()); k<kn&&bcount<bcount_thresh; k++ )
	{
		const auto& s = halton_samples[k];
		ivec2 tc = ivec2(std::min(w-1,int((w-1)*s.x)),std::min(h-1,int((h-1)*s.y)));
		uchar3 c = *i->ptr<uchar3>(tc.y,tc.x);
		if(c.r==c.g&&c.r==c.b) continue;			// skip grayscale
		if(c.b<c.r||c.b<c.g||c.b<127) continue;		// probably wrong
		vec3 n = vec3(c.r,c.g,c.b)/127.5f-1.0f; if(fabs(n.length()-1.0f)>0.3f) continue;	// length around one
		bcount++;
	}

	gx::release_image(&i);
	return cache[file_path] = bcount<bcount_thresh ? false : true;
}

static bool generate_normal_map( path normal_path, path bump_path )
{
	if(!bump_path.exists()){ printf("%s(): %s not exists\n", __func__, bump_path.name().wtoa() ); return false; }
	FILETIME bump_mtime = bump_path.mfiletime();
	if(normal_path.exists()&&normal_path.mfiletime()>=bump_mtime) return true;

	os::timer_t t;

	// test whether the bump map is actually a normal map
	// do not force rgb to bump; and use cache
	image* bump0 = gx::load_image(bump_path,true,false,false); if(!bump0){ printf("%s(): failed to load %s\n", __func__, bump_path.name().wtoa() ); return false; }
	wprintf( L"generating %s from %s... ", normal_path.name().c_str(), bump_path.name().c_str() );

	// convert uchar bumpmap to 1-channel float image
	image* bump = gx::create_image( bump0->width, bump0->height, 32, 1 );
	int yn=bump->height, c0=bump0->channels, xn=bump->width;
	for( int y=0; y<yn; y++ )
	{
		uchar* src = bump0->ptr<uchar>(y);
		float* dst = bump->ptr<float>(y);
		for( int x=0; x<xn; x++, src+=c0, dst++ ) dst[0] = float(src[0])/255.0f;
	}

	// create normal image
	image* normal = gx::create_image( bump->width, bump->height, 8, 3 );

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
	gx::save_image( normal_path, normal );
	if(normal_path.exists()) normal_path.set_filetime(nullptr,nullptr,&bump_mtime);

	gx::release_image(&bump0);
	gx::release_image(&bump);
	gx::release_image(&normal);

	wprintf( L"completed in %.f ms\n", t.end() );
	return true;
}

static path get_normal_path( const path& bump_path, nocase::map<path,path>& bton, nocase::set<path> used_images )
{
	auto it = bton.find( bump_path );
	if(it!=bton.end()&&is_normal_map(it->second)) return it->second;

	path base = bump_path.remove_ext();
	path ext = bump_path.ext();
	
	// remove postfix for bump
	if(base.size()>4&&_wcsistr(substr(base.c_str(),-4),L"bump")) base=path(substr(base.c_str(),0,-4))+L"norm";
	else if(base.back()==L'b') base.back()=L'n';
	else base+="n";
	
	for( int k=0; k<8; k++, base+="n" )
	{
		path dst = base+L"."+ext;
		if(!dst.exists()||used_images.find(dst)==used_images.end()) return dst;
	}
	printf("%s(): unable to find normal_path for %s\n",__func__,bump_path.to_slash().wtoa());
	return L"";
}

static float optimize_textures( path file_path, std::vector<mtl_section_t>& sections, bool& b_dirty )
{
	os::timer_t timer;
	path dir = file_path.dir();
	
	// preliminary cleanup and scan crc
	nocase::map<std::string,uint> crc_map; // sorted by path
	for( auto& section : sections )
	for( auto& t: section )
	{
		if(!t.is_map_type()) continue;
		if(t.map_make_relative(dir)) b_dirty=true; // make absolute to relative path
		path map_path = t.map_path(dir);
		auto it = crc_map.find(t.back());
		t.map_crc = it!=crc_map.end() ? it->second : (crc_map[t.back()]=map_path.exists()?map_path.crc32c():0);
	}

	// invert lookup table for crc-unique maps
	std::map<uint,std::string> crc_lut;
	for( auto& [token,crc] : crc_map ) // sorted by path
	{
		auto it=crc_lut.find(crc);
		if(it==crc_lut.end()) crc_lut[crc] = token;
	}

	// replace redundant map path
	for( auto& section : sections )
	for( auto& t : section.items )
	{
		if(!t.is_map_type()) continue;
		if(!t.map_path(dir).exists()) continue;
		auto &src = t.back(), dst = crc_lut[t.map_crc]; if(_stricmp(src.c_str(),dst.c_str())==0) continue;
		printf( "[%s] replace: %s << %s\n", file_path.name().wtoa(), src.c_str(), dst.c_str() );
		src = dst;
		b_dirty = true;
	}

	// find valid images and delete non-existing images
	nocase::set<path> used_images;
	for( auto& section : sections )
	for( auto& t : section )
	{
		if(!t.is_map_type()) continue;
		path map_path = t.map_path(dir); if(map_path.exists()){ used_images.insert(map_path); continue; }
		printf( "[%s] %s.%s: %s not exists\n", file_path.name().wtoa(), section.name.c_str(), t.key.c_str(), map_path.name().wtoa() );
		t.clear(); b_dirty=true;
	}

	// when bump-as-normal maps are found, add norm entry
	for( auto& section : sections )
	{
		auto* n=section.find("norm"); if(n) continue;
		auto* b=section.find("bump"); if(!b) continue;
		if(!is_normal_map(b->map_path(dir))) continue;
		section.add_norm(b->back());
		b_dirty = true;
	}

	// warning and clear invalid normal maps
	nocase::map<path,bool> normal_test;
	for( auto& section : sections )
	{
		auto* n=section.find("norm"); if(!n) continue;
		path normal_path = n->map_path(dir);
		auto it=normal_test.find(normal_path);
		bool b = it!=normal_test.end() ? it->second : (normal_test[normal_path]=is_normal_map(normal_path)); if(b) continue;
		printf( "[%s] %s.%s: %s is not a normal map\n", file_path.name().wtoa(), section.name.c_str(), n->key.c_str(), normal_path.name().wtoa() );
		n->clear();
		b_dirty = true;
	}

	// find existing bump-to-normal mapping
	nocase::map<path,path> bton;
	for( auto& m: sections )
	{
		auto* n = m.find("norm"); if(!n) continue;
		auto* b = m.find("bump"); if(!b) continue;
		path bump_path = b->map_path(dir);
		auto it=bton.find(bump_path); if(it!=bton.end()) continue;
		bton[bump_path]=n->map_path(dir);
	}

	// auto-generate normal maps
	for( auto& m: sections )
	{
		auto* n = m.find("norm"); if(n) continue;
		auto* b = m.find("bump"); if(!b) continue;
		path bump_path = b->map_path(dir);
		path norm_path = get_normal_path( bump_path, bton, used_images ); if(norm_path.empty()) continue;
		if(!norm_path.exists()&&!generate_normal_map( norm_path, bump_path )) continue;
		m.add_norm( norm_path.relative(false,dir).wtoa() );
		used_images.insert(norm_path);
		bton[bump_path]=norm_path;
		b_dirty=true;
	}

	// delete non-used images
	for( auto& f : dir.scan( false, L"jpg;png" ) )
	{
		if(used_images.find(f)!=used_images.end()||!f.delete_file(true)) continue;
		printf( "[%s] deleting redundancy: %s\n", file_path.name(true).wtoa(), f.name().wtoa() );
	}

	return float(timer.end());
}

std::vector<mtl_section_t> parse_mtl( path file_path, bool& b_dirty )
{
	std::vector<mtl_section_t> v; v.reserve(1024);

	if(!file_path.exists()){ printf("%s(): %s not exists\n", __func__, file_path.wtoa() ); return v; }
	FILE* fp = _wfopen(file_path,L"r"); if(!fp){ printf("%s(): unable to open %s\n", __func__, file_path.wtoa()); return v; }
	
	// fill no-name header entry
	v.emplace_back(mtl_section_t());

	char buff[4096]={};
	auto is_empty_line = []( const char* s )->bool { char c0=s[0]; if(!c0||c0=='#'||strncmp(s,"Wavefront",9)==0) return true; char c1=s[1]; return !c1; };
	for( uint k=0; fgets(buff,4096,fp)&&k<65536; k++ )
	{
		const char* b = rtrim(buff);

		if(is_empty_line(b)){ if(*b) v.back().items.emplace_back(mtl_item_t("",b)); continue; } // add blank lines
		std::vector<std::string> vs = std::move(explode(b));
		if(vs.size()<2){ b_dirty=true; continue; } // cull no-value lines
		const std::string& key = vs[0];

		if(_stricmp(vs[0].c_str(),"illum")==0){ b_dirty=true; continue; } // pre-skip redundancy
		else if(_stricmp(key.c_str(),"newmtl")==0) v.emplace_back(mtl_section_t(vs[1]));
		else
		{
			mtl_item_t t(key); for(size_t k=1;k<vs.size();k++) t.tokens.emplace_back(vs[k]);
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

static bool save_mtl( path file_path, const std::vector<mtl_section_t>& sections, float opt_time )
{
	auto mfiletime0 = file_path.mfiletime();
	FILE* fp = _wfopen( file_path, L"w" ); if(!fp){ printf("%s(): failed to open %s\n", __func__, file_path.to_slash().wtoa() ); return false; }
	printf( "[%s] optimization ... ", file_path.name().wtoa() );

	for( size_t k=0, kn=sections.size(); k<kn; k++ )
	{
		auto& section = sections[k];
		if(k>0) fprintf(fp,"\n");
		fprintf(fp,"%snewmtl %s\n", k>0?"\n":"", section.name.c_str() );
		for( auto& t : section.items )
		{
			auto r = std::move(t.str());
			if(!r.empty()) fprintf( fp, "%s\n", r.c_str() );
		}
	}
	fclose(fp);
	file_path.set_filetime( nullptr, nullptr, &mfiletime0 ); // keep time stamp
	printf( "completed in %.2f ms\n", opt_time );

	return true;
}

//*************************************
namespace obj {
//*************************************

uint find_material( std::vector<material_impl>& materials, const char* name )
{
	for(uint k=0,kn=uint(materials.size());k<kn;k++)
		if(_stricmp(name,materials[k].name)==0) return k;
	return -1;
}

void create_default_material( std::vector<material_impl>& materials )
{
	materials.emplace_back(material_impl(uint(materials.size())));
	auto& m = materials.back();
	strcpy(m.name,"default");
	m.color = vec4(0.7f,0.7f,0.7f,1.0f);
}

void create_light_material( std::vector<material_impl>& materials )
{
	materials.emplace_back(material_impl(uint(materials.size())));
	auto& m = materials.back();
	strcpy(m.name,"light");
	m.color = vec4(1.0f,1.0f,1.0f,1.0f);
	m.metal = 0.0f;
	m.emissive = 1.0f;
}

bool load_mtl( path file_path, std::vector<material_impl>& materials, bool with_cache )
{
	os::timer_t timer;

	// attributes
	bool b_dirty = false; // something changed?

	// pre-parse raw lines
	auto sections = std::move(parse_mtl(file_path, b_dirty)); if(sections.empty()) return false;
	path mtl_dir = file_path.dir();

	// default material for light source (mat_index==0 or emissive>0)
	materials.clear();
	create_light_material( materials );

	// preprocessing only for fresh loading
	float opt_time = with_cache ? 0.0f : optimize_textures( file_path, sections, b_dirty );

	// tags to bypass
	static nocase::set<std::string> passtags
	{
		"ka",		// ambient materials
		"illum",	// illumination model
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

		materials.emplace_back(material_impl(uint(materials.size())+1));
		auto& m = materials.back();
		strcpy( m.name, section.name.c_str() );

		for( auto& t: section.items )
		{
			std::string key = tolower(t.key.c_str());

			if(t.empty()) continue;
			else if(key=="newmtl"); // already processed
			else if(key=="kd")
			{
				if(t.size()<3){ wprintf(L"Kd size < 3\n"); return false; }
				m.color[0] = t.value();
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
				if(m.path.find("bump")==m.path.end()) // only for new entry
					m.path["bump"] = t.tokens.back();
			}
			else if(key=="refl")	// reflection map
			{
				if(_stristr(t.token(),"metal")) m.path["metal"] = t.token(); // some mtl uses refl for map_pm for legacy compatibility
				else if(!t.empty())	m.rough = 0.0f; 	// ignore the reflection map and use the global env map
			}
			else if(key=="ni")	m.n = t.value(); // refractive index
			else if(key=="d")	m.color.a = t.value();
			else if(key=="tr")	m.color.a = 1.0f-t.value(); // transparency
			// PBR extensions: http://exocortex.com/blog/extending_wavefront_mtl_to_support_pbr
			else if(key=="ke"){ m.emissive = 1.0f; }
			else if(key=="pr"){ if(m.rough>0) m.rough = t.value(); } // only for non-reflection
			else if(key=="map_ke"){ m.path["emissive"] = t.token(); }
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
			else if( passtags.find(key)==passtags.end() )
			{
				printf("[%s] unrecognized key: %s\n",file_path.name().wtoa(),key.c_str()); return false;
			}
		}

		// per-material postprocessing
		m.metal = clamp(m.color.r/m.specular,0.0f,1.0f); // convert to metallic from specular
	}

	// update file after bump_as_normal
	if(b_dirty) save_mtl( file_path, sections, opt_time );
	if(!with_cache) wprintf( L"Loading %s ... completed in %.2f ms\n", file_path.name().c_str(), timer.end() );
	
	return true;
}

//*************************************
} // namespace obj
//*************************************