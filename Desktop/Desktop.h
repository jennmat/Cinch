#include "stdafx.h"


class Desktop : public FormDelegate {
public:
	Object loadedForm;
	void formModified();

	void deleteSelected();
};

