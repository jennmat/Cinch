// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.

#pragma once

#define MAX_RESOURCE_LENGTH     256

#include "stdafx.h"

extern HWND designer;


//
//  CLASS: CNewDocumentHandler : IUICommandHandler
//
//  PURPOSE: Implements interface IUICommandHandler. 
//
//  COMMENTS:
//
//    This is the command handler used by the Size and Color gallery in this sample
//    IUICommandHandler should be returned by the application during command creation.
//
class CNewDocumentHandler
    : public IUICommandHandler // Command handlers must implement IUICommandHandler.
{
public:
    STDMETHOD(Execute)(UINT nCmdID,
                       UI_EXECUTIONVERB verb, 
                       __in_opt const PROPERTYKEY* key,
                       __in_opt const PROPVARIANT* ppropvarValue,
                       __in_opt IUISimplePropertySet* pCommandExecutionProperties);

    STDMETHOD(UpdateProperty)(UINT nCmdID,
                              __in REFPROPERTYKEY key,
                              __in_opt const PROPVARIANT* ppropvarCurrentValue,
                              __out PROPVARIANT* ppropvarNewValue);
    
    static HRESULT CreateInstance(__deref_out CNewDocumentHandler **ppHandler);

	
    STDMETHOD_(ULONG, AddRef)();
    STDMETHOD_(ULONG, Release)();
    STDMETHOD(QueryInterface)(REFIID iid, void **ppv);

private:
    CNewDocumentHandler()
        : m_cRef(1),  m_pifbFactory(NULL)
    {
    }
	
	vector<Object> objectTypes;
	
	HRESULT CNewDocumentHandler::CreateUIImageFromBitmapResource(LPCTSTR pszResource, __out IUIImage **ppimg);
	IUIImageFromBitmap * m_pifbFactory;
    LONG m_cRef;
};