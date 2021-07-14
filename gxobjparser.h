#pragma once
#include <gxut/gxstring.h>
#include <gxut/gxfilesystem.h>
#include <gxut/gxos.h>
#include <gxut/gxmath.h>
#include <gxut/gxmesh.h>
#include <future>

static const bool USE_MODEL_CACHE = true;

#if (defined(_unzip_H)||defined(__GXZIP_H__)) && defined(__7Z_H) && defined(__7Z_MEMINSTREAM_H)
	static nocase::set<std::wstring> archive_ext_map = { L"zip", L"7z" };
#elif (defined(_unzip_H)||defined(__GXZIP_H__))
	static nocase::set<std::wstring> archive_ext_map = { L"zip" };
#elif defined(__7Z_H) && defined(__7Z_MEMINSTREAM_H)
	static nocase::set<std::wstring> archive_ext_map = { L"7z" };
#else
	static nocase::set<std::wstring> archive_ext_map;
#endif

//*************************************
// support data and routines for fast string conversion
namespace obj {
//*************************************

// vertex hash key using position ID; significantly faster than bitwise hash
struct vertex_hash {size_t operator()(const ivec3& v)const{return v.x;}};
using vtx_map_t		= std::unordered_map<ivec3,uint,vertex_hash>;
using pos_vector_t	= std::vector<vec3>;
using nrm_vector_t	= std::vector<vec3>;
using tex_vector_t	= std::vector<vec2>;
using vtx_vector_t	= decltype(mesh::vertices);
using idx_vector_t	= decltype(mesh::indices);

vtx_map_t*		vtx = nullptr;
pos_vector_t	pos;
tex_vector_t	tex;
nrm_vector_t	nrm;
vtx_vector_t*	vertices = nullptr;

__forceinline char* rm_newline( char* str, int& len ){ if(str[len-1]=='\r')str[--len]='\0';else if(str[len-2]=='\r')str[len-=2]='\0';else if(str[len-1]=='\n')str[--len]='\0';return str; } // CR never follows NL
__forceinline char* next_token( char* buff ){while(*buff!=' '&&*buff)buff++;*(buff++)=0;while(*buff==' ')++buff;return buff; }
__forceinline char* tab_to_space( char* buff, int len ){ for(int j=0;j<len;j++,buff++)if(*buff=='\t')*buff=' '; return buff; }
__forceinline char* rtrim( char* buff, int& len ){ int j;for(j=len-1;j>=0;j--)if(buff[j]!=' ')break;buff[len=j+1]='\0';return buff; }
__forceinline char  tolower( char c ){ uchar d=static_cast<uchar>(c-'A');return d<26?'a'+d:c;}
__forceinline path decompress( const path& file_path );

//*************************************
} // namespace obj
//*************************************

inline void clear_absent_textures( std::vector<material_impl>& materials, path mesh_dir )
{
	for( auto& m : materials )
		for( auto& it : m.path )
			if(!it.second.empty()&&!(mesh_dir+it.second).exists()) it.second.clear();
}

inline void convert_unsupported_texture_to_jpeg( std::vector<material_impl>& materials, path mesh_dir )
{
	bool b_first = true;
	for( auto& m : materials )
	for( auto& it : m.path )
	{
		path src = mesh_dir+it.second; if(!src.exists()) continue;
		if(src.ext()==L"tga")
		{
			image* img = gx::load_image(src);
			path dst = src.dir()+src.name(false)+(img->channels==3?L".jpg":L".png");
			if(!dst.exists())
			{
				gx::save_image( dst, img );
				if(b_first){ wprintf( L"\n"); b_first=false; }
				wprintf( L"converting %s to %s\n", src.name().c_str(), dst.name().c_str() );
			}
			gx::release_image(&img);
			it.second = dst.name();
		}
	}
}

inline path generate_normal_map( const path& bump_path, float bump_scale )
{
	if(!bump_path.exists()) return L"";
	path normal_path = bump_path.remove_ext();
	if(normal_path.back()==L'b') normal_path.back()=L'\0'; normal_path += path(L"n.")+bump_path.ext();

	FILETIME bump_mtime = bump_path.mfiletime();
	if(normal_path.exists()&&normal_path.mfiletime()>=bump_mtime) return normal_path;

	//wprintf( L"generating a normal map for %s... ", bump_path.name() );timer->begin();
	image* bump0 = gx::load_image(bump_path,true,false,false);	// do not force rgb to bump
	if(bump0==nullptr){ printf("failed to load the bump map %s\n", wtoa(bump_path) ); return L""; }

	// test whether the bump map is actually a normal map
	if(bump0->channels==3)
	{
		uchar* ptr = bump0->ptr<uchar>();
		if((ptr[0]!=ptr[1]||ptr[0]!=ptr[2])&&(ptr[2]-127)>64)
		{
			gx::release_image(&bump0);
			return bump_path;
		}
	}

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
	bump_scale = min(1000.0f,1.0f/(bump_scale+0.000001f));
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
			float nx = (src2[r]-src2[l]) + (src1[r]-src1[l])*2.0f + (src0[r]-src0[l]);
			float ny = (src2[l]-src0[l]) + (src2[x]-src0[x])*2.0f + (src2[r]-src0[r]);
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
	//wprintf( L" (and generating normal %s)", normal_path.name(true).c_str() );

	gx::release_image(&bump0);
	gx::release_image(&bump);
	gx::release_image(&normal);

	return normal_path;
}

// generate normal maps
inline void generate_normal_maps( std::vector<material_impl>& materials, path dir, std::map<uint,float>& bump_scale_map )
{
	std::map<path,path> btom; // bump_to_normal;
	for( auto& m : materials )
	{
		auto it=m.path.find("normal"); if(it==m.path.end()) continue;
		path bump_path = dir + it->second; if(!bump_path.exists()) continue; // bump map stored in normal when being loaded
		
		auto n = btom.find(bump_path.c_str());
		if(n!=btom.end()&&n->second.exists()) m.path["normal"]=n->second;
		else if(n==btom.end())
		{
			auto s = bump_scale_map.find(m.ID); float bump_scale = s==bump_scale_map.end()?1.0f:s->second;
			path normal_path = generate_normal_map( bump_path, bump_scale );
			if(normal_path.exists()) btom.emplace(bump_path,normal_path);
			m.path["normal"] = normal_path.name();
		}
	}
}

inline uint find_mesh_material( mesh* pMesh, const char* name )
{
	for(uint k=0;k<pMesh->materials.size();k++)
		if(_stricmp(name,pMesh->materials[k].name)==0) return k;
	return -1;
}

inline void create_default_material( std::vector<material_impl>& mats )
{
	mats.emplace_back(material_impl( uint(mats.size()) ));
	auto* m = &mats.back();
	m->color = vec4( 0.7f, 0.7f, 0.7f, 1.0f );
	strcpy(m->name,"default");
}

inline bool load_mtl( path file_path, std::vector<material_impl>& mats )
{
	if(!file_path.exists()){ printf("%s(): %s not exists\n", __func__, file_path.wtoa() ); return false; }
	FILE* fp = _wfopen( file_path, L"r" ); if(!fp){ printf("%s(): unable to open %s\n", __func__, file_path.wtoa()); return false; }
	path mesh_dir=file_path.dir();

	// additional temporary attributes
	std::map<uint,float> bump_scale_map;

	// default material for light source; mat_index==0 or emissive>0 indicates a light source
	mats.clear();
	mats.emplace_back(material_impl(0));
	material_impl* m = &mats.back();
	strcpy(m->name,"light");
	m->color = vec4(1,1,1,1);
	m->metal = 0.0f;
	m->emissive = 1.0f;

	// start parsing
	char buff[8192]={};
	for( uint k=0; fgets(buff,8192,fp)&&k<65536; k++ )
	{
		char c0=buff[0]; if(!c0||c0=='#'||strncmp(buff,"Wavefront",9)==0) continue;
		char c1=buff[1]; if(!c1) continue;

		std::vector<std::string> vs=explode(_strlwr(buff)); if(vs.size()<2) continue;
		std::string& key	= vs[0];
		std::string& token	= vs[1];

		if(key=="newmtl")
		{
			mats.emplace_back(material_impl(uint(mats.size())+1)); m=&mats.back();
			strcpy(m->name,token.c_str());
		}
		else if(key=="ka");		// ambient materials
		else if(key=="kd")
		{
			if(vs.size()<4){ wprintf(L"Kd size < 3\n"); return false; }
			m->color[0] = float(fast::atof(vs[1].c_str()));
			m->color[1] = float(fast::atof(vs[2].c_str()));
			m->color[2] = float(fast::atof(vs[3].c_str()));
			m->color[3] = 1.0f;
		}
		else if(key=="ks") m->specular = float(fast::atof(vs[1].c_str()));
		else if(key=="ns") // specular power
		{
			m->beta = float(fast::atof(token.c_str()));
			m->rough = beta_to_roughness(m->beta); 
		}
		else if(key=="ni")	m->n = (float)fast::atof(token.c_str()); // refractive index
		else if(key=="d")	m->color.a = float(fast::atof(token.c_str()));
		else if(key=="tr")	m->color.a = 1.0f-float(fast::atof(token.c_str())); // transparency
		else if(key=="map_ka"){ m->path["ambient"] = atow(token.c_str()); }	// ambient occlusion
		else if(key=="map_kd"){ m->path["albedo"] = atow(token.c_str()); }
		else if(key=="map_d"||key=="map_opacity"){ m->path["alpha"] = atow(token.c_str()); }
		else if(key=="map_bump"||key=="bump")
		{
			for( uint j=1, jn=uint(vs.size()); j<jn; j++ )
			{
				if(vs[j][0]!='-') m->path["normal"] = atow(token.c_str()); // when it's not an option
				else if(_stricmp("-bm", vs[j].c_str())==0&&vs.size()>=(j+2))
				{ bump_scale_map[m->ID] = float(fast::atof(vs[j+1].c_str())); j++; }
			}
		}
		else if(key=="illum");	// illumination model
		else if(key=="map_ks");	// specular map
		else if(key=="map_ns"); // specular power map
		else if(key=="refl");	// reflection map
		else if(key=="disp");	// displacement map
		else if(key=="tf");		// Transmission filter
		else if(key=="fr");		// Fresnel reflectance
		else if(key=="ft");		// Fresnel transmittance
		else if(key=="Ia");		// ambient light
		else if(key=="Ir");		// intensity from reflected direction
		else if(key=="It");		// intensity from transmitted direction
		// PBR extensions: http://exocortex.com/blog/extending_wavefront_mtl_to_support_pbr
		else if(key=="ke"){ m->emissive = 1.0f; }
		else if(key=="Pr"){ m->rough = float(fast::atof(token.c_str())); }
		else if(key=="map_ke"){ m->path["emissive"] = atow(token.c_str()); }
		else if(key=="map_pr"){ m->path["rough"] = atow(token.c_str()); }
		else if(key=="map_pm"){ m->path["metal"] = atow(token.c_str()); }
		else if(key=="map_ps"){ m->path["sheen"] = atow(token.c_str()); }
		else if(key=="norm"){ m->path["normal"] = atow(token.c_str()); }
		else if(key=="pc"){ m->path["clearcoat"] = atow(token.c_str()); }
		else if(key=="pcr"){ m->path["clearcoatroughness"] = atow(token.c_str()); }
		else if(key=="aniso"){ m->path["anisotropy"] = atow(token.c_str()); }
		else if(key=="anisor"){ m->path["anisotropyrotation"] = atow(token.c_str()); }
		// DirectXMesh toolkit extension
		else if(key=="map_RMA"){ m->path["rma"] = atow(token.c_str()); } // (roughness, metalness, ambient occlusion)
		else if(key=="map_ORM"){ m->path["orm"] = atow(token.c_str()); } // (ambient occlusion, roughness, metalness)
		// unrecognized
		else if(k>0){ printf("%s(): '%s %s' is unrecognized mtl command\n",__func__,key.c_str(),token.c_str()); return false; }
	}
	fclose(fp);

	//*********************************
	// postprocessing
	//********************************* 

	path mtl_dir = file_path.dir();
	for( auto& m : mats )
	{
		// convert from specular to metallic
		m.metal = clamp(m.color.r/m.specular,0.0f,1.0f);

		// absolute paths to relative paths
		for( auto& it : m.path ) if(!it.second.empty()&&it.second.is_absolute()) it.second=it.second.relative(false,mtl_dir);
	}

	// postprocessing
	clear_absent_textures( mats, mesh_dir );
	convert_unsupported_texture_to_jpeg( mats, mesh_dir );
	generate_normal_maps( mats, mesh_dir, bump_scale_map );

	return true;
}

//*************************************
// Cache Implementation
namespace obj::cache
{
	inline path& get_dir(){ static path d = path::temp(false)+L"global\\mesh\\"; if(!d.exists()&&!d.mkdir()) wprintf(L"Unable to create %s\n",d.c_str()); return d; }
	inline path get_path( path file_path ){ return get_dir()+file_path.name()+L".rxb"; }
	inline uint64_t get_parser_id( const char* timestamp ){ return uint64_t(std::hash<std::string>{}(std::string(__TIMESTAMP__)+std::string(__GX_MESH_H_TIMESTAMP__)+timestamp)); }

	inline void save_mesh( mesh* p_mesh )
	{
		path file_path = path(p_mesh->file_path);
		path cache_path = get_path(file_path);
		FILE* fp = _wfopen( cache_path, L"w" ); if(!fp){ wprintf(L"Unable to write %s\n",cache_path.c_str()); return; }

		// save the parser's id to reflect the revision of the parser and mesh's timestamp
		fprintf( fp, "parserid = %llu\n", get_parser_id(file_path.mtimestamp()) );

		// save mtl path
		path mtl_path = file_path.dir()+p_mesh->mtl_path;
		fprintf( fp, "mtllib = %s\n", wcslen(p_mesh->mtl_path)==0?"default":wtoa(p_mesh->mtl_path) );

		// save counters
		fprintf( fp, "object_count = %u\n", uint(p_mesh->objects.size()) );
		fprintf( fp, "geometry_count = %u\n", uint(p_mesh->geometries.size()) );
		fprintf( fp, "vertex_count = %u\n", uint(p_mesh->vertices.size()) );
		fprintf( fp, "index_count = %u\n", uint(p_mesh->indices.size()) );
	
		// save bound
		fprintf( fp, "bound = %f %f %f %f %f %f\n", p_mesh->box.m[0], p_mesh->box.m[1], p_mesh->box.m[2], p_mesh->box.M[0], p_mesh->box.M[1], p_mesh->box.M[2]);

		// save objects
		for( uint k=0; k < p_mesh->objects.size(); k++ )
		{
			object& obj = p_mesh->objects[k]; const bbox& b=obj.box;
			fprintf( fp, "o[%d] %s ", k, obj.name );
			fprintf( fp, "%f %f %f %f %f %f", b.m[0], b.m[1], b.m[2], b.M[0], b.M[1], b.M[2] );
			fprintf( fp, "\n" );
		}

		// save geometries
		for( uint k=0; k < p_mesh->geometries.size(); k++ )
		{
			auto& g = p_mesh->geometries[k]; const bbox& b=g.box;
			fprintf( fp, "g[%d] %d %d %d %d ", k, g.object_index, g.material_index, g.first_index, g.count );
			fprintf( fp, "%f %f %f %f %f %f", b.m[0], b.m[1], b.m[2], b.M[0], b.M[1], b.M[2] );
			fprintf( fp, "\n" );
		}

		// meaningless empty line for sepration
		fprintf( fp, "\n" );

		// 7.1 close file and reopen as a binary mode: writing must be done in binary mode !!!!
		fclose(fp); _flushall();
		fp = _wfopen( cache_path, L"ab" );	// attach + binary mode

		// 7.2 save vertex and index list
		fwrite( &p_mesh->vertices[0], sizeof(vertex), p_mesh->vertices.size(), fp );
		fwrite( &p_mesh->indices[0], sizeof(uint), p_mesh->indices.size(), fp );

		// 8. close file
		fclose(fp);
	}

	inline mesh* load_mesh( path file_path )
	{
		path cache_path = get_path(file_path); if(!cache_path.exists()) return nullptr;
		FILE* fp = _wfopen( cache_path, L"rb" ); if(!fp) return nullptr;

		// get parser id
		uint64_t parserid;
		char buff[8192]; fgets(buff,8192,fp); sscanf( buff, "parserid = %llu\n", &parserid );
		if(parserid!=get_parser_id(file_path.mtimestamp())){ fclose(fp); return nullptr; }

		// get the mtl name
		char mtl_name[1024]; fgets(buff,8192,fp); sscanf( buff, "mtllib = %s\n", mtl_name );
		path mtl_path = path(file_path).dir()+atow(mtl_name);
		if(!mtl_path.exists()&&mtl_path!=L"default"){ fclose(fp); return nullptr; } // mtl not exists
	
		// now create mesh
		mesh* p_mesh = new mesh();
		wcscpy(p_mesh->file_path,file_path);
		wcscpy(p_mesh->mtl_path,mtl_path.exists()?atow(mtl_name):L"default");
		if(mtl_path.exists()&&!load_mtl(mtl_path, p_mesh->materials)){ delete p_mesh; return nullptr; }
		
		// load counters
		uint object_count=0;	fgets(buff,8192,fp); sscanf(buff,"object_count = %u\n", &object_count );
		uint geometry_count=0;	fgets(buff,8192,fp); sscanf(buff,"geometry_count = %u\n", &geometry_count );
		uint vertex_count=0;	fgets(buff,8192,fp); sscanf(buff,"vertex_count = %u\n", &vertex_count);
		uint index_count=0;		fgets(buff,8192,fp); sscanf(buff,"index_count = %u\n", &index_count);
	
		// exception handling on the counters
		if(object_count>(1<<30)||geometry_count>(1<<30)){ fclose(fp); delete(p_mesh); return nullptr; }

		// bounding box
		vec3 &m=p_mesh->box.m, &M=p_mesh->box.M;
		fgets(buff,8192,fp); sscanf(buff,"bound = %f %f %f %f %f %f\n", &m[0],&m[1],&m[2],&M[0],&M[1],&M[2] );

		// load objects
		p_mesh->objects.reserve(object_count);
		for( uint k=0; k < object_count; k++ )
		{
			object* obj = p_mesh->create_object("");
			vec3 &m=obj->box.m, &M=obj->box.M;
			fgets(buff,8192,fp);sscanf(buff,"o[%d] %s %f %f %f %f %f %f\n", &obj->ID,obj->name,&m[0],&m[1],&m[2],&M[0],&M[1],&M[2]);
		}

		// load geometries
		p_mesh->geometries.reserve(geometry_count);
		for( uint k=0; k < geometry_count; k++ )
		{
			geometry g(p_mesh,uint(p_mesh->geometries.size()),-1,0,0,nullptr,-1);
			vec3 &m=g.box.m, &M=g.box.M;
			fgets(buff,8192,fp); sscanf(buff,"g[%d] %d %d %d %d %f %f %f %f %f %f\n", &g.ID, &g.object_index, &g.material_index, &g.first_index, &g.count, &m[0],&m[1],&m[2],&M[0],&M[1],&M[2] );
			p_mesh->geometries.emplace_back(g);
			p_mesh->objects[g.object_index].children.push_back(g.ID);
		}

		// meaningless empty line for sepration
		fgets(buff,8192,fp);

		// load vertices and indices
		p_mesh->vertices.resize(vertex_count);	fread( &p_mesh->vertices[0], sizeof(vertex), vertex_count, fp );
		p_mesh->indices.resize(index_count);	fread( &p_mesh->indices[0], sizeof(uint), index_count, fp );

		// close file
		fclose(fp);
		return p_mesh;
	}
}

//*************************************
__forceinline uint get_or_create_vertex( char* str )
{
	// parse the key for extracting indices
	ivec3 idx;
	idx.x=fast::atoi(str); while(*str!='/'&&*str) str++;str++;
	idx.y=fast::atoi(str); while(*str!='/'&&*str) str++;str++;
	idx.z=fast::atoi(str);

	if(idx.x<0) idx.x += int(obj::pos.size());
	if(idx.y<0) idx.y += int(obj::tex.size());
	if(idx.z<0) idx.z += int(obj::nrm.size());

	// check whether the vertex exists
	auto it=obj::vtx->find(idx); if(it!=obj::vtx->end()) return uint(it->second);

	// add to index cache and vertex list
	uint vtx_size = uint(obj::vtx->size());
	obj::vtx->emplace( idx, vtx_size );
	obj::vertices->emplace_back( vertex{obj::pos[idx.x],obj::nrm[idx.z],obj::tex[idx.y]} );

	return vtx_size;
}

path obj::decompress( const path& file_path )
{
	path dst_path;
#if defined(__GXZIP_H__)||defined(_unzip_H)
	if(_wcsicmp(file_path.ext(),L"zip")==0)
	{
		zip_t z(file_path);
		if(!z.load()||z.entries.empty()){ printf("%s(): unabled to load %s",__func__,file_path.wtoa()); return dst_path; }
		if(z.entries.size()!=1){ printf("%s(): have only a single file for mesh in %s\n",__func__,file_path.wtoa() ); return dst_path; }
		dst_path = cache::get_dir()+z.entries.front().name;
		if(dst_path.exists()) dst_path.delete_file();
		if(!z.extract_to_files(dst_path.dir())) return path();
	}
#endif
#if defined(__7Z_H) && defined(__7Z_MEMINSTREAM_H)
	if(_wcsicmp(file_path.ext(),L"7z")==0)
	{
		szip_t s(file_path);
		if(!s.load()||s.entries.empty()){ printf("%s(): unabled to load %s",__func__,file_path.wtoa()); return dst_path; }
		if(s.entries.size()!=1){ printf("%s(): have only a single file for mesh in %s\n",__func__,file_path.wtoa() ); return dst_path; }
		dst_path = cache::get_dir()+s.entries.front().name;
		if(dst_path.exists()) dst_path.delete_file();
		if(!s.extract_to_files(dst_path.dir())) return path();
	}
#endif
	return dst_path;
}

//*************************************
inline mesh* load_obj( path file_path, float* pLoadingTime=nullptr, void(*flush_messages)(const char*)=nullptr )
{
	os::timer_t t; t.begin();

	mesh* p_mesh = nullptr;
	if(!file_path.exists()){ wprintf(L"%s not exists",file_path.c_str()); return nullptr; }

	//*****************************************************
	// 1. if there is a cache, load from cache
	if(USE_MODEL_CACHE&&(p_mesh=obj::cache::load_mesh(file_path)))
	{
		if(pLoadingTime) *pLoadingTime += float(t.end());
		return p_mesh;
	}

	//*****************************************************
	// 1.1 decompress zip file and remove it later after saving the cache
	path dec_path;
	bool b_use_archive = archive_ext_map.find(file_path.ext().c_str())!=archive_ext_map.end();
	if(b_use_archive)
	{
		wprintf( L"Decompressing %s ...", file_path.name().c_str() );
		auto dt = std::async(obj::decompress,file_path);
		while(std::future_status::ready!=dt.wait_for(std::chrono::milliseconds(10))) if(flush_messages) flush_messages(nullptr);
		dec_path = dt.get();
		if(!dec_path.empty()) wprintf( L" completed in %.0f ms\n", t.end() );
		else{ wprintf( L" failed in %.0f ms\n", t.end() ); return nullptr; }
	}

	wprintf( L"Loading %s ...", file_path.name().c_str() );
	t.begin();

	//*****************************************************
	// 2. open file
	path target_file_path = dec_path.exists()?dec_path:file_path;
	size_t target_count = max(1ull<<16,size_t(target_file_path.file_size()*0.009)); // estimate vertex/face count
	size_t flush_count = target_count/200;
	FILE* fp = _wfopen( target_file_path, L"rb" ); if(fp==nullptr){ wprintf(L"Unable to open %s", file_path.c_str()); return nullptr; }
	setvbuf( fp, nullptr, _IOFBF, 1<<26 ); // set the buffer size of iobuf to 64M

	p_mesh = new mesh();
	wcscpy(p_mesh->file_path,file_path);

	//*****************************************************
	// 4. current data setup
	object*		o=nullptr;		// current object
	geometry*	g=nullptr;		// current geometry
	uint		mat_index=-1;	// current material

	//*****************************************************
	// 5. temporary buffers/variables
	auto& pos = obj::pos;						pos.reserve(target_count/2);	pos.resize(1); pos.front() = vec3(0.0f,0.0f,0.0f);
	auto& tex = obj::tex;						tex.reserve(target_count/8);	tex.resize(1); tex.front() = vec2(0.0f,0.0f);
	auto& nrm = obj::nrm;						nrm.reserve(target_count/2);	nrm.resize(1); nrm.front() = vec3(0.0f,0.0f,0.0f);
	auto* vtx = obj::vtx=new obj::vtx_map_t();	// do not use vtx->reserve(target_count); from some point of VC update, this makes very slow loading
	auto& vertices = p_mesh->vertices;			vertices.reserve(target_count);		obj::vertices=&p_mesh->vertices;
	auto& indices = p_mesh->indices;			indices.reserve(target_count*3);	// index should be 3 times
	auto& geometries = p_mesh->geometries;		geometries.reserve(1<<16);

	p_mesh->materials.reserve(1<<8);
	p_mesh->objects.reserve(1<<12);

	//*****************************************************
	auto create_geometry = [&]()->geometry*
	{
		if(!o) o=p_mesh->create_object("default_object");
		geometry g1(p_mesh,uint(geometries.size()),o->ID,uint(indices.size()),0,nullptr,mat_index);
		geometries.emplace_back(g1); // do not add to the object::geometries, since empty geometries will be trimmed later
		return &geometries.back();
	};

	//*****************************************************
	// string buffers
	char *buff;

	//*****************************************************
	// start parsing
#ifdef __FGETS_SSE2_H__
	ctx_t* fgets_sse2_ctx = init_fgets_sse2( 4096 );
	next_t* next;
	for( uint k=1; next=fgets_sse2(fgets_sse2_ctx,fp); k++ )
	{
		int len=next->len;
		char* key = fgets_sse2_ctx->buf+next->off; key[len]='\0';
		if(len<2||key[0]=='#') continue;

		//*****************************************************
		// preprocessor
		obj::rm_newline(key,len);
		if(key[len-1]=='\\') // concatenate double lines
		{
			char buff2[4096];memcpy(buff2,key,len); buff2[len-1]=' '; key=buff2;
			next = fgets_sse2(fgets_sse2_ctx,fp);
			memcpy(key+len,fgets_sse2_ctx->buf+next->off,next->len);
			key[len+=next->len] = '\0';
			obj::rm_newline(key,len);
		}
#else
	char key[4096];
	for( int k=1; fgets(key,sizeof(key),fp); k++ )
	{
		int len=int(strlen(key));
		if(len<2||key[0]=='#') continue;

		//*****************************************************
		// preprocessor
		obj::rm_newline(key,len);
		if(key[len-1]=='\\') // concatenate double lines
		{
			key[len-1]=' ';
			char buff2[4096];strcat_s(key,sizeof(key),fgets(buff2,sizeof(buff2),fp));
			key[len+=int(strlen(buff2))] = '\0';
			obj::rm_newline(key,len);
		}
#endif

		//*****************************************************
		// tokenize key and next buff
		char key0=*key;
		if(key0!='v'&&key0!='f')
		{
			obj::rtrim(key,len); // rtrim only for rare attributes
			if(len==0) continue; // early exit
		}
		obj::tab_to_space(key,len);
		buff=obj::next_token(key+1);

		//*****************************************************
		if(key0=='v')
		{
			float x=float(fast::atof(buff));
			float y=float(fast::atof(buff=obj::next_token(buff)));

			if(key[1]==0)			pos.emplace_back(x,y,float(fast::atof(obj::next_token(buff))));
			else if(key[1]=='n')	nrm.emplace_back(x,y,float(fast::atof(obj::next_token(buff))));
			else if(key[1]=='t') 	tex.emplace_back(x,y);
			else if(key[1]=='p'){} // vp: parameter space vertices
		}
		else if(key0=='f')
		{
			// counter-clockwise faces
			uint i0 = get_or_create_vertex(buff);
			uint i1 = get_or_create_vertex(buff=obj::next_token(buff));
			uint i2 = get_or_create_vertex(buff=obj::next_token(buff));
			indices.push_back(i0); indices.push_back(i1); indices.push_back(i2);

			// create default object/geometry if no geometry is given now
			if(g==nullptr) g = create_geometry();
			g->count += 3;

			buff=obj::next_token(buff);
			if(*buff&&*buff!=' ')
			{
				uint i3 = get_or_create_vertex(buff);
				indices.push_back(i0); indices.push_back(i2); indices.push_back(i3);
				g->count += 3;
			}
		}
		else if(strcmp(key,"g")==0||strcmp(key,"mg")==0||strcmp(key,"o")==0)	// group, merging group, object name
		{
			o = p_mesh->create_object(trim(buff));
			g = create_geometry(); // always create a new geometry; do not reuse previous emptry geometry for safety

			if(flush_messages) flush_messages( format("Loading %s (%dK faces) ...", o?o->name:"", int(indices.size()/3000) ) );
		}
		else if(strcmp(key,"usemtl")==0)
		{
			mat_index = find_mesh_material(p_mesh,trim(buff));
			g = create_geometry();
		}
		else if(strcmp(key,"mtllib")==0)
		{
			path mtl_file_path = path(file_path).dir() + atow(trim(buff));
			if(!load_mtl(mtl_file_path, p_mesh->materials )){ printf("unable to load material file: %s\n",wtoa(mtl_file_path)); return nullptr; }
			wcscpy( p_mesh->mtl_path, mtl_file_path.name(true) );

			// default material
			if(!p_mesh->materials.empty()) mat_index = p_mesh->materials.size()>1?1:0;
		}
		else if(strcmp(key,"cstype")==0);		// curve or surface type
		else if(strcmp(key,"deg")==0);			// skip degree
		else if(strcmp(key,"bmat")==0);			// basis matrix
		else if(strcmp(key,"step")==0);			// step size
		else if(strcmp(key,"p")==0);			// skip point elements
		else if(strcmp(key,"l")==0);			// skip line elements
		else if(strcmp(key,"curv")==0);			// skip curve elements
		else if(strcmp(key,"curv2")==0);		// skip 2D curve elements
		else if(strcmp(key,"surf")==0);			// skip surface elements
		else if(strcmp(key,"s")==0);			// skip smoothing
		else if(strcmp(key,"con")==0);			// skip connectivity
		else if(strcmp(key,"parm")==0);			// Free-form curve/surface body statements
		else if(strcmp(key,"trim")==0);			// Free-form curve/surface body statements
		else if(strcmp(key,"trim")==0);			// Free-form curve/surface body statements
		else if(strcmp(key,"hole")==0);			// Free-form curve/surface body statements
		else if(strcmp(key,"scrv")==0);			// Free-form curve/surface body statements
		else if(strcmp(key,"sp")==0);			// Free-form curve/surface body statements
		else if(strcmp(key,"end")==0);			// Free-form curve/surface body statements
		else if(strcmp(key,"bevel")==0);		// bevel interpolation
		else if(strcmp(key,"c_interp")==0);		// color interpolation
		else if(strcmp(key,"d_interp")==0);		// dissolve interpolation
		else if(strcmp(key,"lod")==0);			// level of detail
		else if(strcmp(key,"shadow_obj")==0);	// shadow casting
		else if(strcmp(key,"trace_obj")==0);	// ray tracing
		else if(strcmp(key,"ctech")==0);		// curve approximation technique
		else if(strcmp(key,"stech")==0);		// surface approximation technique
		else { printf( "\n[OBJ] Unrecognized <%s> found at line %d.\n", key, k ); fclose(fp); return nullptr; }

		//*****************************************************
		// flush window messages
		if(k%(flush_count*50)==0){ printf("."); if(flush_messages) flush_messages(nullptr); }
	}
	fclose(fp);

	// trim empty geometries and update geometry ID
	uint gid=0; auto& mg=p_mesh->geometries;
	for(auto it=mg.begin();it!=mg.end();){ if(it->count==0) it=mg.erase(it); else{ it->ID=gid++; it++; } }

	// update child geometries once again after removing empty geometries
	for( auto& obj : p_mesh->objects )	obj.children.clear();
	for( auto& g : p_mesh->geometries )	p_mesh->objects[g.object_index].children.push_back(g.ID);

	// flush the counts
	if(flush_messages) flush_messages( format("%s/%s objects/geometries loaded in %.0fms\n", itoasep(int(p_mesh->objects.size())), itoasep(int(p_mesh->geometries.size())), t.end()) );

	// clear all temporary large-memory buffers
	obj::pos.clear();	obj::pos.shrink_to_fit();
	obj::nrm.clear();	obj::nrm.shrink_to_fit();
	obj::tex.clear();	obj::tex.shrink_to_fit();
	delete obj::vtx;	obj::vtx=nullptr;			// since unordered_map does not release memory

	// tightly fit the arrays
	p_mesh->shrink_to_fit();

	// update bounding box
	p_mesh->update_bound(true);

	// scale vertices by 1000 times for meter-unit scenes
	if(p_mesh->box.radius()<100.0f)
	{
		vec3 center = p_mesh->box.center();
		for( auto& v : p_mesh->vertices ) v.pos = (v.pos-center)*1000.0f+center;
		for( auto& g : p_mesh->geometries ){ g.box.m=(g.box.m-center)*1000.0f+center;g.box.M=(g.box.M-center)*1000.0f+center; }
		p_mesh->update_bound(false);
	}

	if(flush_messages) flush_messages(nullptr);

	// if no materials are present, then create default materials
	if(p_mesh->materials.empty()) // at least, it's one due to light source material
	{
		create_default_material( p_mesh->materials );
		for( auto& g: p_mesh->geometries ) g.material_index = 0;
	}

	// force light-source material to emissive
	for( auto& g : p_mesh->geometries )	if(_stricmp(g.name(),"light")==0||_stricmp(g.name(),"lights")==0) p_mesh->materials[g.material_index].emissive = 1.0f; // force to light source material
	for( auto& m : p_mesh->materials )	if(_stricmp(m.name,"light")==0||_stricmp(m.name,"lights")==0) m.emissive = 1.0f; // force to light source material

	// save content to cache for faster loading at next time
	if(flush_messages) flush_messages(nullptr);
	obj::cache::save_mesh( p_mesh );

	// remove decompressed file
	if(b_use_archive&&dec_path.exists()) dec_path.delete_file();

	// logging
	t.end();
	if(pLoadingTime) *pLoadingTime += (float) t.delta();
	printf( " completed in %.2f ms (%s faces, %s vertices)\n", t.delta(),
		itoasep(int(p_mesh->face_count())), itoasep(int(p_mesh->vertices.size())) );

	return p_mesh;
}
