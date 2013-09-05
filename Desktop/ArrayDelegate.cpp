#include "stdafx.h"


void ArrayDelegate::cellContent(int row, int col, wstring &content)
{
	if ( data[row].isObject() ){
		Object o = data[row].getObject();
		string field = fields[col];
		editorForColumn(col, parent, GetModuleHandle(0));

		if ( data[row].isObject() ){
			string s = editors[col]->toString(data[row].getObject());
			content = s2ws(s);
		}
	}
}

void ArrayDelegate::setData(Array array){
	data = array;
	rowCount = array.size();
}

void ArrayDelegate::setupEditorForCell(HWND editor, int row, int col){
	if ( row >= rowCount ){
		SetWindowText(editor, L"");
	} else {
		editors[col]->loadValue(data[row].getObject());
	}
}

void ArrayDelegate::editingFinished(HWND hwnd, int row, int col){
	if ( row < 0 ) return;
	data[row] = editors[col]->storeValue(data[row].getObject());
}


void ArrayDelegate::prepareNewRow(int row){
	data.push_back(Value());
	rowCount++;
}


Array ArrayDelegate::storeValuesToArray(Array a){
	return data;
}



bool ArrayDelegate::allowEditing(int col){
	return true;
}


bool ArrayDelegate::allowHeaderTitleEditing(int col){
	return true;
}

HWND ArrayDelegate::editorForColumn(int col, HWND parent, HINSTANCE hInst){
	if ( editors[col] == NULL ){
		FormField* formField;
		string type = editorTypes[col];

		formField = createFieldForType(parent, "", type, true);

		editors[col] = formField;
	}
	return editors[col]->getControl();
}


void ArrayDelegate::willLoseFocus(){
	detail->getForm()->SaveDocument(fieldId);
}

bool ArrayDelegate::allowNewRows(){
	return true;
}

bool ArrayDelegate::allowNewColumns(){
	return false;
}

int ArrayDelegate::totalRows(){
	return rowCount;
}
