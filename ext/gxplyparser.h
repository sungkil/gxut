//*********************************************************
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
//*********************************************************

#pragma once
#ifndef __GX_PLYLOADER_H__
#define __GX_PLYLOADER_H__

#if !defined(__GXUT_H__) && __has_include(<gxut/gxut.h>)
	#include <gxut/gxut.h>
#endif
#if __has_include(<gxut/gxmath.h>)
	#include <gxut/gxmath.h>
#endif
#if __has_include(<gxut/gxstring.h>)
	#include <gxut/gxstring.h>
#endif

// timestamp to indicate struct changes in other files
static const char* __GX_PLYLOADER_H_TIMESTAMP__ = strdup(__TIMESTAMP__);

//*************************************
namespace ply {
//*************************************

enum type_t { NONE, CHAR, UCHAR, SHORT, USHORT, INT, UINT, FLOAT, DOUBLE };
struct property_t { uint index; string name; type_t type=NONE; uint offset=0; size_t size() const { if(type==CHAR||type==UCHAR) return 1; else if(type==SHORT||type==USHORT) return 2; else if(type==INT||type==UINT||type==FLOAT) return 4; else if(type==DOUBLE) return 8; return 0; } };
inline type_t atotype( const char* a ){ if(stricmp(a,"char")==0) return CHAR; else if(stricmp(a,"uchar")==0) return UCHAR; else if(stricmp(a,"short")==0) return SHORT; else if(stricmp(a,"ushort")==0) return USHORT; else if(stricmp(a,"int")==0) return INT; else if(stricmp(a,"uint")==0) return UINT; else if(stricmp(a,"float")==0) return FLOAT; else if(stricmp(a,"double")==0) return DOUBLE; return NONE; }

struct ply_t
{
	struct { bool binary=false; float version=0; } format;
	struct { vector<property_t> properties; const void* data=nullptr; size_t count=0, stride=0; size_t size() const { return stride*count; } } vertex;
	struct { size_t count=0, per_face_count=0; vector<uint4> data; } face; // face/index data

	~ply_t(){ if(vertex.data) free((void*)vertex.data); vertex.data=nullptr; }
	uint property_index( const char* name ) const { for( auto& p : vertex.properties ) if(stricmp(p.name.c_str(),name)==0) return p.index; return uint(-1); }
	uint property_offset( const char* name ) const { uint i=property_index(name); return i==-1?-1:vertex.properties[i].offset; }
	template <class T> T& property( size_t vertex_index, uint property_offset ) const { uchar* v=((uchar*)vertex.data)+vertex.stride*vertex_index; return *((T*)(v+property_offset)); }
};

// https://en.m.wikipedia.org/wiki/PLY_(file_format)
__noinline ply_t* load( path ply_path )
{
	struct file_t { FILE* fp=nullptr; ~file_t(){ if(fp) fclose(fp); } } f;
	FILE*& fp=f.fp=fopen( ply_path.c_str(),"rb"); if(fp==nullptr){ printf( "%s(%s) : failed\n", __func__, ply_path.c_str() ); return nullptr; }

	char buff[4096]={};
	if(!fgets(buff, sizeof(buff), fp)) return nullptr;
	if(strcmp(trim(buff),"ply")!= 0){ printf("%s(%s): ply signature not found\n", __func__, ply_path.c_str()); return nullptr; }

	// read header
	bool b_format_binary = false;
	size_t vertex_count=0, face_count=0;
	vector<property_t> properties;
	while(fgets(buff,sizeof(buff),fp))
	{
		auto v = explode(tolower(trim(buff))," \t\r\n");
		auto tag=std::move(v.front());
		if(stricmp(tag.c_str(),"comment")==0) continue; // bypass comments
		if(stricmp(tag.c_str(),"end_header")==0) break;
		if(v.size()<3) continue;
		
		if(tag=="property")
		{
			if(v[1]=="list"){} // actually, useless, but not rigorously implemented
			else
			{
				property_t p;
				p.index = uint(properties.size());
				p.name=v[2];
				p.type=atotype(v[1].c_str());
				p.offset=properties.empty()?0:properties.back().offset+uint(properties.back().size());
				properties.emplace_back(std::move(p));
			}
		}
		else if(tag=="format")
		{
			auto encoding = v[1];
			if(encoding=="ascii") b_format_binary=false;
			else if(encoding=="binary_little_endian") b_format_binary=true;
			else if(encoding=="binary_big_endian"){ printf("%s(%s): binary_big_endian is only supported\n",__func__,ply_path.c_str()); return nullptr; }
			else { printf("%s(%s): unknown format %s\n",__func__,ply_path.c_str(),encoding.c_str()); return nullptr; }
		}
		else if(tag=="element")
		{
			if(v[1]=="vertex")		vertex_count = atoi(v[2].c_str());
			else if(v[1]=="face")	face_count = atoi(v[2].c_str());
			else { printf("%s(%s): unknown element %s\n",__func__,ply_path.c_str(),v[1].c_str()); return nullptr; }
		}
		else { printf("%s(%s): unknown tag %s\n",__func__,ply_path.c_str(),tag.c_str()); return nullptr; }
	}

	// now create ply
	ply_t* ptr = new ply_t();
	ptr->format.binary = b_format_binary;
	ptr->face.count = face_count;
	ptr->vertex.properties = std::move(properties);
	ptr->vertex.count = vertex_count;
	ptr->vertex.stride = ptr->vertex.properties.back().offset+ptr->vertex.properties.back().size();
	ptr->vertex.data = malloc(ptr->vertex.size());
	
	// read binary vertices
	size_t read_count = fread( (void*)ptr->vertex.data, ptr->vertex.stride, ptr->vertex.count, fp ); if(read_count<ptr->vertex.count){ printf("%s(%s): read_count(%zd)<vertex_count(%zd)\n", __func__,ply_path.c_str(),read_count, ptr->vertex.count); return safe_delete(ptr); }

	// read indices
	if(ptr->face.count)
	{
		while(fgets(buff,sizeof(buff),fp))
		{
			auto w=explode(trim(buff)," \t\r\n"); if(w.empty()) continue;
			uint count = atou(w.front().c_str());
			if(w.size()!=count+1){ printf("%s(%s): list indices (%zd) != %d\n", __func__,ply_path.c_str(),w.size()-1,count); return safe_delete(ptr); }
			if(!ptr->face.per_face_count) ptr->face.per_face_count=count;
			uint4 i; for( size_t j=1; j<count; j++ ) i[j-1]=atou(w[j].c_str());
			ptr->face.data.emplace_back(i);
		}
		if(ptr->face.data.size()!=ptr->face.count){ printf( "%s(%s): faces.size(%d)!=face_count(%d)\n",__func__,ply_path.c_str(),int(ptr->face.data.size()),int(ptr->face.count)); return safe_delete(ptr); }
	}
	
	auto cur=_ftelli64(fp); _fseeki64(fp,0,SEEK_END); auto remainder=_ftelli64(fp)-cur;
	if(remainder!=0) printf( "%s(%s): dummy data exists (%zd bytes)\n",__func__,ply_path.c_str(),remainder);

	fclose(fp); fp=nullptr;
	return ptr;
}

//*************************************
} // end namespace ply
//*************************************

#endif // __GX_PLYLOADER_H__