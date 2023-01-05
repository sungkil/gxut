#include <gxut/gxmath.h>
#include <gxut/gxstring.h>
#include <shellapi.h>
#include <gxut/gxfilesystem.h>
#include <gxut/gxos.h>
#include <gxut/ext/gxzip.h>
#include <gxut/gxobjparser.h>
#include <future> // std::async
#if defined(__has_include) && __has_include(<fgets_sse2/fgets_sse2_slee.h>)
	#include <fgets_sse2/fgets_sse2_slee.h>
#endif

// config and timestamps
static const bool USE_MODEL_CACHE = true;
const char* __GX_OBJPARSER_CPP_TIMESTAMP__ = __TIMESTAMP__;
extern const char* __GX_MTLPARSER_CPP_TIMESTAMP__;

//*************************************
// support data and routines for fast string conversion
namespace obj {
//*************************************

static const std::set<std::wstring>& get_archive_extensions()
{
	static std::set<std::wstring> m; if(!m.empty()) return m;
#if defined(__7Z_H) && defined(__7Z_MEMINSTREAM_H)
	m.insert(L"7z");
#endif
#if defined(_unzip_H)||defined(__GXZIP_H__)
	m.insert(L"zip");
#endif
	return m;
}

// vertex hash key using position ID; significantly faster than bitwise hash
struct vertex_hash {size_t operator()(const ivec3& v)const{ return reinterpret_cast<const size_t&>(v.x); }};
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

__forceinline int rm_newline( char* str, int len ){ if(str[len-1]=='\n')str[--len]='\0'; if(str[len-1]=='\r')str[--len]='\0'; return len; } // CR never follows NL
__forceinline char* next_token( char* buff ){while(*buff!=' '&&*buff)buff++;if(!*buff)return nullptr;*(buff++)=0;while(*buff==' ')buff++;return buff; }
__forceinline char* tab_to_space( char* buff, int len ){ for(int j=0;j<len;j++,buff++)if(*buff=='\t')*buff=' '; return buff; }
__forceinline char* rtrim( char* buff, int& len ){ int j;for(j=len-1;j>=0;j--)if(buff[j]!=' ')break;buff[len=j+1]='\0';return buff; }
__forceinline char tolower( char c ){ uchar d=static_cast<uchar>(c-'A');return d<26?'a'+d:c;}
__forceinline path decompress( const path& file_path );
__forceinline bool is_extension_supported( path file_path )
{
	if(_wcsicmp(file_path.ext(),L"obj")==0) return true;
	auto am = get_archive_extensions(); if(am.find(file_path.ext().c_str())!=am.end()) return true;
	return false;
}

//*************************************
} // namespace obj
//*************************************

//*************************************
// cache implementation
namespace obj::cache
{
	inline uint64_t get_parser_id( const char* timestamp )
	{
		static const std::string timestamps = 
			std::string(__GX_MESH_H_TIMESTAMP__)+
			std::string(__GX_OBJPARSER_H_TIMESTAMP__)+
			std::string(__GX_OBJPARSER_CPP_TIMESTAMP__)+
			std::string(__GX_MTLPARSER_CPP_TIMESTAMP__);
		return uint64_t(std::hash<std::string>{}(timestamps+timestamp));
	}

	void clear( mesh* p_mesh, bool b_log )
	{
		if(!p_mesh) return;
		path mesh_path=p_mesh->file_path, mtl_path=p_mesh->mtl_path; if(mesh_path.empty()||!mesh_path.exists()) return;
		path mesh_cache_path = obj::cache::get_path(mesh_path);
		bool r = mesh_cache_path.exists()&&mesh_cache_path.delete_file(false);
		if(r&&b_log) wprintf( L"deleted %s\n", mesh_cache_path.c_str() );

		path bton_path = p_mesh?obj::cache::get_path(mtl_path).remove_ext()+L".bton":path();
		r = bton_path.exists()&&bton_path.delete_file(false);
		if(r&&b_log) wprintf( L"deleted %s\n", bton_path.c_str() );
	}

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
		
		// load materials
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
__forceinline uint get_or_create_vertex( char* str )
{
	ivec3 key;
	key.x=fast::atoi(str); while(*str!='/'&&*str) str++;str++;
	key.y=fast::atoi(str); while(*str!='/'&&*str) str++;str++;
	key.z=fast::atoi(str);

	if(key.x<0) key.x += int(obj::pos.size());
	if(key.y<0) key.y += int(obj::tex.size());
	if(key.z<0) key.z += int(obj::nrm.size());

	auto it=obj::vtx->find(key); if(it!=obj::vtx->end()) return it->second;
	uint vertex_index = uint(obj::vtx->size());
	obj::vtx->emplace( key, vertex_index );

	// fill vertex attributes
	obj::vertices->emplace_back( obj::pos[key.x], obj::nrm[key.z], obj::tex[key.y] );

	return vertex_index;
}

//*************************************
namespace obj {
//*************************************
mesh* load( path file_path, float* pLoadingTime, void(*flush_messages)(const char*) )
{
	os::timer_t t; t.begin();
	mesh* p_mesh = nullptr;	

	if(!is_extension_supported(file_path)){ printf("obj::%s(): unsupported format: %s\n",__func__, file_path.ext().wtoa()); return nullptr; }
	if(!file_path.exists()){ printf("obj::%s(): %s not exists",__func__,file_path.wtoa()); return nullptr; }

	//*********************************
	// 1. if there is a cache, load from cache
	if(USE_MODEL_CACHE&&(p_mesh=obj::cache::load_mesh(file_path)))
	{
		if(pLoadingTime) *pLoadingTime += float(t.end());
		return p_mesh;
	}

	//*********************************
	// 1.1 decompress zip file and remove it later after saving the cache
	path dec_path;
	auto archive_extensions = get_archive_extensions();
	bool b_use_archive = archive_extensions.find(file_path.ext().c_str())!=archive_extensions.end();
	if(b_use_archive)
	{
		if(flush_messages) flush_messages( format( "Decompressing %s ...", file_path.name().wtoa() ) );
		wprintf( L"Decompressing %s ...", file_path.name().c_str() );
		auto dt = std::async(obj::decompress,file_path);
		while(std::future_status::ready!=dt.wait_for(std::chrono::milliseconds(10))) if(flush_messages) flush_messages(nullptr);
		dec_path = dt.get();
		if(!dec_path.empty()) wprintf( L" completed in %.0f ms\n", t.end() );
		else{ wprintf( L" failed in %.0f ms\n", t.end() ); return nullptr; }
	}

	t.begin();
	bool b_log_begin = true;
	auto log_begin = [&](){ if(!b_log_begin) return; b_log_begin=false; wprintf( L"Loading %s ", file_path.name().c_str() ); };

	//*********************************
	// 2. open file
	path target_file_path = dec_path.exists()?dec_path:file_path;
	size_t target_count = max(1ull<<16,size_t(target_file_path.file_size()*0.095)); // estimate vertex/face count
	size_t flush_count = target_count/1000, dot_count = target_count/15;

	FILE* fp = _wfopen( target_file_path, L"rb" ); if(fp==nullptr){ wprintf(L"Unable to open %s", file_path.c_str()); return nullptr; }
	setvbuf( fp, nullptr, _IOFBF, 1<<26 ); // set the buffer size of iobuf to 64M

	p_mesh = new mesh();
	wcscpy(p_mesh->file_path,file_path);

	//*********************************
	// 4. current data setup
	object*		o=nullptr;		// current object
	geometry*	g=nullptr;		// current geometry
	uint		mat_index=-1;	// current material

	//*********************************
	// 5. temporary buffers/variables
	auto& pos = obj::pos;						pos.reserve(target_count/2);	pos.emplace_back(vec3(0.0f,0.0f,0.0f));
	auto& tex = obj::tex;						tex.reserve(target_count/8);	tex.emplace_back(vec2(0.0f,0.0f));
	auto& nrm = obj::nrm;						nrm.reserve(target_count/2);	nrm.emplace_back(vec3(0.0f,0.0f,0.0f));
	auto* vtx = obj::vtx=new obj::vtx_map_t();	// do not use vtx->reserve(target_count); from some point of VC update, this makes very slow loading
	auto& vertices = p_mesh->vertices;			vertices.reserve(target_count*3/4);		obj::vertices=&p_mesh->vertices;
	auto& indices = p_mesh->indices;			indices.reserve(target_count*3);	// index should be 3 times
	auto& geometries = p_mesh->geometries;		geometries.reserve(1<<16);

	p_mesh->materials.reserve(1<<8);
	p_mesh->objects.reserve(1<<12);

	//*********************************
	auto get_or_create_object = [&]( const char* name )->object*
	{
		auto* obj = p_mesh->find_object(name);
		return obj ? obj : p_mesh->create_object(name);
	};

	auto get_or_create_geometry = [&]()->geometry*
	{
		if(!o) o = p_mesh->create_object( "default" );
		if(g&&g->count==0){ g->object_index=o->ID; return g; } // reuse empty existing geometry with the current object
		geometries.emplace_back(geometry{p_mesh,uint(geometries.size()),o->ID,uint(indices.size()),0,nullptr,mat_index});
		return &geometries.back();
	};

	//*********************************
	// string buffers
	char *buff;

	//*********************************
	// start parsing
#ifdef __FGETS_SSE2_H__
	next_t* next;
	ctx_t* ctx = init_fgets_sse2( 4096 );
	char *key, temp[4096];
	for( int k=1; next=fgets_sse2(ctx,fp); k++ )
	{
		key=ctx->buf+next->off; int len=next->len; key[len]='\0';
		if(len<2||key[0]=='#') continue;
		len=obj::rm_newline(key,len); // fgets_sse2 does not fetch newline

		// concatenate double lines
		if(key[len-1]=='\\')
		{
			memcpy(temp,key,size_t(len-1));key=temp;
			key[len-1]=' ';
			next = fgets_sse2(ctx,fp); char* second=ctx->buf+next->off; second[next->len]='\0';
			int sl=obj::rm_newline(second,next->len);
			memcpy(key+len,second,size_t(sl)); key[len+=sl]='\0';
		}
#else
	char key[4096], second[4096];
	for( int k=1; fgets(key,sizeof(key),fp); k++ )
	{
		int len=int(strlen(key));
		if(len<2||key[0]=='#') continue;
		len=obj::rm_newline(key,len); // fgets fetch newline

		// concatenate double lines
		if(key[len-1]=='\\')
		{
			key[len-1]=' ';
			fgets(second,4096,fp);
			int sl=obj::rm_newline(second,int(strlen(second))); // fgets fetch newline
			memcpy(key+len,second,size_t(sl)); key[len+=sl]='\0';
		}
#endif

		//*****************************
		// tokenize key and next buff
		char key0=*key;
		if(key0!='v'&&key0!='f')
		{
			obj::rtrim(key,len); // rtrim only for rare attributes
			if(len==0) continue; // early exit
		}
		obj::tab_to_space(key,len);
		buff=obj::next_token(key+1);

		//*****************************
		if(key0=='v')
		{
			float x=float(fast::atof(buff));
			float y=float(fast::atof(buff=obj::next_token(buff)));
			char k1=key[1];

			if(k1==0)		 pos.emplace_back(x,y,float(fast::atof(obj::next_token(buff))));
			else if(k1=='n') nrm.emplace_back(x,y,float(fast::atof(obj::next_token(buff))));
			else if(k1=='t') tex.emplace_back(x,y);
			//else if(k1=='p'){} // vp: parameter space vertices
		}
		else if(key0=='f')
		{
			// counter-clockwise faces
			uint i0 = get_or_create_vertex(buff);						indices.emplace_back(i0);
			uint i1 = get_or_create_vertex(buff=obj::next_token(buff));	indices.emplace_back(i1);
			uint i2 = get_or_create_vertex(buff=obj::next_token(buff)); indices.emplace_back(i2);

			// create default object/geometry if no geometry is given now
			if(!g) g = get_or_create_geometry();
			g->count += 3;
			
			// process further to read quads
			buff=obj::next_token(buff);
			if(buff&&*buff&&*buff!=' ')
			{
				uint i3 = get_or_create_vertex(buff);
				indices.push_back(i0); indices.push_back(i2); indices.push_back(i3);
				g->count += 3;
			}
		}
		else if(strcmp(key,"o")==0); // ignore object name
		else if(strcmp(key,"g")==0||strcmp(key,"mg")==0) // group, merging group
		{
			o = get_or_create_object(trim(buff));
			g = get_or_create_geometry();
			if(flush_messages) flush_messages( format("Loading %s (%dK faces) ...", o?o->name:"", int(indices.size()/3000) ) );
		}
		else if(strcmp(key,"usemtl")==0)
		{
			g = get_or_create_geometry();
			g->material_index = mat_index = find_material(p_mesh->materials,trim(buff));
		}
		else if(strcmp(key,"mtllib")==0)
		{
			if(!p_mesh->mtl_path[0]) // load material only once
			{
				path mtl_path = path(file_path).dir() + atow(trim(buff));
				if(!load_mtl(mtl_path, p_mesh->materials)){ printf("unable to load material file: %s\n",wtoa(mtl_path)); return nullptr; }
				wcscpy( p_mesh->mtl_path, mtl_path.relative(false,path(file_path).dir()) );
				
				// postprocessing
				if(!p_mesh->materials.empty()) mat_index = p_mesh->materials.size()>1?1:0; // default material
				log_begin(); // logging after loading materials
			}
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
		else { printf( "\nobj::%s(): '%s' at line %d - unrecognized command\n", __func__, key, k ); fclose(fp); return nullptr; }

		// flush window messages
		if(k%dot_count==0) printf(".");
		if(k%flush_count==0){ if(flush_messages) flush_messages(nullptr); }
	}
	fclose(fp);

	// trim empty geometries and update geometry ID
	uint gid=0; auto& mg=p_mesh->geometries;
	int k=0; for( auto it=mg.begin();it!=mg.end(); k++ )
	{
		if(it->count!=0){ it->ID=gid++; it++; continue; }
		printf( "pruning geometry[%d] %s\n", k, it->name() );
		it=mg.erase(it);
	}

	// update child geometries once again after removing empty geometries
	for( auto& obj : p_mesh->objects )	obj.children.clear();
	for( auto& g : p_mesh->geometries )	p_mesh->objects[g.object_index].children.push_back(g.ID);

	// clear all temporary large-memory buffers
	obj::pos.clear();	obj::pos.shrink_to_fit();
	obj::nrm.clear();	obj::nrm.shrink_to_fit();
	obj::tex.clear();	obj::tex.shrink_to_fit();
	delete obj::vtx;	obj::vtx=nullptr;			// since unordered_map does not release memory

	// tightly fit the arrays
	p_mesh->shrink_to_fit();

	// update bounding box
	if(flush_messages) flush_messages( format("Updating bounds ...\n") );
	p_mesh->update_bound(true);

	// flush the counts
	if(flush_messages) flush_messages( format("%s/%s objects/geometries loaded in %.0fms\n", itoasep(int(p_mesh->objects.size())), itoasep(int(p_mesh->geometries.size())), t.end()) );

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
	printf( " completed in %.2f ms\n", t.delta() );
	printf( "faces: %s, vertices: %s\n", itoasep(int(p_mesh->face_count())), itoasep(int(p_mesh->vertices.size())) );

	return p_mesh;
}

//*************************************
} // namespace obj
//*************************************