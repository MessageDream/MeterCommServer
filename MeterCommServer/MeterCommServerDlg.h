
// MeterCommServerDlg.h : ͷ�ļ�
//

#pragma once
#include <list>
#include "afxwin.h"

// CMeterCommServerDlg �Ի���
class CMeterCommServerDlg : public CDialogEx
{
// ����
public:
	CMeterCommServerDlg(CWnd* pParent = NULL);	// ��׼���캯��
	virtual ~CMeterCommServerDlg();

// �Ի�������
	enum { IDD = IDD_METERCOMMSERVER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
private:
	ZSocket m_sockServer;
	CString m_strServerIP;
	CString m_strServerPort;
	CString m_strServerTimeOut;
	CString m_strNetCptIP;
	CString m_strNetCptPort;
	CString m_strNetCptTimeOut;
	CString m_strVersion;
	bool m_bIsListen;
	std::list<CMainData> m_list_md;
	HANDLE m_hMutex;//���ӱ��ؼ��ܻ���Ҫ��ռ
	HANDLE m_hEvtExit;
	CEdit m_editShow;
	int m_nEditLength;
	int m_nTotalReq;
	CButton m_checkClearTime;
	CButton m_checkAutoRun;
	NOTIFYICONDATA m_nid;
	bool m_bIsExit;
	CString m_strUpdateFlag;
protected:
	afx_msg LRESULT OnMsgrecvpro(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTraynotify(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnBnClickedButtonStartserver();
	afx_msg void OnBnClickedButtonStopserver();
	afx_msg void OnBnClickedButtonClearmsg();
	afx_msg void OnBnClickedButtonConfig();
	afx_msg void OnBnClickedCheckCleartime();
	afx_msg void OnBnClickedCheckAutorun();
	static UINT ListenThreadFunc(LPVOID lpParam);
	static UINT ExecReqThreadFunc(LPVOID lpParam);
	afx_msg void OnBnClickedOk();
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	void InitTray(void);
	afx_msg void OnExit();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
