
// Fractal.h : Fractal Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������


// CFractalApp:
// �йش����ʵ�֣������ Fractal.cpp
//

class CFractalApp : public CWinAppEx
{
public:
	CFractalApp();


// ��д
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// ʵ��
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CFractalApp theApp;
