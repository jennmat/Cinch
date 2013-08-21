
#pragma once 

#include "stdafx.h"
#include "JsonBox.h"

using namespace JsonBox;

#define MAX_DETAIL_PAGES 50

#define TABLE_CONTENT 2000
#define TEXTAREA_CONTENT 2001
#define VIEW_CONTENT 2002
#define VIEW_WITH_DOCUMENTS_CONTENT 2004
#define ATTACHMENTS 2003

#define GRID_COLUMNS_CHANGED WM_USER + 10489

class Detail {
private:
	HWND detailPages[MAX_DETAIL_PAGES];
	int initialized[MAX_DETAIL_PAGES];
	int pageCount;
	vector<string> labels;
	wchar_t* fieldName[MAX_DETAIL_PAGES];
	Object configuration;
	HWND detail;
	HWND tabControl;
	Form* form;
	string _id;
	string _rev;
	bool hasLoadedDocument;

	IExplorerBrowser *_peb;
    
public:
	int contentType[MAX_DETAIL_PAGES];
	

	Detail();
	~Detail();
	void setForm(Form *);
	Form* getForm();
	Array serializeUIElements();
	void deserializeUIElements(Object obj);
	void addDetailPage(LPWSTR title);
	void removeAllDetailPages();
	int getDetailPageCount();
	void getDetailPageTitle(int, wchar_t*);
	string getDetailPageFieldName(int);
	void CreateTableForPage(const wchar_t*, GridDelegate* delegate, int i);
	void CreateDetailViewForPage(const wchar_t* label, GridDelegate* delegate, int i);
	void CreateTextareaForPage(const wchar_t*, int i);
	void CreateViewWithDocumentsForPage(const wchar_t* label, GridDelegate* delegate, int i);
	void CreateAttachmentsForPage(const wchar_t*, int i);
	void DestroyPage(int i);
	void InitializePage(int i);
	HWND GetDetailPage(int i);
	void ShowPage(int i);
	void show(HWND parent, HINSTANCE hInst, RECT displayArea);

	HWND getDetailHwnd();
	HWND getTabControlHwnd();

	Object getConfiguration();

	void LoadDocument(Object obj);
	Object StoreValuesToDocument(int changedFieldId, Object obj);

	void LoadAttachments();

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

};

INT_PTR CALLBACK EditColumns(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);


