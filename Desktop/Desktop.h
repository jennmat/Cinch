#include "stdafx.h"

CouchViewDelegate* delegate;

class Desktop : public FormDelegate {
public:
	Object loadedForm;
	void formModified();

};

HWND hWnd; //Main window
HWND grid;
HWND tree;
HWND designer;


Desktop desktop;

CouchDB::Connection conn;
CouchDB::Database db = conn.getDatabase(DATABASE);

CouchDB::Connection listenerConn;
CouchDB::Database listenerDb = listenerConn.getDatabase(DATABASE);


DWORD WINAPI ChangesListener(LPVOID lParam);

HANDLE listenerThread;