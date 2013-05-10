
#include <stdafx.h>
#include "CinchGrid.h"
#include "CouchDB.hpp"

using namespace CouchDB;

class CouchViewDelegate : public GridDelegate {

private:
	Connection& conn;
	int rowCount;
	wchar_t** data;
	string* docids;
	int* rownums;
	wstring view;
	wstring design;
	bool viewInitialized;
	Object viewResults;

	Value highValue;
	string highDocId;
	int highIndex;

	string selectedDocId;
	CinchGrid* grid;
	
	/* Data is stored in limited buffers, so a "page fault" is fulfilled by loading up a new chunk of data into the buffers */
	void loadPage(int);
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

	void willReloadData();
	void didReloadData();

	void didSelectRow(int);

	bool allowNewRows();
	bool allowNewColumns();
	void prepareNewRow(int row);

	HFONT getFont();
	HFONT getEditFont();

	void headerContextClick(HWND hwnd, int x, int y);

	void setView(const wstring&, const wstring&);
	void loadViewResults();

	void setGrid(CinchGrid*);
};
