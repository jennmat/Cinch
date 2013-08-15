
#include "stdafx.h"

extern CSwitchPerspectiveHandler* perspectiveHandler;
Array roles;

void LoadRoles(HWND combo){
	QueryOptions options;
	options.includeDocs = true;
	Object results = db.viewResults("all-roles", "by-label", options);
	if (results["rows"].isArray() ){
		roles = results["rows"].getArray();
		for(unsigned int i=0; i<roles.size(); i++){
			Object row = roles[i].getObject();
			string label = row["key"].getString();
			string id = row["id"].getString();
			ComboBox_AddString(combo, s2ws(label).c_str());
		}
	}
}

INT_PTR CALLBACK NewPerspective(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		LoadRoles(GetDlgItem(hDlg, IDC_ROLE_COMBO));
		return (INT_PTR)TRUE;

	case WM_COMMAND:

		if ( LOWORD(wParam) == IDOK ){
			HWND nameEdit = GetDlgItem(hDlg, IDC_NAME_COMBO);
			HWND combo = GetDlgItem(hDlg, IDC_ROLE_COMBO);

			int len = GetWindowTextLength(nameEdit) + 1;
			LPWSTR nameStr = new WCHAR[len];
			GetWindowText(nameEdit, nameStr, len);

			int roleIndex = ComboBox_GetCurSel(combo);

			string name = ws2s(nameStr);
			string label = name;

			for(unsigned int i=0; i<name.length(); i++){
				if ( name[i] == ' ' ) name[i] = '_';
				name[i] = tolower(name[i]);
			}

			Object role = roles[roleIndex].getObject();
			Object roleDoc = role["doc"].getObject();


			Object newPerspective;
			newPerspective["cinch_type"] = "perspective_definition";
			newPerspective["name"] = name;
			newPerspective["label"] = label;
			Array appliesTo = Array();
			appliesTo.push_back(roleDoc["name"]);
			newPerspective["applies_to_roles"] = appliesTo;

			db.createDocument(Value(newPerspective));
			PROPVARIANT val;
			HRESULT res = g_pFramework->GetUICommandProperty(IDR_CMD_SWITCHPERSPECTIVE, UI_PKEY_ItemsSource, &val);
			if ( res == S_OK ){
				IUICollection* pCollection;
				res = val.punkVal->QueryInterface(IID_PPV_ARGS(&pCollection));
			
				perspectiveHandler->AddPerspective(pCollection, newPerspective);
				 

			}
			delete nameStr;

		}

		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}