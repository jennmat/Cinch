
	// TestMulti.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "TestMulti.h"
#include <stdio.h>
#include <string>
#include <curl/curl.h>

using namespace std;


#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

int domultitransfer(void);
int doeasy(void);
void domultitest();

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_TESTMULTI, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TESTMULTI));

	while (true){

		do_curl_processing();
		
		// Main message loop:
		while (PeekMessage(&msg, NULL,  0, 0, PM_REMOVE)) 
		{ 
		//while (GetMessage(&msg, NULL, 0, 0))
		//{
			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}

	do_curl_cleanup();

	return (int) msg.wParam;
}

void fun(){

}


//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TESTMULTI));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_TESTMULTI);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   edit = CreateWindowEx(0, L"EDIT", L"", WS_BORDER|WS_CHILD|WS_VISIBLE|WS_TABSTOP|ES_MULTILINE|ES_AUTOVSCROLL|ES_WANTRETURN,
						0, 0, 500, 200, hWnd, NULL, GetModuleHandle(0), NULL);

   SetWindowFont(edit, CreateFont(-12,0,0,0,400,0,0,0,1,0,0,0,0,TEXT("Segoe UI")), false);

   ShowWindow(edit, nCmdShow);

   edit2 = CreateWindowEx(0, L"EDIT", L"",WS_BORDER| WS_CHILD|WS_VISIBLE|WS_TABSTOP|ES_MULTILINE|ES_AUTOVSCROLL|ES_WANTRETURN,
						500, 0, 510, 200, hWnd, NULL, GetModuleHandle(0), NULL);

   SetWindowFont(edit2, CreateFont(-12,0,0,0,400,0,0,0,1,0,0,0,0,TEXT("Segoe UI")), false);

   ShowWindow(edit2, nCmdShow);


   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   do_curl_setup();

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
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




static int writer(char *data, size_t size, size_t nmemb, string *dest){
   int written = 0;
   string s = string(data);
   wstring w= s2ws(s);
   //int len = Edit_GetTextLength(edit) + sizeof(wchar_t);
   //wchar_t* cur = new wchar_t[len];
   //Edit_GetText(edit, cur, len);

   //w.append(cur);

   Edit_SetText(edit, w.c_str());
   written = size * nmemb;
   return written;
}


static int writer2(char *data, size_t size, size_t nmemb, string *dest){
   int written = 0;
   string s = string(data);
   wstring w= s2ws(s);
   //int len = Edit_GetTextLength(edit2) + sizeof(wchar_t);
   //wchar_t* cur = new wchar_t[len];
   //Edit_GetText(edit2, cur, len);

   //w.append(cur);

   Edit_SetText(edit2, w.c_str());
   written = size * nmemb;
   return written;
}



/*
 * Download a HTTP file and upload an FTP file simultaneously.
 */

#define HANDLECOUNT 2   /* Number of simultaneous transfers */

void do_curl_setup(void)
{
	curl_global_init(CURL_GLOBAL_DEFAULT);

	http_handle = curl_easy_init();
	http_handle2 = curl_easy_init();

	/* set the options (I left out a few, you'll get the point anyway) */
	curl_easy_setopt(http_handle, CURLOPT_URL, "http://www.google.com/");
	curl_easy_setopt(http_handle, CURLOPT_WRITEFUNCTION, writer);
	curl_easy_setopt(http_handle, CURLOPT_VERBOSE, 1L);
	
	curl_easy_setopt(http_handle2, CURLOPT_URL, "http://www.yahoo.com/");
	curl_easy_setopt(http_handle2, CURLOPT_WRITEFUNCTION, writer2);
	curl_easy_setopt(http_handle2, CURLOPT_VERBOSE, 1L);
	
	/* init a multi stack */
	multi_handle = curl_multi_init();

	/* add the individual transfers */
	curl_multi_add_handle(multi_handle, http_handle);
	curl_multi_add_handle(multi_handle, http_handle2);
	int running;
	/* we start some action by calling perform right away */
	curl_multi_perform(multi_handle, &running);

}

void do_curl_processing(){
	int still_running; /* keep number of running handles */

	struct timeval timeout;
	int rc; /* select() return code */

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
}


void do_curl_cleanup(){
	curl_multi_remove_handle(multi_handle, http_handle);
	
	curl_easy_cleanup(http_handle);

	curl_multi_cleanup(multi_handle);

	curl_global_cleanup();

}
