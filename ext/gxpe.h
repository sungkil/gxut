//*******************************************************************
// Copyright 2011-2040 Sungkil Lee
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//*******************************************************************

#pragma once
#ifndef __GX_PE_H__
#define __GX_PE_H__

#if __has_include("../gxut.h")
	#include "../gxut.h"
#elif __has_include("gxut.h")
	#include "gxut.h"
#endif

#pragma warning( disable: 4091 ) // disable warning for typedef without variable
#include "ImageHlp.h"
#pragma warning( default: 4091 ) // disable warning for typedef without variable
#pragma comment( lib, "Imagehlp" )

// DLL dependencies in pedump code
// http://stackoverflow.com/questions/597260/how-to-determine-a-windows-executables-dll-dependencies-programatically

//*************************************
namespace pe {
//*************************************
struct image_t
{
	ushort subsystem = IMAGE_SUBSYSTEM_UNKNOWN;
	vector<string> depends;

	operator bool () const { return subsystem!=IMAGE_SUBSYSTEM_UNKNOWN; }
	bool is_subsystem_cui() const { return subsystem==IMAGE_SUBSYSTEM_WINDOWS_CUI; }
	bool is_subsystem_gui() const { return subsystem==IMAGE_SUBSYSTEM_WINDOWS_GUI; }
};

__noinline const char* __rva_name( DWORD rva, PIMAGE_NT_HEADERS pNTHeader, PBYTE imageBase )
{
	PIMAGE_SECTION_HEADER section=nullptr, s=IMAGE_FIRST_SECTION(pNTHeader);
	for( uint k=0, kn=pNTHeader->FileHeader.NumberOfSections; k<kn; k++, s++ )
	{
		DWORD size = s->Misc.VirtualSize; if(size==0) size=s->SizeOfRawData; // This 3 line idiocy is because Watcom's linker actually sets the Misc.VirtualSize field to 0.  (!!! - Retards....!!!)
		if((rva>=s->VirtualAddress)&&(rva<(s->VirtualAddress+size))){ section=s; break; }
	}
    if(!section) return nullptr;
    return (const char*)(imageBase+rva-int(section->VirtualAddress-section->PointerToRawData));
}

__noinline image_t load_image( const char* file_path )
{
	image_t i;
	if(!file_path||!*file_path||access(file_path,0)!=0) return i;

	PLOADED_IMAGE ptr = ImageLoad( file_path, nullptr); if(!ptr){ /*printf( "Unable to load PE image %s\n", file_path );*/ return i; }
	auto* file_header = ptr->FileHeader; if(!file_header){ printf( "%s.FileHeader==nullptr\n", file_path ); ImageUnload(ptr); return i; }
	auto optional_header = file_header->OptionalHeader; if(optional_header.NumberOfRvaAndSizes<2){ ImageUnload(ptr); return i; }
	i.subsystem = optional_header.Subsystem;

	// load dependency
	static const nocase::set<string> system_dll_set = { "kernel32.dll", "shell32.dll", "user32.dll", "gdi32.dll", /* d3d10.dll, D3DCOMPILER_47.dll, VCOMP140.dll(OpenMP), vulkan-1.dll, OpenCL.dll */ }; // other common but need-to-check dlls
	vector<string> v; PIMAGE_IMPORT_DESCRIPTOR importDesc= (PIMAGE_IMPORT_DESCRIPTOR) __rva_name(optional_header.DataDirectory[1].VirtualAddress,file_header,ptr->MappedAddress);
	for( ; importDesc->TimeDateStamp||importDesc->Name; importDesc++ )
	{
		const char* name = __rva_name( importDesc->Name, file_header, ptr->MappedAddress );
		if(system_dll_set.find(name)!=system_dll_set.end()) continue; // bypass system dlls
		i.depends.emplace_back(name);
	}

	ImageUnload(ptr);
	return i;
}

//*************************************
} // end namespace pe
//*************************************

#endif // __GX_PE_H__
