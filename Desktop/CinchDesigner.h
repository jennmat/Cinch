#pragma once

#include "resource.h"
#include "stdafx.h"
#define MAX_FIELDS 200


using namespace JsonBox;
using namespace std;


class CinchDesigner {
private:
	HWND edit;
	HWND hWnd;
	HBRUSH hbrBkgnd;
	Form* form;
	string type;

	void loadForm(string database, string type);
public:
	ConditionManager* conditionManager;
	Array tabsForUpdate;
	Object formWithUpdates;
	Object loadedForm;

	vector<Object> hiddenTabDefinitions;
	vector<Object> visibleTabDefinitions;

	CinchDesigner(HWND);
	~CinchDesigner();
	Form* getForm();
	void AddField(HWND hWndParent);
	void MoveField(HWND parent);
	void SetupForm(HWND hWnd, HINSTANCE hInst);
	void ChooseForm();
	void SaveForm();
	void SetupDetail(HWND);
	
	void NewDocument(string database, string type);
	void LoadDocument(string database, string _id, Document doc, Object obj);
	
	Object getLoadedForm();
	void setLoadedForm(Object obj);
	string getType();


	static LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
	
	static HWND CreateCinchDesigner(HWND parent);

};


INT_PTR CALLBACK	EditTabs(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    AddField(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK    EditFields(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

