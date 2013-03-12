#include "CinchGrid.h"



class ReadOnlyDelegate: public GridDelegate {
public:
	ReadOnlyDelegate();
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
	void setupEditorForCell(HWND editor, int row, int col);
	HWND editorForColumn(int, HWND parent, HINSTANCE hInst) ;
	void editingFinished(HWND editor, int row, int col);
	void willLoseFocus();

	bool allowNewRows();
	void prepareNewRow(int row);

	HFONT getFont();
	HFONT getEditFont();
};
