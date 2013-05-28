// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <stdint.h>

#include "Windowsx.h"

#include <strsafe.h>

#include <CommCtrl.h>
#include <string>

#include <Wincrypt.h>


using namespace std;

class Form;
class FormDelegate;
class FormField;

struct ViewPair {
	string emitsDocsWithType;
	string design;
	string view;
};

struct FileWatchController {
	wchar_t* directory;
	bool pause;
	bool exit;
};

#include "Constants.h"
#include "Resource.h"

#include "Toolbar.h"
#include "CinchGrid.h"
#include "CouchViewDelegate.h"
// Comment out if you don't want replication 
//#include "Replication.h"
#include "CouchDB.hpp"
#include "util.h"
#include "ConditionManager.h"
// TODO: reference additional headers your program requires here

#include "JsonBox.h"
#include "util.h"


#include <objbase.h>
#include <shlobj.h>             // shell stuff
#include <sstream>

// TODO: reference additional headers your program requires here
#include "util.h"
#include "base64.h"
#include "FormField.h"
#include "FormLayout.h"
#include "FormDelegate.h"
#include "Detail.h"
#include "Form.h"
#include "ReadOnlyDelegate.h"
#include "BlankDelegate.h"
#include "ArrayOfObjectsDelegate.h"
#include "MaskEdit.h"
#include "NotifyParent.h"
#include "CinchDesigner.h"
#include "DetailViewDelegate.h"

#include "AttachmentViewer.h"

#define ROW_HEIGHT 25
#define LABEL_WIDTH 150
#define LABEL_HEIGHT 25
#define ROW_MARGIN 7
#define CONTROL_WIDTH 200
#define CONTROL_HEIGHT 25
#define MARGIN 10
#define LAYOUT_MARGIN 15

#define IDM_TABULAR		34009
#define IDM_TEXTAREA	34010

//#define DATABASE "cinch-customer-contact-plugin-starter"
//#define DATABASE "cinch-business-starter"
#define DATABASE "cinch-starter"
//#define DATABASE "hvac"

#define DATEPICKER "DatePicker"
#define NUMBER "Number"
#define YESNO "YesNo"
#define RADIO "Radio"
#define EDIT "Edit"
#define REFERENCE "Reference"
#define COMBO "Combo"

#define MULTILINE "multiline"
#define STRING "string"
#define DOCUMENT "document"
#define DATETYPE "date"
#define CODEDVALUE "coded_value"
#define ID		"id"


#define VIEW "View"
#define VIEW_WITH_DOCUMENTS_DETAIL "ViewWithDocuments"
#define TABLE "Table"
#define TEXT_DETAIL "Text"
#define ATTACHMENTS_DETAIL "Attachments"


#define WM_NEW_DATA_ARRIVED WM_USER+140
#define WM_ATTACHMENTS_UPLOADED WM_USER+141


#define DEFAULT_FONT CreateFont(-12,0,0,0,400,0,0,0,1,0,0,0,0,TEXT("Segoe UI"))


#define BUFSIZE 1024
#define MD5LEN  16
