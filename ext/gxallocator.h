#pragma once
#include <malloc.h>

// allocator that uses the initial crt heap
template <class T> struct crt_allocator
{
	typedef T			value_type;
	typedef size_t		size_type;
	typedef ptrdiff_t	difference_type;
	typedef T*			pointer;
	typedef const T*	const_pointer;
	typedef T&			reference;
	typedef const T&	const_reference;
	const intptr_t		crt_heap;			// the heap created the member initially

	crt_allocator() : crt_heap(_get_heap_handle()){}
	template <class U> explicit crt_allocator (const crt_allocator<U>& other ) : crt_heap(_get_heap_handle()){}
	template <class U> struct rebind { typedef crt_allocator<U> other; };		// rebind allocator to type U 

	pointer address( reference value ) const { return &value; }
	const_pointer address( const_reference value ) const { return &value; }
	size_type max_size () const { return 0xFFFFFFFF/sizeof(T); }			// (1<<32) does not work
	void construct( pointer p, const T& value ){ new((void*)p)T(value); }
	void destroy (pointer p){ p->~T(); }
	pointer allocate( size_type n, const void* /*hint*/ = 0 ){ return (n>max_size()||n==0) ? nullptr : (T*) HeapAlloc( (HANDLE) crt_heap, HEAP_NO_SERIALIZE, n*sizeof(T) ); }
	void deallocate( pointer p, size_type n ){ if(p) HeapFree( (HANDLE) crt_heap, HEAP_NO_SERIALIZE, p ); }

	bool operator==( const crt_allocator<T>& other ){ return crt_heap==other.crt_heap; }
	bool operator!=( const crt_allocator<T>& other ){ return crt_heap!=other.crt_heap; }
};

template <class T, size_t N> struct block_allocator
{
	enum { block_allocator_thresh = 1}; // apply block allocator only for allocation with less than block_allocator_thresh
	struct file_map
	{
		size_t	capacity = 0;						// the whole capacity
		size_t	allocated = 0;						// current index
		void*	view = 0;							// the address of this mapping
		HANDLE	hFile = INVALID_HANDLE_VALUE;		// handle to a physical file
		HANDLE	hFileMap = INVALID_HANDLE_VALUE;	// handle to the file mapping
		size_t	num_chunks = 0;						// number of valid chunks
	};
	std::vector<file_map>	file_map_list;

	T* allocate( size_t n )
	{
		file_map* cfm = file_map_list.empty() ? nullptr : &file_map_list.back();
		if( cfm==nullptr || (cfm->allocated+sizeof(T)*n) > cfm->capacity )
		{
			// otherwise create a new file
			SYSTEMTIME s={}; GetSystemTime(&s); wchar_t fileName[1024]={}; wsprintf( fileName, L"fa%p%02d%02d%02d%02d%04d%05d", GetCurrentThreadId(), s.wDay, s.wHour, s.wMinute, s.wSecond, s.wMilliseconds, rand() ); // make unique file name
			path filePath=path::temp_dir()+L"gxallocator\\"+fileName; if(!filePath.dir().exists()) filePath.dir().mkdir();

			file_map fm={};
			fm.capacity = n>block_allocator_thresh ? n*sizeof(T) : N*n*sizeof(T);	// use block allocation only for small allocation
			fm.hFile = CreateFileW( filePath, GENERIC_READ|GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_FLAG_RANDOM_ACCESS|FILE_FLAG_DELETE_ON_CLOSE, nullptr ); if( fm.hFile==INVALID_HANDLE_VALUE ) return nullptr;
			fm.hFileMap = CreateFileMappingW( fm.hFile, nullptr, PAGE_READWRITE, 0, fm.capacity, fileName ); if( fm.hFileMap==INVALID_HANDLE_VALUE ) return nullptr;
			fm.view = MapViewOfFile( fm.hFileMap, FILE_MAP_ALL_ACCESS, 0, 0, fm.capacity ); if(fm.view==nullptr){ CloseHandle( fm.hFileMap ); fm.hFileMap = INVALID_HANDLE_VALUE; return nullptr; }

			// update the current records
			file_map_list.emplace_back( fm );
			cfm = &file_map_list.back();
		}

		T* ptr = (T*)((char*)cfm->view+cfm->allocated);
		cfm->num_chunks ++;
		cfm->allocated += n*sizeof(T);

		return ptr;
	}

	void deallocate( T* p, size_t n )
	{
		if(p==nullptr||n==0) return;

		// find the corresponding chunk
		file_map* pfm = nullptr;
		size_t pfm_index = -1;
		for( size_t k = 0; k<file_map_list.size(); k++ )
		{
			auto& fm = file_map_list[k];
			void* p1 = p+n;
			void* f1 = ((char*)fm.view)+fm.capacity;
			if( fm.view<=p && p1<=f1 ){ pfm_index=k; pfm = &fm; break; }
		}

		// decrese chunk counter
		if(pfm&&pfm->num_chunks>0) pfm->num_chunks--;

		// delete file
		if(pfm&&pfm->num_chunks==0&&pfm->view!=nullptr)
		{
			FlushViewOfFile( pfm->view, 0 );
			UnmapViewOfFile( pfm->view );
			if(pfm->hFileMap!=INVALID_HANDLE_VALUE) CloseHandle( pfm->hFileMap );
			if(pfm->hFile!=INVALID_HANDLE_VALUE) CloseHandle( pfm->hFile );
			file_map_list.erase( file_map_list.begin()+pfm_index);
		}
	}

	bool operator==( const block_allocator<T,N>& other ){ return file_map_list==other.file_map_list; }
	bool operator!=( const block_allocator<T,N>& other ){ return file_map_list!=other.file_map_list; }
};

// allocator that uses the memory-mapped file (MMF; mmap in posix)
template <class T, size_t N=1> struct file_allocator
{
	static block_allocator<T,N>& block(){ static block_allocator<T,N> ba; return ba; }

	typedef T			value_type;
	typedef size_t		size_type;
	typedef ptrdiff_t	difference_type;
	typedef T*			pointer;
	typedef const T*	const_pointer;
	typedef T&			reference;
	typedef const T&	const_reference;

	// constructors, destructor, and rebind
	template <class U> file_allocator ( const file_allocator<U,N>& other ) throw(){}
	template <class U> struct rebind { typedef file_allocator<U,N> other; };

	pointer address( reference value ) const { return &value; }
	const_pointer address( const_reference value ) const { return &value; }
	size_type max_size () const { return 0xFFFFFFFF/sizeof(T); }			// (1<<32) does not work
	void construct( pointer p, const T& value ){ new((void*)p)T(value); }
	void destroy (pointer p){ p->~T(); }

	// allocate/deallocate
	pointer allocate( size_type n, const void* /*hint*/ = 0 ){ return (n>max_size()||n==0) ? nullptr : block().allocate( n ); }
	void deallocate( pointer p, size_type n ){ block().deallocate( p, n ); }

	bool operator==( const file_allocator<T,N>& other ){ return block()==other.block(); }
	bool operator!=( const file_allocator<T,N>& other ){ return block()!=other.block(); }
};

//***********************************************
// allocator that uses the memory-mapped file (MMF; mmap in posix)
template <class T> struct file_allocator_single
{
	typedef T			value_type;
	typedef size_t		size_type;
	typedef ptrdiff_t	difference_type;
	typedef T*			pointer;
	typedef const T*	const_pointer;
	typedef T&			reference;
	typedef const T&	const_reference;

	// constructors, destructor, and rebind
	file_allocator_single() : hFileMap( INVALID_HANDLE_VALUE ){}
	~file_allocator_single(){ if( hFileMap!=INVALID_HANDLE_VALUE ){ CloseHandle( hFileMap ); hFileMap = INVALID_HANDLE_VALUE; } }
	template <class U> explicit file_allocator_single( const file_allocator_single<U>& other ) : hFileMap( INVALID_HANDLE_VALUE ){ if( other.hFileMap!=this->hFileMap ) DuplicateHandle( GetCurrentProcess(), other.hFileMap, GetCurrentProcess(), &this->hFileMap, 0, FALSE, DUPLICATE_SAME_ACCESS ); }
	template <class U> struct rebind { typedef file_allocator_single<U> other; };

	pointer address( reference value ) const { return &value; }
	const_pointer address( const_reference value ) const { return &value; }
	size_type max_size() const { return (0xFFFFFFFF/sizeof(T))-1; }
	void construct( pointer p, const T& value ){ new((void*)p)T( value ); }
	void destroy( pointer p ){ p->~T(); }

	static const wchar_t* _uname(){ static wchar_t fileName[1024]; SYSTEMTIME s; GetSystemTime( &s ); wsprintf( fileName, L"ma%p%02d%02d%02d%02d%04d%05d", GetCurrentThreadId(), s.wDay, s.wHour, s.wMinute, s.wSecond, s.wMilliseconds, rand() ); return fileName; } // make unique file name
	pointer allocate( size_type n, const void* /*hint*/ = 0 ){ if(n==0) return nullptr; hFileMap = CreateFileMappingW( INVALID_HANDLE_VALUE /* pagefile */, nullptr, PAGE_READWRITE, 0, n*sizeof(T), _uname() ); if( hFileMap==INVALID_HANDLE_VALUE ) return nullptr; return (T*)MapViewOfFile( hFileMap, FILE_MAP_READ|FILE_MAP_WRITE, 0, 0, n*sizeof(T) ); }
	void deallocate( pointer p, size_type n ){ if(p){FlushViewOfFile(p,0);UnmapViewOfFile(p);} if(hFileMap!=INVALID_HANDLE_VALUE){CloseHandle(hFileMap );hFileMap=INVALID_HANDLE_VALUE;}}

	bool operator==( const file_allocator_single<T>& other ){ return hFileMap==other.hFileMap; }
	bool operator!=( const file_allocator_single<T>& other ){ return hFileMap!=other.hFileMap; }

	HANDLE	hFileMap;	// handle to the file mapping (using pagefile)
};
