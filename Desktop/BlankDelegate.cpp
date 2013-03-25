
#include "stdafx.h"
#include <string.h>
#include <stdio.h>

BlankDelegate::BlankDelegate(){
	int i,j;
	rowCount = 0;

	for(i=0;i<MAX_ROWS;i++){
		for(j=0; j<TOTAL_COLS; j++){
			data[i][j]=NULL;
		}
	}
}

int BlankDelegate::totalColumns(){
	return TOTAL_COLS;
}

int BlankDelegate::totalRows(){
	return rowCount;
}

int BlankDelegate::columnWidth(int column){
	return 150;
}

int BlankDelegate::rowHeight(){
	return 25;
}

wchar_t* BlankDelegate::headerContent(int col) {
	return TEXT("");
}

bool BlankDelegate::stickyHeaders(){
	return false;
}

const wchar_t* BlankDelegate::cellContent(int row, int col) {
	if( data[row][col] != NULL ){
		return data[row][col];
	}
	
	return TEXT("");

}

HFONT BlankDelegate::getFont(){
	HFONT hFont=CreateFont(17,0,0,0,0,0,0,0,0,0,0,0,0,TEXT("MS Shell Dlg"));
	return hFont;
}

HFONT BlankDelegate::getEditFont(){
	HFONT hFont=CreateFont(18,0,0,0,0,0,0,0,0,0,0,0,0,TEXT("MS Shell Dlg"));
	return hFont;
}

bool BlankDelegate::drawHorizontalGridlines(){
	return true;
}

bool BlankDelegate::drawVerticalGridlines(){
	return true;
}


bool BlankDelegate::rowSelection() {
	return false;
}

bool BlankDelegate::allowEditing(int col){
	return true;
}

HWND BlankDelegate::editorForColumn(int col, HWND parent, HINSTANCE hInst){
	if( col == 0 ){
		return CreateWindowEx(0, DATETIMEPICK_CLASS, TEXT("DateTime"), WS_CHILD|WS_VISIBLE|WS_TABSTOP,
			0, 0, 0, 0, parent, NULL, hInst, NULL);
	}

	return CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_TABSTOP,
		0, 0, 0, 0, parent, NULL, hInst, NULL);
	
}

void BlankDelegate::editingFinished(HWND editor, int row, int col)
{
	data[row][col] = (wchar_t *)malloc(100*sizeof(TCHAR));
	GetWindowText(editor, data[row][col], 100);
}

void BlankDelegate::willLoseFocus(){
}

bool BlankDelegate::allowNewRows() {
	return true;
}

void BlankDelegate::prepareNewRow(int row){
	rowCount++;
}

bool BlankDelegate::allowHeaderTitleEditing(int col) {
	return true;
}

bool BlankDelegate::allowNewColumns() {
	return true;
}

void BlankDelegate::setupEditorForCell(HWND editor, int row, int col){
	if ( col == 0 ){
		const wchar_t* timeStr = this->cellContent(row, col);
		if (timeStr == NULL ) return;

		SYSTEMTIME time;
		int month, day, year;
		GetLocalTime(&time);

		if ( wcslen(timeStr) > 0 ){
			swscanf_s(timeStr, TEXT("%d/%d/%d"), &month, &day, &year);
			time.wMonth = month;
			time.wDay = day;
			time.wYear = year;
		}

		DateTime_SetSystemtime(editor, GDT_VALID, &time);
		return;
	}
	SendMessage(editor, WM_SETTEXT, (WPARAM)0, (LPARAM)this->cellContent(row, col));
}

