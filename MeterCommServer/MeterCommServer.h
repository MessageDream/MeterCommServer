
// MeterCommServer.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������
#include "MyFuncLib.h"
#include "MyClassLib.h"

// CMeterCommServerApp:
// �йش����ʵ�֣������ MeterCommServer.cpp
//

class CMeterCommServerApp : public CWinApp
{
public:
	CMeterCommServerApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CMeterCommServerApp theApp;

#define WM_MSGRECVPRO WM_USER+1
#define WM_TRAYNOTIFY WM_USER+2

enum MSGUSER
{
	MSGUSER_ADDMAINDATA = 0,
	MSGUSER_DELETEMAINDATA,
	MSGUSER_UPDATEEDIT
};