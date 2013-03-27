
#include "stdafx.h"

HWND attachmentsHwnd;

void RegisterAttachmentViewer()
{
    WNDCLASSEX wc;
 
    wc.cbSize         = sizeof(wc);
    wc.lpszClassName  = L"CinchAttachments";
    wc.hInstance      = GetModuleHandle(0);
	wc.lpfnWndProc    = AttachmentViewer::WndProc;
    wc.hCursor        = LoadCursor (NULL, IDC_ARROW);
    wc.hIcon          = 0;
    wc.lpszMenuName   = 0;
	wc.hbrBackground  = (HBRUSH)CreateSolidBrush(RGB(255,255,255));
    wc.style          = 0;
    wc.cbClsExtra     = 0;
    wc.cbWndExtra     = 0;
    wc.hIconSm        = 0;
 
    RegisterClassEx(&wc);
}

AttachmentViewer::AttachmentViewer(HWND _wnd){
	initialized= false;	
	wnd = _wnd;
}

void AttachmentViewer::initialize(){
	if ( initialized == true ) return;

	RECT rc;
	GetClientRect(wnd, &rc);
		
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	if (SUCCEEDED(hr))
	{
		HRESULT hr = CoCreateInstance(CLSID_ExplorerBrowser, NULL, CLSCTX_INPROC, IID_PPV_ARGS(&_peb));
		if (SUCCEEDED(hr)){
			FOLDERSETTINGS fs = {};
			fs.ViewMode = FVM_DETAILS;
			fs.fFlags = 0;

			HRESULT hr = _peb->Initialize(wnd, &rc, &fs);
			initialized = true;

			if ( SUCCEEDED(hr) ){
				if ( hasLoadedDocument == true ){
					PrepareAttachments();
				}
			}
		}
	}
}


void AttachmentViewer::PrepareAttachments(){
	if ( initialized == true ){
		Connection conn;
		Database db = conn.getDatabase(DATABASE);
		Document doc = db.getDocument(_id, _rev);

		string dir = "C:\\temp\\" + _id;
		wstring wdir = s2ws(dir);
		CreateDirectory(wdir.c_str(), NULL);
			
		try {
			vector<Attachment> attachments = doc.getAllAttachments();
			for(unsigned i=0; i<attachments.size(); i++){
				Attachment a = attachments[i];
				a.saveToDirectory(dir);
			}
		}catch(Exception e){
		}
		IShellItem *psi;
		HRESULT hr = SHCreateItemFromParsingName(wdir.c_str(), 0, IID_PPV_ARGS(&psi)); 
		if (SUCCEEDED(hr)){
			_peb->BrowseToObject(psi, 0);
		}

		WatchDirectory(s2ws(dir).c_str());
	}
}

void AttachmentViewer::LoadDocument(Object obj){
	hasLoadedDocument = true;
	_id = obj["_id"].getString();
	_rev = obj["_rev"].getString();

	if ( initialized ){
		PrepareAttachments();
	}
}


LRESULT CALLBACK AttachmentViewer::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	AttachmentViewer* self = (AttachmentViewer *)GetWindowLong(hWnd, GWL_USERDATA);
	switch(message){
	case WM_NCCREATE:
		{
		AttachmentViewer* v = new AttachmentViewer(hWnd);
		attachmentsHwnd = hWnd;
		SetWindowLong(hWnd, GWL_USERDATA, (ULONG_PTR)v);
		}
		break;
	case WM_NCDESTROY:
		{
		AttachmentViewer* v = (AttachmentViewer *)GetWindowLong(hWnd, GWL_USERDATA);
		delete v;
		break;
		}
	case WM_FILES_HAVE_CHANGED:
		MessageBox(hWnd, L"Changed", L"Cinch", 0);
		break;
	case WM_SIZE:
		{
		RECT r;
		GetClientRect(hWnd, &r);
		self->_peb->SetRect(NULL, r);
		break;
		}

	}
			
	
	return DefWindowProc(hWnd, message, wParam, lParam);
}


DWORD WINAPI FileWatchWorker(LPVOID param);


void AttachmentViewer::WatchDirectory(LPCWSTR lpDir)
{
	DWORD threadId;
	int len = wcslen(lpDir) + 2;
	wchar_t* l = new wchar_t[len];
	memset(l, 0, len);
	wcscpy_s(l, len, lpDir);
	CreateThread(NULL, 0, FileWatchWorker, (LPVOID)l, 0, &threadId); 

}


DWORD WINAPI FileWatchWorker(LPVOID param){
   DWORD dwWaitStatus; 
   HANDLE dwChangeHandles[3]; 
   TCHAR lpDrive[4];
   TCHAR lpFile[_MAX_FNAME];
   TCHAR lpExt[_MAX_EXT];

   LPCWSTR lpDir = (LPCWSTR)param;
   _tsplitpath_s(lpDir, lpDrive, 4, NULL, 0, lpFile, _MAX_FNAME, lpExt, _MAX_EXT);

   lpDrive[2] = (TCHAR)'\\';
   lpDrive[3] = (TCHAR)'\0';
 
// Watch the directory for file creation and deletion. 
 
   dwChangeHandles[0] = FindFirstChangeNotification(
      lpDir,                         // directory to watch 
      FALSE,                         // do not watch subtree 
      FILE_NOTIFY_CHANGE_FILE_NAME); // watch file name changes 
 
   if (dwChangeHandles[0] == INVALID_HANDLE_VALUE) 
   {
	   PostMessage(attachmentsHwnd, WM_FILE_WATCH_FAILED, 0, 0);
   }
 
// Watch the subtree for directory creation and deletion. 
 
   dwChangeHandles[1] = FindFirstChangeNotification( 
      lpDir,                       // directory to watch 
      TRUE,                          // watch the subtree 
      FILE_NOTIFY_CHANGE_DIR_NAME);  // watch dir name changes 
 
   if (dwChangeHandles[1] == INVALID_HANDLE_VALUE) 
   {
	   PostMessage(attachmentsHwnd, WM_FILE_WATCH_FAILED, 0, 0);
   }
 

   dwChangeHandles[2] = FindFirstChangeNotification(
		lpDir,
		TRUE,
		FILE_NOTIFY_CHANGE_SIZE);


// Make a final validation check on our handles.

   if ((dwChangeHandles[0] == NULL) || (dwChangeHandles[1] == NULL) || (dwChangeHandles[2] == NULL))
   {
	   PostMessage(attachmentsHwnd, WM_FILE_WATCH_FAILED, 0, 0);
   }

// Change notification is set. Now wait on both notification 
// handles and refresh accordingly. 
 
   while (TRUE) 
   { 
   // Wait for notification.
 
     dwWaitStatus = WaitForMultipleObjects(3, dwChangeHandles, 
         FALSE, INFINITE); 
 
      switch (dwWaitStatus) 
      { 
         case WAIT_OBJECT_0: 
 
         // A file was created, renamed, or deleted in the directory.
         // Refresh this directory and restart the notification.
 
			 PostMessage(attachmentsHwnd, WM_FILES_HAVE_CHANGED, 0, 0);
             if ( FindNextChangeNotification(dwChangeHandles[0]) == FALSE )
             {
             }
             break; 
 
         case WAIT_OBJECT_0 + 1: 
 
         // A directory was created, renamed, or deleted.
         // Refresh the tree and restart the notification.
			 PostMessage(attachmentsHwnd, WM_FILES_HAVE_CHANGED, 0, 0);
             if (FindNextChangeNotification(dwChangeHandles[1]) == FALSE )
             {
			 }
             break; 
		 case WAIT_OBJECT_0 + 2:
			 PostMessage(attachmentsHwnd, WM_FILES_HAVE_CHANGED, 0, 0);
			if (FindNextChangeNotification(dwChangeHandles[2]) == FALSE )
             {
             }
             break; 


			break;

         case WAIT_TIMEOUT:

         // A timeout occurred, this would happen if some value other 
         // than INFINITE is used in the Wait call and no changes occur.
         // In a single-threaded environment you might not want an
         // INFINITE wait.
 
            break;

         default: 
			break;
      }
   }
}
