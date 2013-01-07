
#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>

using namespace std;
using namespace JsonBox;
using namespace CouchDB;

wstring s2ws(const string& s)
{
    int len;
    int slength = (int)s.length() + 1;
    len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0); 
    wchar_t* buf = new wchar_t[len];
    MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
    wstring r(buf);
    delete[] buf;
    return r;
}

CouchViewDelegate::CouchViewDelegate(Database& _db) : db(_db)
{
	for(int i=0; i<100; i++){
		data[i] = NULL;
	}
	Object obj = db.viewResults("friends", "by-name", Value(""), 25);

   
   int i = 0;
   if ( obj["total_rows"].getInt() > 0 ){
	   Array rows = obj["rows"].getArray();
	   for(int j=0; j<rows.size(); j++){
		   Object o = rows[j].getObject();
		   wstring w = s2ws(o["key"].getString());

			data[i] = new wchar_t[80];
			wcscpy_s(data[i], 80, w.c_str());
			i++;
	   }
   }
   
}

int CouchViewDelegate::totalRows()
{
	return 100;
}


int CouchViewDelegate::totalColumns(){
	return 1;
}


int CouchViewDelegate::columnWidth(int col){
	return 250;
}


int CouchViewDelegate::rowHeight(){
	return 25;
}

wchar_t* CouchViewDelegate::headerContent(int col)
{
	return L"Name";
}

const wchar_t* CouchViewDelegate::cellContent(int row, int col){
	if ( data[row] == NULL ){
		return L"";
	} else {
		return data[row];
	}
}

bool CouchViewDelegate::stickyHeaders(){
	return false;
}

bool CouchViewDelegate::drawHorizontalGridlines(){
	return true;
}

bool CouchViewDelegate::drawVerticalGridlines(){
	return true;
}

bool CouchViewDelegate::rowSelection(){
	return true;
}


void CouchViewDelegate::setupEditorForCell(HWND editor, int row, int col)
{
}

bool CouchViewDelegate::allowEditing(int col){
	return false;
}


bool CouchViewDelegate::allowHeaderTitleEditing(int col){
	return true;
}

HWND CouchViewDelegate::editorForColumn(int col, HWND parent, HINSTANCE hInst){
	return NULL;
}

void CouchViewDelegate::editingFinished(HWND hwnd, int row, int col){
}

bool CouchViewDelegate::allowNewRows(){
	return false;
}

bool CouchViewDelegate::allowNewColumns(){
	return false;
}

void CouchViewDelegate::prepareNewRow(int row){
}

HFONT CouchViewDelegate::getFont(){
	HFONT hFont=CreateFont(17,0,0,0,0,0,0,0,0,0,0,0,0,TEXT("MS Shell Dlg"));
	return hFont;
}

HFONT CouchViewDelegate::getEditFont(){
	HFONT hFont=CreateFont(18,0,0,0,0,0,0,0,0,0,0,0,0,TEXT("MS Shell Dlg"));
	return hFont;
}






