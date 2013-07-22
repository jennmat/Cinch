
#include "stdafx.h"


/* RenameItemHandler.cpp */
void HandleExplorerRenames(HWND);
LRESULT CALLBACK HandleExplorerRenamesProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

void CreateApplicationExplorer(HWND tree);
void DestroyApplicationExplorer(HWND tree);


/* Explorer.cpp */
class Explorer {
private:
	string id;
	string rev;
	void AddMenuItems(HWND tree, const Array& items, int level);

public:
	Explorer();
	~Explorer();
	void buildExplorer(HWND tree);
	void saveChanges(HWND tree);
};
