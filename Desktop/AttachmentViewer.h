
#include "stdafx.h"

#define WM_FILES_HAVE_CHANGED WM_USER+192
#define WM_FILE_WATCH_FAILED WM_USER+193


class AttachmentViewer {
private:
	
	HWND wnd;
	HWND detail;
	string _id;
	string _rev;
	bool initialized;
	bool hasLoadedDocument;
	
	FileWatchController* fileWatchController;
public:
	AttachmentViewer(HWND);
	~AttachmentViewer();

	IExplorerBrowser *_peb;

	void initialize();
	void LoadDocument(Object obj);
	
	/* Downloads the attachments to the temp directory if necessary */
	void PrepareAttachments();

	/* Watch for changes to be uploaded */
	void AttachmentViewer::WatchDirectory(LPCWSTR lpDir);


	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

void RegisterAttachmentViewer();
