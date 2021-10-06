#pragma once
#include <gxut/gxfilesystem.h>
#include <gxut/gxmesh.h>

namespace obj
{
	mesh* load( path file_path, float* pLoadingTime=nullptr, void(*flush_messages)(const char*)=nullptr );
}