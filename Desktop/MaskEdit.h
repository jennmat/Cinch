
#include "stdafx.h"

/* From http://www.catch22.net/tuts/masked-edit-input */

LRESULT CALLBACK MaskedEditProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL MaskEditControl(HWND hwndEdit, char szMask[], DWORD nOptions);