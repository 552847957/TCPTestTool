
// TcpTestTool.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CTcpTestToolApp: 
// �йش����ʵ�֣������ TcpTestTool.cpp
//

class CTcpTestToolApp : public CWinApp
{
public:
	CTcpTestToolApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CTcpTestToolApp theApp;