#pragma once
#ifndef __GX_MATH__
#define __GX_MATH__
//###################################################################
// COMMON HEADERS for GXUT
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
// C standard
#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
// STL
#include <algorithm>
#include <array>
#include <map>
#include <set>
#include <string>
#include <vector>
// C++11
#if (_MSC_VER>=1600/*VS2010*/) || (__cplusplus>199711L)
	#include <chrono>
	#include <functional>
	#include <thread>
	#include <typeindex>
	#include <type_traits>
	#include <unordered_map>
	#include <unordered_set>
	using namespace std::placeholders;
#endif
// windows
#if !defined(__GNUC__)&&(defined(_WIN32)||defined(_WIN64))
	#include <windows.h>
	#include <wchar.h>
	#include <stdint.h>
#elif !defined(__forceinline) // GCC and clang
	#define __forceinline inline __attribute__((__always_inline__))
	#include <inttypes.h> // defines int64_t, uint64_t
#endif
#if defined(__clang__) // clang-specific preprocessor
	#pragma clang diagnostic ignored "-Wmissing-braces"				// ignore excessive warning for initialzer
	#pragma clang diagnostic ignored "-Wdelete-non-virtual-dtor"	// ignore non-virtual destructor
#endif
// common macros
#ifndef __REDIR_WIDE_STDIO
	#define __REDIR_WIDE_STDIO
	inline int __wprintf_r( const wchar_t* fmt,... ){va_list a; va_start(a,fmt);int len=_vscwprintf(fmt,a);static int l=0;static wchar_t* w=nullptr;if(l<len)w=(wchar_t*)realloc(w,((l=len)+1)*sizeof(wchar_t));vswprintf_s(w,len+1,fmt,a); va_end(a);int mblen=WideCharToMultiByte(CP_ACP,0,w,-1,0,0,0,0);static int m=0;static char* b=nullptr;if(m<mblen)b=(char*)realloc(b,((m=mblen)+1)*sizeof(char));WideCharToMultiByte(CP_ACP,0,w,-1,b,mblen,nullptr,nullptr);return fputs(b,stdout);}
	#define wprintf(...)	__wprintf_r(__VA_ARGS__)
#endif
#ifndef SAFE_RELEASE
	#define SAFE_RELEASE(a) {if(a){a->Release();a=nullptr;}}
#endif
#ifndef SAFE_DELETE
	#define SAFE_DELETE(p) {if(p){delete p;p=nullptr;}}
#endif
#ifndef SAFE_FREE
	#define SAFE_FREE(p) {if(p){free(p);p=nullptr;}}
#endif
// user types
#ifndef __TARRAY__
#define __TARRAY__
template <class T> struct _tarray2 { union{ struct { T r, g; }; struct { T x, y; }; }; __forceinline T& operator[]( int i ){ return (&x)[i]; } __forceinline const T& operator[]( int i ) const { return (&x)[i]; } };
template <class T> struct _tarray3 { union{ struct { T r, g, b; }; struct { T x, y, z; }; union{ _tarray2<T> xy; _tarray2<T> rg; struct { T _x; union{ _tarray2<T> yz; _tarray2<T> gb; }; }; }; }; __forceinline T& operator[]( int i ){ return (&x)[i]; } __forceinline const T& operator[]( int i ) const { return (&x)[i]; } };
template <class T> struct _tarray4 { union{ struct { T r, g, b, a; }; struct { T x, y, z, w; }; struct { union{ _tarray2<T> xy; _tarray2<T> rg; }; union{ _tarray2<T> zw; _tarray2<T> ba; }; }; union{ _tarray3<T> xyz; _tarray3<T> rgb; }; struct { T _x; union{ _tarray3<T> yzw; _tarray3<T> gba; _tarray2<T> yz; _tarray2<T> gb; }; }; }; __forceinline T& operator[]( int i ){ return (&x)[i]; } __forceinline const T& operator[]( int i ) const { return (&x)[i]; } };
template <class T> struct _tarray9 { union{ T a[9]; struct {T _11,_12,_13,_21,_22,_23,_31,_32,_33;}; }; __forceinline T& operator[]( int i ){ return a[i]; } __forceinline const T& operator[]( int i ) const { return a[i]; } };
template <class T> struct _tarray16{ union{ T a[16]; struct {T _11,_12,_13,_14,_21,_22,_23,_24,_31,_32,_33,_34,_41,_42,_43,_44;}; }; __forceinline T& operator[]( int i ){ return a[i]; } __forceinline const T& operator[]( int i ) const { return a[i]; } };
#endif
using uint		=  unsigned int;		using uchar		= unsigned char;		using ushort	= unsigned short;
using float2	=  _tarray2<float>;		using float3	= _tarray3<float>;		using float4	= _tarray4<float>;
using double2	=  _tarray2<double>;	using double3	= _tarray3<double>;		using double4	= _tarray4<double>;
using int2		=  _tarray2<int>;		using int3		= _tarray3<int>;		using int4		= _tarray4<int>;
using uint2		=  _tarray2<uint>;		using uint3		= _tarray3<uint>;		using uint4		= _tarray4<uint>;
using short2	=  _tarray2<short>;		using short3	= _tarray3<short>;		using short4	= _tarray4<short>;
using ushort2	=  _tarray2<ushort>;	using ushort3	= _tarray3<ushort>;		using ushort4	= _tarray4<ushort>;
using char2		=  _tarray2<char>;		using char3		= _tarray3<char>;		using char4		= _tarray4<char>;
using uchar2	=  _tarray2<uchar>;		using uchar3	= _tarray3<uchar>;		using uchar4	= _tarray4<uchar>;
using bool2		=  _tarray2<bool>;		using bool3		= _tarray3<bool>;		using bool4		= _tarray4<bool>;
using float9	=  _tarray9<float>;		using float16	= _tarray16<float>;
using double9	=  _tarray9<double>;	using double16	= _tarray16<double>;
// end COMMON HEADERS for GXUT
//###################################################################

// definitions
#if !defined(__GNUC__)&&(defined(_WIN32)||defined(_WIN64))&&!defined(__USE_SWIZZLE__)
	#define __USE_SWIZZLE__
#endif

#ifndef PI
#define PI 3.141592653589793f
#endif
#if !defined(clamp)&&defined(min)
	#define clamp(value,vmin,vmax) max(vmin,min(vmax,value))
#elif !defined(clamp)
	#define clamp(value,vmin,vmax) std::max(vmin,std::min(vmax,value))
#endif
#ifndef isnan
	#define	isnan(x) (x!=x)
#endif

#pragma warning( push )
#pragma warning( disable: 4244 )	// diable double to float conversion

//*******************************************************************
// fast atof: taken from fast_atof.c (nearly 5x faster than crt atof())
__forceinline float __atof( const char *p )
{
	while(*p==' '||*p=='\t') p++; // skip leading white spaces
	bool sign=true; if(*p=='-'){ sign=false; p++; } else if(*p=='+') p++; // get sign
	double value=0.0; for(; *p>='0'&&*p<='9'; p++ ) value=value*10.0+double(*p-'0'); // get digits before decimal point or exponent
	if(*p=='.'){ double pow10=0.1; p++; while(*p>='0'&&*p<='9'){ value+=(*p-'0')*pow10; pow10*=0.1; p++; } } // get digits after decimal point
	if(*p!='e'&&*p!='E') return float(sign?value:-value);

	// Handle exponent
	bool frac=false; p++; if(*p=='-'){ frac=true; p++; } else if(*p=='+') p++; // get sign of exponent
	uint expon; for(expon=0; *p>='0'&&*p<='9'; p++ ) expon=expon*10+(*p-'0'); if(expon>308) expon=308;	// get digits of exponent

	// Calculate scaling factor
	double scale = 1.0;
	while(expon >= 50){ scale *= 1E50; expon -= 50; }
	while(expon >=  8){ scale *= 1E8;  expon -=  8; }
	while(expon >   0){ scale *= 10.0; expon -=  1; }
	value = float(frac?(value/scale):(value*scale));
	return sign?value:-value;
}

//*******************************************************************
// half-precision conversion
// https://github.com/HeliumProject/Helium/blob/master/Math/Float16.h
// https://github.com/HeliumProject/Helium/blob/master/Math/Float16.cpp

struct half { ushort mantissa : 10; ushort exponent : 5; ushort sign : 1; inline operator float() const; };	// IEEE 754-2008 half-precision (16-bit) floating-point storage.
__forceinline float htof( half value ){ struct _float32_t {uint mantissa:23,exponent:8,sign:1;}; _float32_t result;result.sign = value.sign;uint exponent=value.exponent,mantissa=value.mantissa; if(exponent==31){result.exponent=255;result.mantissa=0;} else if(exponent==0&&mantissa==0){result.exponent=0;result.mantissa=0;} else if(exponent==0){uint mantissa_shift=10-static_cast<uint>(log2(float(mantissa)));result.exponent=127-(15-1)-mantissa_shift;result.mantissa=mantissa<<(mantissa_shift+23-10);} else{result.exponent=127-15+exponent;result.mantissa=static_cast<uint>(value.mantissa)<<(23-10);} return reinterpret_cast<float&>(result); }
__forceinline half ftoh( float f ){ struct _float32_t {uint mantissa:23,exponent:8,sign:1;}; _float32_t value=reinterpret_cast<_float32_t&>(f);half result;result.sign=value.sign;uint exponent=value.exponent; if(exponent==255){result.exponent=31;result.mantissa=0;} else if(exponent<127-15+1){uint mantissa=(1<<23)|value.mantissa;size_t mantissa_shift=(23-10)+(127-15+1)-exponent;result.exponent=0;result.mantissa=static_cast<ushort>(mantissa>>mantissa_shift);} else if(exponent>127+(31-15-1)){result.exponent=31-1;result.mantissa=1023;} else {result.exponent=static_cast<ushort>(127-15+exponent);result.mantissa=static_cast<ushort>(value.mantissa>>(23-10));} return result; }
inline half::operator float() const { return htof(*this); }
__forceinline half* float2half( const float* pf, half* ph, size_t nElements, size_t half_stride=1 ){ if(pf==nullptr||ph==nullptr||nElements==0) return ph; half* ph0=ph; for( size_t k=0; k < nElements; k++, pf++, ph+=half_stride ) *ph=ftoh(*pf); return ph0; }
__forceinline float* half2float( const half* ph, float* pf, size_t nElements, size_t float_stride=1 ){ if(pf==nullptr||ph==nullptr||nElements==0) return pf; float* pf0=pf; for( size_t k=0; k < nElements; k++, ph++, pf+=float_stride ) *pf=htof(*ph); return pf0; }

//*******************************************************************
// template constants
template <class T> struct precision { static const T value(){ return std::numeric_limits<T>::epsilon()*20; } };	// need to be 20x for robust practical test

//*******************************************************************
template <class T>
struct _tvec2
{
	union
	{
		struct { T r, g; };
		struct { T x, y; };
#ifdef __USE_SWIZZLE__
		_tarray2<T> xy, rg;
#endif
	};

	using value_type = T;
	static const int N=2;

	// constructors
	__forceinline _tvec2(){}
	__forceinline _tvec2( T a ):x(a),y(a){}
	__forceinline _tvec2( T a, T b ):x(a),y(b){}
	__forceinline _tvec2( const _tarray2<T>& v ):x(v[0]),y(v[1]){}
	
	// set
	__forceinline _tvec2& set( T a ){ x=y=a; return *this; }
	__forceinline _tvec2& set( T a, T b ){ x=a;y=b; return *this; }
	__forceinline _tvec2& set( const _tvec2& v ){ x=v.x;y=v.y; return *this; }
	__forceinline _tvec2& set( const _tarray2<T>& v ){ x=v[0];y=v[1]; return *this; }

	// assignment operators
	__forceinline _tvec2& operator=( T a ){ return set(a); }
	__forceinline _tvec2& operator=( const _tarray2<T>& v ){ return set(v); }

	// comparison operators
	__forceinline bool operator!=(const _tvec2& v) const { return !operator==(v); }
	__forceinline bool operator==(const _tvec2& v) const;
	
	// casting operators
	__forceinline operator T*(){ return &x; }
	__forceinline operator const T*() const { return &x; }
	__forceinline operator _tarray2<T>&(){ return reinterpret_cast<_tarray2<T>&>(*this); }
	__forceinline operator const _tarray2<T>&() const { return reinterpret_cast<_tarray2<T>&>(*this); }

	// array access operators
	__forceinline T& operator[]( int i ){ return (&x)[i]; }
	__forceinline const T& operator[]( int i ) const { return (&x)[i]; }

	// unary operators
	__forceinline _tvec2& operator+(){ return *this; }
	__forceinline const _tvec2& operator+() const { return *this; }
	__forceinline _tvec2 operator-() const;

	// binary operators
	__forceinline _tvec2 operator+( T a ) const { return _tvec2(x+a, y+a); }
    __forceinline _tvec2 operator-( T a ) const { return _tvec2(x-a, y-a); }
    __forceinline _tvec2 operator*( T a ) const { return _tvec2(x*a, y*a); }
    __forceinline _tvec2 operator/( T a ) const { return _tvec2(x/a, y/a); }
	__forceinline _tvec2 operator+(const _tvec2& v) const { return _tvec2(x+v.x, y+v.y); }
    __forceinline _tvec2 operator-(const _tvec2& v) const { return _tvec2(x-v.x, y-v.y); }
    __forceinline _tvec2 operator*(const _tvec2& v) const { return _tvec2(x*v.x, y*v.y); }
    __forceinline _tvec2 operator/(const _tvec2& v) const { return _tvec2(x/v.x, y/v.y); }

	// compound assignment operators
	__forceinline _tvec2& operator+=( T a ) { x+=a; y+=a; return *this; }
	__forceinline _tvec2& operator-=( T a ) { x-=a; y-=a; return *this; }
	__forceinline _tvec2& operator*=( T a ) { x*=a; y*=a; return *this; }
	__forceinline _tvec2& operator/=( T a ) { x/=a; y/=a; return *this; }
	__forceinline _tvec2& operator+=(const _tvec2& v) { x+=v.x; y+=v.y; return *this; }
	__forceinline _tvec2& operator-=(const _tvec2& v) { x-=v.x; y-=v.y; return *this; }
	__forceinline _tvec2& operator*=(const _tvec2& v) { x*=v.x; y*=v.y; return *this; }
	__forceinline _tvec2& operator/=(const _tvec2& v) { x/=v.x; y/=v.y; return *this; }

	// norm/length/dot: floating-point only functions
	template <typename=std::enable_if<std::is_floating_point<T>::value,T>>	__forceinline T length2() const { return T(x*x+y*y); }
	template <typename=std::enable_if<std::is_floating_point<T>::value,T>>	__forceinline T norm2() const { return T(x*x+y*y); }
	template <typename=std::enable_if<std::is_floating_point<T>::value,T>>	__forceinline T length() const { return T(sqrt(x*x+y*y)); }
	template <typename=std::enable_if<std::is_floating_point<T>::value,T>>	__forceinline T norm() const { return T(sqrt(x*x+y*y)); }
	template <typename=std::enable_if<std::is_floating_point<T>::value,T>>	__forceinline _tvec2 normalize() const { return operator/(length()); }
	template <typename=std::enable_if<std::is_floating_point<T>::value,T>>	__forceinline T dot( const _tvec2& v ) const { return x*v.x+y*v.y; }
};

// member specialization
template<> __forceinline bool _tvec2<float>::operator==(const _tvec2& v) const { static const float p=precision<float>::value(); return std::abs(x-v.x)<=p&&std::abs(y-v.y)<=p; }
template<> __forceinline bool _tvec2<double>::operator==(const _tvec2& v) const { static const double p=precision<double>::value(); return std::abs(x-v.x)<=p&&std::abs(y-v.y)<=p; }
template<> __forceinline bool _tvec2<int>::operator==(const _tvec2& v) const { return x==v.x&&y==v.y; }
template<> __forceinline bool _tvec2<uint>::operator==(const _tvec2& v) const { return x==v.x&&y==v.y; }
template<> __forceinline bool _tvec2<bool>::operator==(const _tvec2& v) const { return x==v.x&&y==v.y; }
template<> __forceinline _tvec2<float> _tvec2<float>::operator-() const { return _tvec2(-x,-y); }
template<> __forceinline _tvec2<double> _tvec2<double>::operator-() const { return _tvec2(-x,-y); }
template<> __forceinline _tvec2<int> _tvec2<int>::operator-() const { return _tvec2(-x,-y); }

template <class T> struct _tvec3
{
	union
	{
		struct { T x, y, z; };
		struct { T r, g, b; };
#ifdef __USE_SWIZZLE__
		struct { _tvec2<T> xy; };
		struct { _tvec2<T> rg; };
		struct { T _x; _tvec2<T> yz; };
		struct { T _r; _tvec2<T> gb; };
		_tarray3<T> xyz, rgb;
#endif
	};

	using value_type = T;
	static const int N=3;

	// constructors
	__forceinline _tvec3(){}
	__forceinline _tvec3( T a ):x(a),y(a),z(a){}
	__forceinline _tvec3( T a, T b, T c ):x(a),y(b),z(c){}
	__forceinline _tvec3( const _tvec2<T>& v, T c ):x(v.x),y(v.y),z(c){}
	__forceinline _tvec3( T a, const _tvec2<T>& v ):x(a),y(v.x),z(v.y){}
	__forceinline _tvec3( const _tarray3<T>& v ):x(v[0]),y(v[1]),z(v[2]){}

	// set
	__forceinline _tvec3& set( T a ){ x=y=z=a; return *this; }
	__forceinline _tvec3& set( T a, T b, T c ){ x=a;y=b;z=c; return *this; }
	__forceinline _tvec3& set( const _tvec3& v ){ x=v.x;y=v.y;z=v.z; return *this; }
	__forceinline _tvec3& set( const _tarray3<T>& v ){ x=v[0];y=v[1];z=v[2]; return *this; }
	__forceinline _tvec3& set( const _tvec2<T>& v, T c ){ x=v.x;y=v.y;z=c; return *this; }
	__forceinline _tvec3& set( T a, const _tvec2<T>& v ){ x=a;y=v.x;z=v.y; return *this; }

	// assignment operators
	__forceinline _tvec3& operator=( T a ){ return set(a); }
	__forceinline _tvec3& operator=( const _tarray3<T>& v ){ return set(v); }

	// comparison operators
	__forceinline bool operator==(const _tvec3& v) const;
	__forceinline bool operator!=(const _tvec3& v) const { return !operator==(v); }

	// casting operators
	__forceinline operator T*(){ return &x; }
	__forceinline operator const T*() const { return &x; }
	__forceinline operator _tarray3<T>&(){ return reinterpret_cast<_tarray3<T>&>(*this); }
	__forceinline operator const _tarray3<T>&() const { return reinterpret_cast<_tarray3<T>&>(*this); }

	// array access operators
	__forceinline T& operator[]( int i ){ return (&x)[i]; }
	__forceinline const T& operator[]( int i ) const { return (&x)[i]; }

	// unary operators
	__forceinline _tvec3& operator+(){ return *this; }
	__forceinline const _tvec3& operator+() const { return *this; }
	__forceinline _tvec3 operator-() const;

	// binary operators
	__forceinline _tvec3 operator+( T a ) const { return _tvec3(x+a, y+a, z+a); }
    __forceinline _tvec3 operator-( T a ) const { return _tvec3(x-a, y-a, z-a); }
	__forceinline _tvec3 operator*( T a ) const { return _tvec3(x*a, y*a, z*a); }
    __forceinline _tvec3 operator/( T a ) const { return _tvec3(x/a, y/a, z/a); }
    __forceinline _tvec3 operator+( const _tvec3& v ) const { return _tvec3(x+v.x, y+v.y, z+v.z); }
    __forceinline _tvec3 operator-( const _tvec3& v ) const { return _tvec3(x-v.x, y-v.y, z-v.z); }
    __forceinline _tvec3 operator*( const _tvec3& v ) const { return _tvec3(x*v.x, y*v.y, z*v.z); }
    __forceinline _tvec3 operator/( const _tvec3& v ) const { return _tvec3(x/v.x, y/v.y, z/v.z); }

	// compound assignment operators
	__forceinline _tvec3& operator+=( T a ){ x+=a; y+=a; z+=a; return *this; }
	__forceinline _tvec3& operator-=( T a ){ x-=a; y-=a; z-=a; return *this; }
	__forceinline _tvec3& operator*=( T a ){ x*=a; y*=a; z*=a; return *this; }
	__forceinline _tvec3& operator/=( T a ){ x/=a; y/=a; z/=a; return *this; }
	__forceinline _tvec3& operator+=( const _tvec3& v ){ x+=v.x; y+=v.y; z+=v.z; return *this; }
	__forceinline _tvec3& operator-=( const _tvec3& v ){ x-=v.x; y-=v.y; z-=v.z; return *this; }
	__forceinline _tvec3& operator*=( const _tvec3& v ){ x*=v.x; y*=v.y; z*=v.z; return *this; }
	__forceinline _tvec3& operator/=( const _tvec3& v ){ x/=v.x; y/=v.y; z/=v.z; return *this; }

	// norm/length/dot: floating-point only functions
	template <typename=std::enable_if<std::is_floating_point<T>::value,T>>	__forceinline T length2() const { return T(x*x+y*y+z*z); }
	template <typename=std::enable_if<std::is_floating_point<T>::value,T>> 	__forceinline T norm2() const { return T(x*x+y*y+z*z); }
	template <typename=std::enable_if<std::is_floating_point<T>::value,T>> 	__forceinline T length() const { return T(sqrt(x*x+y*y+z*z)); }
	template <typename=std::enable_if<std::is_floating_point<T>::value,T>> 	__forceinline T norm() const { return T(sqrt(x*x+y*y+z*z)); }
	template <typename=std::enable_if<std::is_floating_point<T>::value,T>> 	__forceinline _tvec3 normalize() const { return operator/(length()); }
	template <typename=std::enable_if<std::is_floating_point<T>::value,T>> 	__forceinline T dot( const _tvec3& v ) const { return x*v.x+y*v.y+z*v.z; }

	// _tvec3 only: cross product (floating-point only)
	template <typename=std::enable_if<std::is_floating_point<T>::value,T>> 	__forceinline _tvec3 cross( const _tvec3& v ) const { return _tvec3( y*v.z-z*v.y, z*v.x-x*v.z, x*v.y-y*v.x ); }
};

// member specialization
template<> __forceinline bool _tvec3<float>::operator==(const _tvec3& v) const { static const float p=precision<float>::value(); return std::abs(x-v.x)<=p&&std::abs(y-v.y)<=p&&std::abs(z-v.z)<=p; }
template<> __forceinline bool _tvec3<double>::operator==(const _tvec3& v) const { static const double p=precision<double>::value(); return std::abs(x-v.x)<=p&&std::abs(y-v.y)<=p&&std::abs(z-v.z)<=p; }
template<> __forceinline bool _tvec3<int>::operator==(const _tvec3& v) const { return x==v.x&&y==v.y; }
template<> __forceinline bool _tvec3<uint>::operator==(const _tvec3& v) const { return x==v.x&&y==v.y; }
template<> __forceinline bool _tvec3<bool>::operator==(const _tvec3& v) const { return x==v.x&&y==v.y; }
template<> __forceinline _tvec3<float> _tvec3<float>::operator-() const { return _tvec3(-x,-y,-z); }
template<> __forceinline _tvec3<double> _tvec3<double>::operator-() const { return _tvec3(-x,-y,-z); }
template<> __forceinline _tvec3<int> _tvec3<int>::operator-() const { return _tvec3(-x,-y,-z); }

template <class T> struct _tvec4
{
	union
	{
		struct { T r, g, b, a; };
		struct { T x, y, z, w; };
#ifdef __USE_SWIZZLE__
		struct { _tvec2<T> xy; _tvec2<T> zw; };
		struct { _tvec2<T> rg; _tvec2<T> ba; };
		struct { _tvec3<T> xyz; };
		struct { _tvec3<T> rgb; };
		struct { T _x; _tvec2<T> yz; };
		struct { T __x; _tvec3<T> yzw; };
		struct { T _r; _tvec2<T> gb; };
		struct { T __r; _tvec3<T> gba; };
		_tarray4<T> xyzw, rgba;
#endif
	};

	using value_type = T;
	static const int N=4;

	// constructors
	__forceinline _tvec4(){}
	__forceinline _tvec4( T a ):x(a),y(a),z(a),w(a){}
	__forceinline _tvec4( T a, T b, T c, T d ):x(a),y(b),z(c),w(d){}
	__forceinline _tvec4( T a, const _tvec2<T>& v, T d ):x(a),y(v.x),z(v.y),w(d){}
	__forceinline _tvec4( T a, const _tvec3<T>& v ):x(a),y(v.x),z(v.y),w(v.z){}
	__forceinline _tvec4( T a, T b, const _tvec2<T>& v ):x(a),y(b),z(v.x),w(v.y){}
	__forceinline _tvec4( const _tvec2<T>& v, T c, T d ):x(v.x),y(v.y),z(c),w(d){}
	__forceinline _tvec4( const _tvec2<T>& v1, const _tvec2<T>& v2 ):x(v1.x),y(v1.y),z(v2.x),w(v2.y){}
	__forceinline _tvec4( const _tvec3<T>& v, T d ):x(v.x),y(v.y),z(v.z),w(d){}
	__forceinline _tvec4( const _tarray4<T>& v ):x(v[0]),y(v[1]),z(v[2]),w(v[3]){}

	// set
	__forceinline _tvec4& set( T a ){ x=y=z=w=a; return *this; }
	__forceinline _tvec4& set( T a, T b, T c, T d ){ x=a;y=b;z=c;w=d; return *this; }
	__forceinline _tvec4& set( const _tvec4& v ){ x=v.x;y=v.y;z=v.z;w=v.w; return *this; }
	__forceinline _tvec4& set( const _tarray4<T>& v ){ x=v[0];y=v[1];z=v[2];w=v[3]; return *this; }
	__forceinline _tvec4& set( const _tvec2<T>& v, T c, T d ){ x=v.x;y=v.y;z=c;w=d; return *this; }
	__forceinline _tvec4& set( T a, T b, const _tvec2<T>& v ){ x=a;y=b;z=v.x;w=v.y; return *this; }	
	__forceinline _tvec4& set( const _tvec3<T>& v, T d ){ x=v.x;y=v.y;z=v.z;w=d; return *this; }
	__forceinline _tvec4& set( T a, const _tvec3<T>& v ){ x=a;y=v.x;z=v.y;w=v.z; return *this; }
	__forceinline _tvec4& set( const _tvec2<T>& v1, const _tvec2<T>& v2 ){ x=v1.x;y=v1.y;z=v2.x;w=v2.y; return *this; }

	// assignment operators
	__forceinline _tvec4& operator=( T a ){ return set(a); }
	__forceinline _tvec4& operator=( const _tarray4<T>& v ){ return set(v); }

	// comparison operators
	__forceinline bool operator==(const _tvec4& v) const;
	__forceinline bool operator!=(const _tvec4& v) const { return !operator==(v); }

	// casting operators
	__forceinline operator T*(){ return &x; }
	__forceinline operator const T*() const { return &x; }
	__forceinline operator _tarray4<T>&(){ return reinterpret_cast<_tarray4<T>&>(*this); }
	__forceinline operator const _tarray4<T>&() const { return reinterpret_cast<_tarray4<T>&>(*this); }

	// array access operators
	__forceinline T& operator[]( int i ){ return (&x)[i]; }
	__forceinline const T& operator[]( int i ) const { return (&x)[i]; }

	// unary operators
	__forceinline _tvec4& operator+(){ return *this; }
	__forceinline const _tvec4& operator+() const { return *this; }
	__forceinline _tvec4 operator-() const;

    // binary operators
    __forceinline _tvec4 operator+( T a ) const { return _tvec4(x+a,y+a,z+a,w+a); }
    __forceinline _tvec4 operator-( T a ) const { return _tvec4(x-a,y-a,z-a,w-a); }
	__forceinline _tvec4 operator*( T a ) const { return _tvec4(x*a,y*a,z*a,w*a); }
    __forceinline _tvec4 operator/( T a ) const { return _tvec4(x/a,y/a,z/a,w/a); }
    __forceinline _tvec4 operator+( const _tvec4& v) const { return _tvec4(x+v.x,y+v.y,z+v.z,w+v.w); }
    __forceinline _tvec4 operator-( const _tvec4& v) const { return _tvec4(x-v.x,y-v.y,z-v.z,w-v.w); }
    __forceinline _tvec4 operator*( const _tvec4& v) const { return _tvec4(x*v.x,y*v.y,z*v.z,w*v.w); }
    __forceinline _tvec4 operator/( const _tvec4& v) const { return _tvec4(x/v.x,y/v.y,z/v.z,w/v.w); }

    // compound assignment operators
    __forceinline _tvec4& operator+=( T a ){ x+=a; y+=a; z+=a; w+=a; return *this; }
    __forceinline _tvec4& operator-=( T a ){ x-=a; y-=a; z-=a; w-=a; return *this; }
	__forceinline _tvec4& operator*=( T a ){ x*=a; y*=a; z*=a; w*=a; return *this; }
    __forceinline _tvec4& operator/=( T a ){ x/=a; y/=a; z/=a; w/=a; return *this; }
    __forceinline _tvec4& operator+=( const _tvec4& v){ x+=v.x; y+=v.y; z+=v.z; w+=v.w; return *this; }
    __forceinline _tvec4& operator-=( const _tvec4& v){ x-=v.x; y-=v.y; z-=v.z; w-=v.w; return *this; }
    __forceinline _tvec4& operator*=( const _tvec4& v){ x*=v.x; y*=v.y; z*=v.z; w*=v.w; return *this; }
    __forceinline _tvec4& operator/=( const _tvec4& v){ x/=v.x; y/=v.y; z/=v.z; w/=v.w; return *this; }

	// norm/length/dot: floating-point only functions
	template <typename=std::enable_if<std::is_floating_point<T>::value,T>> 	__forceinline T length2() const { return T(x*x+y*y+z*z+w*w); }
	template <typename=std::enable_if<std::is_floating_point<T>::value,T>> 	__forceinline T norm2() const { return T(x*x+y*y+z*z+w*w); }
	template <typename=std::enable_if<std::is_floating_point<T>::value,T>> 	__forceinline T length() const { return T(sqrt(x*x+y*y+z*z+w*w)); }
	template <typename=std::enable_if<std::is_floating_point<T>::value,T>> 	__forceinline T norm() const { return T(sqrt(x*x+y*y+z*z+w*w)); }
	template <typename=std::enable_if<std::is_floating_point<T>::value,T>> 	__forceinline _tvec4 normalize() const { return operator/(length()); }
	template <typename=std::enable_if<std::is_floating_point<T>::value,T>> 	__forceinline T dot( const _tvec4& v ) const { return x*v.x+y*v.y+z*v.z+w*v.w; }
};

// member specialization
template<> __forceinline bool _tvec4<float>::operator==(const _tvec4& v) const { static const float p=precision<float>::value(); return std::abs(x-v.x)<=p&&std::abs(y-v.y)<=p&&std::abs(z-v.z)<=p&&std::abs(w-v.w)<=p; }
template<> __forceinline bool _tvec4<double>::operator==(const _tvec4& v) const { static const double p=precision<double>::value(); return std::abs(x-v.x)<=p&&std::abs(y-v.y)<=p&&std::abs(z-v.z)<=p&&std::abs(w-v.w)<=p; }
template<> __forceinline bool _tvec4<int>::operator==(const _tvec4& v) const { return x==v.x&&y==v.y; }
template<> __forceinline bool _tvec4<uint>::operator==(const _tvec4& v) const { return x==v.x&&y==v.y; }
template<> __forceinline bool _tvec4<bool>::operator==(const _tvec4& v) const { return x==v.x&&y==v.y; }
template<> __forceinline _tvec4<float> _tvec4<float>::operator-() const { return _tvec4(-x,-y,-z,-w); }
template<> __forceinline _tvec4<double> _tvec4<double>::operator-() const { return _tvec4(-x,-y,-z,-w); }
template<> __forceinline _tvec4<int> _tvec4<int>::operator-() const { return _tvec4(-x,-y,-z,-w); }

//*******************************************************************
// type definitions
using vec2	= _tvec2<float>;	using vec3	= _tvec3<float>;	using vec4	= _tvec4<float>;
using half2 = _tvec2<half>;		using half3 = _tvec3<half>;		using half4 = _tvec4<half>;
using dvec2 = _tvec2<double>;	using dvec3 = _tvec3<double>;	using dvec4 = _tvec4<double>;
using ivec2 = _tvec2<int>;		using ivec3 = _tvec3<int>;		using ivec4 = _tvec4<int>;
using uvec2 = _tvec2<uint>;		using uvec3 = _tvec3<uint>;		using uvec4 = _tvec4<uint>;
using bvec2 = _tvec2<bool>;		using bvec3 = _tvec3<bool>;		using bvec4 = _tvec4<bool>;
using svec2 = _tvec2<size_t>;	using svec3 = _tvec3<size_t>;	using svec4 = _tvec4<size_t>;

//*******************************************************************
// half-float vector conversion
__forceinline vec2 htof( const half2& h ){ return vec2(htof(h.x),htof(h.y)); }
__forceinline vec3 htof( const half3& h ){ return vec3(htof(h.x),htof(h.y),htof(h.z)); }
__forceinline vec4 htof( const half4& h ){ return vec4(htof(h.x),htof(h.y),htof(h.z),htof(h.w)); }
__forceinline half2 ftoh( const vec2& v ){ return half2(ftoh(v.x),ftoh(v.y)); }
__forceinline half3 ftoh( const vec3& v ){ return half3(ftoh(v.x),ftoh(v.y),ftoh(v.z)); }
__forceinline half4 ftoh( const vec4& v ){ return half4(ftoh(v.x),ftoh(v.y),ftoh(v.z),ftoh(v.w)); }

//*******************************************************************
struct mat2
{
	union { float a[4]; struct {float _11,_12,_21,_22;}; };

	// constructors
	__forceinline mat2(){ setIdentity(); }
	__forceinline mat2( float f11, float f12, float f21, float f22 ){_11=f11;_12=f12;_21=f21;_22=f22;}
	__forceinline mat2( const float4& f ){ memcpy(a,&f,sizeof(float)*4); }

	// comparison operators
	__forceinline bool operator==( const mat2& m ) const { for( int k=0; k<std::extent<decltype(a)>::value; k++ ) if(std::abs(a[k]-m[k])>precision<float>::value()) return false; return true; }
	__forceinline bool operator!=( const mat2& m ) const { return !operator==(m); }

	// casting operators
	__forceinline operator float*(){ return a; }
	__forceinline operator const float*() const { return a; }
	__forceinline operator float4& (){ return reinterpret_cast<float4&>(*this); }

	// array access operators
	__forceinline float& operator[]( unsigned i ){ return a[i]; }
	__forceinline float& operator[]( int i ){ return a[i]; }
	__forceinline const float& operator[]( unsigned i ) const { return a[i]; }
	__forceinline const float& operator[]( int i ) const { return a[i]; }

	// unary operators
	__forceinline mat2& operator+(){ return *this; }
	__forceinline const mat2& operator+() const { return *this; }
	__forceinline mat2 operator-() const { return mat2(-_11,-_12,-_21,-_22); }

	// row vectors
	__forceinline const vec2& rvec2( int row ) const { return reinterpret_cast<const vec2&>(a[row*2]); }
	__forceinline vec2& rvec2( int row ){ return reinterpret_cast<vec2&>(a[row*2]); }

	// addition/subtraction operators
	__forceinline mat2& operator+=( const mat2& m ){ for( int k=0; k < std::extent<decltype(a)>::value; k++ ) a[k]+=m[k]; return *this; }
	__forceinline mat2& operator-=( const mat2& m ){ for( int k=0; k < std::extent<decltype(a)>::value; k++ ) a[k]-=m[k]; return *this; }
	__forceinline mat2 operator+( const mat2& m ) const { return mat2(*this).operator+=(m); }
	__forceinline mat2 operator-( const mat2& m ) const { return mat2(*this).operator-=(m); }

	// multiplication operators
	__forceinline mat2& operator*=( float f ){ for( int k=0; k < std::extent<decltype(a)>::value; k++ ) a[k]*=f; return *this; }
	__forceinline mat2& operator*=( const mat2& m ){ mat2 t=m.transpose(); for(uint k=0;k<2;k++) rvec2(k)=t.operator*(rvec2(k)); return *this; } // a bit tricky implementation
	__forceinline mat2 operator*( float f ) const { return mat2(*this).operator*=(f); }
	__forceinline mat2 operator*( const mat2& m ) const { return mat2(*this).operator*=(m); }
	__forceinline vec2 operator*( const vec2& v ) const { return vec2(rvec2(0).dot(v), rvec2(1).dot(v)); }

	// identity and transpose
	__forceinline static mat2 identity(){ return mat2(); }
	__forceinline mat2& setIdentity(){ _12=_21=0.0f;_11=_22=1.0f; return *this; }
	__forceinline mat2 transpose() const { return mat2(_11,_21,_12,_22); }

	// determinant
	__forceinline float det() const { return _11*_22 - _12*_21; }
	
	// inverse
	__forceinline mat2 inverse() const 
	{
		float d=det(), s=1.0f/d; if( d==0 ) printf( "mat2::inverse() might be singular.\n" );
		return mat2( +_22*s, -_12*s, -_21*s, +_11*s );
	}
};

//*******************************************************************
struct mat3
{
	union { float a[9]; struct {float _11,_12,_13,_21,_22,_23,_31,_32,_33;}; };

	// constructors
	__forceinline mat3(){ setIdentity(); }
	__forceinline mat3( float f11, float f12, float f13, float f21, float f22, float f23, float f31, float f32, float f33 ){_11=f11;_12=f12;_13=f13;_21=f21;_22=f22;_23=f23;_31=f31;_32=f32;_33=f33;}
	__forceinline mat3( const float9& f ){ memcpy(a,&f,sizeof(float)*9); }
	
	// comparison operators
	__forceinline bool operator==( const mat3& m ) const { for( int k=0; k<std::extent<decltype(a)>::value; k++ ) if(std::abs(a[k]-m[k])>precision<float>::value()) return false; return true; }
	__forceinline bool operator!=( const mat3& m ) const { return !operator==(m); }

	// casting operators
	__forceinline operator float*(){ return a; }
	__forceinline operator const float*() const { return a; }
	__forceinline operator float9& (){ return reinterpret_cast<float9&>(*this); }

	// array access operators
	__forceinline float& operator[]( unsigned i ){ return a[i]; }
	__forceinline float& operator[]( int i ){ return a[i]; }
	__forceinline const float& operator[]( unsigned i ) const { return a[i]; }
	__forceinline const float& operator[]( int i ) const { return a[i]; }

	// unary operators
	__forceinline mat3& operator+(){ return *this; }
	__forceinline const mat3& operator+() const { return *this; }
	__forceinline mat3 operator-() const { return mat3(-_11,-_12,-_13,-_21,-_22,-_23,-_31,-_32,-_33); }

	// row vectors
	__forceinline const vec3& rvec3( int row ) const { return reinterpret_cast<const vec3&>(a[row*3]); }
	__forceinline vec3& rvec3( int row ){ return reinterpret_cast<vec3&>(a[row*3]); }

	// addition/subtraction operators
	__forceinline mat3& operator+=( const mat3& m ){ for( int k=0; k < std::extent<decltype(a)>::value; k++ ) a[k]+=m[k]; return *this; }
	__forceinline mat3& operator-=( const mat3& m ){ for( int k=0; k < std::extent<decltype(a)>::value; k++ ) a[k]-=m[k]; return *this; }
	__forceinline mat3 operator+( const mat3& m ) const { return mat3(*this).operator+=(m); }
	__forceinline mat3 operator-( const mat3& m ) const { return mat3(*this).operator-=(m); }

	// multiplication operators
	__forceinline mat3& operator*=( float f ){ for( int k=0; k < std::extent<decltype(a)>::value; k++ ) a[k]*=f; return *this; }
	__forceinline mat3& operator*=( const mat3& m ){ mat3 t=m.transpose(); for(uint k=0;k<3;k++) rvec3(k)=t.operator*(rvec3(k)); return *this; } // a bit tricky implementation
	__forceinline mat3 operator*( float f ) const { return mat3(*this).operator*=(f); }
	__forceinline mat3 operator*( const mat3& m ) const { return mat3(*this).operator*=(m); }
	__forceinline vec3 operator*( const vec3& v ) const { return vec3(rvec3(0).dot(v), rvec3(1).dot(v), rvec3(2).dot(v)); }

	// identity and transpose
	__forceinline static mat3 identity(){ return mat3(); }
	__forceinline mat3& setIdentity(){ _12=_13=_21=_23=_31=_32=0.0f;_11=_22=_33=1.0f; return *this; }
	__forceinline mat3 transpose() const { return mat3(_11,_21,_31,_12,_22,_32,_13,_23,_33); }

	// determinant
	__forceinline float det() const { return _11*(_22*_33-_23*_32) + _12*(_23*_31-_21*_33) + _13*(_21*_32-_22*_31); }

	// inverse
	__forceinline mat3 inverse() const 
	{
		float d=det(), s=1.0f/d; if( d==0 ) printf( "mat3::inverse() might be singular.\n" );
		return mat3( (_22*_33-_32*_23)*s, (_13*_32-_12*_33)*s, (_12*_23-_13*_22)*s, (_23*_31-_21*_33)*s, (_11*_33-_13*_31)*s, (_21*_13-_11*_23)*s, (_21*_32-_31*_22)*s, (_31*_12-_11*_32)*s, (_11*_22-_21*_12)*s );
	}
};

//*******************************************************************
// mat4 uses only row-major and right-hand (RH) notations even for D3D
struct mat4
{
	union { float a[16]; struct {float _11,_12,_13,_14,_21,_22,_23,_24,_31,_32,_33,_34,_41,_42,_43,_44;}; };

	// constructors
	__forceinline mat4(){ setIdentity(); }
	__forceinline mat4( float f11, float f12, float f13, float f14, float f21, float f22, float f23, float f24, float f31, float f32, float f33, float f34, float f41, float f42, float f43, float f44 ){_11=f11;_12=f12;_13=f13;_14=f14;_21=f21;_22=f22;_23=f23;_24=f24;_31=f31;_32=f32;_33=f33;_34=f34;_41=f41;_42=f42;_43=f43;_44=f44;}
	__forceinline mat4( const float16& f ){ memcpy(a,&f,sizeof(float)*16); }

	// comparison operators
	__forceinline bool operator==( const mat4& m ) const { for( int k=0; k<std::extent<decltype(a)>::value; k++ ) if(std::abs(a[k]-m[k])>precision<float>::value()) return false; return true; }
	__forceinline bool operator!=( const mat4& m ) const { return !operator==(m); }

	// casting operators
	__forceinline operator float*(){ return a; }
	__forceinline operator const float*() const { return a; }
	__forceinline operator float16&(){ return reinterpret_cast<float16&>(*this); }
	__forceinline operator mat3() const { return std::move(mat3(_11,_12,_13,_21,_22,_23,_31,_32,_33)); }
	__forceinline operator mat2() const { return std::move(mat2(_11,_12,_21,_22)); }

	// array access operators
	__forceinline float& operator[]( unsigned i ){ return a[i]; }
	__forceinline float& operator[]( int i ){ return a[i]; }
	__forceinline const float& operator[]( unsigned i ) const { return a[i]; }
	__forceinline const float& operator[]( int i ) const { return a[i]; }

	// unary operators
	__forceinline mat4& operator+(){ return *this; }
	__forceinline const mat4& operator+() const { return *this; }
	__forceinline mat4 operator-() const { return std::move(mat4(-_11,-_12,-_13,-_14,-_21,-_22,-_23,-_24,-_31,-_32,-_33,-_34,-_41,-_42,-_43,-_44)); }

	// row vectors
	__forceinline const vec4& rvec4( int row ) const { return reinterpret_cast<const vec4&>(a[row*4]); }
	__forceinline vec4& rvec4( int row ){ return reinterpret_cast<vec4&>(a[row*4]); }
	__forceinline const vec3& rvec3( int row ) const { return reinterpret_cast<const vec3&>(a[row*4]); }
	__forceinline vec3& rvec3( int row ){ return reinterpret_cast<vec3&>(a[row*4]); }

	// addition/subtraction operators
	__forceinline mat4& operator+=( const mat4& m ){ for( int k=0; k < std::extent<decltype(a)>::value; k++ ) a[k]+=m[k]; return *this; }
	__forceinline mat4& operator-=( const mat4& m ){ for( int k=0; k < std::extent<decltype(a)>::value; k++ ) a[k]-=m[k]; return *this; }
	__forceinline mat4 operator+( const mat4& m ) const { return std::move(mat4(*this).operator+=(m)); }
	__forceinline mat4 operator-( const mat4& m ) const { return std::move(mat4(*this).operator-=(m)); }

	// multiplication operators
	__forceinline mat4& operator*=( float f ){ for( int k=0; k < std::extent<decltype(a)>::value; k++ ) a[k]*=f; return *this; }
	__forceinline mat4& operator*=( const mat4& m ){ mat4 t=m.transpose(); for(uint k=0;k<4;k++) rvec4(k)=t.operator*(rvec4(k)); return *this; } // a bit tricky implementation
	__forceinline mat4 operator*( float f ) const { return std::move(mat4(*this).operator*=(f)); }
	__forceinline mat4 operator*( const mat4& m ) const { return std::move(mat4(*this).operator*=(m)); }
	__forceinline vec4 operator*( const vec4& v ) const { return vec4(rvec4(0).dot(v),rvec4(1).dot(v),rvec4(2).dot(v),rvec4(3).dot(v)); }
	__forceinline vec3 operator*( const vec3& v ) const { vec4 v4(v,1); return vec3(rvec4(0).dot(v4),rvec4(1).dot(v4),rvec4(2).dot(v4)); }
	
	// identity and transpose
	__forceinline static mat4 identity(){ return mat4(); }
	__forceinline mat4& setIdentity(){ _12=_13=_14=_21=_23=_24=_31=_32=_34=_41=_42=_43=0.0f;_11=_22=_33=_44=1.0f; return *this; }
	__forceinline mat4 transpose() const { return std::move(mat4(_11,_21,_31,_41,_12,_22,_32,_42,_13,_23,_33,_43,_14,_24,_34,_44)); }

	// determinant and inverse: see below implementations
	__forceinline float det() const;
	__forceinline mat4 inverse() const;

	// static row-major transformations
	__forceinline static mat4 translate( const vec3& v ){ return std::move(mat4().setTranslate(v)); }
	__forceinline static mat4 translate( float x, float y, float z ){ return std::move(mat4().setTranslate(x,y,z)); }
	__forceinline static mat4 scale( const vec3& v ){ return std::move(mat4().setScale(v)); }
	__forceinline static mat4 scale( float x, float y, float z ){ return std::move(mat4().setScale(x,y,z)); }
	__forceinline static mat4 shear( const vec2& yz, const vec2& zx, const vec2& xy ){ return std::move(mat4().setShear(yz,zx,xy)); }
	__forceinline static mat4 rotateX( float theta ){ return std::move(mat4().setRotateX(theta)); }
	__forceinline static mat4 rotateY( float theta ){ return std::move(mat4().setRotateY(theta)); }
	__forceinline static mat4 rotateZ( float theta ){ return std::move(mat4().setRotateZ(theta)); }
	__forceinline static mat4 rotateVecToVec( const vec3& v0, vec3 v1 ){ return std::move(mat4().setRotateVecToVec(v0,v1)); }
	__forceinline static mat4 rotate( const vec3& axis, float angle ){ return std::move(mat4().setRotate(axis,angle)); }

	__forceinline static mat4 viewport( int width, int height ){ return std::move(mat4().setViewport(width,height)); }
	__forceinline static mat4 lookAt( const vec3& eye, const vec3& center, const vec3& up ){ return std::move(mat4().setLookAt(eye,center,up)); }
	__forceinline static mat4 lookAtInv( const vec3& eye, const vec3& center, const vec3& up ){ return std::move(mat4().setLookAtInv(eye,center,up)); }
	__forceinline static mat4 perspective( float fovy, float aspect, float dn, float df ){ return std::move(mat4().setPerspective(fovy,aspect,dn,df)); }
	__forceinline static mat4 perspectiveOffCenter( float left, float right, float top, float bottom, float dn, float df ){ return std::move(mat4().setPerspectiveOffCenter(left,right,top,bottom,dn,df)); }
	__forceinline static mat4 ortho( float width, float height, float dn, float df ){ return std::move(mat4().setOrtho(width,height,dn,df)); }
	__forceinline static mat4 orthoOffCenter( float left, float right, float top, float bottom, float dn, float df ){ return std::move(mat4().setOrthoOffCenter(left,right,top,bottom,dn,df)); }
	__forceinline static mat4 perspectiveDX( float fovy, float aspect, float dn, float df ){ return std::move(mat4().setPerspectiveDX(fovy,aspect,dn,df)); }
	__forceinline static mat4 perspectiveOffCenterDX( float left, float right, float top, float bottom, float dn, float df ){ return std::move(mat4().setPerspectiveOffCenterDX(left,right,top,bottom,dn,df)); }
	__forceinline static mat4 orthoDX( float width, float height, float dn, float df ){ return std::move(mat4().setOrthoDX(width,height,dn,df)); }
	__forceinline static mat4 orthoOffCenterDX( float left, float right, float top, float bottom, float dn, float df ){ return std::move(mat4().setOrthoOffCenterDX(left,right,top,bottom,dn,df)); }

	// row-major transformations
	__forceinline mat4& setTranslate( const vec3& v ){ setIdentity(); _14=v.x; _24=v.y; _34=v.z; return *this; }
	__forceinline mat4& setTranslate( float x,float y,float z ){ setIdentity(); _14=x; _24=y; _34=z; return *this; }
	__forceinline mat4& setScale( const vec3& v ){ setIdentity(); _11=v.x; _22=v.y; _33=v.z; return *this; }
	__forceinline mat4& setScale( float x,float y,float z ){ setIdentity(); _11=x; _22=y; _33=z; return *this; }
	__forceinline mat4& setShear( const vec2& yz, const vec2& zx, const vec2& xy ){ setIdentity(); _12=yz.x; _13=yz.y; _21=zx.y; _23=zx.x; _31=xy.x; _32=xy.y; return *this; }
	__forceinline mat4& setRotateX( float theta ){ return setRotate(vec3(1,0,0),theta); }
	__forceinline mat4& setRotateY( float theta ){ return setRotate(vec3(0,1,0),theta); }
	__forceinline mat4& setRotateZ( float theta ){ return setRotate(vec3(0,0,1),theta); }
	__forceinline mat4& setRotateVecToVec( const vec3& v0, const vec3& v1 ){ vec3 n=v0.cross(v1); return setRotate( n.normalize(), asin( min(n.length(),0.9999f) ) ); }
	__forceinline mat4& setRotate( const vec3& axis, float angle )
	{
		float c=cos(angle), s=sin(angle), x=axis.x, y=axis.y, z=axis.z;
		a[0] = x*x*(1-c)+c;		a[1] = x*y*(1-c)-z*s;		a[2] = x*z*(1-c)+y*s;	a[3] = 0.0f;
		a[4] = x*y*(1-c)+z*s;	a[5] = y*y*(1-c)+c;			a[6] = y*z*(1-c)-x*s;	a[7] = 0.0f;
		a[8] = x*z*(1-c)-y*s;	a[9] = y*z*(1-c)+x*s;		a[10] = z*z*(1-c)+c;	a[11] = 0.0f;
		a[12] = 0;				a[13] = 0;					a[14] = 0;				a[15] = 1.0f;
		return *this;
	}

	// viewport, lookat, projection
	__forceinline mat4& setViewport( int width, int height ){ setIdentity(); _11=width*0.5f; _22=-height*0.5f; _14=width*0.5f; _24=height*0.5f; return *this; }
	__forceinline mat4& setLookAt( const vec3& eye, const vec3& center, const vec3& up ){ setIdentity(); rvec3(2) = (eye-center).normalize(); rvec3(0) = (up.cross(rvec3(2))).normalize(); rvec3(1) = rvec3(2).cross(rvec3(0)); return *this = (*this)*(mat4::translate(-eye)); }
	__forceinline mat4& setLookAtInv( const vec3& eye, const vec3& center, const vec3& up ){ setIdentity(); rvec3(2) = (eye-center).normalize(); rvec3(0) = (up.cross(rvec3(2))).normalize(); rvec3(1) = rvec3(2).cross(rvec3(0)); return *this = mat4::translate(eye)*transpose(); }

	// Canonical view volume in OpenGL: [-1,1]^3
	__forceinline mat4& setPerspective( float fovy, float aspectRatio, float dn, float df ){ if(fovy>float(PI)) fovy*=float(PI/180.0); /* autofix for fov in degrees */ setIdentity(); _22=1.0f/tanf(fovy*0.5f); _11=_22/aspectRatio; _33=(dn+df)/(dn-df); _34=2.0f*dn*df/(dn-df); _43=-1.0f;  _44=0.0f; return *this; }
	__forceinline mat4& setPerspectiveOffCenter( float left, float right, float top, float bottom, float dn, float df ){ setIdentity(); _11=2.0f*dn/(right-left); _22=2.0f*dn/(top-bottom); _13=(right+left)/(right-left); _23=(top+bottom)/(top-bottom); _33=(dn+df)/(dn-df); _34=2.0f*dn*df/(dn-df); _43=-1.0f; _44=0.0f; return *this; }
	__forceinline mat4& setOrtho( float width, float height, float dn, float df ){ setIdentity(); _11=2.0f/width; _22=2.0f/height;  _33=2.0f/(dn-df); _34=(dn+df)/(dn-df); return *this; }
	__forceinline mat4& setOrthoOffCenter( float left, float right, float top, float bottom, float dn, float df ){ setOrtho( right-left, top-bottom, dn, df ); _14=(left+right)/(left-right); _24=(bottom+top)/(bottom-top); return *this; }

	// Canonical view volume in DirectX: [-1,1]^2*[0,1]: diffes only in _33 and _34
	__forceinline mat4& setPerspectiveDX( float fovy, float aspectRatio, float dn, float df ){ setPerspective( fovy, aspectRatio, dn, df ); _33=df/(dn-df); _34*=0.5f; return *this; } // equivalent to D3DXMatrixPerspectiveFovRH()
	__forceinline mat4& setPerspectiveOffCenterDX( float left, float right, float top, float bottom, float dn, float df ){ setPerspectiveOffCenter( left, right, top, bottom, dn, df ); _33=df/(dn-df); _34*=0.5f; return *this; }
	__forceinline mat4& setOrthoDX( float width, float height, float dn, float df ){ setOrtho( width, height, dn, df ); _33*=0.5f; _34=dn/(dn-df); return *this; }
	__forceinline mat4& setOrthoOffCenterDX( float left, float right, float top, float bottom, float dn, float df ){ setOrthoOffCenter( left, right, top, bottom, dn, df ); _33*=0.5f; _34=dn/(dn-df); return *this; }
};

__forceinline float mat4::det() const 
{
	float d1 = (_41*_32-_31*_42)*_23 - (_41*_22+_21*_42)*_33 + (_31*_22-_21*_32)*_43;
	float d2 = (_41*_32+_31*_42)*_13 + (_41*_12-_11*_42)*_33 - (_31*_12+_11*_32)*_43;
	float d3 = (_41*_22-_21*_42)*_13 - (_41*_12+_11*_42)*_23 + (_21*_12-_11*_22)*_43;
	float d4 = (_31*_22+_21*_32)*_13 + (_31*_12-_11*_32)*_23 - (_21*_12+_11*_22)*_33;
	return d1*_14-d2*_24+d3*_34-d4*_44;
}

// http://www.cg.info.hiroshima-cu.ac.jp/~miyazaki/knowledge/teche23.html
__forceinline mat4 mat4::inverse() const 
{
	float d1 = (_41*_32-_31*_42)*_23 - (_41*_22+_21*_42)*_33 + (_31*_22-_21*_32)*_43;
	float d2 = (_41*_32+_31*_42)*_13 + (_41*_12-_11*_42)*_33 - (_31*_12+_11*_32)*_43;
	float d3 = (_41*_22-_21*_42)*_13 - (_41*_12+_11*_42)*_23 + (_21*_12-_11*_22)*_43;
	float d4 = (_31*_22+_21*_32)*_13 + (_31*_12-_11*_32)*_23 - (_21*_12+_11*_22)*_33;
	float s = 1.0f/(d1*_14-d2*_24+d3*_34-d4*_44);
	
	return std::move(mat4(	((_32*_43-_42*_33)*_24 + (_42*_23-_22*_43)*_34 - (_32*_23+_22*_33)*_44)*s,
							((_42*_33-_32*_43)*_14 - (_42*_13+_12*_43)*_34 + (_32*_13-_12*_33)*_44)*s,
							((_22*_43-_42*_23)*_14 + (_42*_13-_12*_43)*_24 - (_22*_13+_12*_23)*_44)*s,
							((_32*_23-_22*_33)*_14 - (_32*_13+_12*_33)*_24 + (_22*_13-_12*_23)*_34)*s,
							((_41*_33-_31*_43)*_24 - (_41*_23+_21*_43)*_34 + (_31*_23-_21*_33)*_44)*s,
							((_31*_43-_41*_33)*_14 + (_41*_13-_11*_43)*_34 - (_31*_13+_11*_33)*_44)*s,
							((_41*_23-_21*_43)*_14 - (_41*_13+_11*_43)*_24 + (_21*_13-_11*_23)*_44)*s,
							((_21*_33-_31*_23)*_14 + (_31*_13-_11*_33)*_24 - (_21*_13+_11*_23)*_34)*s,
							((_31*_42-_41*_32)*_24 + (_41*_22-_21*_42)*_34 - (_31*_22+_21*_32)*_44)*s,
							((_41*_32-_31*_42)*_14 - (_41*_12+_11*_42)*_34 + (_31*_12-_11*_32)*_44)*s,
							((_21*_42-_41*_22)*_14 + (_41*_12-_11*_42)*_24 - (_21*_12+_11*_22)*_44)*s,
							((_31*_22-_21*_32)*_14 - (_31*_12+_11*_32)*_24 + (_21*_12-_11*_22)*_34)*s,
							d1*s, (d2-_41*_32*_13*2.0f)*s, d3*s, (d4-_31*_22*_13*2.0f)*s ));
}

//*******************************************************************
// vertor-matrix multiplications
__forceinline vec2 operator*( const vec2& v, const mat2& m ){ return m.transpose()*v; }
__forceinline vec3 operator*( const vec3& v, const mat3& m ){ return m.transpose()*v; }
__forceinline vec3 operator*( const vec3& v, const mat4& m ){ return m.transpose()*v; }
__forceinline vec4 operator*( const vec4& v, const mat4& m ){ return m.transpose()*v; }
__forceinline vec2 mul( const vec2& v, const mat2& m ){ return m.transpose()*v; }
__forceinline vec3 mul( const vec3& v, const mat3& m ){ return m.transpose()*v; }
__forceinline vec3 mul( const vec3& v, const mat4& m ){ return m.transpose()*v; }
__forceinline vec4 mul( const vec4& v, const mat4& m ){ return m.transpose()*v; }
__forceinline vec2 mul( const mat2& m, const vec2& v ){ return m*v; }
__forceinline vec3 mul( const mat3& m, const vec3& v ){ return m*v; }
__forceinline vec3 mul( const mat4& m, const vec3& v ){ return m*v; }
__forceinline vec4 mul( const mat4& m, const vec4& v ){ return m*v; }

//*******************************************************************
// scalar-vector algebra
template <class T> __forceinline _tvec2<T> operator+( T f, const _tvec2<T>& v ){ return v+f; }
template <class T> __forceinline _tvec2<T> operator-( T f, const _tvec2<T>& v ){ return _tvec2<T>(f-v.x,f-v.y); }
template <class T> __forceinline _tvec2<T> operator*( T f, const _tvec2<T>& v ){ return v*f; }
template <class T> __forceinline _tvec2<T> operator/( T f, const _tvec2<T>& v ){ return _tvec2<T>(f/v.x,f/v.y); }
template <class T> __forceinline _tvec3<T> operator+( T f, const _tvec3<T>& v ){ return v+f; }
template <class T> __forceinline _tvec3<T> operator-( T f, const _tvec3<T>& v ){ return _tvec3<T>(f-v.x,f-v.y,f-v.z); }
template <class T> __forceinline _tvec3<T> operator*( T f, const _tvec3<T>& v ){ return v*f; }
template <class T> __forceinline _tvec3<T> operator/( T f, const _tvec3<T>& v ){ return _tvec3<T>(f/v.x,f/v.y,f/v.z); }
template <class T> __forceinline _tvec4<T> operator+( T f, const _tvec4<T>& v ){ return v+f; }
template <class T> __forceinline _tvec4<T> operator-( T f, const _tvec4<T>& v ){ return _tvec4<T>(f-v.x,f-v.y,f-v.z,f-v.w); }
template <class T> __forceinline _tvec4<T> operator*( T f, const _tvec4<T>& v ){ return v*f; }
template <class T> __forceinline _tvec4<T> operator/( T f, const _tvec4<T>& v ){ return _tvec4<T>(f/v.x,f/v.y,f/v.z,f/v.w); }

//*******************************************************************
// global operators for vector length/normalize/dot/cross
__forceinline float length( const vec2& v ){ return v.length(); }
__forceinline float length( const vec3& v ){ return v.length(); }
__forceinline float length( const vec4& v ){ return v.length(); }
__forceinline float length2( const vec2& v ){ return v.length2(); }
__forceinline float length2( const vec3& v ){ return v.length2(); }
__forceinline float length2( const vec4& v ){ return v.length2(); }
__forceinline vec2 normalize( const vec2& v ){ return v.normalize(); }
__forceinline vec3 normalize( const vec3& v ){ return v.normalize(); }
__forceinline vec4 normalize( const vec4& v ){ return v.normalize(); }
template <class T> __forceinline T dot( const _tvec2<T>& v1, const _tvec2<T>& v2){ return v1.dot(v2); }
template <class T> __forceinline T dot( const _tvec3<T>& v1, const _tvec3<T>& v2){ return v1.dot(v2); }
template <class T> __forceinline T dot( const _tvec4<T>& v1, const _tvec4<T>& v2){ return v1.dot(v2); }
template <class T> __forceinline _tvec3<T> cross( const _tvec3<T>& v1, const _tvec3<T>& v2){ return v1.cross(v2); }

//*******************************************************************
// general math functions
template <class T> __forceinline T deg2rad( T f ){ return T(f*PI/T(180.0)); }
template <class T> __forceinline T rad2deg( T f ){ return T(f*T(180.0)/PI); }
__forceinline bool isPowerOfTwo( uint i ){ return (i&(i-1))==0; }		// http://en.wikipedia.org/wiki/Power_of_two
__forceinline uint nextPowerOfTwo( uint n ){ int m=int(n)-1; for( uint k=1; k<uint(sizeof(int))*8; k<<=1 ) m=m|m>>k; return m+1; }
__forceinline uint nearestPowerOfTwo( uint n ){ int m=int(n)-1; for( uint k=1; k<uint(sizeof(int))*8; k<<=1 ) m=m|m>>k; return m+1; }
__forceinline uint getMipLevels( size_t width ){ uint l=0; size_t s=width; while(s){s=s>>1;l++;} return l; }
__forceinline uint getMipLevels( size_t width, size_t height, size_t depth=1 ){ return getMipLevels(max(max(width,height),depth)); }

// Quake3's Fast InvSqrt(): 1/sqrt(x)
__forceinline float invsqrt( float x ){ float y=0.5f*x; int i=*(int*)&x; i=0x5f3759df-(i>>1); x=*(float*)&i; x=x*(1.5f-y*x*x); return x; }
__forceinline float rsqrt( float x ){ float y=0.5f*x; int i=*(int*)&x; i=0x5f3759df-(i>>1); x=*(float*)&i; x=x*(1.5f-y*x*x); return x; }

//*******************************************************************
// {GLSL|HLSL}-like shader intrinsic functions
__forceinline float saturate( float f ){ return min(max(f,0.0f),1.0f); }
__forceinline vec2 saturate( const vec2& v ){ return vec2(saturate(v.x),saturate(v.y)); }
__forceinline vec3 saturate( const vec3& v ){ return vec3(saturate(v.x),saturate(v.y),saturate(v.z)); }
__forceinline vec4 saturate(const  vec4& v ){ return vec4(saturate(v.x),saturate(v.y),saturate(v.z),saturate(v.w)); }
__forceinline float frac( float f ){ return float(f-floor(f)); }
__forceinline vec2 frac( const vec2& v ){ return vec2(frac(v.x),frac(v.y)); }
__forceinline vec3 frac( const vec3& v ){ return vec3(frac(v.x),frac(v.y),frac(v.z)); }
__forceinline vec4 frac( const vec4& v ){ return vec4(frac(v.x),frac(v.y),frac(v.z),frac(v.w)); }
__forceinline vec2 abs( const vec2& v ){ return vec2(fabs(v.x),fabs(v.y)); }
__forceinline vec3 abs( const vec3& v ){ return vec3(fabs(v.x),fabs(v.y),fabs(v.z)); }
__forceinline vec4 abs( const vec4& v ){ return vec4(fabs(v.x),fabs(v.y),fabs(v.z),fabs(v.w)); }
__forceinline int sign( float f ){ return f>0.0f?1:f<0.0f?-1:0; }
__forceinline ivec2 sign( const vec2& v ){ return ivec2(sign(v.x),sign(v.y)); }
__forceinline ivec3 sign( const vec3& v ){ return ivec3(sign(v.x),sign(v.y),sign(v.z)); }
__forceinline ivec4 sign( const vec4& v ){ return ivec4(sign(v.x),sign(v.y),sign(v.z),sign(v.w)); }
__forceinline float smoothstep( float t ){ t=clamp(t,0.0f,1.0f); return t*t*(3-2*t); }							// C1-continuity
__forceinline vec2 smoothstep( const vec2& t ){ return vec2(smoothstep(t.x),smoothstep(t.y)); }
__forceinline vec3 smoothstep( const vec3& t ){ return vec3(smoothstep(t.x),smoothstep(t.y),smoothstep(t.z)); }
__forceinline vec4 smoothstep( const vec4& t ){ return vec4(smoothstep(t.x),smoothstep(t.y),smoothstep(t.z),smoothstep(t.w)); }
__forceinline float smootherstep( float t ){ t=clamp(t,0.0f,1.0f); return t*t*t*(6.0f*t*t-15.0f*t+10.0f); }		// C2-continuity (by Ken Perlin)
__forceinline vec2 smootherstep( const vec2& t ){ return vec2(smootherstep(t.x),smootherstep(t.y)); }
__forceinline vec3 smootherstep( const vec3& t ){ return vec3(smootherstep(t.x),smootherstep(t.y),smootherstep(t.z)); }
__forceinline vec4 smootherstep( const vec4& t ){ return vec4(smootherstep(t.x),smootherstep(t.y),smootherstep(t.z),smootherstep(t.w)); }
__forceinline vec3 reflect( const vec3& I, const vec3& N ){ return I-N*dot(I,N)*2.0f; }	// I: incident vector, N: normal
__forceinline vec3 refract( const vec3& I, const vec3& N, float eta /* = n0/n1 */ ){ float d = I.dot(N); float k = 1.0f-eta*eta*(1.0f-d*d); return k<0.0f?0.0f:(I*eta-N*(eta*d+sqrtf(k))); } // I: incident vector, N: normal
__forceinline float lerp( float v1, float v2, float t ){ return v1*(1.0f-t)+v2*t; }
__forceinline vec2 lerp( const vec2& y1, const vec2& y2, float t ){ return y1*(-t+1.0f)+y2*t; }
__forceinline vec3 lerp( const vec3& y1, const vec3& y2, float t ){ return y1*(-t+1.0f)+y2*t; }
__forceinline vec4 lerp( const vec4& y1, const vec4& y2, float t ){ return y1*(-t+1.0f)+y2*t; }
__forceinline vec2 lerp( const vec2& y1, const vec2& y2, const vec2& t ){ return y1*(-t+1.0f)+y2*t; }
__forceinline vec3 lerp( const vec3& y1, const vec3& y2, const vec3& t ){ return y1*(-t+1.0f)+y2*t; }
__forceinline vec4 lerp( const vec4& y1, const vec4& y2, const vec4& t ){ return y1*(-t+1.0f)+y2*t; }
__forceinline double lerp( double v1, double v2, double t ){ return v1*(1.0-t)+v2*t; }
__forceinline double lerp( double v1, double v2, float t ){ return v1*(1.0f-t)+v2*t; }
__forceinline dvec2 lerp( const dvec2& y1, const dvec2& y2, double t ){ return y1*(-t+1.0)+y2*t; }
__forceinline dvec3 lerp( const dvec3& y1, const dvec3& y2, double t ){ return y1*(-t+1.0)+y2*t; }
__forceinline dvec4 lerp( const dvec4& y1, const dvec4& y2, double t ){ return y1*(-t+1.0)+y2*t; }
__forceinline dvec2 lerp( const dvec2& y1, const dvec2& y2, float t ){ return y1*(-t+1.0)+y2*t; }
__forceinline dvec3 lerp( const dvec3& y1, const dvec3& y2, float t ){ return y1*(-t+1.0)+y2*t; }
__forceinline dvec4 lerp( const dvec4& y1, const dvec4& y2, float t ){ return y1*(-t+1.0)+y2*t; }
__forceinline mat4 lerp( const mat4& v1, const mat4& v2, float t ){ return v1*(1.0f-t)+v2*t; }

//*******************************************************************
// spline interpolations
template <class T> T hermite( T v0, T v1, T v2, T v3, double t, double tension=0.5, double bias=0.0, double continuity=-0.5 )
{
	// REF: http://en.wikipedia.org/wiki/Kochanek%E2%80%93Bartels_spline
	// parameters[-1,1]: tension (round--tight), bias (postshoot--preshoot), continuity (box--inverted corners)
	double t2=t*t, t3=t*t*t, a[4] = { 2*t3-3*t2+1,t3-2*t2+t,t3-t2,-2*t3+3*t2 };
	T m0 = (v1-v0)*(1+bias)*(1-tension)*(1+continuity)*0.5 + (v2-v1)*(1-bias)*(1-tension)*(1-continuity)*0.5;
	T m1 = (v2-v1)*(1+bias)*(1-tension)*(1-continuity)*0.5 + (v3-v2)*(1-bias)*(1-tension)*(1+continuity)*0.5;
	return v1*a[0]+m0*a[1]+m1*a[2]+v2*a[3];
}

template <class T> T catmullRom( T v0, T v1, T v2, T v3, double t )
{
	// REF: http://en.wikipedia.org/wiki/B-spline
	double t2=t*t, t3=t*t*t, a[4] = { -t3+2*t2-t, 3*t3-5*t2+2, -3*t3+4*t2+t, t3-t2 };
	return (v0*a[0]+v1*a[1]+v2*a[2]+v3*a[3])*0.5;
}

template <class T> T bezier( T v0, T v1, T v2, T v3, double t )
{
	// REF: http://en.wikipedia.org/wiki/Bezier_curve
	double t2=(1.0-t), a[4] = { t2*t2*t2, 3*t2*t2*t, 3*t2*t*t, t*t*t };
	return (v0*a[0]+v1*a[1]+v2*a[2]+v3*a[3]);
}

//*******************************************************************
// pseudo random number generator
__forceinline uint& pseed(){ static uint seed=0; return seed; }
__forceinline void sprand( uint seed ){ pseed()=seed; }
__forceinline uint urand(){ pseed() = pseed()*214013L+2531011L; return ((pseed()>>16)&0x7fff); }
__forceinline float prand(){ return urand()/float(RAND_MAX); }
__forceinline vec2 prand2(){ return vec2(prand(),prand()); }
__forceinline vec3 prand3(){ return vec3(prand(),prand(),prand()); }
__forceinline vec4 prand4(){ return vec4(prand(),prand(),prand(),prand()); }

//*******************************************************************
#pragma warning( pop )		// restore the waring : 4244

//*******************************************************************
#endif __GX_MATH__