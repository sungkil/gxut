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
	std::string	raw; // raw line
	std::string key; // key or tag
	std::vector<std::string> tokens;
	uint	map_crc = 0; // map crc

	mtl_item_t() = default;
	mtl_item_t( const std::string& _key, const char* _raw ):key(tolower(_key.c_str())),raw(_raw){}

	void clear(){ raw.clear(); key.clear(); tokens.clear(); map_crc=0; }
	bool empty() const { return tokens.empty(); }
	size_t size() const { return tokens.size(); }
	const char* token( int index=0 ) const { return tokens[index].c_str(); }
	const wchar_t* wtoken(int index=0 ) const { return atow(tokens[index].c_str()); }
	float value( int index=0 ) const { return float(fast::atof(tokens[index].c_str())); }

	path map_path( const path& dir ) const { return (dir+tokens.back().c_str()).canonical(); }
	std::string& map_token() { return tokens.back(); }
	std::string map_raw() const
	{
		std::string r = raw.empty()?key:explode(raw.c_str()).front(); // restore case
		for( auto& t : tokens ){ r+=" "; r+=t; }
		return r;
	}
	bool is_map_type() const
	{
		static std::set<std::string> map_keys = {"bump","refl","norm","pc","pcr","aniso","anisor"};
		return (key.size()>4&&strncmp(key.c_str(),"map_",4)==0) || map_keys.find(key)!=map_keys.end();
	}
	bool map_make_relative( const path& dir )
	{
		path map_token_path = tokens.back().c_str(); if(!map_token_path.is_absolute()) return false;
		tokens.back() = map_token_path.is_subdir(dir) ? map_token_path.relative(false,dir).wtoa() : map_token_path.name().wtoa();
		return true;
	}
};

struct mtl_section_t
{
	std::string					name;
	std::vector<mtl_item_t>		items;

	bool empty() const { return name.empty()||items.empty(); }
	mtl_item_t* find( const char* key ) const
	{
		for(auto& t:items)
		{
			if(items.empty()) continue;
			if(_stricmp(t.key.c_str(),key)==0) return (mtl_item_t*)&t;
		}
		return nullptr;
	}
	int find_index( const char* key ) const	{ for( int k=0, kn=int(items.size()); k<kn; k++ ){ if(items[k].empty()) continue; if(_stricmp(items[k].key.c_str(),key)==0) return k; } return -1; }
	void add_norm( std::string token )
	{
		mtl_item_t t("norm",format("norm %s\n",token.c_str())); t.tokens.emplace_back(token);
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
	int w=i->width, h=i->height; int bcount=0;
	for( auto s : halton_samples )
	{
		ivec2 tc = ivec2(std::min(w-1,int((w-1)*s.x)),std::min(h-1,int((h-1)*s.y)));
		uchar3 c = *i->ptr<uchar3>(tc.y,tc.x);	if(c.b<c.r||c.b<c.g||c.b<127) continue;
		vec3 n = vec3(c.r,c.g,c.b)/127.5f-1.0f;	if(fabs(n.length()-1.0f)>0.3f) continue;
		bcount++;
	}
	gx::release_image(&i);
	return cache[file_path] = bcount>int(halton_samples.size()*0.9f) ? true : false;
}

static path generate_normal_map( path normal_path, path bump_path )
{
	if(!bump_path.exists()) return L"";
	FILETIME bump_mtime = bump_path.mfiletime();
	if(normal_path.exists()&&normal_path.mfiletime()>=bump_mtime) return normal_path;

	os::timer_t t;

	// test whether the bump map is actually a normal map
	// do not force rgb to bump; and use cache
	image* bump0 = gx::load_image(bump_path,true,false,false); if(!bump0){ printf("failed to load the bump map %s\n", wtoa(bump_path) ); return L""; }
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

	return normal_path;
}

static path get_normal_path( const path& bump_path, nocase::set<path> used_images )
{
	path base = bump_path.remove_ext(); if(base.back()==L'b') base.back()=0;
	path ext = bump_path.ext();
	base += "n"; for( int k=0; k<16; k++, base+="n" )
	{
		path dst = base+L"."+ext;
		if(!dst.exists()||used_images.find(dst)==used_images.end()) return dst;
	}
	return L"";
}

static void optimize_textures( path file_path, std::vector<mtl_section_t>& sections, bool& b_dirty )
{
	path dir = file_path.dir();
	
	// cleanup and scan crc
	nocase::map<std::string,uint> crc_map; // sorted by path
	for( auto& section : sections )
	for( auto& t: section.items )
	{
		if(!t.is_map_type()) continue;
		if(t.map_make_relative(dir)) b_dirty=true; // make absolute to relative path

		// remove non-existent textures
		path map_path = t.map_path(dir);
		if(!map_path.exists()&&t.key!="norm")
		{
			printf( "[%s] %s.%s: %s not exists\n", file_path.name().wtoa(), section.name.c_str(), t.key.c_str(), map_path.name().wtoa() );
			t.clear();
			b_dirty=true;
			continue;
		}

		// update per-item crc
		auto it = crc_map.find(t.map_token());
		t.map_crc = it!=crc_map.end() ? it->second : (crc_map[t.map_token()]=map_path.crc32c());
	}

	// invert lookup table for crc-unique maps
	std::map<uint,std::string> crc_lut;
	for( auto& [token,crc] : crc_map ) // sorted by path
	{
		auto it=crc_lut.find(crc);
		if(it==crc_lut.end()) crc_lut[crc] = token;
	}

	// replace redundant map path
	std::vector<path> redundant_files;
	for( auto& section : sections )
	for( auto& t : section.items )
	{
		if(!t.is_map_type()) continue;
		auto &src = t.map_token(), dst = crc_lut[t.map_crc];
		if(_stricmp(src.c_str(),dst.c_str())==0) continue;

		printf( "replace: %s >> %s\n", src.c_str(), dst.c_str() );
		redundant_files.emplace_back(t.map_path(dir));
		src = dst;
	}

	// add norm for bump-as-normal maps
	for( auto& section : sections )
	{
		auto* n=section.find("norm"); if(n) continue;
		auto* b=section.find("bump"); if(!b) continue;
		if(!is_normal_map(b->map_path(dir))) continue;
		section.add_norm(b->map_token());
		b_dirty = true;
	}

	// find valid used images
	nocase::set<path> used_images;
	for( auto& section : sections )
		for( auto& t : section.items )
			if(t.is_map_type()) used_images.insert(t.map_path(dir));

	// generate normal maps
	for( auto& m: sections )
	{
		auto* n = m.find("norm"); if(n&&n->map_path(dir).exists()) continue;
		auto* b = m.find("bump"); if(!b) continue;
		path bump_path = b->map_path(dir); if(!bump_path.exists()) continue;
		path norm_path = get_normal_path( bump_path, used_images ); if(norm_path.empty()){ printf("%s(): unable to find normal_path for %s\n",__func__,bump_path.to_slash().wtoa()); continue; }
		m.add_norm( norm_path.relative(false,dir).wtoa() );
		if(!norm_path.exists())
		{
			//if(norm_path.exists()) printf("%s already exists\n", norm_path.name().wtoa());
			if(!generate_normal_map( norm_path, bump_path ))
				printf( "failed to generate normal map %s\n", norm_path.wtoa() );
		}
		used_images.insert(norm_path);
		b_dirty=true;
	}

	// delete redundant files
	if(!redundant_files.empty())
	{
		b_dirty = true;
		for( auto& f : redundant_files )
		{
			if(f.delete_file(true))
				printf( "[%s] deleting duplicates: %s\n", file_path.name(true).wtoa(), f.name().wtoa() );
		}
	}

	// delete non-used images
	for( auto& f : dir.scan( false, L"jpg;jpeg;png;tga" ) )
	{
		if(used_images.find(f)==used_images.end()&&f.delete_file(true))
			printf( "[%s] deleting redundancy: %s\n", file_path.name(true).wtoa(), f.name().wtoa() );
	}
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

		if(is_empty_line(b)){ v.back().items.emplace_back(mtl_item_t("",b)); continue; } // add blank lines
		std::vector<std::string> vs = std::move(explode(b));
		if(vs.size()<2){ b_dirty=true; continue; } // cull no-value lines
		
		std::string key = tolower(vs[0].c_str());
		if(key=="newmtl") v.emplace_back(mtl_section_t(vs[1]));
		else if(key=="illum"){ b_dirty=true; continue; } // pre-skip redundancy

		mtl_item_t t(key,b); for(size_t k=1;k<vs.size();k++) t.tokens.emplace_back(vs[k]);
		v.back().items.emplace_back(t);		// add split tokens
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
	m.color = vec4(0.7f,0.7f,0.7f,1.0f);
	strcpy(m.name,"default");
}

bool load_mtl( path file_path, std::vector<material_impl>& materials )
{
	// attributes
	bool b_dirty = false; // something changed?

	// pre-parse raw lines
	auto sections = std::move(parse_mtl(file_path, b_dirty)); if(sections.empty()) return false;
	path mtl_dir = file_path.dir();

	// default material for light source (mat_index==0 or emissive>0)
	materials.clear();
	materials.emplace_back(material_impl(0));
	auto& m = materials.back();
	strcpy(m.name,"light");
	m.color = vec4(1.0f,1.0f,1.0f,1.0f);
	m.metal = 0.0f;
	m.emissive = 1.0f;

	// preprocessing
	optimize_textures( file_path, sections, b_dirty );
	
	// start loading
	for( auto& section : sections )
	{
		if(section.empty()) continue;

		materials.emplace_back(material_impl(uint(materials.size())+1));
		auto& m = materials.back();
		strcpy( m.name, section.name.c_str() );

		for( auto& t: section.items )
		{
			const auto& key = t.key;

			if(t.empty()) continue;
			else if(key=="newmtl"); // already processed
			else if(key=="ka"); // ambient materials
			else if(key=="kd")
			{
				if(t.size()<3){ wprintf(L"Kd size < 3\n"); return false; }
				m.color[0] = t.value();
				m.color[1] = t.value(1);
				m.color[2] = t.value(2);
				m.color[3] = 1.0f;
			}
			else if(key=="ks")
			{
				m.specular = t.value();
			}
			else if(key=="ns") // specular power
			{
				m.beta = t.value();
				if(m.rough>0) m.rough = beta_to_roughness(m.beta); // only for non-reflection
			}
			else if(key=="ni")	m.n = t.value(); // refractive index
			else if(key=="d")	m.color.a = t.value();
			else if(key=="tr")	m.color.a = 1.0f-t.value(); // transparency
			else if(key=="map_ka"){ m.path["ambient"] = t.wtoken(); }	// ambient occlusion
			else if(key=="map_kd"){ m.path["albedo"] = t.wtoken(); }
			else if(key=="map_d"||key=="map_opacity"){ m.path["alpha"] = t.wtoken(); }
			else if(key=="bump")
			{
				if(m.path.find("bump")==m.path.end()) // only for new entry
					m.path["bump"] = t.tokens.back();
			}
			else if(key=="refl")	// reflection map
			{
				if(_wcsistr(t.wtoken(),L"metal")) m.path["metal"] = t.wtoken(); // some mtl uses refl for map_pm for legacy compatibility
				else if(!t.empty())	m.rough = 0.0f; 	// ignore the reflection map and use the global env map
			}
			else if(key=="illum");	// illumination model
			else if(key=="map_ks");	// specular map
			else if(key=="map_ns"); // specular power map
			else if(key=="disp");	// displacement map
			else if(key=="tf");		// Transmission filter
			else if(key=="fr");		// Fresnel reflectance
			else if(key=="ft");		// Fresnel transmittance
			else if(key=="Ia");		// ambient light
			else if(key=="Ir");		// intensity from reflected direction
			else if(key=="It");		// intensity from transmitted direction
			// PBR extensions: http://exocortex.com/blog/extending_wavefront_mtl_to_support_pbr
			else if(key=="ke"){ m.emissive = 1.0f; }
			else if(key=="Pr"){ if(m.rough>0) m.rough = t.value(); } // only for non-reflection
			else if(key=="map_ke"){ m.path["emissive"] = t.wtoken(); }
			else if(key=="map_pr"){ m.path["rough"] = t.wtoken(); }
			else if(key=="map_pm"){ m.path["metal"] = t.wtoken(); }
			else if(key=="map_ps"){ m.path["sheen"] = t.wtoken(); }
			else if(key=="norm"){ m.path["normal"] = t.wtoken(); }
			else if(key=="pc"){ m.path["clearcoat"] = t.wtoken(); }
			else if(key=="pcr"){ m.path["clearcoatroughness"] = t.wtoken(); }
			else if(key=="aniso"){ m.path["anisotropy"] = t.wtoken(); }
			else if(key=="anisor"){ m.path["anisotropyrotation"] = t.wtoken(); }
			// DirectXMesh toolkit extension
			else if(key=="map_RMA"){ m.path["rma"] = t.wtoken(); } // (roughness, metalness, ambient occlusion)
			else if(key=="map_ORM"){ m.path["orm"] = t.wtoken(); } // (ambient occlusion, roughness, metalness)
			// unrecognized
			else { printf("obj::%s(): unrecognized command - %s\n",__func__,key.c_str()); return false; }
		}

		// per-material postprocessing
		m.metal = clamp(m.color.r/m.specular,0.0f,1.0f); // convert to metallic from specular
	}

	// update file after bump_as_normal
	if(b_dirty)
	{
		auto mfiletime0 = file_path.mfiletime();
		FILE* fp = _wfopen( file_path, L"w" ); if(!fp){ printf("%s(): failed to open %s\n", __func__, file_path.to_slash().wtoa() ); return false; }
		printf( "writing %s\n", file_path.name().wtoa() );

		for( size_t k=0, kn=sections.size(); k<kn; k++ )
		{
			auto& section = sections[k];
			if(k>0) fprintf(fp,"\n");
			for( auto& t : section.items )
			{
				if(t.raw.empty()) continue;
				else if(!t.is_map_type()) fprintf( fp, "%s\n", t.raw.c_str() );
				else fprintf( fp, "%s\n", t.map_raw().c_str() );
			}
		}
		fclose(fp);
		file_path.set_filetime( nullptr, nullptr, &mfiletime0 ); // keep time stamp
	}

	return true;
}

//*************************************
} // namespace obj
//*************************************