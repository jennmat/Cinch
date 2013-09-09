
#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>

using namespace std;
using namespace JsonBox;
using namespace CouchDB;

#define PAGESIZE 200

CouchViewDelegate::CouchViewDelegate(Connection& _conn) : conn(_conn)
{
	rowCount = 0;
	viewInitialized = false;
	descending = false;
	docids = nullptr;
}

CouchViewDelegate::~CouchViewDelegate(){
}

void CouchViewDelegate::setView(const string& _design, const string& _view)
{
	view = _view;
	design = _design;
	Object results = db.viewResults(design, view);
	rowCount = results["total_rows"].getInt();

	//loadViewResults();
}

int CouchViewDelegate::LoadSegment(int start_row, int len, wchar_t*** data){
	QueryOptions options;
	options.descending = descending;
	options.limit = len;
	options.skip = start_row;
	docids = new string[len];
	Object results = db.viewResults(design, view, options);
	if ( results["rows"].isArray() ){
		Array rows = results["rows"].getArray();
		UINT i = 0;
		for(i=0; i<min((unsigned int)len, rows.size()); i++){
			Object row = rows[i].getObject();
			wstring w = s2ws(row["key"].getString());
			int len = w.length() + sizeof(wchar_t);
			data[i][0] = new wchar_t[w.length()+sizeof(wchar_t)];
			wcscpy_s(data[i][0], len, w.c_str());
			docids[i] = row["id"].getString();
		}
		return i;
	}

	return 0;
}

void CouchViewDelegate::CleanupSegment(int len, wchar_t*** data){
	delete[] docids;
	docids = nullptr;
	for(int i=0; i<len; i++){
		delete data[i][0];
	}
}

void CouchViewDelegate::loadViewResults(){

	if ( viewInitialized == false ) return;

	

	QueryOptions options;
	options.descending = descending;
	options.limit = PAGESIZE;

	viewResults = db.viewResults(design, view, options);
	
	rowCount = viewResults["total_rows"].getInt();
	
	int i = 0;
	if ( viewResults["total_rows"].getInt() > 0 ){
	   Array rows = viewResults["rows"].getArray();
	   for(unsigned int j=0; j<rows.size(); j++){
			Object o = rows[j].getObject();
			wstring w = s2ws(o["key"].getString());
			
			i++;
		}
	}
}

int CouchViewDelegate::totalRows()
{
	return rowCount;
}


int CouchViewDelegate::totalColumns(){
	return 1;
}


int CouchViewDelegate::columnWidth(int col){
	return CINCH_GRID_MAXIMIZE_WIDTH;
}


int CouchViewDelegate::rowHeight(){
	return 25;
}

void CouchViewDelegate::headerContent(int col, wstring &content)
{
	content = L"Name";
}



bool CouchViewDelegate::stickyHeaders(){
	return true;
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


void CouchViewDelegate::setupEditorForCell(HWND editor, int row, int col){
}

bool CouchViewDelegate::allowEditing(int col){
	return false;
}


bool CouchViewDelegate::allowHeaderTitleEditing(int col){
	return false;
}

HWND CouchViewDelegate::editorForColumn(int col, HWND parent, HINSTANCE hInst){
	return NULL;
}

void CouchViewDelegate::editingFinished(HWND hwnd, int row, int col){
}

void CouchViewDelegate::willLoseFocus(){
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
	HFONT hFont=DEFAULT_FONT;
	return hFont;
}

HFONT CouchViewDelegate::getEditFont(){
	HFONT hFont=DEFAULT_FONT;
	return hFont;
}


string CouchViewDelegate::getDocumentIdForRow(int row){
	if( row < 0 ) return "";
	return docids[row];
}


void CouchViewDelegate::headerContextClick(HWND grid, int x, int y){
}


void CouchViewDelegate::willReloadData(){
	loadViewResults();
}

void CouchViewDelegate::didReloadData(){
	/*if ( viewInitialized == false ) return;
	if ( selectedDocId.length() == 0 ) return;
	bool found = false;
	for(int i=0; i<PAGESIZE && !found; i++){
		if ( docids[i].compare(selectedDocId) == 0 ){
			grid->SetActiveRow(i+1, true);
			grid->ScrollRowIntoView(i+1);
			found = true;
		}
	}
	if ( !found ){
		if ( rowCount > 0 ){
			grid->SetActiveRow(1);
			grid->ScrollRowIntoView(1);
		}
	}*/
}

void CouchViewDelegate::didSelectRow(int row){
	selectedDocId = docids[row];
}


void CouchViewDelegate::setGrid(CinchGrid* g){
	grid = g;
}

void CouchViewDelegate::didChangeColumnWidth(int, int){
}

bool CouchViewDelegate::allowSorting(int col){
	return true;
}

void CouchViewDelegate::sortAscending(int col){
	descending = false;
}


void CouchViewDelegate::sortDescending(int col){
	descending = true;
}

void CouchViewDelegate::sortOff(int col){
	descending = false;
}
