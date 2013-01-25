
#include "stdafx.h"

LRESULT CALLBACK ToolbarProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

void RegisterCinchToolbar(){
	
    WNDCLASSEX wc;
	wc.cbSize         = sizeof(wc);
    wc.lpszClassName  = L"CinchToolbar";
    wc.hInstance      = GetModuleHandle(0);
	wc.lpfnWndProc    = ToolbarProc;
    wc.hCursor        = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon          = 0;
    wc.lpszMenuName   = 0;
	wc.hbrBackground  = (HBRUSH)CreateSolidBrush(RGB(240,240,240));
    wc.style          = 0;
    wc.cbClsExtra     = 0;
    wc.cbWndExtra     = 0;
    wc.hIconSm        = 0;
 
    RegisterClassEx(&wc);

}


HWND CreateCinchToolbar(HWND hWndParent)
{
	//HWND hWnd = CreateWindowEx(0,
	//	L"CinchToolbar",
	//	_T("Cinch Toolbar"),
	//	WS_VISIBLE | WS_CHILD,
	//	0, 0, 500, 500,
	//	parent,
	//	NULL, GetModuleHandle(0), NULL);

	//HWND btn = CreateWindowEx(0, L"BUTTON", L"OK", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | BS_TOP, 0, 0, 70, 70, hWnd, NULL, GetModuleHandle(0), NULL);
	//const int icon_size=25;

	//HANDLE hIcon = LoadImage(GetModuleHandle(NULL),MAKEINTRESOURCE(IDI_NEW), IMAGE_ICON, 25, 25, NULL);

	//HFONT hFont=CreateFont(18,0,0,0,0,0,0,0,0,0,0,0,0,TEXT("MS Shell Dlg"));
	
	//SendMessage(btn, WM_SETFONT, (WPARAM)hFont, 0);
	//SendDlgItemMessage(GetDlgItem(hwnd,IDB_BUTTON),BM_SETIMAGE,(WPARAM)IMAGE_ICON,(LPARAM)hMyIcon);
	
	//SendMessage(btn, BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)hIcon);
	//return hWnd;

	HIMAGELIST g_hImageList = NULL;
	HINSTANCE g_hInst = GetModuleHandle(0);
    // Declare and initialize local constants.
    const int ImageListID    = 0;
    const int numButtons     = 1;
    const int bitmapSize     = 24;
    
    const DWORD buttonStyles = BTNS_AUTOSIZE;

    // Create the toolbar.
    HWND hWndToolbar = CreateWindowEx(0, TOOLBARCLASSNAME, NULL, 
                                      WS_CHILD | TBSTYLE_WRAPABLE, 0, 0, 0, 0, 
                                      hWndParent, NULL, g_hInst, NULL);
        
    if (hWndToolbar == NULL)
        return NULL;

    // Create the image list.
    g_hImageList = ImageList_Create(bitmapSize, bitmapSize,   // Dimensions of individual bitmaps.
                                    ILC_COLOR16 | ILC_MASK,   // Ensures transparent background.
                                    numButtons, 0);

    // Set the image list.
    SendMessage(hWndToolbar, TB_SETIMAGELIST, 
                (WPARAM)ImageListID, 
                (LPARAM)g_hImageList);

    // Load the button images.
    SendMessage(hWndToolbar, TB_LOADIMAGES, 
		(WPARAM)IDB_STD_LARGE_COLOR, 
                (LPARAM)HINST_COMMCTRL);

    // Initialize button info.
    // IDM_NEW, IDM_OPEN, and IDM_SAVE are application-defined command constants.
    
    TBBUTTON tbButtons[numButtons] = 
    {
		{ MAKELONG(STD_FILENEW,  ImageListID), IDM_NEW_DOCUMENT,  TBSTATE_ENABLED, buttonStyles, {0}, 0, (INT_PTR)L"New" }
    };

    // Add buttons.
    SendMessage(hWndToolbar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
    SendMessage(hWndToolbar, TB_ADDBUTTONS,       (WPARAM)numButtons,       (LPARAM)&tbButtons);

    // Resize the toolbar, and then show it.
    //SendMessage(hWndToolbar, TB_AUTOSIZE, 0, 0); 
    ShowWindow(hWndToolbar,  TRUE);
    
    return hWndToolbar;
}



LRESULT CALLBACK ToolbarProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//int wmId, wmEvent;
	//PAINTSTRUCT ps;
	//HDC hdc;

	//switch (message)
	//{
	//default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	//}
	//return 0;
}
