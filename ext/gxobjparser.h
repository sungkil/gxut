#pragma once
// include gxut.h without other headers
#ifndef __GXUT_H__
#pragma push_macro("__GXUT_EXCLUDE_HEADERS__")
#define __GXUT_EXCLUDE_HEADERS__
#include <gxut/gxut.h>
#pragma pop_macro("__GXUT_EXCLUDE_HEADERS__")
#endif
#include <gxut/gxstring.h>
#include <shellapi.h>
#include <gxut/gxfilesystem.h>
#include <gxut/gxmesh.h>
#include <gxut/gxtimer.h>

static const char* __GX_OBJPARSER_H_TIMESTAMP__ = __TIMESTAMP__;

namespace obj
{
	// mesh loader (with material)
	mesh* load( path file_path, float* pLoadingTime=nullptr, void(*flush_messages)(const char*)=nullptr );
	bool update_object_lights( mesh* p_mesh );

	// cache forward declaration
	namespace cache
	{
		inline uint get_parser_id( path file_path );
		inline path& get_dir(){ static path d = apptemp()+"mesh\\"; if(!d.exists()&&!d.mkdir()) printf("Unable to create %s\n",d.c_str()); return d; }
		inline path get_path( path file_path ){ return get_dir()+file_path.name()+".rxb"; }
	}
}

namespace mtl
{
	void	create_default( vector<material_impl>& materials );
	bool	load( path file_path, vector<material_impl>& materials, bool with_cache=false );
	uint	find( vector<material_impl>& materials, const char* name );
}