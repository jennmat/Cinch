
	// TestMulti.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "TestMulti.h"
#include <stdio.h>
#include <string>
#include <curl/curl.h>

using namespace std;

HWND hDlg;
  

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

int domultitransfer(void);
int doeasy(void);
void domultitest();

string buffer1;
string buffer2;



static int writer(char *data, size_t size, size_t nmemb, string *dest){
   int written = 0;
   buffer1.append(data);
   written = size * nmemb;
   return written;
}


static int writer2(char *data, size_t size, size_t nmemb, string *dest){
   int written = 0;
   buffer2.append(data);
  
   written = size * nmemb;
   return written;
}

#pragma comment(linker, \
  "\"/manifestdependency:type='Win32' "\
  "name='Microsoft.Windows.Common-Controls' "\
  "version='6.0.0.0' "\
  "processorArchitecture='*' "\
  "publicKeyToken='6595b64144ccf1df' "\
  "language='*'\"")

#pragma comment(lib, "ComCtl32.lib")

void WinHttp_CallBack(AsyncWinHttp* asyncWinHttp)
{
	if (asyncWinHttp->status.Status() == ASYNC_WINHTTP_ERROR)
	{
		//printf("%S", asyncWinHttp->status.Desc().c_str());
	}
	else
	{
		std::string response;
		asyncWinHttp->GetResponseRaw(response);
		
		wstring w = s2ws(response);
			
		Edit_SetText(GetDlgItem(hDlg, IDC_EDIT1), w.c_str());

	}
}



void WinHttp_CallBack2(AsyncWinHttp* asyncWinHttp)
{
	if (asyncWinHttp->status.Status() == ASYNC_WINHTTP_ERROR)
	{
		//printf("%S", asyncWinHttp->status.Desc().c_str());
	}
	else
	{
		std::string response;
		asyncWinHttp->GetResponseRaw(response);
		
		wstring w = s2ws(response);
			
		Edit_SetText(GetDlgItem(hDlg, IDC_EDIT2), w.c_str());

	}
}

INT_PTR CALLBACK DialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch(uMsg)
  {
  case WM_COMMAND:
    switch(LOWORD(wParam))
    {
    case IDCANCEL:
      SendMessage(hDlg, WM_CLOSE, 0, 0);
      return TRUE;
    case IDC_BUTTON1:
		{
			HWND h = GetDlgItem(hDlg, IDC_EDIT3);
			int len = GetWindowTextLength(h);
			len+=sizeof(wchar_t);
			wchar_t* t = new wchar_t[len];
			GetWindowText(h, t, len);
			
			/*
			string s = ws2s(t);
			download_url(s.c_str(), writer);*/


			http.Initialize(WinHttp_CallBack);
			http.SetTimeout(30 * 60 * 1000);
			http.SendRequest(t);
	
			
		}
	  break;
	case IDC_BUTTON2:
		{
			HWND h = GetDlgItem(hDlg, IDC_EDIT4);
			int len = GetWindowTextLength(h);
			len+=sizeof(wchar_t);
			wchar_t* t = new wchar_t[len];
			GetWindowText(h, t, len);
			string s = ws2s(t);
			//download_url(s.c_str(), writer2);

			http.Initialize(WinHttp_CallBack2);
			http.SetTimeout(3 * 60 * 1000);
			http.SendRequest(t);
	
			
		}
	  break;
    }
    break;
	
  case WM_CLOSE:
    DestroyWindow(hDlg);
    return TRUE;

  case WM_DESTROY:
    PostQuitMessage(0);
    return TRUE;
  }

  return FALSE;
}


int WINAPI _tWinMain(HINSTANCE hInst, HINSTANCE h0, LPTSTR lpCmdLine, int nCmdShow)
{


  MSG msg;
  BOOL ret;

  InitCommonControls();
  hDlg = CreateDialogParam(hInst, MAKEINTRESOURCE(IDD_DIALOG1), 0, DialogProc, 0);
  ShowWindow(hDlg, nCmdShow);

  do_curl_setup();

  while ( true ){ 

	  do_curl_processing();

	  while((ret = PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) != 0) {
		if(ret == -1)
		  return -1;

		if(!IsDialogMessage(hDlg, &msg)) {
		  TranslateMessage(&msg);
		  DispatchMessage(&msg);
		}
	  }
  }

  return 0;
}

// Message handler for about box.
INT_PTR CALLBACK Main(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}


wstring s2ws(const string& s)
{
    int len;
    int slength = (int)s.length() + 1;
    len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0); 
    wchar_t* buf = new wchar_t[len];
    MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
    wstring r(buf);
    delete[] buf;
    return r;
}


std::string ws2s(const std::wstring& s)
{
    int len;
    int slength = (int)s.length() + 1;
    len = WideCharToMultiByte(CP_ACP, 0, s.c_str(), slength, 0, 0, 0, 0); 
    char* buf = new char[len];
    WideCharToMultiByte(CP_ACP, 0, s.c_str(), slength, buf, len, 0, 0); 
    std::string r(buf);
    delete[] buf;
    return r;
}



void download_url(const char *url, void* writeFunc){
	if( writeFunc == writer ){
		http_handle = curl_easy_init();
		curl_easy_setopt(http_handle, CURLOPT_URL, url);
		curl_easy_setopt(http_handle, CURLOPT_WRITEFUNCTION, writeFunc);
		curl_easy_setopt(http_handle, CURLOPT_VERBOSE, 1L);
	
		curl_multi_add_handle(multi_handle, http_handle);
	} else {
		http_handle2 = curl_easy_init();
		curl_easy_setopt(http_handle2, CURLOPT_URL, url);
		curl_easy_setopt(http_handle2, CURLOPT_WRITEFUNCTION, writeFunc);
		curl_easy_setopt(http_handle2, CURLOPT_VERBOSE, 1L);
	
		curl_multi_add_handle(multi_handle, http_handle2);
	
	}
		
	
	int running;
	/* we start some action by calling perform right away */
	curl_multi_perform(multi_handle, &running);

}

/*
 * Download a HTTP file and upload an FTP file simultaneously.
 */

#define HANDLECOUNT 2   /* Number of simultaneous transfers */

void do_curl_setup(void)
{
	curl_global_init(CURL_GLOBAL_DEFAULT);

	/* init a multi stack */
	multi_handle = curl_multi_init();

}

void do_curl_processing(){
	int still_running; /* keep number of running handles */

	struct timeval timeout;
	int rc; /* select() return code */

	CURLMsg *msg; /* for picking up messages with the transfer status */
	  int msgs_left; /* how many messages are left */

	fd_set fdread;
	fd_set fdwrite;
	fd_set fdexcep;
	int maxfd = -1;

	long curl_timeo = -1;

	FD_ZERO(&fdread);
	FD_ZERO(&fdwrite);
	FD_ZERO(&fdexcep);

	/* set a suitable timeout to play around with */
	timeout.tv_sec = 1;
	timeout.tv_usec = 0;

	curl_multi_timeout(multi_handle, &curl_timeo);
	if(curl_timeo >= 0) {
		timeout.tv_sec = curl_timeo / 1000;
		if(timeout.tv_sec > 1)
			timeout.tv_sec = 1;
		else
			timeout.tv_usec = (curl_timeo % 1000) * 1000;
	}

	/* get file descriptors from the transfers */
	curl_multi_fdset(multi_handle, &fdread, &fdwrite, &fdexcep, &maxfd);

	/* In a real-world program you OF COURSE check the return code of the
		function calls.  On success, the value of maxfd is guaranteed to be
		greater or equal than -1.  We call select(maxfd + 1, ...), specially in
		case of (maxfd == -1), we call select(0, ...), which is basically equal
		to sleep. */

	if ( maxfd > -1 ){
		rc = select(maxfd+1, &fdread, &fdwrite, &fdexcep, &timeout);

		switch(rc) {
		case -1:
			/* select error */
			still_running = 0;
			break;
		case 0:
		default:
			/* timeout or readable/writable sockets */
			curl_multi_perform(multi_handle, &still_running);
			break;
		}
	}

	/* See how the transfers went */
  while ((msg = curl_multi_info_read(multi_handle, &msgs_left))) {
    if (msg->msg == CURLMSG_DONE) {
      int idx, found = 0;

      /* Find out which handle this message is about */
      for (idx=0; idx<HANDLECOUNT; idx++) {
        found = (msg->easy_handle == http_handle);
        if(found)
          break;
      }

	  if ( msg->easy_handle == http_handle ){
		  wstring w = s2ws(buffer1);
			
			Edit_SetText(GetDlgItem(hDlg, IDC_EDIT1), w.c_str());

			buffer1.clear();

	  } else {
		  wstring w = s2ws(buffer2);
			
			Edit_SetText(GetDlgItem(hDlg, IDC_EDIT2), w.c_str());

			buffer2.clear();
	  
      }
    }
  }

}


void do_curl_cleanup(){
	curl_multi_remove_handle(multi_handle, http_handle);
	
	curl_easy_cleanup(http_handle);

	curl_multi_cleanup(multi_handle);

	curl_global_cleanup();

}
