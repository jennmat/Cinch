
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
	data = NULL;
	rownums = NULL;
	docids = NULL;
	viewInitialized = false;
	descending = false;
}

CouchViewDelegate::~CouchViewDelegate(){
	if ( viewInitialized == true ){
		//for(int i=0; i<PAGESIZE; i++) 
//			delete data[i];
		delete rownums;
		delete[] data;
		delete[] docids;
	}
}

void CouchViewDelegate::setView(const string& _design, const string& _view)
{
	view = _view;
	design = _design;
	viewInitialized = true;
	loadViewResults();
}

void CouchViewDelegate::loadViewResults(){

	if ( viewInitialized == false ) return;

	delete rownums;
	delete[] data;
	delete[] docids;

	QueryOptions options;
	options.descending = descending;
	options.limit = PAGESIZE;

	viewResults = db.viewResults(design, view, options);
	
	rowCount = viewResults["total_rows"].getInt();
	data = new wstring[PAGESIZE];
	rownums = new int[PAGESIZE];
	docids = new string[PAGESIZE];

	
	int i = 0;
	if ( viewResults["total_rows"].getInt() > 0 ){
	   Array rows = viewResults["rows"].getArray();
	   for(unsigned int j=0; j<rows.size(); j++){
			Object o = rows[j].getObject();
			wstring w = s2ws(o["key"].getString());
			data[i] = w;
			rownums[i] = i;
			docids[i] = o["id"].getString();

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
	return 185;
}


int CouchViewDelegate::rowHeight(){
	return 25;
}

void CouchViewDelegate::headerContent(int col, wstring &content)
{
	content = L"Name";
}

void CouchViewDelegate::loadPage(int row){
	/* Load some more documents  */
	

	int j = row - 1;
	int skip = 1;
	while ( j>=0 && rownums[j % PAGESIZE] != j ){
		j--;
		skip++;
	}


	Object obj;
	int i;
	if ( j < 0 ){
		QueryOptions options;
		options.descending = true;
		options.limit = PAGESIZE;
		options.skip = skip - 1;
		obj = db.viewResults(design, view, options);
		i = skip-1;
	} else {
		QueryOptions options;
		options.startKey = Value(ws2s(data[j%PAGESIZE]));
		options.startKeyDocId = docids[j%PAGESIZE];
		options.limit = PAGESIZE;
		options.skip = skip;
		obj = db.viewResults(design, view);
		i = rownums[j%PAGESIZE] + skip;
	}


	if ( obj["total_rows"].getInt() > 0 ){
		Array rows = obj["rows"].getArray();
		for(unsigned int j=0; j<rows.size(); j++){
			Object o = rows[j].getObject();
			wstring w = s2ws(o["key"].getString());
			//delete data[i%PAGESIZE];
			//data[i%PAGESIZE] = new wchar_t[80];
			//wcscpy_s(data[i%PAGESIZE], 80, w.c_str());
			data[i] = w;
			rownums[i%PAGESIZE] = i;
			docids[i%PAGESIZE] = o["id"].getString();
			i++;
		}
	}
}

void CouchViewDelegate::cellContent(int row, int col, wstring &content)
{
	if ( rownums[row % PAGESIZE] != row ){
		loadPage(row);
	}

	content = data[row % PAGESIZE];
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
	if ( rownums[row % PAGESIZE] != row ){
		loadPage(row);
	}

	if (viewResults["total_rows"].getInt() > 0 ){
	   return docids[row % PAGESIZE];
	}

	return "";
}


void CouchViewDelegate::headerContextClick(HWND grid, int x, int y){
}


void CouchViewDelegate::willReloadData(){
	loadViewResults();
}

void CouchViewDelegate::didReloadData(){
	if ( viewInitialized == false ) return;
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
	}
}

void CouchViewDelegate::didSelectRow(int row){
	if ( rownums[row % PAGESIZE] != row ){
		loadPage(row);
	}
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
