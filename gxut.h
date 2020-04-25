//*********************************************************
// Copyright 2011-2020 Sungkil Lee
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
#ifndef __GXUT_H__
#define __GXUT_H__

// includes only essential header files
#if defined(__has_include)
	#if __has_include("gxmath.h")
		#include "gxmath.h"
	#endif
	#if __has_include("gxfilesystem.h")
		#include "gxfilesystem.h"
	#endif
	#if __has_include("gxos.h")
		#include "gxos.h"
	#endif
	#if __has_include("gxstring.h")
		#include "gxstring.h"
	#endif
	#if __has_include("gxmemory.h")
		#include "gxmemory.h"
	#endif
#endif

//*************************************
#endif // __GXUT_H__
