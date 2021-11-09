#pragma once
#ifndef __GXML_H__
#define __GXML_H__

#include <gxut/gxfilesystem.h>
#include <gxut/gxstring.h>
#include <gxut/gxmath.h>

//***********************************************
namespace xml {
//***********************************************
struct node
{
public:
	struct less { bool operator()(const std::wstring& a,const std::wstring& b)const{return _wcsicmp(a.c_str(),b.c_str())<0;}};
	using attrib_map_t = std::map<std::wstring,std::wstring,node::less>;
	using iterator = std::vector<node*>::iterator;
	using const_iterator = std::vector<node*>::const_iterator;

protected:
	std::wstring		_name;
	std::wstring		_value;
	attrib_map_t		_attrib_map;
	bool				_multi_line;
	bool				_as_cdata;
	node*				_parent;
	std::vector<node*>	_childs;

public:
	node();
	node( const wchar_t* name );
	node( const wchar_t* name, unsigned int len );
	node( const wchar_t* name, const wchar_t* value );
	virtual ~node(){}

	node* add_child( node* n );
	node* add_child( const wchar_t* name );
	node* add_child_after( node* n, node* after );
	node* add_child_before( node* n, node* before );
	node* create_child( const wchar_t* name );
	node* create_child( const wchar_t* name, const wchar_t* value );
	void  remove_child( node* n );
	void  remove_childs();
	void  sort_child_by_name();
	void  sort_child_by_attrib( const wchar_t* attr );
	void  sort_child_by_name_attrib( const wchar_t* attr );
	node* clone();

	// iterator
	bool empty() const { return _childs.empty(); }
	size_t size() const { return _childs.size(); }
	iterator begin(){ return _childs.begin(); }
	iterator end(){ return _childs.end(); }
	const_iterator begin() const { return _childs.begin(); }
	const_iterator end() const { return _childs.end(); }
	node* front(){ return _childs.front(); }
	node* back(){ return _childs.back(); }

	// set interface
	void set_name( const wchar_t* name );
	void set_attrib( const wchar_t* name, const wchar_t* value );
	void set_value( const wchar_t* value );
	void clear_value();
	void set_as_cdata( bool b=true ){ _as_cdata=b; }
	void remove_attrib( const wchar_t* name );

	// get interface (except value)
	inline const wchar_t* name() const { return _name.c_str(); }
	const wchar_t* attrib( const wchar_t* name );
	inline node* parent() const { return _parent; }
	inline attrib_map_t& attrib_map(){ return _attrib_map; }
	inline size_t child_count() const { return _childs.size(); }
	inline size_t attrib_count() const { return _attrib_map.size(); }

	// get child
	node* child( int idx ){ return idx<0||idx>=int(_childs.size())?nullptr:_childs[idx]; }
	node* child( const wchar_t* childName ){ for(uint k=0;k<_childs.size();k++) if(_wcsicmp(childName,_childs[k]->name())==0) return _childs[k]; return nullptr; }
	node* child_by_attrib( const wchar_t* attribName, const wchar_t* attribValue ){ for(uint k=0;k<_childs.size();k++) if(_wcsicmp(_childs[k]->attrib(attribName),attribValue)==0) return _childs[k]; return nullptr; }
	node* child_by_name_attrib( const wchar_t* childName, const wchar_t* attribName, const wchar_t* attribValue ){ for(uint k=0;k<_childs.size();k++) if(_wcsicmp(childName,_childs[k]->name())==0&&_wcsicmp(_childs[k]->attrib(attribName),attribValue)==0) return _childs[k]; return nullptr; }

	// get child list
	inline std::vector<node*>& childs(){ return _childs; }
	inline std::vector<node*> childs( const wchar_t* name_filter=nullptr ) const { if(name_filter==nullptr||name_filter[0]==0) return _childs; std::vector<node*> v; for(size_t k=0,kn=_childs.size();k<kn;k++) if(_wcsicmp(name_filter,_childs[k]->name())==0) v.emplace_back(_childs[k]); return v; }

	// overloading add_child() by type
	node* create_child( const wchar_t* name, bool value );
	node* create_child( const wchar_t* name, int value );
	node* create_child( const wchar_t* name, unsigned int value );
	node* create_child( const wchar_t* name, float value );
	node* create_child( const wchar_t* name, const float2& value );
	node* create_child( const wchar_t* name, const float3& value );
	node* create_child( const wchar_t* name, const float4& value );
	node* create_child( const wchar_t* name, const mat2& value );
	node* create_child( const wchar_t* name, const mat3& value );
	node* create_child( const wchar_t* name, const mat4& value );

	// get value by type
	inline std::wstring& value(){	return _value; }
	inline const wchar_t* c_str() const { return _value.c_str(); }

	// value casting
	template <class T> inline T value_cast() const;
	template<> inline bool		value_cast<bool>() const {	return _wtoi(_value.c_str())!=0; }
	template<> inline int		value_cast<int>() const {	return _wtoi(_value.c_str()); }
	template<> inline uint		value_cast<uint>() const {	return uint(_wtoi(_value.c_str())); }
	template<> inline float		value_cast<float>() const {	return float(_wtof(_value.c_str())); }
	template<> inline float2	value_cast<float2>() const {float2 v;	swscanf( _value.c_str(), L"%f %f", &v[0], &v[1] ); return v; }
	template<> inline float3	value_cast<float3>() const {float3 v;	swscanf( _value.c_str(), L"%f %f %f", &v[0], &v[1], &v[2] ); return v; }
	template<> inline float4	value_cast<float4>() const {float4 v;	swscanf( _value.c_str(), L"%f %f %f %f", &v[0], &v[1], &v[2], &v[3] ); return v; }
	template<> inline mat2		value_cast<mat2>() const {	mat2 m;		swscanf( _value.c_str(), L"%f %f %f %f", &m._11, &m._12, &m._21, &m._22 ); return m; }
	template<> inline mat3		value_cast<mat3>() const {	mat3 m;		swscanf( _value.c_str(), L"%f %f %f %f %f %f %f %f %f", &m._11, &m._12, &m._13, &m._21, &m._22, &m._23, &m._31, &m._32, &m._33 ); return m; }
	template<> inline mat4		value_cast<mat4>() const {	mat4 m;		swscanf( _value.c_str(), L"%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f", &m._11, &m._12, &m._13, &m._14, &m._21, &m._22, &m._23, &m._24, &m._31, &m._32, &m._33, &m._34, &m._41, &m._42, &m._43, &m._44 ); return m; }

	// value casting by operator overloading
	operator bool() const {		return value_cast<bool>(); }
	operator int() const {		return value_cast<int>(); }
	operator float() const {	return value_cast<float>(); }
	operator float2() const {	return value_cast<float2>(); }
	operator float3() const {	return value_cast<float3>(); }
	operator float4() const {	return value_cast<float4>(); }
	operator mat3() const {		return value_cast<mat3>(); }
	operator mat4() const {		return value_cast<mat4>(); }

	// print
	void begin_print( FILE* fp, int depth, const wchar_t* indent );
	void end_print( FILE* fp, int depth, const wchar_t* indent );

	// traversal with callback
	void traverse( void(*callback_func)(node*) );
	void traverse_if( bool(*callback_func)(node*) );			// while callback function returns true, depth-first-traverse the nodes
	void traverse_remove_child_if( bool(*callback_func)(node*) );		// if callback function returns true, then remove the nodes
};

//***********************************************
struct parser : public node
{
protected:
	float	_load_time;
	float	_save_time;
	bool	_b_use_bom = false;
	std::wstring	_indent = L"\t";
	std::wstring	_file_path;

public:
	parser(){}
	parser( const wchar_t* name ):node(name){}
	~parser(){ release(this); }

	void release( node* n );
	void load( const wchar_t* file_path );
	void load( const void* memData, size_t length );
	void save(){ save_as( _file_path.c_str() ); }
	void save_as( const wchar_t* file_path );

	// get interfaces
	inline float load_time() const { return _load_time; }
	inline float save_time() const { return _save_time; }
	inline bool get_use_bom() const { return _b_use_bom; }
	inline const wchar_t* get_indent() const { return _indent.c_str(); }

	// set interfaces
	inline void set_use_bom( bool b ) { _b_use_bom=b; }
	inline void set_indent( const wchar_t* i ){ _indent=i; }
};

//***********************************************
// node finder
struct finder
{
	std::vector<node*> ptr;
	using iterator = std::vector<node*>::iterator;
	using const_iterator = std::vector<node*>::const_iterator;

	// constructors
	inline finder( node* n=nullptr ){ if(n) ptr.emplace_back(n); }

	// query
	inline bool exists(){ return !ptr.empty(); }
	inline bool empty(){ return ptr.empty(); }
	inline size_t size(){ return ptr.size(); }

	// iterator
	inline iterator begin(){ return ptr.begin(); }
	inline iterator end(){ return ptr.end(); }
	inline const_iterator begin() const { return ptr.begin(); }
	inline const_iterator end() const { return ptr.end(); }

	// clear/erase
	inline void clear(){ ptr.clear(); }

	// casting/aceess operators
	inline operator node* () const { return ptr.empty()?nullptr:ptr.front(); }
	inline node* operator->() const { return ptr.empty()?nullptr:ptr.front(); }
	inline node* operator[]( size_t idx ) const { return idx<ptr.size()?ptr[idx]:nullptr; }
	inline node* operator[]( int idx ) const { return idx<int(ptr.size())?ptr[idx]:nullptr; }

	// get/set value
	inline const wchar_t* c_str() const { return ptr.empty()?L"":ptr.front()->c_str(); }
	inline void set_value( const wchar_t* v ){ if(!ptr.empty()) ptr.front()->set_value(v); }

	// find by child index or name
	inline finder find( size_t child_index ){ finder f(nullptr); node* n=operator->(); if(n&&n->childs().size()>child_index) f.ptr.emplace_back(n->childs()[child_index]); return f; }
	inline finder find( const wchar_t* child_name ){ finder f(nullptr); for(size_t k=0,kn=ptr.size();k<kn;k++){ auto& c=ptr[k]->childs(); for(size_t j=0,jn=c.size();j<jn;j++) if(_wcsicmp(child_name,c[j]->name())==0) f.ptr.emplace_back(c[j]); } return f; }
	inline finder find( const wchar_t* child_name, const wchar_t* attrib_name0, const wchar_t* attrib_value0, const wchar_t* attrib_name1=nullptr, const wchar_t* attrib_value1=nullptr, const wchar_t* attrib_name2=nullptr, const wchar_t* attrib_value2=nullptr );
};

inline finder finder::find( const wchar_t* child_name, const wchar_t* an0, const wchar_t* av0, const wchar_t* an1, const wchar_t* av1, const wchar_t* an2, const wchar_t* av2 )
{
	finder f(nullptr);
	for(uint k=0,kn=uint(ptr.size());k<kn;k++)
	for(uint j=0,jn=uint(ptr[k]->child_count());j<jn;j++)
	{
		auto* c=ptr[k]->child(int(j));
		if(_wcsicmp(child_name,c->name())!=0) continue;
		if(an0&&av0&&_wcsicmp(c->attrib(an0),av0)!=0) continue;
		if(an1&&av1&&_wcsicmp(c->attrib(an1),av1)!=0) continue;
		if(an2&&av2&&_wcsicmp(c->attrib(an2),av2)!=0) continue;
		f.ptr.emplace_back(c);
	}
	return f;
}

//***********************************************
} // end namespace
//***********************************************

#endif // __GXML_H__
