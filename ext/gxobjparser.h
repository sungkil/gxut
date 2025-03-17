#pragma once

#if __has_include(<gxut/gxfilesystem.h>) && __has_include(<gxut/gxmesh.h>)
	#include <gxut/gxfilesystem.h>
	#include <gxut/gxmesh.h>
#endif

static const char* __GX_OBJPARSER_H_TIMESTAMP__ = __TIMESTAMP__;

namespace obj
{
	// mesh loader (with material)
	mesh* load( path file_path, float* pLoadingTime=nullptr, void(*flush_messages)(const char*)=nullptr );
	bool update_object_lights( mesh* p_mesh );
}

namespace mtl
{
	void create_default_material( vector<material_impl>& materials );
	bool load_mtl( path file_path, vector<material_impl>& materials, bool with_cache=false );
	uint find_material( vector<material_impl>& materials, const char* name );
}

// cache forward declaration
namespace obj::cache
{
	inline uint get_parser_id( path file_path );
	inline path& get_dir(){ static path d = apptemp()+"mesh\\"; if(!d.exists()&&!d.mkdir()) printf("Unable to create %s\n",d.c_str()); return d; }
	inline path get_path( path file_path ){ return get_dir()+file_path.name()+".rxb"; }
}