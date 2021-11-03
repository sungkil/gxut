#pragma once
#include <ios>

#define REDIR_SLEEP_TIME		1
#define REDIR_BUFFER_SIZE		(1<<24)	// should be enough for buffering: now, 16M bytes; otherwise, stdout will not be written no more, which printf hangs
#define SAFE_CLOSE_HANDLE(h)	{if(h!=INVALID_HANDLE_VALUE) CloseHandle(h); h=INVALID_HANDLE_VALUE;} 

inline void read_redir( HANDLE hread, HANDLE hprocess )
{
	static char* buff=nullptr; static size_t blen=0;
	DWORD n_avail=0, n_read=0;
	while( PeekNamedPipe( hread, nullptr, 0, nullptr, &n_avail, nullptr ))
	{
		DWORD exit; GetExitCodeProcess(hprocess,&exit); if(exit!= STILL_ACTIVE) break;
		if(n_avail==0){ Sleep(REDIR_SLEEP_TIME); continue; }
		if(blen<n_avail) buff = (char*) realloc(buff,((blen=n_avail)+1)*sizeof(char));
		if(!ReadFile(hread, buff, n_avail, &n_read, nullptr)) return; if(n_read==0) continue;
		buff[n_read]=0; fprintf( stdout, buff );
	}
}

inline void create_process_redir( const wchar_t* cmd )
{
	SECURITY_DESCRIPTOR sd;
	InitializeSecurityDescriptor(&sd,SECURITY_DESCRIPTOR_REVISION);
    SetSecurityDescriptorDacl(&sd,true,NULL,false);
	SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), &sd, TRUE /*inheritance*/ };

	HANDLE stdout_read=INVALID_HANDLE_VALUE;		// parent process's stdout read handle
	HANDLE stdout_write=INVALID_HANDLE_VALUE;	// child process' stdout write handle
	if(!CreatePipe( &stdout_read, &stdout_write, &sa, REDIR_BUFFER_SIZE )){ fprintf( stdout, "[redir] CreatePipe(stdout) failed: %s\n", os::get_last_error() ); return; }
	if(stdout_read==INVALID_HANDLE_VALUE||stdout_write==INVALID_HANDLE_VALUE){ fprintf( stdout, "[redir] CreatePipe(stdout) failed: %s\n", os::get_last_error() ); return; }
	if(!SetHandleInformation(stdout_read,HANDLE_FLAG_INHERIT,0)){ fprintf( stdout, "[redir] SetHandleInformation() failed: %s\n", os::get_last_error() ); return; }

	// additional configuration for non-buffered IO
	setvbuf( stdout, nullptr, _IONBF, 0 );	// absolutely needed
	setvbuf( stderr, nullptr, _IONBF, 0 );	// absolutely needed
	std::ios::sync_with_stdio();

	// create the child process
	PROCESS_INFORMATION pi;	memset( &pi, 0, sizeof(pi) );
	STARTUPINFO si; memset( &si, 0, sizeof(si) );
	si.cb = sizeof(si);
	si.hStdError = stdout_write;
	si.hStdOutput = stdout_write;
	si.hStdInput = INVALID_HANDLE_VALUE;
	si.wShowWindow = SW_HIDE;
	si.dwFlags = STARTF_USESHOWWINDOW|STARTF_USESTDHANDLES;

	// Launch the child process.
	if(!CreateProcessW(nullptr, (LPWSTR)cmd, nullptr, nullptr, TRUE, 0, nullptr, path::cwd(), &si, &pi )) return void(fprintf( stdout, "[redir] CreateProcess() failed: %s\n", os::get_last_error() ));

	// infinite loop to read
	read_redir( stdout_read, pi.hProcess );

	SAFE_CLOSE_HANDLE( pi.hThread );
	SAFE_CLOSE_HANDLE( pi.hProcess );
	SAFE_CLOSE_HANDLE( stdout_read );
	SAFE_CLOSE_HANDLE( stdout_write );

	return;
}
