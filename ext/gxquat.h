#ifndef __GXQUAT_H__
#define __GXQUAT_H__

#include <gxut/gxmath.h>

//*************************************
struct quat
{
	float w; union { struct { vec3 v; }; struct { float x, y, z; }; }; // scalar-first representation

	// constructors
	__forceinline quat(){w=1.0f;x=y=z=0.0f;}
	__forceinline quat( quat&& q ) = default;
	__forceinline quat( const quat& q ):quat(q.w,q.x,q.y,q.z){}
	__forceinline quat( float w, float x, float y, float z ){ this->w=w; this->x=x; this->y=y; this->z=z; }
	__forceinline quat( float w, const vec3& v ):quat(w,v.x,v.y,v.z){}
	__forceinline quat( const mat4& m ){ operator=(m); }
	__forceinline quat( const vec3& v1, const vec3& v2 ):quat()	// rotation from v1 to v2
	{
		vec3 n12 = v1.cross(v2); float n12_len = n12.length();
		if(n12_len>0.0001f){ float tcos = v1.normalize().dot(v2.normalize()); v=n12/n12_len*sqrt(0.5f*(1.0f-tcos)); w=sqrt(0.5f*(1.0f+tcos)); }
	}

	// assignments
	__forceinline quat& operator=( quat&& q ) = default;
	__forceinline quat& operator=( const quat& q ){ memcpy(this,&q,sizeof(q)); return *this; }
	__forceinline quat& operator+=( const quat& q ){ return *this=operator+(q); }
	__forceinline quat& operator-=( const quat& q ){ return *this=operator-(q); }
	__forceinline quat& operator*=( const quat& q ){ return *this=operator*(q); }
	__forceinline quat& operator/=( const quat& q ){ return *this=operator/(q); }

	// scalar operators
	__forceinline quat operator*( float s ) const { return quat( w*s, v*s ); }	// scalar multiplication
	__forceinline quat operator/( float s ) const { return quat( w/s, v/s ); }	// scalar division

	// quat unary operators
	__forceinline quat operator+() const { return *this; }
	__forceinline quat operator-() const { return quat(-w,-v); }

	// quat binary operators
	__forceinline quat operator+( const quat& q ) const { return quat(w+q.w,v+q.v); }
	__forceinline quat operator-( const quat& q ) const { return quat(w-q.w,v-q.v); }
	__forceinline quat operator*( const quat& q ) const { return quat(w*q.w-v.dot(q.v),v*q.w+q.v*w+v.cross(q.v)); }
	__forceinline quat operator/( const quat& q ) const { return *this*q.inverse(); }

	// dot product
	__forceinline float dot( const quat& q ) const { return v.dot(q.v)+w*q.w; }

	// norm/length
	__forceinline float length() const { return sqrtf(x*x+y*y+z*z+w*w); }
	__forceinline float length2() const { return x*x+y*y+z*z+w*w; }
	__forceinline float norm() const { return sqrtf(x*x+y*y+z*z+w*w); }
	__forceinline float norm2() const { return x*x+y*y+z*z+w*w; }
	__forceinline quat normalize() const { float l=1.0f/norm(); return quat(w*l,x*l,y*l,z*l); }

	// conjugate/inverse
	__forceinline quat conjugate() const { return quat(w,-v); }
	__forceinline quat inverse() const { return conjugate()/norm2(); }

	// conversion with row-major rotation matrix
	__forceinline quat& operator=( const mat4& m ) // convert row-major rotation matrix to quaternion
	{
		float trace = m._00+m._11+m._22;
		if( trace>0.0000001f ){				float s=sqrtf(trace+1.0f)*2.0f;				return *this=quat( 0.25f*s, (m._21-m._12)/s, (m._02-m._20)/s, (m._10-m._01)/s ); }
		else if(m._00>m._11&&m._00>m._22){	float s=sqrtf(1.0f+m._00-m._11-m._22)*2.0f;	return *this=quat( (m._21-m._12)/s, 0.25f*s, (m._10+m._01 )/s, (m._01+m._20)/s ); }
		else if (m._11>m._22){				float s=sqrtf(1.0f+m._11-m._00-m._22)*2.0f;	return *this=quat( (m._02-m._20)/s, (m._10+m._01 )/s, 0.25f*s, (m._21+m._12)/s ); }
		else{								float s=sqrtf(1.0f+m._22-m._00-m._11)*2.0f;	return *this=quat( (m._10-m._01)/s, (m._02+m._20)/s, (m._12+m._21)/s, 0.25f*s ); }
	}

	__forceinline operator mat3 () const
	{
		return mat3	(1.0f-(2.0f*y*y)-(2.0f*z*z),	(2.0f*x*y)-(2.0f*w*z),		(2.0f*x*z)+(2.0f*w*y),
					(2.0f*x*y)+(2.0f*w*z),			1.0f-(2.0f*x*x)-(2.0f*z*z),	(2.0f*y*z)-(2.0f*w*x),
					(2.0f*x*z)-(2.0f*w*y),			(2.0f*y*z)+(2.0f*w*x),		1.0f-(2.0f*x*x)-(2.0f*y*y) );
	}

	__forceinline operator mat4 () const
	{
		return mat4	(1.0f-(2.0f*y*y)-(2.0f*z*z),	(2.0f*x*y)-(2.0f*w*z),		(2.0f*x*z)+(2.0f*w*y),		0,
					(2.0f*x*y)+(2.0f*w*z),			1.0f-(2.0f*x*x)-(2.0f*z*z),	(2.0f*y*z)-(2.0f*w*x),		0,
					(2.0f*x*z)-(2.0f*w*y),			(2.0f*y*z)+(2.0f*w*x),		1.0f-(2.0f*x*x)-(2.0f*y*y),	0,
					0,								0,							0,							1.0f );
	}
};

//*************************************
// scalar/quaternion products
__forceinline float dot( const quat& p, const quat& q ){ return p.dot(q); }

//*************************************
// quaternion maths
__forceinline quat exp( const quat& q ){ float s=q.v.norm(); return s<0.0001f?quat(0,0,0,1):quat(q.v/s*sin(s),cos(s))*exp(q.w); }
__forceinline quat log( const quat& q ){ float s=q.v.norm(); return s<0.0001f?quat(0,0,0,0):quat(q.v/s*atan2(s,q.w),log(q.norm())); }	// natural logarithm: atan2(s,q.w)==acos(s/q.norm())
__forceinline quat pow( const quat& q, float exponent ){ return exp(log(q)*exponent); }

//*************************************
// quaternion interpolation
__forceinline quat lerp( const quat& q1, const quat & q2, float t ){ return q1*(1-t)+q2*t; }
__forceinline quat spline( const quat& q0, const quat& q1, const quat & q2 ){ quat i1=q1.inverse(); return (q1*exp((log(i1*q0)+log(i1*q2))*(-0.25f))).normalize(); } // also called the inner_quad_point()
__forceinline quat slerp( const quat& q1, const quat q2, float t, bool invert=false ){ quat q2_=q2; float d=q1.dot(q2_); if(invert&&d<0.0f){d=-d;q2_=-q2_;} float a=acos(d); return (d<-0.98f||d>0.98f)?lerp(q1,q2,t).normalize():((q1*sin(a*(1.0f-t))+q2_*sin(a*t))/sin(a)).normalize(); }
__forceinline quat squad( const quat& q0, const quat& q1, const quat& q2, const quat& q3, float t ){ quat s1=spline(q0,q1,q2), s2=spline(q1,q2,q3); return slerp( slerp(q1,q2,t), slerp(s1,s2,t), 2.0f*t*(1.0f-t) ); }
__forceinline quat shermite( const quat& q0, const quat& q1, const quat& q2, const quat& q3, float t, double tension=0.5, double bias=0.0, double continuity=-0.5 ){ return squad(q0,q1,q2,q3,t); } // need to implement correctly

//*************************************
// vector interpolation
__forceinline vec3 slerp( vec3 v1, vec3 v2, float t ){ float l1=v1.length(), l2=v2.length(); vec3 w1=v1/l1,w2=v2/l2; mat4 r = slerp(quat(),quat(w1,w2),t,true); return (r*w1)*lerp(l1,l2,t); }

//*************************************
#endif
