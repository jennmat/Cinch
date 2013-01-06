
#include <stdafx.h>
#include "CinchGrid.h"
#include "CouchDB.hpp"

using namespace CouchDB;

class CouchViewDelegate : public GridDelegate {

private:
	Database& db;
	wchar_t* data[100];
public:
	CouchViewDelegate(Database&);
	int totalRows();
	int totalColumns();

	int columnWidth(int column);
	int rowHeight();

	wchar_t* headerContent(int);
	const wchar_t* cellContent(int, int);

	bool stickyHeaders();
	
	bool rowSelection();

	bool drawHorizontalGridlines();
	bool drawVerticalGridlines();

	bool allowEditing(int);
	bool allowHeaderTitleEditing(int);
	void setupEditorForCell(HWND editor, int row, int col);
	HWND editorForColumn(int, HWND parent, HINSTANCE hInst) ;
	void editingFinished(HWND editor, int row, int col);

	bool allowNewRows();
	bool allowNewColumns();
	void prepareNewRow(int row);

	HFONT getFont();
	HFONT getEditFont();
};
