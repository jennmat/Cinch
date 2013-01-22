
#include "stdafx.h"
#include <string.h>

ReadOnlyDelegate::ReadOnlyDelegate(){
}

int ReadOnlyDelegate::totalColumns(){
	return 10;
}

int ReadOnlyDelegate::totalRows(){
	return 0;
}

int ReadOnlyDelegate::columnWidth(int column){
	return 150;
}

int ReadOnlyDelegate::rowHeight(){
	return 25;
}

wchar_t* ReadOnlyDelegate::headerContent(int col) {
	return TEXT("");
}

bool ReadOnlyDelegate::stickyHeaders(){
	return false;
}

const wchar_t* ReadOnlyDelegate::cellContent(int row, int col) {
	
	return TEXT("");
}

HFONT ReadOnlyDelegate::getFont(){
	HFONT hFont=CreateFont(17,0,0,0,0,0,0,0,0,0,0,0,0,TEXT("MS Shell Dlg"));
	return hFont;
}

HFONT ReadOnlyDelegate::getEditFont(){
	HFONT hFont=CreateFont(18,0,0,0,0,0,0,0,0,0,0,0,0,TEXT("MS Shell Dlg"));
	return hFont;
}

bool ReadOnlyDelegate::drawHorizontalGridlines(){
	return true;
}

bool ReadOnlyDelegate::drawVerticalGridlines(){
	return true;
}


bool ReadOnlyDelegate::rowSelection() {
	return false;
}

bool ReadOnlyDelegate::allowEditing(int col){
	return false;
}

HWND ReadOnlyDelegate::editorForColumn(int col, HWND parent, HINSTANCE hInst){
	return NULL;	
}

void ReadOnlyDelegate::editingFinished(HWND editor, int row, int col)
{
	
}

void ReadOnlyDelegate::willLoseFocus(){
}

bool ReadOnlyDelegate::allowNewRows() {
	return true;
}

void ReadOnlyDelegate::prepareNewRow(int row){
	
}

void ReadOnlyDelegate::setupEditorForCell(HWND editor, int row, int col){
	
}

