#ifndef __GXNERP_H__
#define __GXNERP_H__

#include "gxquat.h"	// quaternion

//*************************************
// interpolate eye
inline std::tuple<vec3,vec3,vec3,vec3> extract_eyes( const mat4& m0, const mat4& m1, const mat4& m2, const mat4& m3 )
{
	vec3 e0 = mat3(m0).transpose()*(-vec3(m0._14,m0._24,m0._34));
	vec3 e1 = mat3(m1).transpose()*(-vec3(m1._14,m1._24,m1._34));
	vec3 e2 = mat3(m2).transpose()*(-vec3(m2._14,m2._24,m2._34));
	vec3 e3 = mat3(m3).transpose()*(-vec3(m3._14,m3._24,m3._34));
	return std::tuple<vec3,vec3,vec3,vec3>{e0,e1,e2,e3};
}

inline mat4& assign_eyes( mat4& r, const vec3& eye )
{
	// re-translate the eye
	r._14 = -eye.dot(r.v[0].xyz);
	r._24 = -eye.dot(r.v[1].xyz);
	r._34 = -eye.dot(r.v[2].xyz);
	return r;
}

//*************************************
// spherical interpolation for four vec3
inline vec3 slerp( vec3 v1, vec3 v2, float t )
{
	float len1 = v1.length(), len2=v2.length();
	vec3 w1	= v1/len1, w2 = v2/len2;
	mat4 r = slerp(quat(),quat(w1,w2),t,true);
	return (r*w1)*lerp(len1,len2,t);
}

inline vec3 squad( vec3 v0, vec3 v1, vec3 v2, vec3 v3, float t )
{
	float len0=v0.length(), len1=v1.length(), len2=v2.length(), len3=v3.length();
	vec3 w0=v0/len0, w1=v1/len1, w2=v2/len2, w3=v3/len3;
	if(cross(v1/len1,v2/len2).length()<0.001f) return lerp(v1/len1,v2/len2,t)*hermite(len0,len1,len2,len3,t,0.0f,0.0f,0.0f);
	mat4 r = squad( quat(w1,w0), quat(), quat(w1,w2), quat(w1,w3), t );
	return (r*w1)*hermite(len0,len1,len2,len3,t);
}

inline vec3 shermite( vec3 v0, vec3 v1, vec3 v2, vec3 v3, float t )
{
	// need to implement correctly using quaternion hermite interpolation
	return squad(v0,v1,v2,v3,t);
}

//*************************************
// spherical interpolation for four mat4
inline mat4 slerp( const mat4& m1, const mat4& m2, float t )
{
	mat4 m=slerp(quat(m1),quat(m2),t,true);
	auto eyes=extract_eyes(m1,m1,m2,m2);
	vec3 e=lerp(std::get<1>(eyes),std::get<2>(eyes),t);
	return assign_eyes(m,e);
}

inline mat4 squad( const mat4& m0, const mat4& m1, const mat4& m2, const mat4& m3, float t )
{
	mat4 m=squad(quat(m0),quat(m1),quat(m2),quat(m3),t);
	auto eyes=extract_eyes(m0,m1,m2,m3);
	vec3 e=hermite<vec3>(std::get<0>(eyes),std::get<1>(eyes),std::get<2>(eyes),std::get<3>(eyes),t);
	return assign_eyes(m,e);
}

inline mat4 shermite( const mat4& m0, const mat4& m1, const mat4& m2, const mat4& m3, float t )
{
	mat4 m=shermite(quat(m0),quat(m1),quat(m2),quat(m3),t);
	auto eyes=extract_eyes(m0,m1,m2,m3);
	vec3 e=hermite<vec3>(std::get<0>(eyes),std::get<1>(eyes),std::get<2>(eyes),std::get<3>(eyes),t);
	return assign_eyes(m,e);
}

//*************************************
// string interpolation

template <class T> T _atof( const char* s ){		return T(); }
template <> float _atof<float>( const char* s ){	return float(atof(s)); }
template <> double _atof<double>( const char* s ){	return double(atof(s)); }
template <> vec2 _atof<vec2>( const char* s ){		return reinterpret_cast<vec2&>(atof2(s)); }
template <> vec3 _atof<vec3>( const char* s ){		return reinterpret_cast<vec3&>(atof3(s)); }
template <> vec4 _atof<vec4>( const char* s ){		return reinterpret_cast<vec4&>(atof4(s)); }
template <> dvec2 _atof<dvec2>( const char* s ){	return reinterpret_cast<dvec2&>(atod2(s)); }
template <> dvec3 _atof<dvec3>( const char* s ){	return reinterpret_cast<dvec3&>(atod3(s)); }
template <> dvec4 _atof<dvec4>( const char* s ){	return reinterpret_cast<dvec4&>(atod4(s)); }
template <> mat4 _atof<mat4>( const char* s ){		return mat4(atof16(s)); }

template <class T> T lerp( const char* s1, const char* s2, float t ){ return lerp(_atof<T>(s1),_atof<T>(s2),t); }
template <class T> T hermite( const char* s0, const char* s1, const char* s2, const char* s3, float t ){ return hermite(_atof<T>(s0),_atof<T>(s1),_atof<T>(s2),_atof<T>(s3),t,0,0,0); }//,0f,1.0f,-1.0f); }
template <class T> T slerp( const char* s1, const char* s2, float t ){ return slerp(_atof<T>(s1),_atof<T>(s2),t); }
template <class T> T squad( const char* s0, const char* s1, const char* s2, const char* s3, float t ){ return squad(_atof<T>(s0),_atof<T>(s1),_atof<T>(s2),_atof<T>(s3),t); }
template <class T> T shermite( const char* s0, const char* s1, const char* s2, const char* s3, float t ){ return shermite(_atof<T>(s0),_atof<T>(s1),_atof<T>(s2),_atof<T>(s3),t); }

//*************************************
#endif
