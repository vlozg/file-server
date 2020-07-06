
// MFCCLient.h : main header file for the PROJECT_NAME application
//

#pragma once


#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols
#include "Client.h"


// CMFCCLientApp:
// See MFCCLient.cpp for the implementation of this class
//

class CMFCCLientApp : public CWinApp
{
public:
	CMFCCLientApp();
	

// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};
//Client client;

extern CMFCCLientApp theApp;
extern Client client;