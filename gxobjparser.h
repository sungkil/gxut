#pragma once
#include <gxut/gxstring.h>
#include <gxut/gxfilesystem.h>
#include <gxut/gxmath.h>
#include <gxut/gxmesh.h>
#include <future>

static const bool USE_MODEL_CACHE = true;
static const float SPECULAR_BETA_SCALE = 100.0f;

#if defined(_unzip_H) && defined(__7Z_H) && defined(__7Z_MEMINSTREAM_H)
	static nocase::set<std::wstring> archive_ext_map = { L"zip", L"7z" };
#elif defined(_unzip_H)
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
__forceinline char* next_token( char* buff ){while(*buff!=' ')buff++; *(buff++)=0;while(*buff==' ')++buff;return buff; }
__forceinline char* tab_to_space( char* buff, int len ){ for(int j=0;j<len;j++,buff++)if(*buff=='\t')*buff=' '; return buff; }
__forceinline char* rtrim( char* buff, int& len ){ int j;for(j=len-1;j>=0;j--)if(buff[j]!=' ')break;buff[len=j+1]='\0';return buff; }
__forceinline char  tolower( char c ){ uchar d=static_cast<uchar>(c-'A');return d<26?'a'+d:c;}
__forceinline path decompress( const path& file_path );

//*************************************
} // namespace obj
//*************************************

inline void clear_absent_textures( std::vector<material_impl>& mats, path mesh_dir )
{
	for( uint k=0; k < mats.size(); k++ )
	{
		auto& m = mats[k];
		if(m.path.diffuse[0]&&!(mesh_dir+m.path.diffuse).exists())	m.path.diffuse[0] = L'\0';
		if(m.path.bump[0]&&!(mesh_dir+m.path.bump).exists())		m.path.bump[0] = L'\0';
		if(m.path.normal[0]&&!(mesh_dir+m.path.normal).exists())	m.path.normal[0] = L'\0';
		if(m.path.cube[0]&&!(mesh_dir+m.path.cube).exists())		m.path.cube[0] = L'\0';
		if(m.path.alpha[0]&&!(mesh_dir+m.path.alpha).exists())		m.path.alpha[0] = L'\0';
	}
}

inline void convert_unsupported_texture_to_jpeg( std::vector<material_impl>& mats, path mesh_dir )
{
	std::vector<wchar_t*> path_list;
	for( uint k=0; k < mats.size(); k++ )
	{
		auto& m = mats[k];
		if(m.path.diffuse[0]&&(mesh_dir+m.path.diffuse).exists())	path_list.emplace_back(m.path.diffuse);
		if(m.path.bump[0]&&(mesh_dir+m.path.bump).exists())			path_list.emplace_back(m.path.bump);
		if(m.path.normal[0]&&(mesh_dir+m.path.normal).exists())		path_list.emplace_back(m.path.normal);
		if(m.path.cube[0]&&(mesh_dir+m.path.cube).exists())			path_list.emplace_back(m.path.cube);
		if(m.path.alpha[0]&&(mesh_dir+m.path.alpha).exists())		path_list.emplace_back(m.path.alpha);
	}

	bool bFirst = true;
	for( uint k=0; k < path_list.size(); k++ )
	{
		wchar_t* src0 = path_list[k];
		path src = mesh_dir+src0; if(!src.exists()) continue;

		if(_wcsicmp(src.ext(),L"TGA")==0)
		{
			image* img = gx::load_image(src);
			path dst = src.dir()+src.name(false)+(img->channels==3?L".jpg":L".png");
			if(!dst.exists())
			{
				gx::save_image( dst, img );
				if(bFirst){ wprintf( L"\n"); bFirst=false; }
				wprintf( L"converting %s to %s\n", src.name().c_str(), dst.name().c_str() );
			}
			gx::release_image(&img);
			wcscpy(src0,dst.name());
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
	image* bump0 = gx::load_image(bump_path,false,false,false);	// do not force rgb to bump
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
	for( int y=0, yn=bump->height; y<yn; y++ )
	{
		uchar* src = (uchar*) (bump0->data+y*bump0->stride());
		float* dst = (float*) (bump->data+y*bump->stride());
		for( int x=0, c=bump0->channels, xn=bump->width; x<xn; x++, src+=c, dst++ ) dst[0] = float(src[0])/255.0f;
	}

	// create normal image
	image* normal = gx::create_image( bump->width, bump->height, 8, 3 );

	// apply sobel filter to the bump
	bump_scale = min(1000.0f,1.0f/(bump_scale+0.000001f));
	uchar *B=bump->data, *N=normal->data;
	int NW=int(normal->stride()), BW=int(bump->stride());
	for( int y=0, yn=bump->height, xn=bump->width; y<yn; y++ )
	{
		uchar3* dst = (uchar3*)(N+y*NW);
		float* src2 = (float*) (B+((y-1+yn)%yn)*BW);
		float* src1 = (float*) (B+y*BW);
		float* src0 = (float*) (B+((y+1)%yn)*BW);

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

inline void generate_normal_maps( std::vector<material_impl>& mats, path mesh_dir )
{
	std::map<path,path> done;
	for( uint k=0; k < mats.size(); k++ )
	{
		path normal_path, bump_path=mesh_dir+mats[k].path.bump;
		if(mats[k].path.bump[0]==L'\0'||!bump_path.exists()) continue;

		auto it=done.find(bump_path.c_str());
		if(it!=done.end()) normal_path=it->second;
		else
		{
			normal_path = generate_normal_map( bump_path, mats[k].bump_scale );
			if(!normal_path.empty()&&normal_path.exists()) done.emplace(bump_path,normal_path);
		}
		if(normal_path.exists()) wcscpy( mats[k].path.normal, normal_path.name() );
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
	strcpy(m->name,"default");
}

inline bool load_mtl( path filePath, std::vector<material_impl>& mats )
{
	// open file
	if(!filePath.exists()){ wprintf(L"%s not exists\n", filePath.c_str() ); return false; }
	FILE* fp = _wfopen( filePath, L"r" );
	if(!fp){ wprintf(L"unable to open %s\n", filePath.c_str()); return false; }

	//*********************************
	// default material for light source
	// mat_index==0 or emissive>0 indicates a light source
	mats.emplace_back(material_impl(0));
	material_impl* m = &mats.back();
	strcpy(m->name,"light");
	m->color = vec4(1,1,1,1);
	m->specular = 0.0f;
	m->emissive = 1.0f;

	//*****************************************************
	// start parsing
	char buff[8192];
	for( uint k=0; fgets(buff,8192,fp); k++ )
	{
		char c0 = buff[0];
		if(c0==0||c0=='#'||strncmp(buff,"Wavefront",9)==0) continue;
		char c1 = buff[1]; if(c1==0) continue;

		std::vector<std::string> vs = explode(_strlwr(buff));
		if(vs.size()<2) continue;
		std::string& key = vs[0];
		std::string& token = vs[1];

		if(key=="newmtl")
		{
			mats.emplace_back(material_impl(uint(mats.size())));
			m = &mats.back();
			strcpy(m->name,token.c_str());
		}
		else if(key=="ka"){} // ignore ambient materials
		else if(key=="kd")
		{
			if(vs.size()<4){ wprintf(L"Kd size < 3\n"); return false; }
			m->color[0] = float(fast::atof(vs[1].c_str()));
			m->color[1] = float(fast::atof(vs[2].c_str()));
			m->color[2] = float(fast::atof(vs[3].c_str()));
		}
		else if(key=="ks") // specular
		{
			if(vs.size()<2){ wprintf(L"Ks size < 2\n"); return false; }
			m->specular = float(fast::atof(vs[1].c_str()));
		}
		else if(key=="ke") // emissive
		{
			m->emissive = 1.0f;
		}
		else if(key=="ns")
		{
			m->beta = float(fast::atof(token.c_str()))*SPECULAR_BETA_SCALE; // 100x scaled
		}
		else if(key=="ni") m->n = (float)fast::atof(token.c_str()); 				// optical index
		else if(key=="illum"){}	//	mat->bSpecular=(token=="2");			// illumination model: ignored
		else if(key=="d") m->color.a = float(fast::atof(token.c_str()));
		else if(key=="map_ka"){} // m->ambient_path=token;
		else if(key=="map_kd"){ if(token!=".") wcscpy(m->path.diffuse,atow(token.c_str())); }
		else if(key=="map_ks"){} // m->specularMap=token;
		else if(key=="map_ke"){} // ignored
		else if(key=="map_ns"){} // m->powerMap=token;
		else if(key=="refl"){ if(token!=".") wcscpy(m->path.cube,atow(token.c_str())); }		// cubemap as a reflection map
		else if(key=="map_d"||key=="map_opacity"){ if(token!=".") wcscpy(m->path.alpha,atow(token.c_str())); }
		else if(key=="disp"){}	// displacement map: ignored
		else if(key=="map_bump"||key=="bump")
		{
			// process OBJ option for bump multiplier
			if(token!=".")
			{
				for( uint j=1; j<vs.size(); j++ )
				{
					if(vs[j][0]!='-') wcscpy(m->path.bump,atow(vs[j].c_str()));	// when it's not an option
					else if(_stricmp("-bm", vs[j].c_str())==0&&vs.size()>=(j+2)){ m->bump_scale = float(fast::atof(vs[j+1].c_str())); j++; }
				}
			}
		}
		else if(key=="tf");		// Transmission filter: ignored
		else if(key=="tr");		// Transparent: ignored
		else if(key=="fr");		// Fresnel reflectance
		else if(key=="ft");		// Fresnel transmittance
		else if(key=="Ia");		// ambient light
		else if(key=="Ir");		// intensity from reflected direction
		else if(key=="It");		// intensity from transmitted direction

		else if(k>0){ printf("\nload_mtl(): '%s %s' is unrecognized mtl command\n",key.c_str(),token.c_str()); return false; }
	}
	fclose(fp);

	// absolute paths to relative paths
	path mtl_dir = filePath.dir();
	for( auto& m : mats)
	{
		std::vector<wchar_t*> v = {m.path.alpha,m.path.bump,m.path.diffuse,m.path.cube,m.path.normal};
		for( auto* p : v ) if(p[0]&&path(p).is_absolute()) wcscpy(p,path(p).relative(mtl_dir).remove_first_dot());
	}

	// postprocessing for materials
	clear_absent_textures( mats, filePath.dir() );
	convert_unsupported_texture_to_jpeg( mats, filePath.dir() );
	generate_normal_maps( mats, filePath.dir() );

	return true;
}

//*************************************
// Cache Implementation

inline path& get_mesh_cache_dir()
{
#ifdef REX_FACTORY_IMPL
	static path mesh_cache_dir = path::global::temp().add_backslash()+L"global\\mesh\\";
#else
	static path mesh_cache_dir = path::temp().add_backslash()+L"global\\mesh\\";
#endif
	if(!mesh_cache_dir.exists()&&!mesh_cache_dir.mkdir()) wprintf(L"Unable to create %s\n",mesh_cache_dir.c_str());
	return mesh_cache_dir;
}

inline path get_mesh_cache_path( path file_path )
{
	return get_mesh_cache_dir()+file_path.name()+L".rxb";
}

// parser's id: to differetiate the parser implementations
inline uint64_t get_mesh_cache_id( const char* timestamp )
{
	return uint64_t(std::hash<std::string>{}(std::string(__TIMESTAMP__)+std::string(__GX_MESH_H_TIMESTAMP__)+timestamp));
}

inline bool mesh_cache_exists( path file_path )
{
	path cache_path = get_mesh_cache_path(file_path); if(!cache_path.exists()) return false;
	FILE* fp=_wfopen(cache_path,L"r"); if(fp==nullptr) false;
	uint64_t pid; fscanf( fp, "%llu\n", &pid ); // get parser id
	fclose(fp);

	// compare parser id (timestamp)
	return pid==get_mesh_cache_id(file_path.mtimestamp());
}

inline void save_mesh_cache( mesh* pMesh )
{
	path file_path = path(pMesh->file_path);
	path cache_path = get_mesh_cache_path(file_path);
	FILE* fp = _wfopen( cache_path, L"w" ); if(fp==nullptr){ wprintf(L"Unable to open %s\n",cache_path.c_str()); return; }

	// 0. save the parser's id to reflect the revision of the parser and mesh's timestamp
	fprintf( fp, "%llu\n", get_mesh_cache_id(file_path.mtimestamp()) );

	// 1. save time stamp of the mesh file and material file
	path mtl_path = file_path.dir()+pMesh->mtl_path;
	fprintf( fp, "mtllib: %s\n", wcslen(pMesh->mtl_path)==0?"default":wtoa(pMesh->mtl_path) );
	fprintf( fp, "%llu\n", get_mesh_cache_id(mtl_path.mtimestamp()) );

	// 2. boundingbox and numface
	fprintf( fp, "mesh_box: %f %f %f %f %f %f\n", pMesh->box.m[0], pMesh->box.m[1], pMesh->box.m[2], pMesh->box.M[0], pMesh->box.M[1], pMesh->box.M[2]);
	// 4. save material list
	fprintf( fp, "material_count: %Iu\n", pMesh->materials.size() );
	for( size_t k=0, kn=pMesh->materials.size(); k<kn; k++ )
	{
		material_impl& m = pMesh->materials[k];

		// make maps string
		std::wstring maps = L"";
		if(m.path.diffuse[0])	maps += format(L";path.diffuse:%s",m.path.diffuse);
		if(m.path.bump[0])		maps += format(L";path.bump:%s",m.path.bump);
		if(m.path.normal[0])	maps += format(L";path.normal:%s",m.path.normal);
		if(m.path.cube[0])		maps += format(L";path.cube:%s",m.path.cube);
		if(m.path.alpha[0])		maps += format(L";path.alpha:%s",m.path.alpha);

		// delete first ;
		if(!maps.empty()) maps = maps.substr(1,maps.length());

		// write
		fprintf( fp, "material[%d] %s %g %g %g %g %g %g %g %g %g %s\n",
						int(k), m.name,
						m.color[0], m.color[1], m.color[2], m.color[3],
						m.specular, m.beta, m.emissive, m.n,
						m.bump_scale,
						wtoa(maps.c_str()) );
	}

	// 5. save object list
	fprintf( fp, "object_count: %zu\n", pMesh->objects.size() );
	for( uint k=0; k < pMesh->objects.size(); k++ )
	{
		object& obj = pMesh->objects[k]; const bbox& b=obj.box;
		fprintf( fp, "object[%d] %s ", k, obj.name );
		fprintf( fp, "%f %f %f %f %f %f", b.m[0], b.m[1], b.m[2], b.M[0], b.M[1], b.M[2] );
		fprintf( fp, "\n" );
	}

	// 6. save geometry list
	fprintf( fp, "geometry_count: %zu\n", pMesh->geometries.size() );
	for( uint k=0; k < pMesh->geometries.size(); k++ )
	{
		auto& g = pMesh->geometries[k]; const bbox& b=g.box;
		fprintf( fp, "geometry[%d] %d %d %d %d ", k, g.object_index, g.material_index, g.first_index, g.count );
		fprintf( fp, "%f %f %f %f %f %f", b.m[0], b.m[1], b.m[2], b.M[0], b.M[1], b.M[2] );
		fprintf( fp, "\n" );
	}

	// 7. save number of vertex and index list
	fprintf( fp, "vertex_count: %Iu, index_count: %Iu\n", pMesh->vertices.size(), pMesh->indices.size() );

	// 7.1 close file and reopen as a binary mode: writing must be done in binary mode !!!!
	fclose(fp); _flushall();
	fp = _wfopen( cache_path, L"ab" );	// attach + binary mode

	// 7.2 save vertex and index list
	fwrite( &pMesh->vertices[0], sizeof(vertex), pMesh->vertices.size(), fp );
	fwrite( &pMesh->indices[0], sizeof(uint), pMesh->indices.size(), fp );

	// 8. close file
	fclose(fp);
}

inline mesh* load_mesh_cache( path file_path )
{
	path cachePath = get_mesh_cache_path(file_path);
	FILE* fp = _wfopen( cachePath, L"rb" );
	if(fp==nullptr){ wprintf(L"Unable to open %s\n",cachePath.c_str()); return nullptr; }

	char buff[8192], mtl_name[1024];

	// 0. get parserid
	fgets(buff,8192,fp); // get parser id

	// 1.1 get the mtl name
	fgets(buff,8192,fp); sscanf( buff, "mtllib: %s\n", mtl_name );
	path mtl_path = path(file_path).dir()+atow(mtl_name);

	// 1.2 get the mtl timestnamp and compare
	fgets(buff,8192,fp);
	const char* mtl_timestamp = mtl_path.exists()&&mtl_path!=L"default"?mtl_path.mtimestamp():"";
	uint64_t mat_cache_id; sscanf(buff, "%llu\n", &mat_cache_id );
	if(mtl_path.exists()&&get_mesh_cache_id(mtl_timestamp)!=mat_cache_id) return nullptr;	// there was a change on the existing mtl file

	//*****************************************************
	// now create mesh
	mesh* p_mesh = new mesh();
	wcscpy(p_mesh->file_path,file_path);
	wcscpy(p_mesh->mtl_path,mtl_path.exists()?atow(mtl_name):L"default");

	// 2. bounding box and numface
	fgets(buff,8192,fp); sscanf(buff,"mesh_box: %f %f %f %f %f %f\n",
		&p_mesh->box.m[0], &p_mesh->box.m[1], &p_mesh->box.m[2], &p_mesh->box.M[0], &p_mesh->box.M[1], &p_mesh->box.M[2] );

	// 4. read material list
	uint material_count=0;	// NEVER USE size_t
	fgets(buff,8192,fp); sscanf(buff,"material_count: %u\n", &material_count );
	if(material_count>65536){ if(fp) fclose(fp); return nullptr; }
	p_mesh->materials.reserve(material_count);

	for( size_t k=0; k < material_count; k++ )
	{
		p_mesh->materials.emplace_back(material_impl(uint(p_mesh->materials.size())));
		material_impl* m = &p_mesh->materials.back();

		// get material attribute
		fgets(buff,8192,fp); sscanf(buff,"material[%*d] %s %f %f %f %f %f %f %f %f %f %s\n",
						m->name,
						&m->color[0], &m->color[1], &m->color[2], &m->color[3],
						&m->specular, &m->beta, &m->emissive, &m->n,
						&m->bump_scale,
						buff );

		// read maps
		std::vector<std::string> vs = explode(buff,";");
		for( size_t j=0; j < vs.size(); j++ )
		{
			if(vs[j].empty()) continue;
			std::vector<std::string> vs2 = explode(vs[j].c_str(),":");
			if(vs2.size()<2) continue;
			const std::string& key = vs2[0];
			std::wstring value = atow(vs2[1].c_str());

			if(key=="path.diffuse")			wcscpy(m->path.diffuse,value.c_str());
			else if(key=="path.bump")		wcscpy(m->path.bump,value.c_str());
			else if(key=="path.normal")		wcscpy(m->path.normal,value.c_str());
			else if(key=="path.cube")		wcscpy(m->path.cube,value.c_str());
			else if(key=="path.alpha")		wcscpy(m->path.alpha,value.c_str());
		}
	}

	// 5. convert unsupported format to jpeg
	convert_unsupported_texture_to_jpeg( p_mesh->materials, file_path );

	// 6. get object list
	uint object_count=0; // never use size_t here for sscanf
	fgets(buff,8192,fp); sscanf(buff,"object_count: %u\n", &object_count );
	if(object_count>(1<<30)){ if(fp) fclose(fp); return nullptr; }

	p_mesh->objects.reserve(object_count);
	for( uint k=0; k < object_count; k++ )
	{
		object* obj = p_mesh->create_object("");
		fgets(buff,8192,fp);
		sscanf(buff,"object[%d] %s ", &obj->ID, obj->name );
		sscanf(buff,"%f %f %f %f %f %f\n", &obj->box.m[0], &obj->box.m[1], &obj->box.m[2], &obj->box.M[0], &obj->box.M[1], &obj->box.M[2] );
	}

	// 7. get geometry list
	uint geometry_count=0;
	fgets(buff,8192,fp); sscanf(buff,"geometry_count: %u\n", &geometry_count );
	if(geometry_count>(1<<30)){ if(fp) fclose(fp); return nullptr; }

	p_mesh->geometries.reserve(geometry_count);
	for( uint k=0; k < geometry_count; k++ )
	{
		geometry g(p_mesh,uint(p_mesh->geometries.size()),-1,0,0,nullptr,-1);
		fgets(buff,8192,fp); sscanf(buff,"geometry[%d] %d %d %d %d %f %f %f %f %f %f\n", &g.ID, &g.object_index, &g.material_index, &g.first_index, &g.count, &g.box.m[0], &g.box.m[1], &g.box.m[2], &g.box.M[0], &g.box.M[1], &g.box.M[2] );
		p_mesh->geometries.emplace_back(g);
		p_mesh->objects[g.object_index].children.push_back(g.ID);
	}

	// 8. load vertex and index list
	uint vertex_count=0, index_count=0;
	fgets(buff,8192,fp); sscanf(buff,"vertex_count: %u, index_count: %u\n", &vertex_count, &index_count );
	p_mesh->vertices.resize(vertex_count);	fread( &p_mesh->vertices[0], sizeof(vertex), vertex_count, fp );
	p_mesh->indices.resize(index_count);	fread( &p_mesh->indices[0], sizeof(uint), index_count, fp );

	// 9. close file
	fclose(fp);
	return p_mesh;
}

//*************************************
__forceinline uint get_or_create_vertex( char* str )
{
	// parse the key for extracting indices
	ivec3 idx;
	idx.x=fast::atoi(str); while(*str!='/')str++;str++;
	idx.y=fast::atoi(str); while(*str!='/')str++;str++;
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
#ifdef _unzip_H
	if(_wcsicmp(file_path.ext(),L"zip")==0)
	{
		zip_t z(file_path);
		if(!z.load()||z.entries.empty()){ wprintf(L"loadOBJ(): unabled to load %s",file_path.c_str()); return dst_path; }
		if(z.entries.size()!=1){ wprintf(L"loadOBJ(): have only a single file for mesh in %s\n", file_path.c_str() ); return dst_path; }
		dst_path = get_mesh_cache_dir()+z.entries.front().name;
		if(dst_path.exists()) dst_path.delete_file();
		if(!z.extract_to_files(dst_path.dir())) return path();
	}
#endif
#if defined(__7Z_H) && defined(__7Z_MEMINSTREAM_H)
	if(_wcsicmp(file_path.ext(),L"7z")==0)
	{
		szip_t s(file_path);
		if(!s.load()||s.entries.empty()){ wprintf(L"loadOBJ(): unabled to load %s",file_path.c_str()); return dst_path; }
		if(s.entries.size()!=1){ wprintf(L"loadOBJ(): have only a single file for mesh in %s\n", file_path.c_str() ); return dst_path; }
		dst_path = get_mesh_cache_dir()+s.entries.front().name;
		if(dst_path.exists()) dst_path.delete_file();
		if(!s.extract_to_files(dst_path.dir())) return path();
	}
#endif
	return dst_path;
}

//*************************************
inline mesh* load_obj( path file_path, float* pLoadingTime=nullptr, void(*flush_messages)(const char*)=nullptr )
{
	gx::timer_t t; t.begin();

	mesh* p_mesh = nullptr;
	if(!file_path.exists()){ wprintf(L"%s not exists",file_path.c_str()); return nullptr; }

	//*****************************************************
	// 1. if there is a cache, load from cache
	if(USE_MODEL_CACHE&&mesh_cache_exists(file_path)&&(p_mesh=load_mesh_cache(file_path))){ if(pLoadingTime) *pLoadingTime += (float)t.end(); return p_mesh; }

	//*****************************************************
	// 1.1 decompress zip file and remove it later after saving the cache
	path dec_path;
	bool use_archive = archive_ext_map.find(file_path.ext().c_str())!=archive_ext_map.end();
	if(use_archive)
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
	auto& pos = obj::pos;						pos.reserve(target_count/2);	pos.resize(1);
	auto& tex = obj::tex;						tex.reserve(target_count/8);	tex.resize(1);
	auto& nrm = obj::nrm;						nrm.reserve(target_count/2);	nrm.resize(1);
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
	for( uint k=1; fgets(key,sizeof(key),fp); k++ )
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
		if(key0!='v'&&key0!='f') // only for rare attributes
		{
			obj::tab_to_space(obj::rtrim(key,len),len); // tab2space and rtrim
			if(len==0) continue; // early exit
		}
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
			indices.push_back(get_or_create_vertex(buff));
			indices.push_back(get_or_create_vertex(buff=obj::next_token(buff)));
			indices.push_back(get_or_create_vertex(obj::next_token(buff)));

			// create default object/geometry if no geometry is given now
			if(g==nullptr) g = create_geometry();
			g->count += 3;
		}
		else if(strcmp(key,"g")==0||strcmp(key,"mg")==0||strcmp(key,"o")==0)	// mtllib, group, merging group, object name
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
	save_mesh_cache( p_mesh );

	// remove decompressed file
	if(use_archive&&dec_path.exists()) dec_path.delete_file();

	// logging
	t.end();
	if(pLoadingTime) *pLoadingTime += (float) t.delta();
	printf( " completed in %.2f ms (%s faces, %s vertices)\n", t.delta(),
		itoasep(int(p_mesh->face_count())), itoasep(int(p_mesh->vertices.size())) );

	return p_mesh;
}
