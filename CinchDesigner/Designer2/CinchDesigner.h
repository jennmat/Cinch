#pragma once

#include "resource.h"
#include "stdafx.h"
#define MAX_FIELDS 200

#define DATEPICKER "DatePicker"
#define NUMBER "Number"
#define YESNO "YesNo"
#define RADIO "Radio"
#define EDIT "Edit"

using namespace JsonBox;
using namespace std;

class CinchDesigner {
private:
	HWND edit;
	HWND hWnd;
	HBRUSH hbrBkgnd;
	Form* form;
	Object loadedForm;
	string type;
public:
	CinchDesigner(HWND);

	Form* getForm();
	void AddField(HWND hWndParent);
	void MoveField(HWND parent);
	void SetupForm(HWND hWnd, HINSTANCE hInst);
	void ChooseForm();
	void SaveForm();
	void SetupDetail(HWND);
	
	void LoadDocument(string database, string _id, Object obj);
	
	Object getLoadedForm();
	void setLoadedForm(Object obj);
	string getType();


	static LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
	
	static HWND CreateCinchDesigner(HWND parent);
	
};


INT_PTR CALLBACK	EditTabs(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    AddField(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK    EditFields(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
