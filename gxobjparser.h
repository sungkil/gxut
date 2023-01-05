#pragma once
#include <gxut/gxfilesystem.h>
#include <gxut/gxmesh.h>

static const char* __GX_OBJPARSER_H_TIMESTAMP__ = __TIMESTAMP__;

namespace obj
{
	// mesh loader (with material)
	mesh* load( path file_path, float* pLoadingTime=nullptr, void(*flush_messages)(const char*)=nullptr );

	// material loader
	void create_default_material( std::vector<material_impl>& materials );
	bool load_mtl( path file_path, std::vector<material_impl>& materials, bool with_cache=false );
	uint find_material( std::vector<material_impl>& materials, const char* name );
}

// cache forward declaration
namespace obj::cache
{
	inline uint64_t get_parser_id( const char* timestamp );
	inline path& get_dir(){ static path d = path::temp(false)+L"global\\mesh\\"; if(!d.exists()&&!d.mkdir()) wprintf(L"Unable to create %s\n",d.c_str()); return d; }
	inline path get_path( path file_path ){ return get_dir()+file_path.name()+L".rxb"; }
}