//////////////////////////////////////////////////////////////////////////////
///
/// @file AutoCombo.c
///
/// @brief Auto Compleating ComboBox and ComboBoxEx.
/// 		This code makes the Combobox an autocomplete that performs
/// 		similarly to Internet Explorer's address field.
///
/// @author David MacDermot
///
/// @par Comments:
/// 		This source is distributed in the hope that it will be useful,
/// 		but WITHOUT ANY WARRANTY; without even the implied warranty of
/// 		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
/// 
/// @date 08/11/06	Created
/// 	  05/12/07	Added support for ComboBoxEx
/// 	  05/18/10	Refactored to support Win64 and Unicode
/// 	  01/27/11	Various improvements marked as follows: "27Jan11 - added"
/// 
/// @todo 
///
/// @bug 
///
//////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#define DEFAULT_TXT_LIM 0x7530 ///< Constant

/// @name Macroes
/// @{

/// @def NELEMS(a)
///
/// @brief Computes number of elements of an array.
///
/// @param a An array.

#define NELEMS(a)  (sizeof(a) / sizeof((a)[0]))

#ifndef ComboBoxEx_GetEditControl

/// @def ComboBoxEx_GetEditControl(hwnd)
///
/// @brief Gets the handle to the edit control portion of a ComboBoxEx control.
/// 	   A ComboBoxEx control uses an edit box when it is set to the CBS_DROPDOWN style.
///
/// @param hwnd Handle of the comboboxEx.
///
/// @returns The handle to the edit control within the ComboBoxEx control if
/// 		  it uses the CBS_DROPDOWN style. Otherwise, the message returns NULL.

#define ComboBoxEx_GetEditControl(hwnd) (HWND)SNDMSG((hwnd),CBEM_GETEDITCONTROL,0,0)
#endif

/// @def Combo_SetEditSel(hwndCtl, ichStart,ichEnd)
///
/// @brief Selects characters in the edit control of a combo box.
///
/// @param hwndCtl Handle of the combobox.
/// @param ichStart The starting position. If this is –1, the selection, if any,
/// 				 is removed.
/// @param ichEnd The ending position. If this is –1, all text from the starting
/// 			   position to the last character in the edit control is selected.

#define Combo_SetEditSel(hwndCtl, ichStart, ichEnd) IsExtended(hwndCtl) ? \
	(Edit_SetSel(ComboBoxEx_GetEditControl(hwndCtl),ichStart,ichEnd),0) : \
	(ComboBox_SetEditSel(hwndCtl,ichStart,ichEnd),0)

/// @}

/// @brief Determine if a combobox is the extended version.
///
/// @param hwndCtl Handle of the control.
///
/// @returns TRUE For the ComboboxEx otherwise FALSE.

static BOOL IsExtended(HWND hwndCtl)
{
	static TCHAR name[MAX_PATH];
	GetClassName(hwndCtl, name, MAX_PATH);
	return 0 == _tcsicmp(name, WC_COMBOBOXEX);
}

/// @brief Finds the first string in a combo box list that begins with the specified string.
///
/// @param hwndCtl Handle of the combobox.
/// @param indexStart The zero-based index of the item before the first item
/// 				   to be searched. When the search reaches the bottom of the
/// 				   list, it continues searching from the top of the list back
/// 				   to the item specified by the indexStart parameter. If
/// 				   indexStart is –1, the entire list is searched from the
/// 				   beginning.
/// @param lpszFind The string to find.
///
/// @returns The index of the matching item, or CB_ERR if the search was unsuccessful.

static int Combo_FindString(HWND hwndCtl, INT indexStart, LPTSTR lpszFind)
{
	// Note: ComboBox_FindString does not work with ComboBoxEx and so it is necessary
	//	to furnish our own version of the function.  We will use this version for
	//	both types of comboBoxes.

	TCHAR lpszBuffer[DEFAULT_TXT_LIM];
	TCHAR tmp[DEFAULT_TXT_LIM];
	int ln = _tcslen(lpszFind) + 1;
	if (ln == 1 || indexStart > ComboBox_GetCount(hwndCtl))
		return CB_ERR;

	for (int i = indexStart == -1 ? 0 : indexStart; i < ComboBox_GetCount(hwndCtl); i++)
	{
		ComboBox_GetLBText(hwndCtl, i, lpszBuffer);
		lstrcpyn(tmp, lpszBuffer, ln);
		if (!_tcsicmp(lpszFind, tmp))
			return i;
	}
	return CB_ERR;
}

/// @brief Window procedure for the combobox control.
///
/// @param hwnd Handle of the combobox.
/// @param ch the keyboard char to handle.
///
/// @returns VOID

static void DoAutoComplete(HWND hwnd, TCHAR ch)
{
	// Note: If user presses VK_RETURN or VK_TAB then
	//	the ComboBox Notification = CBN_SELENDCANCEL and
	//	a call to ComboBox_GetCurSel() will return the canceled index.
	//	If the user presses any other key that causes a selection
	//	and closure of the dropdown then
	//	the ComboBox Notification = CBN_SELCHANGE

	static TCHAR buf[DEFAULT_TXT_LIM];
	static TCHAR toFind[DEFAULT_TXT_LIM];
	static BOOL fMatched = TRUE;
	int index = 0;

	// Handle keyboard input
	if (VK_RETURN == ch)
	{
		ComboBox_ShowDropdown(hwnd, FALSE);
		Combo_SetEditSel(hwnd, 0, -1); //selects the entire item
	}
	else if (VK_BACK == ch)
	{
		if(fMatched)// 27Jan11 - added
		{
			DWORD range = ComboBox_GetEditSel(hwnd);
			int bs = LOWORD(range);
			int hs = HIWORD(range);
			if ( hs - bs >= 0 ){
				FORWARD_WM_KEYDOWN(hwnd, ch, 0, 0, SNDMSG);
			} else {
				toFind[_tcslen(toFind) - hs - bs] = 0;
				ComboBox_SetText(hwnd, toFind);
				Combo_SetEditSel(hwnd, -1, -1);
			}


			//Backspace normally erases highlighted match
			//	we only want to move the highlighter back a step
			//index = ComboBox_GetCurSel(hwnd);
			//int bs = LOWORD(ComboBox_GetEditSel(hwnd)) - 1;

			// keep current selection selected
			//ComboBox_SetCurSel(hwnd, index);

			// Move cursor back one space to the insertion point for new text
			// and hilight the remainder of the selected match or near match
			//Combo_SetEditSel(hwnd, bs, -1);
		}
		else// 27Jan11 - added
		{
			toFind[_tcslen(toFind) -1] = 0;
			ComboBox_SetText(hwnd, toFind);
			Combo_SetEditSel(hwnd, -1, -1);
			FORWARD_WM_KEYDOWN(hwnd, VK_END, 0, 0, SNDMSG);
		}
	}
	else if (!_istcntrl(ch))
	{

		BOOL status = GetWindowLongPtr(hwnd, GWL_STYLE) & CBS_DROPDOWN;
		if (status)
			ComboBox_ShowDropdown(hwnd, TRUE);

		if (IsExtended(hwnd)) // keep focus on edit box
			SetFocus(ComboBoxEx_GetEditControl(hwnd));

		// Get the substring from 0 to start of selection
		ComboBox_GetText(hwnd, buf, NELEMS(buf));
		buf[LOWORD(ComboBox_GetEditSel(hwnd))] = 0;

		_stprintf(toFind, NELEMS(toFind),
#ifdef _UNICODE
			_T("%ls%lc"),
#else
			_T("%s%c"),
#endif
			buf, ch);

		// Find the first item in the combo box that matches ToFind
		index = ComboBox_FindStringExact(hwnd, -1, toFind);

		if (CB_ERR == index) //no match
		{
			// Find the first item in the combo box that starts with ToFind
			index = Combo_FindString(hwnd, -1, toFind);
		}
		if (CB_ERR != index)
		{
			// Else for match
			fMatched = TRUE;
			ComboBox_SetCurSel(hwnd, index);
			Combo_SetEditSel(hwnd, _tcslen(toFind), -1);
		}
		else // 27Jan11 - Display text that is not in the selected list 
		{
			fMatched = FALSE;
			ComboBox_SetText(hwnd, toFind);
			Combo_SetEditSel(hwnd, _tcslen(toFind), -1);
			FORWARD_WM_KEYDOWN(hwnd, VK_END, 0, 0, SNDMSG);
		}
	}
}

/// @brief Window procedure for the combobox control.
///
/// @param hwnd Handle of the combobox or a child edit control.
/// @param msg Which message?
/// @param wParam Message parameter.
/// @param lParam Message parameter.
///
/// @returns LRESULT depends on message.

static LRESULT CALLBACK ComboBox_Proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HWND hCombo;

	switch (msg)
	{
		case WM_GETDLGCODE:
			return VK_TAB == wParam ? FALSE: DLGC_WANTALLKEYS;
		case WM_CHAR:
			hCombo = GetParent(GetParent(hwnd));
			if (!IsExtended(hCombo))
				hCombo = GetParent(hwnd);

			DoAutoComplete(hCombo, (TCHAR)wParam);
			break;
		case WM_DESTROY:	//Unsubclass the edit control
			SetWindowLongPtr(hwnd, GWLP_WNDPROC, (DWORD)GetProp(hwnd, TEXT("Wprc")));
			RemoveProp(hwnd, TEXT("Wprc"));
			break;
		default:
			return CallWindowProc((WNDPROC)GetProp(hwnd, TEXT("Wprc")), hwnd, msg, wParam, lParam);
	}
	return FALSE;
}

/// @brief Subclass a combobox control.
///
/// @param hComboBox Handle of the combobox to subclass.
///
/// @returns VOID.

void MakeAutocompleteCombo(HWND hComboBox)
{
	// SubClass the combo's Edit control
	HWND hEdit = IsExtended(hComboBox) ?
		ComboBoxEx_GetEditControl(hComboBox) :
			FindWindowEx(hComboBox, NULL, WC_EDIT, NULL);

	SetProp(hEdit, TEXT("Wprc"), (HANDLE)GetWindowLongPtr(hEdit, GWLP_WNDPROC));
	SubclassWindow(hEdit, ComboBox_Proc);

	// Set the text limit to standard size
	ComboBox_LimitText(hComboBox, DEFAULT_TXT_LIM);
}
