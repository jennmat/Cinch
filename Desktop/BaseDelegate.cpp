
#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>

using namespace std;
using namespace JsonBox;
using namespace CouchDB;


BaseDelegate::BaseDelegate(Detail * _d, int _fieldId){
	detail = _d;
	fieldId = _fieldId;
}

BaseDelegate::~BaseDelegate(){
	for(int i=0; i<totalColumns(); i++){
		if ( editors[i] != NULL ){
			delete editors[i];
		}
	}
}


void BaseDelegate::addColumn(std::string field, std::wstring title, string editorType, Value config){
	fields.push_back(field);
	titles.push_back(title);
	widths.push_back(250);
	editorTypes.push_back(editorType);
	editorConfigs.push_back(config);
}


int BaseDelegate::totalColumns(){
	int a = fields.size();
	return a;
}

int BaseDelegate::columnWidth(int col){
	return widths[col];
}

int BaseDelegate::rowHeight(){
	return 25;
}

void BaseDelegate::headerContent(int col, wstring &content)
{
	content = titles[col];

}

bool BaseDelegate::stickyHeaders(){
	return true;
}

bool BaseDelegate::drawHorizontalGridlines(){
	return true;
}

bool BaseDelegate::drawVerticalGridlines(){
	return true;
}

bool BaseDelegate::rowSelection(){
	return false;
}




HFONT BaseDelegate::getFont(){
	HFONT hFont=DEFAULT_FONT;
	return hFont;
}

HFONT BaseDelegate::getEditFont(){
	HFONT hFont=DEFAULT_FONT;
	return hFont;
}


void BaseDelegate::deserializeUIElements(HWND _parent, Object obj){
	parent = _parent;
	fields.clear();
	titles.clear();
	widths.clear();
	editorConfigs.clear();
	editorTypes.clear();
	editors.clear();

	if( obj["columns"].isArray() ){
		Array columns = obj["columns"].getArray();
		for(unsigned int i=0; i<columns.size(); i++){
			Object col = columns[i].getObject();
			string field = col["field"].getString();
			Object fieldConfig = getTypeDefinition(field);
			
			fields.push_back(fieldConfig["_id"].getString());
			editorTypes.push_back(field);
			editors.push_back(NULL);
			//editorConfigs.push_back(col["config"]);
			titles.push_back(s2ws(fieldConfig["label"].getString()));
			if ( col["width"].isInteger() && col["width"].getInt() > 0 ){
				widths.push_back(col["width"].getInt());
			} else {
				widths.push_back(250);
			}
		}
	}

}

void BaseDelegate::serializeUIElements(Object& o){
	Array columns;
	for(unsigned int i=0; i<fields.size(); i++){
		Object col;
		col["field"] = Value(fields[i]);
		col["width"] = Value(widths[i]);
		columns.push_back(col);
	}
	o["columns"] = columns;
}

void BaseDelegate::headerContextClick(HWND grid, int x, int y){
	HMENU hPopupMenu = CreatePopupMenu();
	InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, IDD_EDIT_COLUMNS, L"Edit Columns...");
	POINT point;
	point.x = x;
	point.y = y;
    ClientToScreen(grid, &point);
	TrackPopupMenu(hPopupMenu, TPM_TOPALIGN | TPM_LEFTALIGN, point.x, point.y, 0, detail->getDetailHwnd(), NULL);
}

void BaseDelegate::willReloadData(){
}

void BaseDelegate::didReloadData(){
}

void BaseDelegate::didSelectRow(int){
}

void BaseDelegate::setGrid(CinchGrid* g){
	grid = g;
}

void BaseDelegate::didChangeColumnWidth(int col, int newWidth){
	widths[col] = newWidth;
	detail->getForm()->getDelegate()->formModified();
}


bool BaseDelegate::allowSorting(int col){
	return false;
}

void BaseDelegate::sortAscending(int col){
}

void BaseDelegate::sortDescending(int col){
}

void BaseDelegate::sortOff(int col){
}