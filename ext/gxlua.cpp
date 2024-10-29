#include "gxlua.h"

//*************************************
namespace sol::detail {
//*************************************
SOL_DEMANGLE_ONCE(vec2)
SOL_DEMANGLE_ONCE(vec3)
SOL_DEMANGLE_ONCE(vec4)
SOL_DEMANGLE_ONCE(ivec2)
SOL_DEMANGLE_ONCE(ivec3)
SOL_DEMANGLE_ONCE(ivec4)
SOL_DEMANGLE_ONCE(uvec2)
SOL_DEMANGLE_ONCE(uvec3)
SOL_DEMANGLE_ONCE(uvec4)
//*************************************
} // namespace sol::detail
//*************************************

template <class V, class U>
auto register_signed_functions( U& u )
{
	u[sol::meta_function::unary_minus] = sol::resolve<V()const>(&V::operator-);
}

template <class V, class U>
auto register_operators( U& u )
{
	u[sol::meta_function::length]			= &V::size;
	u[sol::meta_function::addition]			= sol::resolve<V(const V&)const>(&V::operator+);
	u[sol::meta_function::subtraction]		= sol::resolve<V(const V&)const>(&V::operator-);
	u[sol::meta_function::multiplication]	= sol::resolve<V(const V&)const>(&V::operator*);
	u[sol::meta_function::division]			= sol::resolve<V(const V&)const>(&V::operator/);
}

template <class V, typename T=V::value_type>
auto register_tvec2( lua::state& t )
{
	sol::constructors<V(),V(const V&),V(T),V(T,T)> ctor;
	auto u=t.new_usertype<V>(sol::detail::demangle_once<V>(), ctor);
	u["x"] = &V::x;	u["r"] = &V::x;
	u["y"] = &V::y;	u["g"] = &V::y;
	u[sol::meta_function::to_string] = []( const V& v ){ return std::string(ntoa(v)); };
	u[sol::meta_function::concatenation] = &lua::concat;
	register_operators<V>( u );
	return std::move(u);
}

template <class V, typename T=V::value_type>
auto register_tvec3( lua::state& t )
{
	sol::constructors<V(),V(const V&),V(T),V(T,T,T)> ctor;
	auto u=t.new_usertype<V>(sol::detail::demangle_once<V>(), ctor);
	u["x"] = &V::x;	u["r"] = &V::x;
	u["y"] = &V::y;	u["g"] = &V::y;
	u["z"] = &V::z;	u["b"] = &V::z;
	u[sol::meta_function::to_string] = []( const V& v ){ return std::string(ntoa(v)); };
	u[sol::meta_function::concatenation] = &lua::concat;
	register_operators<V>( u );
	return std::move(u);
}

template <class V, typename T=V::value_type>
auto register_tvec4( lua::state& t )
{
	sol::constructors<V(),V(const V&),V(T),V(T,T,T,T)> ctor;
	auto u=t.new_usertype<V>(sol::detail::demangle_once<V>(), ctor);
	u["x"] = &V::x;	u["r"] = &V::x;
	u["y"] = &V::y;	u["g"] = &V::y;
	u["z"] = &V::z;	u["b"] = &V::z;
	u["w"] = &V::w;	u["a"] = &V::w;
	u[sol::meta_function::to_string] = []( const V& v ){ return std::string(ntoa(v)); };
	u[sol::meta_function::concatenation] = &lua::concat;
	register_operators<V>( u );
	return std::move(u);
}

#define register_float_functions_impl(V,T) \
	u["length2"]	= &V::length2<T,enable_float_t<T>>; \
	u["norm2"]		= &V::norm2<T,enable_float_t<T>>; \
	u["length"]		= &V::length<T,enable_float_t<T>>; \
	u["norm"]		= &V::norm<T,enable_float_t<T>>; \
	u["dot"]		= &V::dot<T,enable_float_t<T>>; \
	u["normalize"]	= &V::normalize<T,enable_float_t<T>>;

#define register_float_functions(V) register_float_functions_impl(V,V::value_type)

void lua::state::open_gxmath()
{
	auto& t = *this;

	{ auto u=register_tvec2<vec2>(t);	register_signed_functions<vec2>(u);		register_float_functions(vec2); }
	{ auto u=register_tvec3<vec3>(t);	register_signed_functions<vec3>(u);		register_float_functions(vec3); }
	{ auto u=register_tvec4<vec4>(t);	register_signed_functions<vec4>(u);		register_float_functions(vec4); }

	{ auto u=register_tvec2<ivec2>(t);	register_signed_functions<ivec2>(u); }
	{ auto u=register_tvec3<ivec3>(t);	register_signed_functions<ivec3>(u); }
	{ auto u=register_tvec4<ivec4>(t);	register_signed_functions<ivec4>(u); }

	{ auto u=register_tvec2<uvec2>(t); }
	{ auto u=register_tvec3<uvec3>(t); }
	{ auto u=register_tvec4<uvec4>(t); }

	// vector math functions
	t["radians"] = &radians<float>;
	t["degrees"] = &degrees<float>;
	t["reflect"] = &reflect;
	t["refract"] = &refract;
	t["lerp"] = sol::overload(sol::resolve<float(float,float,float)>(&lerp),sol::resolve<vec2(const vec2&,const vec2&,float)>(&lerp),sol::resolve<vec3(const vec3&,const vec3&,float)>(&lerp),sol::resolve<vec4(const vec4&,const vec4&,float)>(&lerp));
	
#define register4(L,F) \
	(L)[#F] = sol::overload(sol::resolve<float(float)>(&F),sol::resolve<vec2(const vec2&)>(&F),sol::resolve<vec3(const vec3&)>(&F),sol::resolve<vec4(const vec4&)>(&F))

	register4(t,saturate);
	register4(t,fract);
	register4(t,abs);
	register4(t,sign);
	register4(t,smoothstep);
	register4(t,smootherstep);
	
	// register matrix
}
