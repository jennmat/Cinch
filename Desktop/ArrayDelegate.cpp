#include "stdafx.h"

/*
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
*/

void ArrayDelegate::LoadSegment(int start_row, int len, wchar_t*** segment, int* rows_loaded, int* cols_loaded){
	int segment_index = 0;
	for(UINT row_index=start_row; row_index<len; row_index++){
		if ( data[row_index].isObject() ){
			for(int col=0; col < totalColumns(); col++ ){
				Object o = data[row_index].getObject();
				string field = fields[col];
				string value = serializeForDisplay(o[field], field);
				wstring w = s2ws(value);
				int len = w.length() + sizeof(wchar_t);
				segment[segment_index][col] = new wchar_t[w.length()+sizeof(wchar_t)];
				wcscpy_s(segment[segment_index][col], len, w.c_str());
				
			}
		}
		segment_index++;
	}
	*rows_loaded = segment_index;
	*cols_loaded = totalColumns();
}

void ArrayDelegate::CleanupSegment(int rows, int cols, wchar_t*** segment){
	for(int i=0; i<rows; i++){
		for(int col=0; col<cols; col++){
			delete segment[i][col];
		}
	}
}





void ArrayDelegate::setData(Array array){
	data = array;
	rowCount = array.size();
}

void ArrayDelegate::setupEditorForCell(HWND editor, int row, int col, wchar_t*** displayData){
	if ( row >= rowCount ){
		SetWindowText(editor, L"");
	} else {
		editors[col]->loadValue(data[row].getObject());
	}
}

void ArrayDelegate::editingFinished(HWND hwnd, int row, int col, wchar_t*** displayData){
	if ( row < 0 ) return;
	delete displayData[row][col];
	data[row] = editors[col]->storeValue(data[row].getObject());
	string field = fields[col];
	string disp = serializeForDisplay(data[row][field], fields[col]);
	wstring w = s2ws(disp);
	int len = w.length() + sizeof(wchar_t);
	displayData[row][col] = new wchar_t[len];
	wcscpy_s(displayData[row][col], len, w.c_str());
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
