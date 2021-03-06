
#include <stdafx.h>

class CouchViewDelegate : public GridDelegate {

private:
	Connection& conn;
	int rowCount;
	string view;
	string design;
	bool viewInitialized;
	bool descending;
	Object viewResults;
	string* docids;
	
	string selectedDocId;
	CinchGrid* grid;
	
	/* Data is stored in limited buffers, so a "page fault" is fulfilled by loading up a new chunk of data into the buffers */
	void loadPage(int);
public:
	CouchViewDelegate(Connection&);
	~CouchViewDelegate();
	int totalRows();
	int totalColumns();
	string getDocumentIdForRow(int);
	int columnWidth(int column);
	int rowHeight();

	void headerContent(int, wstring &);
	//void cellContent(int, int, wstring &);
	void LoadSegment(int, int, wchar_t***, int*, int*);
	void CleanupSegment(int, int, wchar_t***);

	bool stickyHeaders();
	
	bool rowSelection();

	bool drawHorizontalGridlines();
	bool drawVerticalGridlines();

	bool allowEditing(int);
	bool allowHeaderTitleEditing(int);
	void setupEditorForCell(HWND editor, int row, int col, wchar_t*** data);
	HWND editorForColumn(int, HWND parent, HINSTANCE hInst) ;
	void editingFinished(HWND editor, int row, int col, wchar_t*** data);
	void willLoseFocus();

	bool allowSorting(int col);
	void sortAscending(int col);
	void sortDescending(int col);
	void sortOff(int col);

	void willReloadData();
	void didReloadData();

	void didChangeColumnWidth(int,int);

	void didSelectRow(int);

	bool allowNewRows();
	bool allowNewColumns();
	void prepareNewRow(int row);

	HFONT getFont();
	HFONT getEditFont();

	void headerContextClick(HWND hwnd, int x, int y);

	void setView(const string&, const string&);
	void loadViewResults();

	void setGrid(CinchGrid*);
};
