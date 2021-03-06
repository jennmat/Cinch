
#include "stdafx.h"
#include "JsonBox.h"
#include "Windowsx.h"

using namespace JsonBox;
using namespace std;

TCHAR detailClassName[] = _T("CinchDetail");

#define DETAIL_START_ID 14

HWND detailHWnd;

INT_PTR CALLBACK AddColumn(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);


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


Detail::Detail() {
	detail = nullptr;
	for(int i=0; i<MAX_DETAIL_PAGES; i++){
		detailPages[i] = NULL;
	}
	memset(initialized, 0, MAX_DETAIL_PAGES);
	pageCount = 0;
}

Detail::~Detail(){
	
}

void Detail::ShowPage(int i){
	if ( initialized[i] == 0 ){
		InitializePage(i);
	}

	if ( contentType[i] == VIEW_CONTENT ){
		if ( loaded[i] == 0 ){
			Object obj = db.getDocument(_id, _rev).getData().getObject();
			GridDelegate* delegate = (GridDelegate *)pageData[i];
			DetailViewDelegate* d = (DetailViewDelegate*)delegate;
			d->LoadDocument(obj);
			loaded[i] = 1;
			CinchGrid* grid = (CinchGrid*)GetWindowLong(detailPages[i], GWL_USERDATA);
			grid->reloadData();
		}
		
	}

	

	RECT tabs;
	RECT tabControlClient;
	TabCtrl_GetItemRect(tabControl, 0, &tabs);
	GetClientRect(tabControl, &tabControlClient);
	SetWindowPos(detailPages[i], HWND_TOP, tabControlClient.left, tabs.bottom, tabControlClient.right, tabControlClient.bottom-tabs.bottom, 0);
	ShowWindow(detailPages[i], SW_SHOW);

	memset(visible,0,MAX_DETAIL_PAGES);
	visible[i] = 1;
			
}

void Detail::DestroyPage(int i){
	if ( initialized[i] == 1 ){
		if ( contentType[i] == TABLE_CONTENT || contentType[i] == VIEW_CONTENT || contentType[i] == VIEW_WITH_DOCUMENTS_CONTENT ){
			CinchGrid* grid = (CinchGrid*)GetWindowLong(detailPages[i], GWL_USERDATA);
			GridDelegate* d = grid->getDelegate();
			DestroyWindow(detailPages[i]);
			delete d;
		} else {
			DestroyWindow(detailPages[i]);
		}
	}
	delete fieldName[i];
}

void Detail::CreateTableForPage(const wchar_t* field, GridDelegate* delegate, int i){
	detailPages[i] = CinchGrid::CreateCinchGrid(detail, delegate);
	contentType[i] = TABLE_CONTENT;
	pageCount++;
	int len = wcslen(field) + sizeof(wchar_t);
	fieldName[i] = new wchar_t[len];
	memset(fieldName[i], 0, len);
	wcscpy_s(fieldName[i], len, field);
	initialized[i] = 1;
	ShowPage(i);
}

void Detail::CreateDetailViewForPage(const wchar_t* label, GridDelegate* delegate, int i){
	pageData[i] = (ULONG_PTR)delegate;
	contentType[i] = VIEW_CONTENT;
	pageCount++;
	int len = wcslen(label) + sizeof(wchar_t);
	fieldName[i] = new wchar_t[len];
	memset(fieldName[i], 0, len);
	wcscpy_s(fieldName[i], len, label);
	initialized[i] = 0;
	//ShowPage(i);
}

void Detail::CreateViewWithDocumentsForPage(const wchar_t* label, GridDelegate* delegate, int i){
    HWND wnd = CinchGrid::CreateCinchGrid(detail, delegate);
	detailPages[i] = wnd;
	contentType[i] = VIEW_WITH_DOCUMENTS_CONTENT;
	pageCount++;
	int len = wcslen(label) + sizeof(wchar_t);
	fieldName[i] = new wchar_t[len];
	memset(fieldName[i], 0, len);
	wcscpy_s(fieldName[i], len, label);
	initialized[i] = 1;
	ShowPage(i);
}

void Detail::CreateTextareaForPage(const wchar_t* field, int i){
	detailPages[i] = CreateWindowEx(0, L"EDIT", L"", WS_CHILD|WS_VISIBLE|WS_TABSTOP|ES_MULTILINE|ES_AUTOVSCROLL|ES_WANTRETURN,
						0, 0, CONTROL_WIDTH, 200, detail, (HMENU)(i+DETAIL_START_ID), GetModuleHandle(0), NULL);
	contentType[i] = TEXTAREA_CONTENT;
	pageCount++;
	int len = wcslen(field) + sizeof(wchar_t);
	fieldName[i] = new wchar_t[len];
	memset(fieldName[i], 0, len);
	wcscpy_s(fieldName[i], len, field);

	HFONT hFont=DEFAULT_FONT;
	SendMessage(detailPages[i], WM_SETFONT,(WPARAM)hFont,0);	
	initialized[i] = 1;
	ShowPage(i);
}

void Detail::CreateAttachmentsForPage(const wchar_t* field, int i){
	RegisterAttachmentViewer();
	pageCount++;

	int len = wcslen(field) + sizeof(wchar_t);
	fieldName[i] = new wchar_t[len];
	memset(fieldName[i], 0, len);
	wcscpy_s(fieldName[i], len, field);

	detailPages[i] = CreateWindowEx(0, L"CinchAttachments", L"", WS_CHILD, 0, 0, CONTROL_WIDTH, 200, detail, NULL, GetModuleHandle(0), this);
	contentType[i] = ATTACHMENTS;
	initialized[i] = 0;
}


void Detail::InitializePage(int i){
	if ( contentType[i] == ATTACHMENTS ){
		AttachmentViewer* viewer = (AttachmentViewer *)GetWindowLong(detailPages[i], GWL_USERDATA);
		viewer->initialize();
	} else if ( contentType[i] == VIEW_CONTENT ){
		GridDelegate* delegate = (GridDelegate *)pageData[i];
		HWND wnd = CinchGrid::CreateCinchGrid(detail, delegate);
		if ( hasLoadedDocument ) {
			Object obj = db.getDocument(_id, _rev).getData().getObject();
			DetailViewDelegate* d = (DetailViewDelegate*)delegate;
			d->LoadDocument(obj);
		}
		
		detailPages[i] = wnd;
		initialized[i] = 1;
	}
}


HWND Detail::GetDetailPage(int i){
	return detailPages[i];
}

void Detail::deserializeUIElements(Object obj)
{
	
	for(int i=0; i<getDetailPageCount(); i++){
		detailPages[i] = NULL;
		contentType[i] = 0;
		initialized[i] = 0;
		fieldName[i] = NULL;
	}
	labels.clear();

	pageCount = 0;

	configuration = obj;

	Array tabs = obj["tabs"].getArray();

	
	for(unsigned int i=0; i<tabs.size(); i++){
		Object tab = tabs[i].getObject();
		string field = tab["field"].getString();
		string label = tab["label"].getString();
		
		wstring wtitle = s2ws(label);
		addDetailPage(LPWSTR(wtitle.c_str()));
		labels.push_back(label);
		string content = tab["content"].getString();
		if ( content.compare(VIEW_WITH_DOCUMENTS_DETAIL) == 0 ){
			Object config = tab["config"].getObject();
			string design = config["design"].getString();
			string view = config["view"].getString();
			string viewDefId = config["view_definition_id"].getString();
			string refField = config["referencing_field"].getString();
			string docs_of_type = "";

			DetailViewDelegate * del = new DetailViewDelegate(this, i+DETAIL_START_ID, design, view, viewDefId, docs_of_type, refField);
			CreateDetailViewForPage(s2ws(label).c_str(), del, i);
			del->setIncludeDocs(true);
			del->deserializeUIElements(detailPages[i], tab["config"].getObject());
		}
		else if ( content.compare(VIEW) == 0 ){
			
			Object config = tab["config"].getObject();
			string design = config["design"].getString();
			string view = config["view"].getString();
			string viewDefId = config["view_definition_id"].getString();
			string refField = config["referencing_field"].getString();
			string docs_of_type = "";

			DetailViewDelegate * del = new DetailViewDelegate(this, i+DETAIL_START_ID, design, view, viewDefId, docs_of_type, refField);
			if ( !config["columns"].isArray() ){

				QueryOptions options;
				options.startKey = Value(docs_of_type);
				options.endKey = Value(docs_of_type);

				Object results = db.viewResults("all-document-types", "by-name", options);
				if ( results["rows"].isArray() ){
					Array rows = results["rows"].getArray();
					if ( rows.size() > 0 ){
						Object row = rows[0].getObject();
						string id = row["id"].getString();
						Document doc  = db.getDocument(id);
						Object obj = doc.getData().getObject();
						Array columns = Array();
						Object column = Object();
						column["name"] = obj["first_field_name"].getString();
						column["label"] = obj["first_field_label"].getString();
						column["width"] = 250;
						column["editor"] = EDIT;
						columns.push_back(column);
						config["columns"] = columns;
						//del->addColumn(obj["first_field_name"].getString(), s2ws(obj["first_field_label"].getString()), EDIT);
					}
				}
			}

			CreateDetailViewForPage(s2ws(label).c_str(), del, i);
			del->deserializeUIElements(detailPages[i], tab["config"].getObject());
			//del->addColumn("title", L"Title", EDIT);
			
		} else if( content.compare(TABLE) == 0 ){
			Document doc = db.getDocument(field);
			Object obj = doc.getData().getObject();

			string name = obj["_id"].getString();

			ArrayDelegate* delegate = new ArrayDelegate(this, i+DETAIL_START_ID);
			CreateTableForPage(s2ws(name).c_str(), delegate, i);
			delegate->deserializeUIElements(detailPages[i], tab["config"].getObject());
			
		} else if ( content.compare(TEXT_DETAIL) == 0 ){
			Document doc = db.getDocument(field);
			Object obj = doc.getData().getObject();

			string name = obj["_id"].getString();

			CreateTextareaForPage(s2ws(name).c_str(), i);
		} else if ( content.compare(ATTACHMENTS_DETAIL) == 0 ){
			CreateAttachmentsForPage(L"attachments", i);
		}
	}

	
	ShowWindow(detail, SW_SHOW);
	
	if ( getDetailPageCount() > 0 ){
		//ShowWindow(detailPages[0], SW_SHOW);
		ShowPage(0);
	}

}



Array Detail::serializeUIElements()
{
	Array tabs;

	for(int i=0; i<getDetailPageCount(); i++){
		wchar_t title[80];
		getDetailPageTitle(i, title);

		char* ctitle = (char *)malloc(80 * sizeof(char));
		size_t t;
		wcstombs_s(&t, ctitle, 80, title, 80);

		Object tab;
		tab["label"] = labels[i];
		if ( contentType[i] == TABLE_CONTENT ){
			tab["field"] = ws2s(fieldName[i]);
			tab["content"] = Value(TABLE);
			CinchGrid* grid = (CinchGrid *)GetWindowLong(detailPages[i], GWL_USERDATA);
			ArrayDelegate* delegate = (ArrayDelegate *)grid->getDelegate();
			Object config;
			delegate->serializeUIElements(config);
			tab["config"] = config;

		} else if (contentType[i] == TEXTAREA_CONTENT ){
			tab["name"] = ws2s(fieldName[i]);
			tab["content"] = Value(TEXT_DETAIL);
		} else if ( contentType[i] == VIEW_CONTENT ){
			tab["content"] = "View";
			DetailViewDelegate* delegate = (DetailViewDelegate *)pageData[i];
			Object config;
			delegate->serializeUIElements(config);
			tab["config"] = config;

		} else if ( contentType[i] == ATTACHMENTS ){
			tab["content"] = ATTACHMENTS_DETAIL;
			tab["field"] = "attachments";
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
			int wmId    = LOWORD(wParam);
			int wmEvent = HIWORD(wParam);
			if ( wmId == IDD_EDIT_COLUMNS ){
				DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_EDIT_COLUMNS), hWnd, EditColumns);
				break;
			}
			else if ( wmEvent == EN_KILLFOCUS || wmEvent == NM_KILLFOCUS || wmEvent == BN_KILLFOCUS ){
				self->getForm()->SaveDocument(wmId);
			} else {
				int i = TabCtrl_GetCurSel(self->tabControl);
				if ( i >= 0 ){
					if( wmId == IDM_TABULAR ){
						//self->CreateTableForPage(i);
					} else if ( wmId == IDM_TEXTAREA ){
						//self->CreateTextareaForPage(i);	
					}
				}
			}
		}
		break;
	case WM_KILLFOCUS:
		break;
	case WM_NCCREATE:
		{
		CREATESTRUCT* c = (CREATESTRUCT*)lParam;

		Detail * detail = (Detail *)c->lpCreateParams;
		SetWindowLong(hWnd, GWL_USERDATA, (LONG)detail);
		detailHWnd = hWnd;
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
	case WM_ATTACHMENTS_UPLOADED:
		{
			self->getForm()->ReloadDocument();
			break;
		}
	case WM_ERASEBKGND:
		{
			RECT rect;
			GetClientRect(hWnd, &rect);
			UINT rheight;
			GetRibbonHeight(&rheight);
			rect.top = rheight;
			HDC dc = (HDC)wParam;
			FillRect(dc, &rect, CreateSolidBrush(DEFAULT_BACKGROUND_COLOR));

		}
		return 1;
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

			for(int i=0; i<self->pageCount; i++){
				if ( self->detailPages[i] != NULL ){
					ShowWindow(self->detailPages[i], SW_HIDE);
				}
			}
			
			if ( self->initialized[nTabItem] == 0 ){
				self->InitializePage(nTabItem);
			}
				
			if ( self->detailPages[nTabItem] != NULL ){
				self->ShowPage(nTabItem);
				ShowWindow(self->detailPages[nTabItem], SW_SHOW);
				//ShowWindow(self->detailPages[nTabItem], SW_SHOW);
			}
            
		}
		}
        return 0;
	case GRID_COLUMNS_CHANGED:
		for(int i=0; i<self->getDetailPageCount(); i++){
			HWND wnd = self->GetDetailPage(i);
			CinchGrid* grid = (CinchGrid *)GetWindowLong(wnd, GWL_USERDATA);
			if ( grid != NULL ){
				grid->reloadData();
			}
		}
		break;

		}
	
	return DefWindowProc(hWnd, message, wParam, lParam);
}



INT_PTR CALLBACK EditColumns(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	Detail* detail = (Detail *)GetWindowLong(detailHWnd, GWL_USERDATA);

	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		{
	
			HWND visibleFields = GetDlgItem(hDlg, IDC_VISIBLE_FIELDS);
			HWND hiddenFields = GetDlgItem(hDlg, IDC_HIDDEN_FIELDS);

			int currentTab = TabCtrl_GetCurSel(detail->getTabControlHwnd());
			Object configuration = detail->getConfiguration();
			Array tabs = configuration["tabs"].getArray();
			
			Object tab = tabs[currentTab].getObject();
			Object config = tab["config"].getObject();

			string type;
			if ( tab["content"].getString().compare(VIEW) == 0 ){
				string view = config["view_definition_id"].getString();
				Object viewDef = db.getDocument(view).getData().getObject();
				type = viewDef["emits"].getString();
			}
			if ( tab["content"].getString().compare(TABLE) == 0 ) {
				type = config["field"].getString();
			}
			
			Array columns = config["columns"].getArray();
			for(unsigned int j=0; j<columns.size(); j++){
				Object columnConfig = columns[j].getObject();
				string field = columnConfig["field"].getString();
				Object fieldConfig = db.getDocument(field).getData().getObject();

				wstring label = s2ws(fieldConfig["label"].getString());
				int index = SendMessage(visibleFields, LB_ADDSTRING, 0, (LPARAM) label.c_str()); 
				ListBox_SetItemData(visibleFields, index, new Object(columnConfig));
			}

			vector<string> attributes = collectAttributes(type);

			for(unsigned int i=0; i<attributes.size(); i++){
				string attribute = attributes[i];

				bool found = false;

				for(unsigned int j=0; j<columns.size(); j++){
					Object columnConfig = columns[j].getObject();
					string field = columnConfig["field"].getString();
					if ( field.compare(attribute) == 0 ){
						found = true;
					}
				}

				if ( !found ){
					Object columnConfig;
					columnConfig["field"] = attribute;
					columnConfig["width"] = 200;
					Object obj = db.getDocument(attribute).getData().getObject();


					wstring label = s2ws(obj["label"].getString());
					int index = SendMessage(hiddenFields, LB_ADDSTRING, 0, (LPARAM) label.c_str()); 
					ListBox_SetItemData(hiddenFields, index, new Object(columnConfig));
				}
			}

			/*
			for(unsigned int i=0; i<tabs.size(); i++){
				Object tab = tabs[i].getObject();
				if ( tab["field"].getString().compare(tabName) == 0 ){

					Object obj = db.getDocument(tabName).getData().getObject();
					string array_contents = obj["array_contents"].getString();
					

				
				}
			}
			*/
		
			return (INT_PTR)TRUE;
		}
	case WM_DESTROY:
		{
			HWND visibleFields = GetDlgItem(hDlg, IDC_VISIBLE_FIELDS);
			HWND hiddenFields = GetDlgItem(hDlg, IDC_HIDDEN_FIELDS);

			int count = ListBox_GetCount(visibleFields);
			for(int i=0; i<count; i++){
				Object* o = (Object *)ListBox_GetItemData(visibleFields, i);
				delete o;
			}
			count = ListBox_GetCount(hiddenFields);
			for(int i=0; i<count; i++){
				Object* o = (Object *)ListBox_GetItemData(hiddenFields, i);
				delete o;
			}
		}
		break;
	case WM_COMMAND:
		if ( LOWORD(wParam) == IDCANCEL ){
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		if (LOWORD(wParam) == IDOK)
		{

			HWND visibleFields = GetDlgItem(hDlg, IDC_VISIBLE_FIELDS);
			
			int currentTab = TabCtrl_GetCurSel(detail->getTabControlHwnd());
			Object configuration = detail->getConfiguration();
			Array tabs = configuration["tabs"].getArray();

			Object tabConfig = tabs[currentTab].getObject();
			Array columns;
			int count = ListBox_GetCount(visibleFields);
			for(int i=0; i<count; i++){
				ULONG_PTR data = ListBox_GetItemData(visibleFields, i);
				Object obj = *(Object *)data;

				columns.push_back(obj);
			}

			tabConfig["columns"] = columns;

			if ( detail->contentType[currentTab] == TABLE_CONTENT ){
				CinchGrid* grid = (CinchGrid *)GetWindowLong(detail->GetDetailPage(currentTab), GWL_USERDATA);
				ArrayDelegate* delegate = (ArrayDelegate *)grid->getDelegate();
				delegate->deserializeUIElements(detailHWnd, tabConfig);
			} else if ( detail->contentType[currentTab] == VIEW_CONTENT ){
				CinchGrid* grid = (CinchGrid *)GetWindowLong(detail->GetDetailPage(currentTab), GWL_USERDATA);
				DetailViewDelegate* delegate = (DetailViewDelegate *)grid->getDelegate();
				delegate->deserializeUIElements(detailHWnd, tabConfig);
				grid->reloadLayout();
				grid->reloadData();
	
			}
	
			detail->getForm()->getDelegate()->formModified();

			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		else if ( LOWORD(wParam) == IDC_ADD_TO_VISIBLE_FIELDS )
		{
			HWND hiddenFields = GetDlgItem(hDlg, IDC_HIDDEN_FIELDS);
			HWND visibleFields = GetDlgItem(hDlg, IDC_VISIBLE_FIELDS);

		
			int selected = ListBox_GetCurSel(hiddenFields);
			if( selected != LB_ERR ){
				wchar_t text[80];
				ListBox_GetText(hiddenFields, selected, text);
				ULONG_PTR data = ListBox_GetItemData(hiddenFields, selected);

				int pos = (int)SendMessage(visibleFields, LB_ADDSTRING, 0, (LPARAM)text); 
				ListBox_SetItemData(visibleFields, pos, data);
				//_this->getForm()->addField(FormField::createEditField(hWnd, GetModuleHandle(0), text, text));
				ListBox_DeleteString(hiddenFields, selected);

			}
		} 
		else if ( LOWORD(wParam) == IDC_REMOVE_FROM_VISIBLE_FIELDS )
		{
			HWND hidden = GetDlgItem(hDlg, IDC_HIDDEN_FIELDS);
			HWND visible = GetDlgItem(hDlg, IDC_VISIBLE_FIELDS);

		
			int selected = ListBox_GetCurSel(visible);
			if( selected != LB_ERR ){
				wchar_t text[80];
				ListBox_GetText(visible, selected, text);
				
				int pos = (int)SendMessage(hidden, LB_ADDSTRING, 0, (LPARAM)text); 

				ListBox_DeleteString(visible, selected);
				//_this->getForm()->getLayout()->removeField(selected);
			}
		}
		else if ( LOWORD(wParam) == IDC_FIELD_UP )
		{
			HWND hidden = GetDlgItem(hDlg, IDC_HIDDEN_FIELDS);
			HWND visible = GetDlgItem(hDlg, IDC_VISIBLE_FIELDS);

		
			int selected = ListBox_GetCurSel(visible);
			if( selected != LB_ERR && selected > 0 ){
				wchar_t text[80];
				ListBox_GetText(visible, selected, text);
				ULONG_PTR data = ListBox_GetItemData(visible, selected);

				ListBox_DeleteString(visible, selected);
				ListBox_InsertString(visible, selected-1, text);
				ListBox_SetItemData(visible, selected-1, data);

				ListBox_SetCurSel(visible, selected-1);

				//_this->getForm()->getLayout()->swapFields(selected, selected-1);
				
				
			}
		}
		else if ( LOWORD(wParam) == IDC_FIELD_DOWN )
		{
			HWND hidden = GetDlgItem(hDlg, IDC_HIDDEN_FIELDS);
			HWND visible = GetDlgItem(hDlg, IDC_VISIBLE_FIELDS);

		
			int selected = ListBox_GetCurSel(visible);
			int count = ListBox_GetCount(visible);
			if( selected != LB_ERR && selected < count -1 ){
				wchar_t text[80];
				ListBox_GetText(visible, selected, text);
				ULONG_PTR data = ListBox_GetItemData(visible, selected);
				
				ListBox_DeleteString(visible, selected);
				ListBox_InsertString(visible, selected+1, text);
				ListBox_SetItemData(visible, selected+1, data);
				ListBox_SetCurSel(visible, selected+1);

				//_this->getForm()->getLayout()->swapFields(selected, selected+1);
				
			}

		} else if (LOWORD(wParam) == IDC_NEW_FIELD ){
			DialogBox(GetModuleHandle(0), MAKEINTRESOURCE(IDD_ADD_FIELD), hDlg, AddColumn);
		}
		break;
	}
	return (INT_PTR)FALSE;
}


void Detail::removeAllDetailPages(){
	for(int i=0; i<getDetailPageCount(); i++){
		DestroyPage(i);
	}
	TabCtrl_DeleteAllItems(tabControl);
}

void Detail::addDetailPage(LPWSTR title){
	LPTCITEM tie = new TCITEM;
	tie->mask = TCIF_TEXT;
	tie->pszText = new wchar_t[80];
	wcscpy_s(tie->pszText, 80, title);
	
	int total = TabCtrl_GetItemCount(tabControl);
	TabCtrl_InsertItem(tabControl, total, tie);
	delete tie->pszText;
	delete tie;
	ShowWindow(detail, SW_SHOW);
}


int Detail::getDetailPageCount(){
	return pageCount;
}

void Detail::getDetailPageTitle(int page, wchar_t* buffer){
	LPTCITEM item = new TCITEM();
	item->pszText = new wchar_t[80];
	memset(item->pszText, 0, 80 * sizeof(wchar_t));
	item->cchTextMax = 80;
	item->mask = TCIF_TEXT;
	TabCtrl_GetItem(tabControl, page, item);

	wcscpy_s(buffer, 80, item->pszText);
	delete item->pszText;
	delete item;
	
}

void Detail::show(HWND parent, HINSTANCE hInst, RECT displayArea){

	if ( detail == NULL ){
		
		RegisterCinchTabControl();

		detail = CreateWindowEx(0, detailClassName, _T("Cinch Detail"), WS_VISIBLE | WS_CHILD, 0, 0, 100, 100, parent, NULL, GetModuleHandle(0), this);

		tabControl = CreateWindow(WC_TABCONTROL, L"", 
			WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE, 
			0, 0, 0, 0, detail, NULL, hInst, NULL); 
		
		

		//grid = CinchGrid::CreateCinchGrid(parent, new ReferenceDelegate());
	
		/*tab1 = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"", WS_CHILD|WS_VISIBLE|WS_TABSTOP|ES_MULTILINE|ES_AUTOVSCROLL|ES_WANTRETURN,
					0, 0, CONTROL_WIDTH, 200, tabControl, NULL, hInst, NULL);
		tab2 = CinchGrid::CreateCinchGrid(tabControl, new BlankDelegate());
		tab3 = CinchGrid::CreateCinchGrid(tabControl, new BlankDelegate());*/
		
		HFONT hFont=DEFAULT_FONT;
		//SendMessage(tab1, WM_SETFONT,(WPARAM)hFont,0);	
		SendMessage(tabControl, WM_SETFONT,(WPARAM)hFont,0);	
		//SetWindowSubclass(tabControl, Detail::WndProc, 0, (DWORD_PTR)this);

		
	}

	SetWindowPos(detail, HWND_TOP, 
		displayArea.left,
		displayArea.top,
		displayArea.right,
		displayArea.bottom-displayArea.top, 0);


	//ShowWindow(detail, SW_HIDE);
	
	SetWindowPos(tabControl, HWND_TOP, 
		displayArea.left,
		displayArea.top,
		displayArea.right,
		displayArea.bottom-displayArea.top, SWP_NOMOVE);

	ShowWindow(tabControl, SW_SHOW);

	
	RECT tr;
	RECT tabRect;
	TabCtrl_GetItemRect(tabControl, 0, &tr);
	GetClientRect(tabControl, &tabRect);

	for(int i=0; i<getDetailPageCount(); i++){
		SetWindowPos(detailPages[i], HWND_TOP, 0, tr.bottom, tabRect.right, tabRect.bottom-tr.bottom, 0);
	}


	/*SetWindowPos(tab1, HWND_TOP, 0, tr.bottom, displayArea.right-displayArea.left, displayArea.bottom-displayArea.top, 0);
	SetWindowPos(tab2, HWND_TOP, 0, tr.bottom, displayArea.right-displayArea.left, displayArea.bottom-displayArea.top, 0);
	SetWindowPos(tab3, HWND_TOP, 0, tr.bottom, displayArea.right-displayArea.left, displayArea.bottom-displayArea.top, 0);
	ShowWindow(tab1, SW_SHOW);
	ShowWindow(tab2, SW_HIDE);
	ShowWindow(tab3, SW_HIDE);*/

		
	//ShowWindow(tab2, SW_SHOW);
}


void Detail::setForm(Form *_form){
	form = _form;
}

Form * Detail::getForm(){
	return form;
}



void Detail::LoadDocument(Object obj){
	hasLoadedDocument = true;
	memset(loaded, 0, MAX_DETAIL_PAGES);
	_id = obj["_id"].getString();
	_rev = obj["_rev"].getString();

	for(int i=0; i<getDetailPageCount(); i++){
		HWND detail = GetDetailPage(i);
		if ( contentType[i] == VIEW_CONTENT && initialized[i] == 1) {
			
			CinchGrid* gridcontrol = (CinchGrid *)GetWindowLong(detail, GWL_USERDATA);
			DetailViewDelegate * d = (DetailViewDelegate *)gridcontrol->getDelegate();
			
			if ( visible[i] == 1 && loaded[i] == 0 ){
				d->LoadDocument(obj);
				gridcontrol->reloadData();
				loaded[i] = 1;
			}
		} else if ( contentType[i] == TABLE_CONTENT ){ 
			CinchGrid* gridcontrol = (CinchGrid *)GetWindowLong(detail, GWL_USERDATA);
			wchar_t* field = fieldName[i];
			string f = ws2s(field);
			const char* cfieldname = f.c_str();
		
			Array a = obj[cfieldname].getArray();

			/*ArrayOfObjectsDelegate* d = new ArrayOfObjectsDelegate(this, i+DETAIL_START_ID);
			d->addColumn("date", L"Date");
			d->addColumn("result", L"Result");
			d->setData(obj[cfieldname].getArray());
			gridcontrol->setDelegate(d);*/
			ArrayDelegate * d = (ArrayDelegate *)gridcontrol->getDelegate();
			d->setData(obj[cfieldname].getArray());

			gridcontrol->reloadData();
		} else if ( contentType[i] == TEXTAREA_CONTENT ) {
			wchar_t* field = fieldName[i];
			string f = ws2s(field);
			const char* cfieldname = f.c_str();
		
			if ( obj[cfieldname].isString() ){
				string val = obj[cfieldname].getString();
				wstring valw =s2ws(val);
				LPCWSTR r = valw.c_str();
				SetWindowText(detailPages[i], r);
			} else {
				SetWindowText(detailPages[i], L"");
			}
		} else if ( contentType[i] == ATTACHMENTS ){
			AttachmentViewer* viewer = (AttachmentViewer *)GetWindowLong(detailPages[i], GWL_USERDATA);
			viewer->LoadDocument(obj);
		}
	}
}


Object Detail::StoreValuesToDocument(int changedFieldId, Object obj){
	int page = changedFieldId - DETAIL_START_ID;
	if ( page >= 0 && page < getDetailPageCount() && contentType[page] != ATTACHMENTS ){
		wchar_t* field = fieldName[page];
		string f = ws2s(field);
		const char* cfieldname = f.c_str();
		HWND detail = GetDetailPage(page);
		
		if ( contentType[page] == TEXTAREA_CONTENT ){
			int len = GetWindowTextLength(detailPages[page]);
			len++;
			wchar_t* text = new wchar_t[len];
			memset(text, 0, len);
			GetWindowText(detailPages[page], text, len); 
	
			string s =ws2s(text);
			obj[cfieldname] = s;
		} else if ( contentType[page] == TABLE_CONTENT ){
			CinchGrid* gridcontrol = (CinchGrid *)GetWindowLong(detail, GWL_USERDATA);
			
			ArrayDelegate * d = (ArrayDelegate *)gridcontrol->getDelegate();

			Array a = obj[cfieldname].getArray();
			Array updated = d->storeValuesToArray(a);
			obj[cfieldname] = updated;
		}
	}

	return obj;
}


HWND Detail::getDetailHwnd(){
	return detail;
}

HWND Detail::getTabControlHwnd(){
	return tabControl;
}

string Detail::getDetailPageFieldName(int i){
	return string(ws2s(fieldName[i]));
}

Object Detail::getConfiguration(){
	return configuration;
}


INT_PTR CALLBACK AddColumn(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	Detail* detail = (Detail *)GetWindowLong(detailHWnd, GWL_USERDATA);
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		{
		HWND combo = GetDlgItem(hDlg, IDC_NEW_FIELD_TYPE);
		SendMessage(combo, CB_ADDSTRING, 0, (LPARAM)L"Text");
		SendMessage(combo, CB_ADDSTRING, 0, (LPARAM)L"Date");
		
		SetWindowText(hDlg, L"Add A New Column");
	
		ComboBox_SelectString(combo, 0, L"Text");
		return (INT_PTR)TRUE;
		}

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			int len = GetWindowTextLength(GetDlgItem(hDlg, IDC_NEW_FIELD_NAME));
			if ( len > 0 ){
				wchar_t* szNewFieldName = new wchar_t[80];
				wchar_t* szNewFieldLabel = new wchar_t[80];

				GetDlgItemText(hDlg, IDC_NEW_FIELD_NAME, szNewFieldName, 80);
				GetDlgItemText(hDlg, IDC_NEW_FIELD_LABEL, szNewFieldLabel, 80);
				HWND parent = GetParent(hDlg);
				HWND visibleFieldsList = GetDlgItem(parent, IDC_VISIBLE_FIELDS);
				int count = ListBox_GetCount(visibleFieldsList);

				ListBox_InsertString(visibleFieldsList, count, szNewFieldLabel);

				HWND typeCombo = GetDlgItem(hDlg, IDC_NEW_FIELD_TYPE);
				int selected = SendMessage(GetDlgItem(hDlg, IDC_NEW_FIELD_TYPE), CB_GETCURSEL, 0, 0);
				int type = SendMessage(typeCombo, CB_GETCURSEL, 0, 0);
				
				int currentTab = TabCtrl_GetCurSel(detail->getTabControlHwnd());
				string tabName = detail->getDetailPageFieldName(currentTab);
				HWND hwnd = detail->GetDetailPage(currentTab);
				CinchGrid* grid = (CinchGrid*)GetWindowLong(hwnd, GWL_USERDATA);
				ArrayDelegate * d = (ArrayDelegate *)grid->getDelegate();

				switch(type){
				case 0:
					d->addColumn(ws2s(szNewFieldName), szNewFieldLabel, EDIT, Value());
					break;
				case 1:
					d->addColumn(ws2s(szNewFieldName), szNewFieldLabel, DATEPICKER, Value());
					break;
				default:
					break;
				}

				PostMessage(detail->getDetailHwnd(), GRID_COLUMNS_CHANGED, 0, 0);
				detail->getForm()->getDelegate()->formModified();
	
			}

			
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
