
// TcpTestToolDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"

#include <string>
#include "TcpClient.h"



// CTcpTestToolDlg �Ի���
class CTcpTestToolDlg : public CDialogEx
{
// ����
public:
	CTcpTestToolDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TCPTESTTOOL_DIALOG };
#endif

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
	afx_msg LRESULT OnUserConnecting(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUserConnectFail(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUserSending(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUserSendOnce(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUserSendFail(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUserRecving(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUserRecvFail(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUserRecved(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUserRecvData(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUserRecvOnce(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUserTimes(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUserTimegap(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUserExit(WPARAM wParam, LPARAM lParam);
	static UINT ThreadFun(LPVOID pParam);


	DECLARE_MESSAGE_MAP()

public:
	void Log(const char* format, ...);
	void SaveExcelFile(CString& fileName);
	// ��ȡ��ǰ��ʱ��(Ĭ�ϸ�ʽΪ2019-07-27 12:32:20)
	std::string GetNowTime(const char* format = nullptr);

public:
	afx_msg void OnBnClickedStart();
	static __int64 TimeDiff(SYSTEMTIME& left, SYSTEMTIME& right);
	CIPAddressCtrl m_ipControl;
	short m_port;
	int m_connectNum;
	CString m_send;
	CString m_rec;
	CListBox m_listLog;
	CListCtrl m_listOutput;

	CWinThread** pThread; //һ���߳�
	CComboBox m_comboType;
	int m_cycle;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	int m_run;
	CString m_runtime;
	int m_revtotal;
	int m_sendtotal;
	afx_msg void OnBnClickedFile();
	afx_msg void OnBnClickedSave();
	int m_cyclegap;
	volatile bool bTesting;
	int m_runningClient;// ��Ŀͻ���
	afx_msg void OnBnClickedPause();
	afx_msg void OnBnClickedClear();
};

//�̲߳���
struct ThreadParam
{
	std::string ip;
	SYSTEMTIME stSend;
	SYSTEMTIME stRev;
	short port;
	int index;         //�߳����ڵ����
	int threadid;      //ϵͳ���߳�id
	int cycle;         //ѭ������
	int sleepTime;     //����ʱ�� 
	int times;         //�Ѿ�ѭ���Ĵ���
	CTcpTestToolDlg* pThis; //����ָ��
	CTcpClient Client;
};