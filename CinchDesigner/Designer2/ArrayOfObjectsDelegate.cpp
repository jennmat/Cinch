
#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>

using namespace std;
using namespace JsonBox;
using namespace CouchDB;


ArrayOfObjectsDelegate::ArrayOfObjectsDelegate(Detail * _d, int _fieldId){
	detail = _d;
	fieldId = _fieldId;
}

int ArrayOfObjectsDelegate::totalRows()
{
	return rowCount;
}

void ArrayOfObjectsDelegate::addColumn(std::string field, std::wstring title){
	fields.push_back(field);
	titles.push_back(title);
}

void ArrayOfObjectsDelegate::setData(Array array){
	rowCount = array.size();
	for(unsigned int i=0; i<array.size(); i++){
		Object o = array[i].getObject();
		vector<wstring> rowData;
		for(unsigned int j=0; j<fields.size(); j++){
			string data = o[fields[j]].getString();
			wstring w = Designer::s2ws(data);
			rowData.push_back(w);	
		}
		data.push_back(rowData);
	} 
}

int ArrayOfObjectsDelegate::totalColumns(){
	return fields.size();
}


int ArrayOfObjectsDelegate::columnWidth(int col){
	return 200;
}


int ArrayOfObjectsDelegate::rowHeight(){
	return 25;
}

wchar_t* ArrayOfObjectsDelegate::headerContent(int col)
{
	wchar_t* result;

	wstring title = titles[col];
	int size = title.size() + sizeof(wchar_t);
	result = new wchar_t[size];
	wcscpy_s(result, size, title.c_str());
	return result;
}

const wchar_t* ArrayOfObjectsDelegate::cellContent(int row, int col)
{
	wchar_t* result;

    vector<wstring> rowData = data[row];
	wstring data = rowData[col];

	result = new wchar_t[80];
	wcscpy_s(result, 80, data.c_str());
	return result;
}

bool ArrayOfObjectsDelegate::stickyHeaders(){
	return false;
}

bool ArrayOfObjectsDelegate::drawHorizontalGridlines(){
	return true;
}

bool ArrayOfObjectsDelegate::drawVerticalGridlines(){
	return true;
}

bool ArrayOfObjectsDelegate::rowSelection(){
	return false;
}


void ArrayOfObjectsDelegate::setupEditorForCell(HWND editor, int row, int col){
	if ( row >= rowCount ){
		SetWindowText(editor, L"");
	} else {
		const wchar_t* c = cellContent(row, col);
		SetWindowText(editor, c);
	}
}

bool ArrayOfObjectsDelegate::allowEditing(int col){
	return true;
}


bool ArrayOfObjectsDelegate::allowHeaderTitleEditing(int col){
	return true;
}

HWND ArrayOfObjectsDelegate::editorForColumn(int col, HWND parent, HINSTANCE hInst){
	return CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_TABSTOP,
		0, 0, 0, 0, parent, NULL, hInst, NULL);
}

void ArrayOfObjectsDelegate::editingFinished(HWND hwnd, int row, int col){
	if ( row < 0 ) return;
	string field = fields[col];

	int len = GetWindowTextLength(hwnd);
	len += sizeof(wchar_t);

	wchar_t* text = new wchar_t[len];
	GetWindowText(hwnd, text, len);
	data[row][col] = text;

}

void ArrayOfObjectsDelegate::willLoseFocus(){
	detail->getForm()->SaveDocument(fieldId);
}

bool ArrayOfObjectsDelegate::allowNewRows(){
	return true;
}

bool ArrayOfObjectsDelegate::allowNewColumns(){
	return false;
}

void ArrayOfObjectsDelegate::prepareNewRow(int row){
	vector<wstring> rowData;
	for(unsigned int i=0; i<fields.size(); i++){
		rowData.push_back(L"");
	}
	data.push_back(rowData);
	rowCount++;
}

HFONT ArrayOfObjectsDelegate::getFont(){
	HFONT hFont=CreateFont(17,0,0,0,0,0,0,0,0,0,0,0,0,TEXT("MS Shell Dlg"));
	return hFont;
}

HFONT ArrayOfObjectsDelegate::getEditFont(){
	HFONT hFont=CreateFont(18,0,0,0,0,0,0,0,0,0,0,0,0,TEXT("MS Shell Dlg"));
	return hFont;
}



Array ArrayOfObjectsDelegate::storeValuesToArray(Array a){
	unsigned int i = 0;
	for(i=0; i<a.size(); i++){
		Object obj = a[i].getObject();
		vector<wstring> rowData = data[i];
		for(unsigned int j=0; j<fields.size(); j++){
			obj[fields[j]] = Designer::ws2s(rowData[j]);
		}

		a[i] = obj;
	}

	for(; i<data.size(); i++){
		Object obj;
		vector<wstring> rowData = data[i];
		for(unsigned int j=0; j<fields.size(); j++){
			obj[fields[j]] = Designer::ws2s(rowData[j]);
		}
		a.push_back(obj);
	}

	return a;
}

