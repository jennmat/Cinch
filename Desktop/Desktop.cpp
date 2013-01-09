// Desktop.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Desktop.h"
#include <stdlib.h>
#include <stdio.h>

using namespace std;
using namespace JsonBox;
using namespace CouchDB;

#define MAX_LOADSTRING 100

#define TREE_WIDTH 200
#define LIST_WIDTH 200

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

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
	LoadString(hInstance, IDC_DESKTOP, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DESKTOP));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
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
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DESKTOP));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_DESKTOP);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}


// Adds items to a tree-view control. 
// Returns the handle to the newly added item. 
// hwndTV - handle to the tree-view control. 
// lpszItem - text of the item to add. 
// nLevel - level at which to add the item. 
//
// g_nClosed, and g_nDocument - global indexes of the images.

HTREEITEM AddItemToTree(HWND hwndTV, LPWSTR lpszItem, int nLevel)
{ 
    TVITEM tvi; 
    TVINSERTSTRUCT tvins; 
    static HTREEITEM hPrev = (HTREEITEM)TVI_FIRST; 
    static HTREEITEM hPrevRootItem = NULL; 
    static HTREEITEM hPrevLev2Item = NULL; 
    HTREEITEM hti; 

    tvi.mask = TVIF_TEXT | TVIF_IMAGE 
               | TVIF_SELECTEDIMAGE | TVIF_PARAM; 

    // Set the text of the item. 
    tvi.pszText = lpszItem; 
    tvi.cchTextMax = sizeof(tvi.pszText)/sizeof(tvi.pszText[0]); 

    // Assume the item is not a parent item, so give it a 
    // document image. 
    //tvi.iImage = g_nDocument; 
    //tvi.iSelectedImage = g_nDocument; 

    // Save the heading level in the item's application-defined 
    // data area. 
    tvi.lParam = (LPARAM)nLevel; 
    tvins.item = tvi; 
    tvins.hInsertAfter = hPrev; 

    // Set the parent item based on the specified level. 
    if (nLevel == 1) 
        tvins.hParent = TVI_ROOT; 
    else if (nLevel == 2) 
        tvins.hParent = hPrevRootItem; 
    else 
        tvins.hParent = hPrevLev2Item; 

    // Add the item to the tree-view control. 
    hPrev = (HTREEITEM)SendMessage(hwndTV, TVM_INSERTITEM, 
        0, (LPARAM)(LPTVINSERTSTRUCT)&tvins); 

    if (hPrev == NULL)
        return NULL;

    // Save the handle to the item. 
    if (nLevel == 1) 
        hPrevRootItem = hPrev; 
    else if (nLevel == 2) 
        hPrevLev2Item = hPrev; 

    // The new item is a child item. Give the parent item a 
    // closed folder bitmap to indicate it now has child items. 
    if (nLevel > 1)
    { 
        hti = TreeView_GetParent(hwndTV, hPrev); 
        tvi.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE; 
        tvi.hItem = hti; 
        //tvi.iImage = g_nClosed; 
        //tvi.iSelectedImage = g_nClosed; 
        TreeView_SetItem(hwndTV, &tvi); 
    } 

    return hPrev; 
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

   
   RECT client;
   GetClientRect(hWnd, &client);

   InitCommonControls();

   tree = CreateWindowEx(0, WC_TREEVIEW, TEXT("Tree View"), WS_VISIBLE | WS_CHILD | WS_BORDER | TVS_HASLINES,
	   0, 0, TREE_WIDTH, client.bottom,
	   hWnd, (HMENU) IDC_VIEW_TREE, hInst, 0);

  


   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   delegate = new CouchViewDelegate(conn);
   //delegate->setView(s2ws(string("_design/properties")), s2ws(string("by-nickname")));

   grid = CinchGrid::CreateCinchGrid(hWnd, delegate);

   SetWindowPos(grid, HWND_TOP, TREE_WIDTH, 0, TREE_WIDTH + LIST_WIDTH, client.bottom, SW_SHOW);

   Database db = conn.getDatabase("property");
   Object views = db.listViews();

   if ( views["total_rows"].getInt() > 0 ){
	   Array rows = views["rows"].getArray();
	   for(unsigned int i=0; i<rows.size(); i++){
		   Object row = rows[i].getObject();
		
		   wstring name = s2ws(row["id"].getString());
		   LPWSTR str = new wchar_t[80];
		   wcscpy_s(str, 80, name.c_str());

		   AddItemToTree(tree, str, 1);
	
		   Object doc = row["doc"].getObject();
		   Object views = doc["views"].getObject();
		   Object::const_iterator it = views.begin();
		   for(it=views.begin(); it != views.end(); it++){
				pair<string, Value> p = *it;
				wstring name = s2ws(p.first);
				LPWSTR str = new wchar_t[80];
				wcscpy_s(str, 80, name.c_str());

				AddItemToTree(tree, str, 2);
		
		   }
		   //for(unsigned int j = 0; j<views.size(); j++){
			 //  Object view = views[j].getObject();
			  // int a = 1;
			//}

	   }
   }

   ShowWindow(tree, SW_SHOW);
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
	case WM_NOTIFY:
		{

		LPNMHDR pnmhdr = (LPNMHDR)lParam;
		if  (pnmhdr->idFrom == IDC_VIEW_TREE && pnmhdr->code == TVN_SELCHANGED ){
			
			HTREEITEM item = ((LPNM_TREEVIEW)pnmhdr)->itemNew.hItem;

			TVITEM tvitem;

			tvitem.hItem = item;
			tvitem.mask = TVIF_TEXT;
			tvitem.cchTextMax = 80;
			wchar_t text[80];
			memset(text, 0, 80);
			tvitem.pszText = text;
			if( item != TreeView_GetRoot(tree) ){
				TreeView_GetItem(tree, &tvitem);
				wstring s(text);

				HTREEITEM parent = TreeView_GetParent(tree, item);
				tvitem.hItem = parent;
				TreeView_GetItem(tree, &tvitem);
				wstring design(text);

				delegate->setView(design, s);

				CinchGrid* gridcontrol = (CinchGrid *)GetWindowLong(grid, GWL_USERDATA);
				gridcontrol->reloadData();

			}

		}
		break;
		}
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
