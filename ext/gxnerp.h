#ifndef __GXNERP_H__
#define __GXNERP_H__

#include "gxquat.h"	// quaternion

// view matrix analysis
inline vec3 extract_eye( const mat4& m ){ return mat3(m).transpose()*(-vec3(m._03,m._13,m._23)); }
inline std::pair<vec3,vec3> extract_eyes( const mat4& m0, const mat4& m1 ){ return {extract_eye(m0),extract_eye(m1)}; }
inline std::tuple<vec3,vec3,vec3,vec3> extract_eyes( const mat4& m0, const mat4& m1, const mat4& m2, const mat4& m3 ){ return {extract_eye(m0),extract_eye(m1),extract_eye(m2),extract_eye(m3)}; }

inline mat4& assign_eyes( mat4& r, const vec3& eye )
{
	// re-translate the eye
	r._03 = -eye.dot(r.rvec(0).xyz);
	r._13 = -eye.dot(r.rvec(1).xyz);
	r._23 = -eye.dot(r.rvec(2).xyz);
	return r;
}

//*************************************
// spherical interpolation for four vector
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
	auto eyes=extract_eyes(m1,m2);
	vec3 e=lerp(std::get<0>(eyes),std::get<1>(eyes),t);
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

// string interpolation
template <class T> T lerp( const char* s1, const char* s2, float t ){ return lerp(atof<T>(s1),atof<T>(s2),t); }
template <class T> T hermite( const char* s0, const char* s1, const char* s2, const char* s3, float t ){ return hermite(atof<T>(s0),atof<T>(s1),atof<T>(s2),atof<T>(s3),t,0,0,0); }//,0f,1.0f,-1.0f); }
template <class T> T slerp( const char* s1, const char* s2, float t ){ return slerp(atof<T>(s1),atof<T>(s2),t); }
template <class T> T squad( const char* s0, const char* s1, const char* s2, const char* s3, float t ){ return squad(atof<T>(s0),atof<T>(s1),atof<T>(s2),atof<T>(s3),t); }
template <class T> T shermite( const char* s0, const char* s1, const char* s2, const char* s3, float t ){ return shermite(atof<T>(s0),atof<T>(s1),atof<T>(s2),atof<T>(s3),t); }

//*************************************
#endif
