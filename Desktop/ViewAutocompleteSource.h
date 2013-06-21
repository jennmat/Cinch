#include "stdafx.h"

extern Connection conn;
extern Database db;

class ViewAutocompleteSource : public IEnumString {
private:
	int refCount;
	unsigned int currentElement;
	vector<wstring> elements;

public:
	string design;
	string view;
	
	ViewAutocompleteSource(){
		setup();
	}

	void setup(){
		currentElement = 0;
		
		Object results = db.viewResults(design, view, 500, 0, false, true, 1);

		elements.clear();
	
		Array rows = results["rows"].getArray();
		for(unsigned int i=0; i<rows.size(); i++){
			Object row = rows[i].getObject();
			string key = row["key"].getString();

			wstring wkey =s2ws(key);
			elements.push_back(wkey);
		}
	}

	//
	//	IUnknown implementation
	//
	STDMETHODIMP_(ULONG) AddRef()
	{
		return ::InterlockedIncrement(reinterpret_cast<LONG*>(&refCount));
	}

	STDMETHODIMP_(ULONG) Release()
	{
		ULONG nCount = 0;
		nCount = (ULONG) ::InterlockedDecrement(reinterpret_cast<LONG*>(&refCount));

		if (nCount == 0)
			delete this;

		return nCount;

	}

	STDMETHODIMP QueryInterface(REFIID riid, void** ppvObject)
	{

		HRESULT hr = E_NOINTERFACE;
		
		if (ppvObject != NULL)
		{
			*ppvObject = NULL;

			if (IID_IUnknown == riid)
				*ppvObject = static_cast<IUnknown*>(this);

			if (IID_IEnumString == riid)
				*ppvObject = static_cast<IEnumString*>(this);

			if (*ppvObject != NULL)
			{
				hr = S_OK;
				((LPUNKNOWN)*ppvObject)->AddRef();
			}

		}
		else
		{
			hr = E_POINTER;
		}
		
		return hr;

	}


	//
	//	IEnumString implementation
	//
	STDMETHODIMP Next(ULONG celt, LPOLESTR* rgelt, ULONG* pceltFetched)
	{
		
		HRESULT hr = S_FALSE;

		if (!celt)
			celt = 1;

		ULONG i;

		for (i = 0; i < celt; i++)
		{

			if (currentElement == (ULONG)elements.size())
				break;

			int len = sizeof(WCHAR) * (elements[currentElement].length() + 1);
			rgelt[i] = (LPWSTR)::CoTaskMemAlloc((ULONG)len );
			lstrcpyn(rgelt[i], elements[currentElement].c_str(), len);

			if (pceltFetched)
				*pceltFetched++;
			
			currentElement++;
		}

		if (i == celt)
			hr = S_OK;

		return hr;
	}
 
	STDMETHODIMP Skip(ULONG celt)
	{
		currentElement += celt;
		
		if (currentElement > (ULONG)elements.size())
			currentElement = 0;

		return S_OK;
	}
 
	STDMETHODIMP Reset(void)
	{
		setup();
		return S_OK;
	}
 
	STDMETHODIMP Clone(IEnumString** ppenum)
	{
		if (!ppenum)
			return E_POINTER;
		
		ViewAutocompleteSource* pnew = new ViewAutocompleteSource();

		pnew->AddRef();
		*ppenum = pnew;

		return S_OK;
	}
};
