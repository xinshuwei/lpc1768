
// lpc1768_ontest.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// Clpc1768_ontestApp: 
// �йش����ʵ�֣������ lpc1768_ontest.cpp
//

class Clpc1768_ontestApp : public CWinApp
{
public:
	Clpc1768_ontestApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern Clpc1768_ontestApp theApp;