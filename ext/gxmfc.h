#pragma once
#include <Shlobj.h>

// modified macro for inline message map
#if _MSC_VER >= 1911 // VS2017 Preview
	#define BEGIN_MESSAGE_MAP_INLINE(theClass, baseClass) \
	PTM_WARNING_DISABLE \
	const AFX_MSGMAP* GetMessageMap() const { return GetThisMessageMap(); } \
	static const AFX_MSGMAP* PASCAL GetThisMessageMap() \
	{ \
		typedef theClass ThisClass;						   \
		typedef baseClass TheBaseClass;					   \
		__pragma(warning(push))							   \
		__pragma(warning(disable: 4640)) /* message maps can only be called by single threaded message pump */ \
		static const AFX_MSGMAP_ENTRY _messageEntries[] =  \
		{
#else
	#define BEGIN_MESSAGE_MAP_INLINE(theClass, baseClass) \
	PTM_WARNING_DISABLE \
	virtual const AFX_MSGMAP* GetMessageMap() const { return GetThisMessageMap(); } \
	static const AFX_MSGMAP* PASCAL GetThisMessageMap() \
	{ \
		typedef theClass ThisClass;						   \
		typedef baseClass TheBaseClass;					   \
		static const AFX_MSGMAP_ENTRY _messageEntries[] =  \
		{
#endif

//*************************************
namespace mfc {
//*************************************

// utility functions
inline CRect get_client_rect( CWnd* w ){ RECT rc; w->GetClientRect( &rc ); return rc; }
inline CRect get_client_rect( CWnd& w ){ RECT rc; w.GetClientRect( &rc ); return rc; }
inline CRect get_client_rect( HWND h ){ RECT rc; GetClientRect( h, &rc ); return rc; }
inline CRect get_window_rect( CWnd* w ){ RECT rc; w->GetWindowRect( &rc ); return rc; }
inline CRect get_window_rect( CWnd& w ){ RECT rc; w.GetWindowRect( &rc ); return rc; }
inline CRect get_window_rect( HWND h ){ RECT rc; GetWindowRect( h, &rc ); return rc; }
inline CRect empty_rect(){ return CRect(0,0,0,0); }

// thread wrappers for MFC/AFX
inline CWinThread* begin_thread( AFX_THREADPROC proc, void* pUserData=nullptr, int priority=THREAD_PRIORITY_NORMAL, bool wait=false )
{
	CWinThread* pThread= AfxBeginThread(proc,pUserData,priority,0,wait?CREATE_SUSPENDED:0); if(!pThread) return nullptr;
	if(wait)
	{
		pThread->m_bAutoDelete=FALSE; pThread->ResumeThread();
		while(pThread&&WaitForSingleObject(pThread->m_hThread,0)!=WAIT_OBJECT_0){MSG m;for(int k=0;k<100&&PeekMessageW(&m,nullptr,0,0,PM_REMOVE);k++)SendMessage(m.hwnd,m.message,m.wParam,m.lParam);Sleep(1);}
		safe_delete(pThread); // have to manually delete thread
	}
	return pThread;
}

template <class T> T* begin_thread( CRuntimeClass* pRuntimeClass, int priority=THREAD_PRIORITY_NORMAL, bool auto_delete=false )
{
	CWinThread* pThread= AfxBeginThread(pRuntimeClass,priority,0,auto_delete?0:CREATE_SUSPENDED); if(!pThread) return nullptr;
	if(!auto_delete){ pThread->m_bAutoDelete=FALSE; pThread->ResumeThread(); }
	return (T*) pThread;
}

inline void copy_string_to_clipboard( const char* str )
{
	size_t len = strlen(str);

	OpenClipboard(0);
	EmptyClipboard();
	if(len==0){ CloseClipboard(); return; }

	HGLOBAL hglobal = GlobalAlloc(GMEM_MOVEABLE, (len+1)*sizeof(char));
	if(hglobal==nullptr){ CloseClipboard(); return; }

	char* buffer = (char*) GlobalLock(hglobal);
	if(buffer)
	{
		memcpy( buffer, str, len*sizeof(char));
		buffer[len] = '\0';
	}

	SetClipboardData( CF_TEXT, hglobal );
	CloseClipboard();

	GlobalUnlock(hglobal);
	GlobalFree(hglobal);
}

inline void copy_string_to_clipboard( const wchar_t* str )
{
	size_t len = wcslen(str);

	OpenClipboard(0);
	EmptyClipboard();
	if(len==0){ CloseClipboard(); return; }

	HGLOBAL hglobal = GlobalAlloc(GMEM_MOVEABLE, (len+1)*sizeof(wchar_t));
	if(hglobal==nullptr){ CloseClipboard(); return; }

	wchar_t* buffer = (wchar_t*) GlobalLock(hglobal);
	if(buffer)
	{
		memcpy( buffer, str, len*sizeof(wchar_t));
		buffer[len] = '\0';
	}

	SetClipboardData( CF_UNICODETEXT, hglobal );
	CloseClipboard();

	GlobalUnlock(hglobal);
	GlobalFree(hglobal);
}

// font functions
inline LOGFONT& get_log_font( CFont* f ){ static LOGFONT lf; f->GetLogFont(&lf); return lf; }

inline LOGFONT& get_log_font_gui()
{
	static LOGFONT lf;
	SystemParametersInfo(SPI_GETICONTITLELOGFONT,sizeof(lf),&lf,0);
	return lf;
}

inline CFont* get_default_gui_font( bool bBold=false, int deltaHeight=0 )
{
	static vector<CFont*>	fontList;

	LOGFONT target=get_log_font_gui();

	if(bBold) target.lfWeight=FW_BOLD;
	if(deltaHeight) target.lfHeight -= deltaHeight;
	for( unsigned k=0; k < fontList.size(); k++ )
		if(memcpy(&get_log_font(fontList[k]),&target,sizeof(LOGFONT))==0) return fontList[k];

	CFont* newFont=new CFont;
	::DeleteObject(newFont->Detach());
	newFont->CreateFontIndirectW(&target);
	fontList.emplace_back(newFont);
	return newFont;
}

inline CFont* get_edit_font()
{
	static CFont* font = new CFont;
	if(font->GetSafeHandle()) return font;

	if(
		!font->CreatePointFont(90,L"Segoe UI Mono")&&
		!font->CreatePointFont(90,L"Consolas")&&
		!font->CreatePointFont(90,L"Monaco")&&
		!font->CreatePointFont(90,L"FixedSys")&&
		!font->CreatePointFont(90,L"Courier New")
	) return get_default_gui_font(false,0);

	LOGFONT lf; font->GetLogFont(&lf);
	lf.lfPitchAndFamily = FIXED_PITCH|FF_MODERN;	// fixed|monospace: just desired spec. (non-necessarily loaded)
	::DeleteObject(font->Detach());
	font->CreateFontIndirectW(&lf);
	return font;
}

inline CFont* get_sublime_gui_font()
{
	static CFont* font = new CFont;
	if(font->GetSafeHandle()) return font;
	if(
		!font->CreatePointFont(100,L"Open Sans") &&
		!font->CreatePointFont(100,L"Segoe UI")
	) return get_default_gui_font(false,0);

	LOGFONT lf; font->GetLogFont(&lf);
	lf.lfPitchAndFamily = FIXED_PITCH|FF_MODERN;	// fixed|monospace: just desired spec. (non-necessarily loaded)
	::DeleteObject(font->Detach());
	font->CreateFontIndirectW(&lf);
	return font;
}

//*************************************
} // end namespace mfc
//*************************************

// windows explorer
struct explorer_t
{
	IShellWindows*		psw=nullptr;
	IDispatch*			pd=nullptr;
	IWebBrowserApp*		pwba=nullptr;
	HWND				hWndWBA=nullptr;
	IServiceProvider*	psp=nullptr;
	IShellBrowser*		psb=nullptr;
	IShellView*			psv=nullptr;
	IFolderView*		pfv=nullptr;
	IPersistFolder2*	ppf2=nullptr;
	LPITEMIDLIST		pidlFolder=nullptr;

	explorer_t(){ CoInitializeEx(nullptr,COINIT_APARTMENTTHREADED|COINIT_DISABLE_OLE1DDE); }
	~explorer_t(){ CoUninitialize(); }

	void clear()
	{
		safe_release(ppf2)	= nullptr;
		safe_release(pfv)	= nullptr;
		safe_release(psv)	= nullptr;
		safe_release(psb)	= nullptr;
		safe_release(psp)	= nullptr;
		safe_release(pwba)	= nullptr;
		safe_release(pd)	= nullptr;
	}

	void refresh( HWND hExplorerWnd )
	{
		clear();
		bool bFound=false; VARIANT v={}; V_VT(&v)=VT_I4;
		if(!SUCCEEDED(CoCreateInstance(CLSID_ShellWindows,nullptr,CLSCTX_ALL,IID_IShellWindows,(void**)&psw))){ clear(); return; }
		for( V_I4(&v)=0; !bFound && psw->Item(v,&pd)==S_OK; V_I4(&v)++ )
		{
			pwba=nullptr; if(!SUCCEEDED(pd->QueryInterface(IID_IWebBrowserApp, (void**)&pwba ))){ clear(); continue; }
			hWndWBA=nullptr; if(!(SUCCEEDED(pwba->get_HWND((LONG_PTR*)&hWndWBA))&&hWndWBA)||hWndWBA!=hExplorerWnd){ clear(); continue; }
			psp=nullptr; if(!SUCCEEDED(pwba->QueryInterface(IID_IServiceProvider, (void**)&psp))){ clear(); continue; }
			psb=nullptr; if(!SUCCEEDED(psp->QueryService(SID_STopLevelBrowser,IID_IShellBrowser,(void**)&psb))){ clear(); continue; }
			psv=nullptr; if(!SUCCEEDED(psb->QueryActiveShellView(&psv))){ clear(); continue; }
			pfv=nullptr; if(!SUCCEEDED(psv->QueryInterface(IID_IFolderView,(void**)&pfv))){ clear(); continue; }
			ppf2=nullptr; if(!SUCCEEDED(pfv->GetFolder(IID_IPersistFolder2,(void**)&ppf2))){ clear(); continue; }
			psv->Refresh();	// refresh explorer window
			clear();
		}
		clear();
	}

	path_t get_folder_path( HWND hExplorerWnd )
	{
		clear();
		path_t folder_path; bool bFound=false; VARIANT v={}; V_VT(&v)=VT_I4;
		if(!SUCCEEDED(CoCreateInstance(CLSID_ShellWindows,nullptr,CLSCTX_ALL,IID_IShellWindows,(void**)&psw))){ clear(); return ""; }
		for( V_I4(&v)=0; !bFound && psw->Item(v,&pd)==S_OK; V_I4(&v)++ )
		{
			pwba=nullptr; if(!SUCCEEDED(pd->QueryInterface(IID_IWebBrowserApp, (void**)&pwba ))){ clear(); continue; }
			hWndWBA=nullptr; HRESULT h=pwba->get_HWND((LONG_PTR*)&hWndWBA);
			if(!SUCCEEDED(pwba->get_HWND((LONG_PTR*)&hWndWBA))||!hWndWBA||hWndWBA!=hExplorerWnd){ clear(); continue; }
			psp=nullptr; if(!SUCCEEDED(pwba->QueryInterface(IID_IServiceProvider, (void**)&psp))){ clear(); continue; }
			psb=nullptr; if(!SUCCEEDED(psp->QueryService(SID_STopLevelBrowser,IID_IShellBrowser,(void**)&psb))){ clear(); continue; }
			psv=nullptr; if(!SUCCEEDED(psb->QueryActiveShellView(&psv))){ clear(); continue; }
			pfv=nullptr; if(!SUCCEEDED(psv->QueryInterface(IID_IFolderView,(void**)&pfv))){ clear(); continue; }
			ppf2=nullptr; if(!SUCCEEDED(pfv->GetFolder(IID_IPersistFolder2,(void**)&ppf2))){ clear(); continue; }
			pidlFolder=nullptr; if(SUCCEEDED(ppf2->GetCurFolder(&pidlFolder))){ wchar_t b[path_t::capacity]; if(SHGetPathFromIDListW(pidlFolder,b)) folder_path=wtoa(b); else folder_path.clear(); } CoTaskMemFree(pidlFolder);
			clear();
		}
		clear();
		return folder_path.empty()?folder_path:folder_path.append_slash();
	}

	void goto_folder( HWND hExplorerWnd, const path_t& folder_path )
	{
		clear();
		bool bFound=false; VARIANT v={}; V_VT(&v)=VT_I4;
		if(!SUCCEEDED(CoCreateInstance(CLSID_ShellWindows,nullptr,CLSCTX_ALL,IID_IShellWindows,(void**)&psw))){ clear(); return; }
		for( V_I4(&v)=0; !bFound && psw->Item(v,&pd)==S_OK; V_I4(&v)++ )
		{
			pwba=nullptr; if(!SUCCEEDED(pd->QueryInterface(IID_IWebBrowserApp, (void**)&pwba ))){ clear(); continue; }
			hWndWBA=nullptr; if(!(SUCCEEDED(pwba->get_HWND((LONG_PTR*)&hWndWBA))&&hWndWBA)||hWndWBA!=hExplorerWnd){ clear(); continue; }
			psp=nullptr; if(!SUCCEEDED(pwba->QueryInterface(IID_IServiceProvider, (void**)&psp))){ clear(); continue; }
			psb=nullptr; if(!SUCCEEDED(psp->QueryService(SID_STopLevelBrowser,IID_IShellBrowser,(void**)&psb))){ clear(); continue; }
			psv=nullptr; if(!SUCCEEDED(psb->QueryActiveShellView(&psv))){ clear(); continue; }
			pfv=nullptr; if(!SUCCEEDED(psv->QueryInterface(IID_IFolderView,(void**)&pfv))){ clear(); continue; }
			ppf2=nullptr; if(!SUCCEEDED(pfv->GetFolder(IID_IPersistFolder2,(void**)&ppf2))){ clear(); continue; }
			auto* pidl = ILCreateFromPathW(atow(folder_path.c_str()));
			bFound = SUCCEEDED(psb->BrowseObject( pidl, SBSP_SAMEBROWSER ));
			ILFree(pidl);
			clear();
		}
		clear();
	}

	static void open_folder( const char* dir )
	{
		if(!path_t(dir).exists()) return;
		HWND foreground_window = GetForegroundWindow(); // retrieve current foreground window
		explorer_t e;
		for( HWND hChild=::GetWindow(::GetWindow(::GetDesktopWindow(), GW_CHILD),GW_HWNDFIRST); hChild!=nullptr; hChild=::GetWindow(hChild, GW_HWNDNEXT) )
		{
			wchar_t buff[1024]; GetClassNameW(hChild, buff, MAX_PATH); if(_wcsicmp(buff, L"CabinetWClass")!=0&&_wcsicmp(buff,L"ExplorWClass")!=0) continue;
			path_t fold_path = e.get_folder_path(hChild);
			if(fold_path!=dir) continue;

			if(!::IsIconic(hChild)) return;
			::ShowWindow( hChild, SW_RESTORE );
			if(foreground_window) ::SetForegroundWindow( foreground_window ); // restore the old foreground window
			return;
		}

#ifdef _INC_SHELLAPI
		if(!path_t(dir).empty()&&path_t(dir).exists()){ ShellExecuteW(GetDesktopWindow(),L"Open",atow(auto_quote(dir)),0,0,SW_SHOW); }
#endif
		if(foreground_window) ::SetForegroundWindow( foreground_window ); // restore the old foreground window
	}
};
