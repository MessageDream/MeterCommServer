
// MeterCommServerDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MeterCommServer.h"
#include "MeterCommServerDlg.h"
#include "afxdialogex.h"
#include "Config.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const TCHAR tcSplitError = _T('$');
const TCHAR tcSplitNetData = _T('*');
const CString strValueName(_T("MeterCommServer"));
//��ʱ��
const int nClearMsg=1;

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMeterCommServerDlg �Ի���




CMeterCommServerDlg::CMeterCommServerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMeterCommServerDlg::IDD, pParent)
	, m_strServerIP(_T("INADDR_ANY"))
	, m_strServerPort(_T("8888"))
	, m_strServerTimeOut(_T("20000"))
	, m_strNetCptIP(_T("AUTO"))
	, m_strNetCptPort(_T("AUTO"))
	, m_strNetCptTimeOut(_T("20000"))
	, m_strVersion(_T("1.0.0.1"))
	, m_bIsListen(false)
	, m_hMutex(NULL)
	, m_hEvtExit(NULL)
	, m_nEditLength(0)
	, m_nTotalReq(0)
	, m_bIsExit(false)
	, m_strUpdateFlag(_T("1"))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	ZSocket::CallSocketDll();
	ZDLT645_2007::CallExDll();
	ZDLT698_45::CallExDll();
}

CMeterCommServerDlg::~CMeterCommServerDlg()
{
	if(m_hEvtExit!=NULL)
	{
		CloseHandle(m_hEvtExit);
		m_hEvtExit=NULL;
	}
	if(m_hMutex!=NULL)
	{
		CloseHandle(m_hMutex);
		m_hMutex=NULL;
	}
	ZDLT698_45::UncallExDll();
	ZDLT645_2007::UncallExDll();
	ZSocket::UncallSocketDll();
}

void CMeterCommServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_SHOW, m_editShow);
	DDX_Control(pDX, IDC_CHECK_ClEARTIME, m_checkClearTime);
	DDX_Control(pDX, IDC_CHECK_AUTORUN, m_checkAutoRun);
}

BEGIN_MESSAGE_MAP(CMeterCommServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_STARTSERVER, &CMeterCommServerDlg::OnBnClickedButtonStartserver)
	ON_BN_CLICKED(IDC_BUTTON_STOPSERVER, &CMeterCommServerDlg::OnBnClickedButtonStopserver)
	ON_BN_CLICKED(IDC_BUTTON_CLEARMSG, &CMeterCommServerDlg::OnBnClickedButtonClearmsg)
	ON_BN_CLICKED(IDC_BUTTON_CONFIG, &CMeterCommServerDlg::OnBnClickedButtonConfig)
	ON_BN_CLICKED(IDC_CHECK_ClEARTIME, &CMeterCommServerDlg::OnBnClickedCheckCleartime)
	ON_BN_CLICKED(IDC_CHECK_AUTORUN, &CMeterCommServerDlg::OnBnClickedCheckAutorun)
	ON_MESSAGE(WM_MSGRECVPRO, &CMeterCommServerDlg::OnMsgrecvpro)
	ON_BN_CLICKED(IDOK, &CMeterCommServerDlg::OnBnClickedOk)
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_MESSAGE(WM_TRAYNOTIFY, &CMeterCommServerDlg::OnTraynotify)
	ON_COMMAND(ID_EXIT, &CMeterCommServerDlg::OnExit)
END_MESSAGE_MAP()


// CMeterCommServerDlg ��Ϣ�������

BOOL CMeterCommServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	m_hMutex = CreateMutex(NULL, FALSE, NULL);
	m_hEvtExit =CreateEvent(NULL,TRUE,TRUE,NULL);
	SetDlgItemText(IDC_EDIT_TIME,_T("10"));
	GetDlgItem(IDC_EDIT_NUM)->EnableWindow(FALSE);
	if(ZOperatorRegedit::CheckAutoRun(ZUtil::GetExePath(), strValueName))
		m_checkAutoRun.SetCheck(TRUE);
	InitTray();
	OnBnClickedButtonStartserver();
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CMeterCommServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CMeterCommServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CMeterCommServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMeterCommServerDlg::OnBnClickedButtonStartserver()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_bIsListen=true;
	m_sockServer.InitSocket();  
	m_sockServer.Bind(m_strServerIP,m_strServerPort);  
	m_sockServer.Listen(5);  
	while(AfxBeginThread(ListenThreadFunc,(LPVOID)this)==NULL);
	GetDlgItem(IDC_BUTTON_STARTSERVER)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_CONFIG)->EnableWindow(FALSE);
}


void CMeterCommServerDlg::OnBnClickedButtonStopserver()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_bIsListen=false;
	m_sockServer.StopComm();
	m_sockServer.CloseSocket();
	WaitForSingleObject(m_hEvtExit,INFINITE);
	GetDlgItem(IDC_BUTTON_STARTSERVER)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_CONFIG)->EnableWindow(TRUE);
}


void CMeterCommServerDlg::OnBnClickedButtonClearmsg()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	SetDlgItemText(IDC_EDIT_SHOW,_T(""));
	m_nEditLength=0;
}

void CMeterCommServerDlg::OnBnClickedButtonConfig()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CConfig config;
	config.SetParam(m_strServerIP,m_strServerPort,m_strServerTimeOut,m_strNetCptIP,m_strNetCptPort,m_strNetCptTimeOut,m_strVersion,m_strUpdateFlag);
	if(config.DoModal()==IDOK)
	{
		config.GetParam(m_strServerIP,m_strServerPort,m_strServerTimeOut,m_strNetCptIP,m_strNetCptPort,m_strNetCptTimeOut,m_strVersion,m_strUpdateFlag);
	}
}

void CMeterCommServerDlg::OnBnClickedCheckCleartime()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(m_checkClearTime.GetCheck()==1)
	{
		GetDlgItem(IDC_EDIT_TIME)->EnableWindow(FALSE);
		SetTimer(nClearMsg,GetDlgItemInt(IDC_EDIT_TIME)*1000,NULL);
	}
	else
	{
		KillTimer(nClearMsg);
		GetDlgItem(IDC_EDIT_TIME)->EnableWindow(TRUE);
	}
}

void CMeterCommServerDlg::OnBnClickedCheckAutorun()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	ZOperatorRegedit::SetAutoRun(ZUtil::GetExePath(), strValueName, m_checkAutoRun.GetCheck());
	BOOL nRtn = ZOperatorRegedit::CheckAutoRun(ZUtil::GetExePath(), strValueName);
	if (nRtn != m_checkAutoRun.GetCheck())
	{
		AfxMessageBox(_T("����ʧ�ܣ���Ҫ����ԱȨ�ޣ�"));
		m_checkAutoRun.SetCheck(nRtn);
	}
}

UINT CMeterCommServerDlg::ListenThreadFunc(LPVOID lpParam)
{
	CMeterCommServerDlg* pMainDlg = (CMeterCommServerDlg *)lpParam;
	int nRtn;
	CString strClientIP;
	ResetEvent(pMainDlg->m_hEvtExit);
	while(pMainDlg->m_bIsListen)
	{
		ZFileSocket sockClient;
		nRtn=pMainDlg->m_sockServer.Accept(sockClient,&strClientIP);
		if(!nRtn)
		{
			CMainData md(sockClient,strClientIP,pMainDlg);
			SendMessageTimeout(pMainDlg->m_hWnd,WM_MSGRECVPRO,(WPARAM)&md, MSGUSER_ADDMAINDATA,SMTO_BLOCK,500,NULL);
		}
	}
	SetEvent(pMainDlg->m_hEvtExit);
	return 0;
}

afx_msg LRESULT CMeterCommServerDlg::OnMsgrecvpro(WPARAM wParam, LPARAM lParam)
{
	switch(lParam)
	{
	case MSGUSER_ADDMAINDATA:
		{
			CMainData *p_md=(CMainData *)wParam;
			p_md->m_sockClient.SetTimeOut(ZSocket::TIMEOUT_RECV,_ttoi(m_strServerTimeOut));
			p_md->m_dlt645.SetNetCptInfo(m_strNetCptIP,m_strNetCptPort,m_strNetCptTimeOut);
			p_md->m_dlt698.SetNetCptInfo(m_strNetCptIP, m_strNetCptPort, m_strNetCptTimeOut);
			m_list_md.push_back(*p_md);
			while(AfxBeginThread(ExecReqThreadFunc,(LPVOID)(&(m_list_md.back())))==NULL);
			++m_nTotalReq;
			CString str;
			str.Format(_T("%d/%d"),m_list_md.size(),m_nTotalReq);
			SetDlgItemText(IDC_EDIT_NUM,str);
		}
		break;
	case MSGUSER_DELETEMAINDATA:
		{
			CMainData *p_md=(CMainData *)wParam;
			for(std::list<CMainData>::iterator it=m_list_md.begin();it!=m_list_md.end();)
			{
				if(&(*it)==p_md)
					 it=m_list_md.erase(it); 
				else
					++it;
			}
			CString str;
			str.Format(_T("%d/%d"),m_list_md.size(),m_nTotalReq);
			SetDlgItemText(IDC_EDIT_NUM,str);
		}
		break;
	case MSGUSER_UPDATEEDIT:
		{
			CString * p_str = (CString *)wParam; 
			if(m_editShow.GetLimitText()<=UINT(m_nEditLength+p_str->GetLength()))
				m_editShow.SetLimitText(m_editShow.GetLimitText()*2);
			m_editShow.SetSel(m_nEditLength,m_nEditLength);
			m_editShow.ReplaceSel(*p_str);
			m_nEditLength+=p_str->GetLength();
			m_editShow.SendMessage(WM_VSCROLL,SB_BOTTOM ,0);
		}
		break;
	}
	return 0;
}

UINT CMeterCommServerDlg::ExecReqThreadFunc(LPVOID lpParam)
{
	CMainData *p_md = (CMainData *)lpParam;
	CMeterCommServerDlg * pMainDlg = (CMeterCommServerDlg *)p_md->m_pMainDlg;
	CString strRecvData, strSendData, strTime, strHead, strError;
	CString strEditShow(_T("--------------------------------------------------------\r\n"));
	CTime curTime;
	int nRtn = p_md->m_sockClient.StringRecv(strRecvData);
	strEditShow += _T("From ") + p_md->m_strClientIP + _T("\r\n");
	curTime = CTime::GetCurrentTime();
	strTime = curTime.Format(_T("%Y-%m-%d %H:%M:%S"));
	strEditShow += strTime + _T(" ") + strRecvData + _T("\r\n");
	if (!nRtn)
	{
		if (strRecvData.Mid(0, 1) == _T("[") && strRecvData.Mid(9, 1) == _T("]") && _ttoi(strRecvData.Mid(1, 6)) == strRecvData.GetLength())
		{
			int nFuncCode = _ttoi(strRecvData.Mid(7, 2));
			strRecvData.Delete(0, 10);
			switch (nFuncCode)
			{
			case 1://�������
			{
				CString strPath;
				strPath = ZUtil::GetExeCatalogPath() + _T("\\update\\MeterComm.exe");
				ULONGLONG llLength = 0;
				p_md->m_sockClient.SetFilePath(strPath);
				CStdioFile file;
				if (file.Open(strPath, CFile::modeRead | CFile::typeBinary))
					llLength = file.GetLength();
				file.Close();
				p_md->m_sockClient.SetFileLength(llLength);
				int nRtn = 0;
				strSendData = _T("�������ʧ��");
				CString strLength, strCurTimes;
				strLength.Format(_T("%lld"), llLength);
				nRtn = p_md->m_sockClient.StringSend(strLength);
				if (nRtn)
					goto end1;
				nRtn = p_md->m_sockClient.StringRecv(strCurTimes);
				if (nRtn)
					goto end1;
				ULONGLONG llCurTimes = _ttoi64(strCurTimes);
				p_md->m_sockClient.SetCurTimes(llCurTimes);
				nRtn = p_md->m_sockClient.FileSend();
				if (nRtn)
					goto end1;
				strSendData = _T("������³ɹ�");
			end1:
				strHead.Format(_T("[%06d01]"), strSendData.GetLength() + 10);
				strSendData = strHead + strSendData;
			}
			break;
			case 2://���°汾
			{
				strSendData = pMainDlg->m_strVersion + _T("*") + pMainDlg->m_strUpdateFlag;
				strHead.Format(_T("[%06d02]"), strSendData.GetLength() + 10);
				strSendData = strHead + strSendData;
			}
			break;
			case 3://����Ŀ¼
			{
				std::vector<CString> vec_strPaths;
				ZUtil::BrowseCurrentDir(ZUtil::GetExeCatalogPath() + _T("\\plan\\network"), vec_strPaths);
				int nSize = vec_strPaths.size();
				for (int i = 0; i < nSize; ++i)
				{
					if (vec_strPaths[i].Right(3) == _T(".pn"))
						strSendData += vec_strPaths[i].Mid(vec_strPaths[i].ReverseFind(_T('\\')) + 1) + _T("*");
				}
				strHead.Format(_T("[%06d03]"), strSendData.GetLength() + 10);
				strSendData = strHead + strSendData;
			}
			break;
			case 4://��������
			{
				std::vector<CString> vec_strPaths;
				ZUtil::BrowseCurrentDir(ZUtil::GetExeCatalogPath() + _T("\\plan\\network"), vec_strPaths);
				int nSize = vec_strPaths.size();
				int i = 0;
				for (; i < nSize; ++i)
				{
					if (strRecvData == vec_strPaths[i].Mid(vec_strPaths[i].ReverseFind(_T('\\')) + 1))
						break;
				}
				ULONGLONG llLength = 0;
				p_md->m_sockClient.SetFilePath(vec_strPaths[i]);
				CStdioFile file;
				if (file.Open(vec_strPaths[i], CFile::modeRead | CFile::typeBinary))
					llLength = file.GetLength();
				file.Close();
				p_md->m_sockClient.SetFileLength(llLength);
				int nRtn = 0;
				strSendData = _T("�����ļ�ʧ��");
				CString strLength, strCurTimes;
				strLength.Format(_T("%lld"), llLength);
				nRtn = p_md->m_sockClient.StringSend(strLength);
				if (nRtn)
					goto end4;
				nRtn = p_md->m_sockClient.StringRecv(strCurTimes);
				if (nRtn)
					goto end4;
				ULONGLONG llCurTimes = _ttoi64(strCurTimes);
				p_md->m_sockClient.SetCurTimes(llCurTimes);
				nRtn = p_md->m_sockClient.FileSend();
				if (nRtn)
					goto end4;
				strSendData = _T("�����ļ��ɹ�");
			end4:
				strHead.Format(_T("[%06d04]"), strSendData.GetLength() + 10);
				strSendData = strHead + strSendData;
			}
			break;
			case 5://���ݿ⴫��
			{
				CString strPath;
				strPath = ZUtil::GetExeCatalogPath() + _T("\\res\\DataItem.di");
				ULONGLONG llLength = 0;
				p_md->m_sockClient.SetFilePath(strPath);
				CStdioFile file;
				if (file.Open(strPath, CFile::modeRead | CFile::typeBinary))
					llLength = file.GetLength();
				file.Close();
				p_md->m_sockClient.SetFileLength(llLength);
				int nRtn = 0;
				strSendData = _T("�����ļ�ʧ��");
				CString strLength, strCurTimes;
				strLength.Format(_T("%lld"), llLength);
				nRtn = p_md->m_sockClient.StringSend(strLength);
				if (nRtn)
					goto end5;
				nRtn = p_md->m_sockClient.StringRecv(strCurTimes);
				if (nRtn)
					goto end5;
				ULONGLONG llCurTimes = _ttoi64(strCurTimes);
				p_md->m_sockClient.SetCurTimes(llCurTimes);
				nRtn = p_md->m_sockClient.FileSend();
				if (nRtn)
					goto end5;
				strSendData = _T("�����ļ��ɹ�");
			end5:
				strHead.Format(_T("[%06d05]"), strSendData.GetLength() + 10);
				strSendData = strHead + strSendData;
			}
			break;
			case 11://645�������ݴ���
			{
				if (p_md->m_dlt645.SetParamByCstring(strRecvData))
				{
					int nDllType = p_md->m_dlt645.GetDllChoose();
					if (nDllType == 0)
						WaitForSingleObject(pMainDlg->m_hMutex, INFINITE);
					CString strIP, strPort, strTimeOut;
					p_md->m_dlt645.GetNetCptInfo(strIP, strPort, strTimeOut);
					if (strIP == _T("AUTO") && strPort == _T("AUTO"))
					{
						switch (nDllType)
						{
						case 2:
						{
							strIP = _T("10.0.17.99");
							strPort = _T("6666");
						}
						break;
						case 3:
						{
							strIP = _T("10.0.17.109");
							strPort = _T("6666");
						}
						break;
						case 4:
						{
							strIP = _T("10.0.17.108");
							strPort = _T("6666");
						}
						break;
						}
						p_md->m_dlt645.SetNetCptInfo(strIP, strPort, strTimeOut);
					}
					CString strData;
					int nRtnEncode = p_md->m_dlt645.DataEncode(strData, strError);
					if (nRtnEncode)
					{
						strError.Format(_T("error:%4d%c%s"), 1000 + nRtnEncode, tcSplitError, strError.Mid(strError.Find(tcSplitError) + 1));
						strSendData.Format(_T("%d%c%s"), 1000 + nRtnEncode, tcSplitNetData, strError);
					}
					else
					{
						strSendData = _T("0000") + CString(tcSplitNetData) + p_md->m_dlt645.GetParamByCstring() + strData;
					}
					strHead.Format(_T("[%06d11]"), strSendData.GetLength() + 10);
					strSendData = strHead + strSendData;
					if (nDllType == 0)
						ReleaseMutex(pMainDlg->m_hMutex);
				}
			}
			break;
			case 12://698�������ݴ���
			{
				if (p_md->m_dlt698.SetParamByCstring(strRecvData))
				{
					int nDllType = p_md->m_dlt698.GetDllChoose();
					CString strReqResCode = p_md->m_dlt698.GetReqResCode();
					CString strObject = p_md->m_dlt698.GetTheObject();
					if (strReqResCode == _T("02") || strObject.Find(_T("F1000700")) != -1)
						WaitForSingleObject(pMainDlg->m_hMutex, INFINITE);
					CString strIP, strPort, strTimeOut;
					p_md->m_dlt698.GetNetCptInfo(strIP, strPort, strTimeOut);
					if (strIP == _T("AUTO") && strPort == _T("AUTO"))
					{
						switch (nDllType)
						{
						case 1:
						{
							strIP = _T("10.0.17.107");
							strPort = _T("6666");
						}
						break;
						}
						p_md->m_dlt698.SetNetCptInfo(strIP, strPort, strTimeOut);
					}
					CString strData;
					int nRtnEncode = p_md->m_dlt698.DataEncode(strData, strError);
					if (nRtnEncode)
					{
						strError.Format(_T("error:%4d%c%s"), 1000 + nRtnEncode, tcSplitError, strError.Mid(strError.Find(tcSplitError) + 1));
						strSendData.Format(_T("%d%c%s"), 1000 + nRtnEncode, tcSplitNetData, strError);
					}
					else
					{
						strSendData = _T("0000") + CString(tcSplitNetData) + p_md->m_dlt698.GetParamByCstring() + strData;
					}
					strHead.Format(_T("[%06d12]"), strSendData.GetLength() + 10);
					strSendData = strHead + strSendData;
					if (strReqResCode == _T("02") || strObject.Find(_T("F1000700")) != -1)
						ReleaseMutex(pMainDlg->m_hMutex);
				}
			}
			break;
			case 13://698�������ݴ���
			{
				if (p_md->m_dlt698.SetParamByCstring(strRecvData.Left(strRecvData.ReverseFind(_T(',')))))
				{
					int nDllType = p_md->m_dlt698.GetDllChoose();
					CString strIP, strPort, strTimeOut;
					p_md->m_dlt698.GetNetCptInfo(strIP, strPort, strTimeOut);
					if (strIP == _T("AUTO") && strPort == _T("AUTO"))
					{
						switch (nDllType)
						{
						case 1:
						{
							strIP = _T("10.0.17.107");
							strPort = _T("6666");
						}
						break;
						}
						p_md->m_dlt698.SetNetCptInfo(strIP, strPort, strTimeOut);
					}
					CString strValue;
					bool bRtnDncode = p_md->m_dlt698.DataDecode(strRecvData.Mid(strRecvData.ReverseFind(_T(',')) + 1), strValue);
					if (bRtnDncode)
						strSendData = _T("1");
					else
						strSendData = _T("0");
					strSendData+= CString(tcSplitNetData) + p_md->m_dlt698.GetParamByCstring()+strValue;
					strHead.Format(_T("[%06d13]"), strSendData.GetLength() + 10);
					strSendData = strHead + strSendData;
				}
			}
			break;
			case 14://ת����12
			{
				CString strSendDataTemp, strRecvDataTemp;
				strSendDataTemp = strRecvData;
				strHead.Format(_T("[%06d12]"), strSendDataTemp.GetLength() + 10);
				strSendDataTemp = strHead + strSendDataTemp;
				ZStringSocket zSock;
				int nRtn = zSock.InitSocket();
				if (nRtn)
					goto end14;
				zSock.SetTimeOut(ZSocket::TIMEOUT_CONT, _ttoi(pMainDlg->m_strServerTimeOut));
				zSock.SetTimeOut(ZSocket::TIMEOUT_SEND, _ttoi(pMainDlg->m_strServerTimeOut));
				zSock.SetTimeOut(ZSocket::TIMEOUT_RECV, _ttoi(pMainDlg->m_strServerTimeOut));
				nRtn = zSock.Connect(_T("10.0.17.111"), _T("8888"));
				if (nRtn)
					goto end14;
				nRtn = zSock.StringSend(strSendDataTemp);
				if (nRtn)
					goto end14;
				nRtn = zSock.StringRecv(strRecvDataTemp);
			end14:
				zSock.CloseSocket();
				if (!nRtn)
				{
					if (strRecvDataTemp.Mid(0, 1) == _T("[") && strRecvDataTemp.Mid(9, 1) == _T("]") && _ttoi(strRecvDataTemp.Mid(1, 6)) == strRecvDataTemp.GetLength())
					{
						strSendData = strRecvDataTemp;
						strSendData.Delete(0, 10);
						strHead.Format(_T("[%06d14]"), strSendData.GetLength() + 10);
						strSendData = strHead + strSendData;
					}
				}
			}
			break;
			case 15://ת����13
			{
				CString strSendDataTemp, strRecvDataTemp;
				strSendDataTemp = strRecvData;
				strHead.Format(_T("[%06d13]"), strSendDataTemp.GetLength() + 10);
				strSendDataTemp = strHead + strSendDataTemp;
				ZStringSocket zSock;
				int nRtn = zSock.InitSocket();
				if (nRtn)
					goto end15;
				zSock.SetTimeOut(ZSocket::TIMEOUT_CONT, _ttoi(pMainDlg->m_strServerTimeOut));
				zSock.SetTimeOut(ZSocket::TIMEOUT_SEND, _ttoi(pMainDlg->m_strServerTimeOut));
				zSock.SetTimeOut(ZSocket::TIMEOUT_RECV, _ttoi(pMainDlg->m_strServerTimeOut));
				nRtn = zSock.Connect(_T("10.0.17.111"), _T("8888"));
				if (nRtn)
					goto end15;
				nRtn = zSock.StringSend(strSendDataTemp);
				if (nRtn)
					goto end15;
				int nMaxRecvLen = 10240;
				nRtn = zSock.StringRecv(strRecvDataTemp);
			end15:
				zSock.CloseSocket();
				if (!nRtn)
				{
					if (strRecvDataTemp.Mid(0, 1) == _T("[") && strRecvDataTemp.Mid(9, 1) == _T("]") && _ttoi(strRecvDataTemp.Mid(1, 6)) == strRecvDataTemp.GetLength())
					{
						strSendData = strRecvDataTemp;
						strSendData.Delete(0, 10);
						strHead.Format(_T("[%06d15]"), strSendData.GetLength() + 10);
						strSendData = strHead + strSendData;
					}
				}
			}
			break;
			}
			p_md->m_sockClient.StringSend(strSendData);
			curTime = CTime::GetCurrentTime();
			strTime = curTime.Format(_T("%Y-%m-%d %H:%M:%S"));
			strEditShow += strTime + _T(" ") + strSendData + _T("\r\n");
		}
	}
	p_md->m_sockClient.CloseSocket();
	SendMessageTimeout(pMainDlg->m_hWnd, WM_MSGRECVPRO, (WPARAM)&strEditShow, MSGUSER_UPDATEEDIT, SMTO_BLOCK, 500, NULL);
	SendMessageTimeout(pMainDlg->m_hWnd, WM_MSGRECVPRO, (WPARAM)p_md, MSGUSER_DELETEMAINDATA, SMTO_BLOCK, 500, NULL);
	return 0;
}

void CMeterCommServerDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	Shell_NotifyIcon(NIM_DELETE, &m_nid);
	m_bIsListen=false;
	m_sockServer.StopComm();
	m_sockServer.CloseSocket();
	WaitForSingleObject(m_hEvtExit,INFINITE);
	while(m_list_md.size())
		Sleep(100);
	CDialogEx::OnOK();
}


void CMeterCommServerDlg::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if(!m_bIsExit)
	{
		ShowWindow(SW_HIDE);
		return;
	}
	Shell_NotifyIcon(NIM_DELETE, &m_nid);
	m_bIsListen=false;
	m_sockServer.StopComm();
	m_sockServer.CloseSocket();
	WaitForSingleObject(m_hEvtExit,INFINITE);
	while(m_list_md.size())
		Sleep(100);
	CDialogEx::OnClose();
}

void CMeterCommServerDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	switch(nIDEvent)
	{
	case nClearMsg:
		{
			SetDlgItemText(IDC_EDIT_SHOW,_T(""));
			m_nEditLength=0;
		}
		break;
	}
	CDialogEx::OnTimer(nIDEvent);
}

void CMeterCommServerDlg::InitTray(void)
{
	m_nid.cbSize=(DWORD)sizeof(NOTIFYICONDATA);
	m_nid.hWnd =this->m_hWnd;
	m_nid.uID=IDR_MAINFRAME;
	m_nid.uFlags = NIF_ICON|NIF_MESSAGE |NIF_TIP;
	m_nid.uCallbackMessage=WM_TRAYNOTIFY;
	m_nid.hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	_tcscpy_s(m_nid.szTip,_tcslen(_T("MeterCommServer"))+1,_T("MeterCommServer"));
	Shell_NotifyIcon(NIM_ADD,&m_nid);
}

afx_msg LRESULT CMeterCommServerDlg::OnTraynotify(WPARAM wParam, LPARAM lParam)
{
	if(wParam!=IDR_MAINFRAME) 
		return 1;
	switch(lParam)
	{
	case WM_RBUTTONUP:
		{
			CPoint pt;
			GetCursorPos(&pt);
			CMenu menu;  
			menu.LoadMenu(IDR_MENU_MAIN);  
			CMenu* pPopup=menu.GetSubMenu(0);  
			SetForegroundWindow();
			if(pPopup) 
				pPopup->TrackPopupMenu(TPM_LEFTALIGN,pt.x,pt.y,this); 
		}
		break;
	case WM_LBUTTONDOWN:
		{
			ShowWindow(SW_SHOWNORMAL); 
			SetForegroundWindow();
		} 
		break;
	}
	return 0;
}

void CMeterCommServerDlg::OnExit()
{
	// TODO: �ڴ���������������
	m_bIsExit=true;
	PostMessage(WM_CLOSE,0,0);
}


BOOL CMeterCommServerDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���
	if(pMsg->message==WM_KEYDOWN)   
	{
		UINT nKey=pMsg->wParam;
		if(nKey==VK_ESCAPE||nKey==VK_RETURN)
			return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}
