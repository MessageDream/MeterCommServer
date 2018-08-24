#pragma once


// CConfig �Ի���

class CConfig : public CDialogEx
{
	DECLARE_DYNAMIC(CConfig)

public:
	CConfig(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CConfig();

// �Ի�������
	enum { IDD = IDD_CONFIG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
private:
	CString m_strServerIP;
	CString m_strServerPort;
	CString m_strServerTimeOut;
	CString m_strNetCptIP;
	CString m_strNetCptPort;
	CString m_strNetCptTimeOut;
	CString m_strVersion;
	CString m_strUpdateFlag;
public:
	virtual BOOL OnInitDialog();
	void SetParam(const CString & strServerIP,const CString & strServerPort,const CString & strServerTimeOut,const CString & strNetCptIP,const CString & strNetCptPort,const CString & strNetCptTimeOut,const CString & strVersion,const CString & strUpdateFlag);
	void GetParam(CString & strServerIP,CString & strServerPort,CString & strServerTimeOut,CString & strNetCptIP,CString & strNetCptPort,CString & strNetCptTimeOut,CString & strVersion,CString & strUpdateFlag);
	afx_msg void OnBnClickedOk();
};
