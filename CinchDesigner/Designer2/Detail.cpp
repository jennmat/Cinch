
#include "stdafx.h"
#include "JsonBox.h"
#include "Windowsx.h"

using namespace JsonBox;
using namespace std;
using namespace Designer;

TCHAR detailClassName[] = _T("CinchDetail");



void RegisterCinchTabControl()
{
    WNDCLASSEX wc;
 
    wc.cbSize         = sizeof(wc);
    wc.lpszClassName  = detailClassName;
    wc.hInstance      = GetModuleHandle(0);
	wc.lpfnWndProc    = Detail::WndProc;
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


Detail::Detail(){
	detail = NULL;
	for(int i=0; i<MAX_DETAIL_PAGES; i++){
		detailPages[i] = NULL;
	}
}

void Detail::ShowPage(int i){
	RECT tabs;
	RECT tabControlClient;
	TabCtrl_GetItemRect(tabControl, 0, &tabs);
	GetClientRect(tabControl, &tabControlClient);
	SetWindowPos(detailPages[i], HWND_TOP, tabControlClient.left, tabs.bottom, tabControlClient.right, tabControlClient.bottom, 0);
	ShowWindow(detailPages[i], SW_SHOW);
	

}

void Detail::CreateTableForPage(int i){
	detailPages[i] = CinchGrid::CreateCinchGrid(tabControl, new BlankDelegate());
	contentType[i] = TABLE_CONTENT;

	ShowPage(i);
}

void Detail::CreateTextareaForPage(int i){
	detailPages[i] = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"", WS_CHILD|WS_VISIBLE|WS_TABSTOP|ES_MULTILINE|ES_AUTOVSCROLL|ES_WANTRETURN,
						0, 0, CONTROL_WIDTH, 200, tabControl, NULL, GetModuleHandle(0), NULL);
	contentType[i] = TEXTAREA_CONTENT;
	HFONT hFont=CreateFont(18,0,0,0,0,0,0,0,0,0,0,0,0,TEXT("MS Shell Dlg"));
	SendMessage(detailPages[i], WM_SETFONT,(WPARAM)hFont,0);	
	
	ShowPage(i);
}

void Detail::load(Object obj)
{
	Array tabs = obj["tabs"].getArray();

	for(unsigned int i=0; i<tabs.size(); i++){
		Object tab = tabs[i].getObject();
		string title = tab["title"].getString();
		wstring wtitle = s2ws(title);
		addDetailPage(LPWSTR(wtitle.c_str()));

		string content = tab["content"].getString();
		if( content.compare("Table") == 0 ){
			CreateTableForPage(i);
		} else if ( content.compare("Text") == 0 ){
			CreateTextareaForPage(i);
		}
		ShowPage(i);
		ShowWindow(detail, SW_SHOW);
	}
}

Array Detail::JsonFormat()
{
	Array tabs;

	for(int i=0; i<getDetailPageCount(); i++){
		wchar_t title[80];
		getDetailPageTitle(i, title);

		char* ctitle = (char *)malloc(80 * sizeof(char));
		size_t t;
		wcstombs_s(&t, ctitle, 80, title, 80);

		Object tab;
		tab["title"] = Value(ctitle);
		if ( contentType[i] == TABLE_CONTENT ){
			tab["content"] = Value("Table");
		} else if (contentType[i] == TEXTAREA_CONTENT ){
			tab["content"] = Value("Text");
		}
		tabs.push_back(tab);

	}

	return tabs;
}

LRESULT CALLBACK Detail::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	
	Detail* self = (Detail *)GetWindowLong(hWnd, GWL_USERDATA);
	switch(message){
	case WM_COMMAND:
		{
			int i = TabCtrl_GetCurSel(self->tabControl);
			if ( i >= 0 ){
				int wmId    = LOWORD(wParam);
				int wmEvent = HIWORD(wParam);
		
				if( wmId == IDM_TABULAR ){
					self->CreateTableForPage(i);
				} else if ( wmId == IDM_TEXTAREA ){
					self->CreateTextareaForPage(i);	
				}
				
			}
		}
		break;
	case WM_NCCREATE:
		{
		CREATESTRUCT* c = (CREATESTRUCT*)lParam;

		Detail * detail = (Detail *)c->lpCreateParams;
		SetWindowLong(hWnd, GWL_USERDATA, (LONG)detail);
		break;
		}
	case WM_RBUTTONDOWN:
		{
			POINT point;
			point.x = GET_X_LPARAM(lParam);
			point.y = GET_Y_LPARAM(lParam);
			HMENU hPopupMenu = CreatePopupMenu();
			InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, IDM_TEXTAREA, L"Text Area");
            InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, IDM_TABULAR, L"Tabular Data");
            ClientToScreen(self->tabControl, &point);
			TrackPopupMenu(hPopupMenu, TPM_TOPALIGN | TPM_LEFTALIGN, point.x, point.y, 0, hWnd, NULL);
		}
		break;
	case WM_NOTIFY:
		{
		LPNMHDR lpnmhdr = (LPNMHDR)lParam;

        // if we received the TCN_SELCHANGE message, process it
        // (TCN_SELCHANGE is when the selection changes from
        // one tab item to another)
        if (lpnmhdr->code == TCN_SELCHANGE)
        {
            // get the newly selected tab item
            INT nTabItem = TabCtrl_GetCurSel(self->tabControl);

			for(int i=0; i<MAX_DETAIL_PAGES; i++){
				if ( self->detailPages[i] != NULL ){
					ShowWindow(self->detailPages[i], SW_HIDE);
				}
			}

			if ( self->detailPages[nTabItem] != NULL ){
				ShowWindow(self->detailPages[nTabItem], SW_SHOW);
			}
            
		}
		}
        return 0;

	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

void Detail::removeAllDetailPages(){
	TabCtrl_DeleteAllItems(tabControl);
}

void Detail::addDetailPage(LPWSTR title){
	LPTCITEM tie = (LPTCITEM)malloc(sizeof(TCITEM));
	tie->mask = TCIF_TEXT;
	tie->pszText = (wchar_t*)malloc(80*sizeof(wchar_t));
	wcscpy_s(tie->pszText, 80, title);
	
	int total = TabCtrl_GetItemCount(tabControl);
	TabCtrl_InsertItem(tabControl, total, tie);

	ShowWindow(detail, SW_SHOW);
}


int Detail::getDetailPageCount(){
	return TabCtrl_GetItemCount(tabControl);
}

void Detail::getDetailPageTitle(int page, wchar_t* buffer){
	LPTCITEM item = new TCITEM();
	item->pszText = (wchar_t*)malloc(80 * sizeof(wchar_t));
	memset(item->pszText, 0, 80 * sizeof(wchar_t));
	item->cchTextMax = 80;
	item->mask = TCIF_TEXT;
	TabCtrl_GetItem(tabControl, page, item);

	wcscpy_s(buffer, 80, item->pszText);
	
}

void Detail::show(HWND parent, HINSTANCE hInst, RECT displayArea){

	if ( detail == NULL ){
		
		RegisterCinchTabControl();

		detail = CreateWindowEx(0, detailClassName, _T("Cinch Detail"), WS_VISIBLE | WS_CHILD, 0, 0, 100, 100, parent, NULL, GetModuleHandle(0), this);

		tabControl = CreateWindow(WC_TABCONTROL, L"", 
			WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE, 
			0, 0, 100, 100, detail, NULL, hInst, NULL); 
    
		//grid = CinchGrid::CreateCinchGrid(parent, new ReferenceDelegate());
	
		/*tab1 = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"", WS_CHILD|WS_VISIBLE|WS_TABSTOP|ES_MULTILINE|ES_AUTOVSCROLL|ES_WANTRETURN,
					0, 0, CONTROL_WIDTH, 200, tabControl, NULL, hInst, NULL);
		tab2 = CinchGrid::CreateCinchGrid(tabControl, new BlankDelegate());
		tab3 = CinchGrid::CreateCinchGrid(tabControl, new BlankDelegate());*/
		
		HFONT hFont=CreateFont(18,0,0,0,0,0,0,0,0,0,0,0,0,TEXT("MS Shell Dlg"));
		//SendMessage(tab1, WM_SETFONT,(WPARAM)hFont,0);	
		SendMessage(tabControl, WM_SETFONT,(WPARAM)hFont,0);	
		//SetWindowSubclass(tabControl, Detail::DetailWndProc, 0, (DWORD_PTR)this);

		
	}

	SetWindowPos(detail, HWND_TOP, 
		displayArea.left,
		displayArea.top,
		displayArea.right-displayArea.left,
		displayArea.bottom-displayArea.top, 0);


	//ShowWindow(detail, SW_HIDE);
	
	SetWindowPos(tabControl, HWND_TOP, 
		displayArea.left,
		displayArea.top,
		displayArea.right-displayArea.left,
		displayArea.bottom-displayArea.top, SWP_NOMOVE);

	ShowWindow(tabControl, SW_SHOW);

	RECT tr;
	TabCtrl_GetItemRect(tabControl, 0, &tr);

	for(int i=0; i<getDetailPageCount(); i++){
		SetWindowPos(detailPages[i], HWND_TOP, 0, tr.bottom, displayArea.right-displayArea.left, displayArea.bottom-displayArea.top, 0);
	}


	/*SetWindowPos(tab1, HWND_TOP, 0, tr.bottom, displayArea.right-displayArea.left, displayArea.bottom-displayArea.top, 0);
	SetWindowPos(tab2, HWND_TOP, 0, tr.bottom, displayArea.right-displayArea.left, displayArea.bottom-displayArea.top, 0);
	SetWindowPos(tab3, HWND_TOP, 0, tr.bottom, displayArea.right-displayArea.left, displayArea.bottom-displayArea.top, 0);
	ShowWindow(tab1, SW_SHOW);
	ShowWindow(tab2, SW_HIDE);
	ShowWindow(tab3, SW_HIDE);*/

		
	//ShowWindow(tab2, SW_SHOW);
}

