
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
	fileWatchController = NULL;
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
		if ( fileWatchController != NULL ){
			fileWatchController->pause = TRUE;  //Needs work but this is to prevent me from getting into an infinite loop
		}
		;
		
		Document doc = db.getDocument(_id, _rev);

		wchar_t tempdir[MAX_PATH];
		GetTempPath(MAX_PATH, tempdir);

		StringCchCat(tempdir, MAX_PATH, L"cinch\\");
		
		CreateDirectory(tempdir, NULL);

		StringCchCat(tempdir, MAX_PATH, s2ws(_id).c_str());
		
		CreateDirectory(tempdir, NULL);
		
		string dir = ws2s(tempdir);
		string idfile = dir + "\\.id";
		string revfile = dir + "\\.rev";
		
		SetFileAttributes(s2ws(idfile).c_str(), FILE_ATTRIBUTE_NORMAL);
		SetFileAttributes(s2ws(revfile).c_str(), FILE_ATTRIBUTE_NORMAL);


		FILE* f;
		errno_t err = fopen_s(&f, idfile.c_str(), "w");
		if (err == 0 ){
			fwrite(_id.c_str(), sizeof(char), _id.length(), f);
			fclose(f);
		}

		err = fopen_s(&f, revfile.c_str(), "w");
		if ( err == 0 ){
			fwrite(_rev.c_str(), sizeof(char), _rev.length(), f);
			fclose(f);
		}

		SetFileAttributes(s2ws(idfile).c_str(), FILE_ATTRIBUTE_HIDDEN);
		SetFileAttributes(s2ws(revfile).c_str(), FILE_ATTRIBUTE_HIDDEN);


		try {
			vector<Attachment> attachments = doc.getAllAttachments();
			for(unsigned i=0; i<attachments.size(); i++){
				Attachment a = attachments[i];
				a.saveToDirectory(dir);
			}
		}catch(Exception e){
		}
		
		IShellItem *psi;
		HRESULT hr = SHCreateItemFromParsingName(tempdir, 0, IID_PPV_ARGS(&psi)); 
		if (SUCCEEDED(hr)){
			HRESULT hr = _peb->BrowseToObject(psi, EBF_NONE);
		}

		if ( fileWatchController != NULL ){
			fileWatchController->pause = FALSE;
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
	if ( fileWatchController == NULL ){
		fileWatchController = new FileWatchController;
		fileWatchController->exit = FALSE;
		fileWatchController->pause = FALSE;

		fileWatchController->directory = new wchar_t[len];
		memset(fileWatchController->directory, 0, len);
		wcscpy_s(fileWatchController->directory, len, lpDir);

		CreateThread(NULL, 0, FileWatchWorker, (LPVOID)fileWatchController, 0, &threadId); 
	} else {
		delete fileWatchController->directory;

		fileWatchController->directory = new wchar_t[len];
		memset(fileWatchController->directory, 0, len);
		wcscpy_s(fileWatchController->directory, len, lpDir);

	}


}

char* md5file(wchar_t* file){
	 DWORD dwStatus = 0;
    BOOL bResult = FALSE;
    HCRYPTPROV hProv = 0;
    HCRYPTHASH hHash = 0;
    HANDLE hFile = NULL;
    BYTE rgbFile[BUFSIZE];
    DWORD cbRead = 0;
    BYTE rgbHash[MD5LEN];
    DWORD cbHash = 0;
    CHAR rgbDigits[] = "0123456789abcdef";
    // Logic to check usage goes here.

    hFile = CreateFile(file,
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_SEQUENTIAL_SCAN,
        NULL);

    if (INVALID_HANDLE_VALUE == hFile)
    {
        dwStatus = GetLastError();
		return "";
    }

    // Get handle to the crypto provider
    if (!CryptAcquireContext(&hProv,
        NULL,
        NULL,
        PROV_RSA_FULL,
        CRYPT_VERIFYCONTEXT))
    {
        dwStatus = GetLastError();
        (hFile);
		CloseHandle(file);
        return "";
    }

    if (!CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash))
    {
        dwStatus = GetLastError();
        CloseHandle(hFile);
        CryptReleaseContext(hProv, 0);
        return "";
    }

    while (bResult = ReadFile(hFile, rgbFile, BUFSIZE, 
        &cbRead, NULL))
    {
        if (0 == cbRead)
        {
            break;
        }

        if (!CryptHashData(hHash, rgbFile, cbRead, 0))
        {
            dwStatus = GetLastError();
            printf("CryptHashData failed: %d\n", dwStatus); 
            CryptReleaseContext(hProv, 0);
            CryptDestroyHash(hHash);
            CloseHandle(hFile);
            return "";
        }
    }

    if (!bResult)
    {
        dwStatus = GetLastError();
        printf("ReadFile failed: %d\n", dwStatus); 
        CryptReleaseContext(hProv, 0);
        CryptDestroyHash(hHash);
        CloseHandle(hFile);
        return "";
    }

    cbHash = MD5LEN;
	char* rc = "";
    if (CryptGetHashParam(hHash, HP_HASHVAL, rgbHash, &cbHash, 0))
    {
     	size_t size;
		rc = base64_encode((const unsigned char*)rgbHash, cbHash, &size);
	}

    CryptDestroyHash(hHash);
    CryptReleaseContext(hProv, 0);
    CloseHandle(hFile);

	return rc;

}

string readSingleLine(const char *filename){
	FILE *f;

	long size;

	errno_t err = fopen_s(&f, filename, "r");
	if ( err == 0 ){
		fseek(f, 0, SEEK_END);
		size = ftell(f);
		rewind(f);

		char* c = new char[size+1];
		memset(c, 0, size+1);
		fread_s(c, size, sizeof(char), size, f);
		fclose(f);
		return string(c);

	} else {
		return "";
	}

}

void ProcessChanges(wchar_t* dir){
	;
	

	HANDLE hFind = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA ffd;
	TCHAR szDir[MAX_PATH];

	StringCchCopy(szDir, MAX_PATH, dir);
    StringCchCat(szDir, MAX_PATH, TEXT("\\*"));

	stringstream idfile;
	idfile << ws2s(dir) << "\\.id";

	stringstream revfile;
	revfile << ws2s(dir) << "\\.rev";

	
	string id = readSingleLine(idfile.str().c_str());
	string rev = readSingleLine(revfile.str().c_str());
	
	if ( id.length() == 0 ){
		return;
	}

	hFind = FindFirstFile(szDir, &ffd);

	bool hasUploadedAttachments = false;
				

	do
    {
		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
		}
		else
		{
			if ( wcscmp(L".id", ffd.cFileName) != 0 && wcscmp(L".rev", ffd.cFileName) != 0 ){
				wchar_t filename[MAX_PATH];
				StringCchCopy(filename, MAX_PATH, dir);
				StringCchCat(filename, MAX_PATH, L"\\");
				StringCchCat(filename, MAX_PATH, ffd.cFileName);

				char* c = md5file(filename);

				/* Compare to content md5 of attachment in the db */
				Document doc = db.getDocument(id, rev);
				try {
					Attachment a = doc.getAttachment(ws2s(ffd.cFileName));
					string serverMD5 = a.getContentMD5();
					const char* j = serverMD5.c_str();
					int i = strcmp(c, j);
					if ( i != 0 ) {
						doc.updateAttachmentFromFile(a.getID(), ws2s(filename));
						hasUploadedAttachments = true;
					}
				}catch(AttachmentNotFoundException e){
					doc.addAttachmentFromFile(ws2s(ffd.cFileName), "", ws2s(filename));
					hasUploadedAttachments = true;
				}
			}
		}
	}
	while (FindNextFile(hFind, &ffd) != 0);

	FindClose(hFind);

	if ( hasUploadedAttachments ){
		PostMessage(GetParent(attachmentsHwnd), WM_ATTACHMENTS_UPLOADED, 0, 0);
	}

}

DWORD WINAPI FileWatchWorker(LPVOID param){
   DWORD dwWaitStatus; 
   HANDLE dwChangeHandle; 
   
   FileWatchController* controller = (FileWatchController *)param;

   LPCWSTR lpDir = controller->directory;


   
// Watch the directory for file creation and deletion. 
 
   dwChangeHandle = FindFirstChangeNotification(lpDir, FALSE, 
	   FILE_NOTIFY_CHANGE_FILE_NAME |
	   FILE_NOTIFY_CHANGE_SIZE
	  ); 


   wchar_t* watchedDir = new wchar_t[MAX_PATH];
   memset(watchedDir, 0, MAX_PATH);
   wcscpy_s(watchedDir, MAX_PATH, controller->directory);


// Make a final validation check on our handles.

   if ((dwChangeHandle == NULL))
   {
	   PostMessage(attachmentsHwnd, WM_FILE_WATCH_FAILED, 0, 0);
   }

// Change notification is set. Now wait on both notification 
// handles and refresh accordingly. 
 
   while (!controller->exit) 
   { 
   // Wait for notification.
 
	   dwWaitStatus = WaitForSingleObject(dwChangeHandle, 100);

      switch (dwWaitStatus) 
      { 
         case WAIT_OBJECT_0: 
 
         // A file was created, renamed, or deleted in the directory.
         // Refresh this directory and restart the notification.
 
			 if ( controller->pause == FALSE ){
				ProcessChanges(watchedDir);
			 }

			 if ( FindNextChangeNotification(dwChangeHandle) == FALSE )
             {
             }
             break; 
 
         case WAIT_TIMEOUT:
			 /* See if the controller wants me to switch to watching a different directory
			  * This corresponds to the user accessing a different document */

         
			if ( wcscmp(controller->directory, watchedDir) != 0 ){
				memset(watchedDir, 0, MAX_PATH);
				wcscpy_s(watchedDir, MAX_PATH, controller->directory);
				dwChangeHandle = FindFirstChangeNotification(watchedDir, FALSE, 
					FILE_NOTIFY_CHANGE_FILE_NAME |
					FILE_NOTIFY_CHANGE_SIZE
				); 

			}

            break;

         default: 
			break;
      }
   }

   return 0;
}
