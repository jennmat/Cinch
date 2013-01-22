#include "CinchGrid.h"

#define MAX_ROWS 4000
#define TOTAL_COLS 3


class BlankDelegate: public GridDelegate {
private:
	int rowCount;
	wchar_t* data[MAX_ROWS][TOTAL_COLS];

public:
	BlankDelegate();
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
	bool allowHeaderTitleEditing(int col);
	void setupEditorForCell(HWND editor, int row, int col);
	HWND editorForColumn(int, HWND parent, HINSTANCE hInst) ;
	void editingFinished(HWND editor, int row, int col);
	void willLoseFocus();

	bool allowNewRows();
	bool allowNewColumns();
	
	void prepareNewRow(int row);

	HFONT getFont();
	HFONT getEditFont();
};
