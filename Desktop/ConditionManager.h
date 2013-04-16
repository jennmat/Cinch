
#include "stdafx.h"


struct Condition {
	HWND fieldCombo;
	HWND compareCombo;
	FormField* value;
	string setupForField;
};

/* When creating a new view, the user can add conditions based on the fields in the template, this class
   is responsible for managing the windows */
class ConditionManager {
private:
	vector<Condition *> *conditions;
	string type;

public:
	ConditionManager();
	~ConditionManager();
	void arrangeWindowsInParent(HWND parent, int startX, int startY);
	void addEmptyCondition(string type, HWND parent);
	void updateConditions(HWND parent);
	vector<Condition *>* getConditions();
	string getJavascript();
};
