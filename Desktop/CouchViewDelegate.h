
#include <stdafx.h>
#include "CinchGrid.h"
#include "CouchDB.hpp"

using namespace CouchDB;

class CouchViewDelegate : public GridDelegate {

private:
	Connection& conn;
	int rowCount;
	wchar_t** data;
	wstring view;
	wstring design;
	bool viewInitialized;
	Object lastRead;
	Object viewResults;
public:
	CouchViewDelegate(Connection&);
	int totalRows();
	int totalColumns();
	string getDocumentIdForRow(int);
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
	void willLoseFocus();

	bool allowNewRows();
	bool allowNewColumns();
	void prepareNewRow(int row);

	HFONT getFont();
	HFONT getEditFont();

	void headerContextClick(HWND hwnd, int x, int y);

	void setView(const wstring&, const wstring&);
	void loadViewResults();
};
