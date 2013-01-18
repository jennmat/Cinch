
#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>

using namespace std;
using namespace JsonBox;
using namespace CouchDB;


ArrayOfObjectsDelegate::ArrayOfObjectsDelegate(Array _array)
{
	array = _array;
	rowCount = array.size();

}

int ArrayOfObjectsDelegate::totalRows()
{
	return rowCount;
}


int ArrayOfObjectsDelegate::totalColumns(){
	/*if ( array.size() > 0 ){
		Object obj = array[0].getObject();
		return obj.size();
	}

	return 0;*/
	return 2;
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

    Object o = array[0].getObject();
	Object::const_iterator it = o.begin();
	int i=0;
	while ( i < col ) {
		i++;
		it++;
	}
	pair<string, Value> p = *it;

	wstring w = s2ws(p.first);

	result = new wchar_t[80];
	wcscpy_s(result, 80, w.c_str());
	return result;
}

const wchar_t* ArrayOfObjectsDelegate::cellContent(int row, int col)
{
	wchar_t* result;

    Object o = array[row].getObject();
	Object::const_iterator it = o.begin();
	int i=0;
	while ( i < col ) {
		i++;
		it++;
	}
	pair<string, Value> p = *it;

	wstring w = s2ws(p.second.getString());

	result = new wchar_t[80];
	wcscpy_s(result, 80, w.c_str());
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
	return true;
}


void ArrayOfObjectsDelegate::setupEditorForCell(HWND editor, int row, int col)
{
}

bool ArrayOfObjectsDelegate::allowEditing(int col){
	return false;
}


bool ArrayOfObjectsDelegate::allowHeaderTitleEditing(int col){
	return true;
}

HWND ArrayOfObjectsDelegate::editorForColumn(int col, HWND parent, HINSTANCE hInst){
	return NULL;
}

void ArrayOfObjectsDelegate::editingFinished(HWND hwnd, int row, int col){
}

bool ArrayOfObjectsDelegate::allowNewRows(){
	return false;
}

bool ArrayOfObjectsDelegate::allowNewColumns(){
	return false;
}

void ArrayOfObjectsDelegate::prepareNewRow(int row){
}

HFONT ArrayOfObjectsDelegate::getFont(){
	HFONT hFont=CreateFont(17,0,0,0,0,0,0,0,0,0,0,0,0,TEXT("MS Shell Dlg"));
	return hFont;
}

HFONT ArrayOfObjectsDelegate::getEditFont(){
	HFONT hFont=CreateFont(18,0,0,0,0,0,0,0,0,0,0,0,0,TEXT("MS Shell Dlg"));
	return hFont;
}


