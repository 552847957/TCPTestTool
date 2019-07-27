
// TcpTestToolDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TcpTestTool.h"
#include "TcpTestToolDlg.h"
#include "afxdialogex.h"
#include "oomsFunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

enum ColType
{
	col_index = 0,
	col_threadid = 1,
	col_maxtime = 2,
	col_mintime = 3,
	col_times=4, 
	col_sendonce=5, 
	col_recvonce=6, 
	col_sendall=7,
    col_recvall=8,
	col_state = 9,
	col_total = 10              //����ֵ����ܹ��е���������һ��ά��          
};

HRESULT AutoWrap(int autoType, VARIANT *pvResult, IDispatch *pDisp, LPOLESTR ptName, int cArgs...) {
	// Begin variable-argument list...  
	va_list marker;
	va_start(marker, cArgs);
	if (!pDisp) {
		return 0;
	}
	// Variables used...  
	DISPPARAMS dp = { NULL, NULL, 0, 0 };
	DISPID dispidNamed = DISPID_PROPERTYPUT;
	DISPID dispID;
	HRESULT hr;
//	char buf[200];
	char szName[200];
	// Convert down to ANSI  
	WideCharToMultiByte(CP_ACP, 0, ptName, -1, szName, 256, NULL, NULL);
	// Get DISPID for name passed...  
	hr = pDisp->GetIDsOfNames(IID_NULL, &ptName, 1, LOCALE_USER_DEFAULT, &dispID);
	if (FAILED(hr)) {
		return hr;
	}
	// Allocate memory for arguments...  
	VARIANT *pArgs = new VARIANT[cArgs + 1];
	// Extract arguments...  
	for (int i = 0; i < cArgs; i++) {
		pArgs[i] = va_arg(marker, VARIANT);
	}
	// Build DISPPARAMS  
	dp.cArgs = cArgs;
	dp.rgvarg = pArgs;
	// Handle special-case for property-puts!  
	if (autoType & DISPATCH_PROPERTYPUT) {
		dp.cNamedArgs = 1;
		dp.rgdispidNamedArgs = &dispidNamed;
	}
	// Make the call!  
	hr = pDisp->Invoke(dispID, IID_NULL, LOCALE_SYSTEM_DEFAULT, autoType, &dp, pvResult, NULL, NULL);
	if (FAILED(hr)) {
		delete[] pArgs;
		return hr;
	}
	// End variable-argument section...  
	va_end(marker);
	delete[] pArgs;
	return hr;
}

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CTcpTestToolDlg �Ի���



CTcpTestToolDlg::CTcpTestToolDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_TCPTESTTOOL_DIALOG, pParent)
	, m_port(0)
	, m_connectNum(0)
	, m_send(_T(""))
	, m_rec(_T(""))
	, pThread(NULL)
	, m_cycle(0)
	, m_runtime(_T(""))
	, m_revtotal(0)
	, m_sendtotal(0)
	, m_cyclegap(100)
	, m_run(0)
	, bTesting(false)
	, m_runningClient(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTcpTestToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IPADDRESS1, m_ipControl);
	DDX_Text(pDX, IDC_EDIT4, m_port);
	DDX_Text(pDX, IDC_EDIT5, m_connectNum);
	DDX_Text(pDX, IDC_EDIT2, m_send);
	DDX_Text(pDX, IDC_EDIT1, m_rec);
	DDX_Control(pDX, IDC_LIST3, m_listLog);
	DDX_Control(pDX, IDC_LIST1, m_listOutput);
	DDX_Control(pDX, IDC_COMBO1, m_comboType);
	DDX_Text(pDX, IDC_EDIT3, m_cycle);
	DDX_Text(pDX, IDC_EDIT7, m_runtime);
	DDX_Text(pDX, IDC_EDIT8, m_revtotal);
	DDX_Text(pDX, IDC_EDIT9, m_sendtotal);
	DDX_Text(pDX, IDC_EDIT6, m_cyclegap);
	DDX_Text(pDX, IDC_EDIT10, m_runningClient);
}

BEGIN_MESSAGE_MAP(CTcpTestToolDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON3, &CTcpTestToolDlg::OnBnClickedStart)
	ON_MESSAGE(WM_USER_CONNECTING, OnUserConnecting)
	ON_MESSAGE(WM_USER_CONNECTFAIL, OnUserConnectFail)
	ON_MESSAGE(WM_USER_SENDING, OnUserSending)
	ON_MESSAGE(WM_USER_SENDFAIL, OnUserSendFail)
	ON_MESSAGE(WM_USER_SENDONCE, OnUserSendOnce)
	ON_MESSAGE(WM_USER_RECVING, OnUserRecving)
	ON_MESSAGE(WM_USER_RECVONCE, OnUserRecvOnce)
	ON_MESSAGE(WM_USER_RECVFAIL, OnUserRecvFail)
	ON_MESSAGE(WM_USER_RECVDATA, OnUserRecvData)
	ON_MESSAGE(WM_USER_RECVED, OnUserRecved)
	ON_MESSAGE(WM_USER_TIMES, OnUserTimes)
	ON_MESSAGE(WM_USER_TIMEGAP, OnUserTimegap)
	ON_MESSAGE(WM_USER_EXIT, OnUserExit)//
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON1, &CTcpTestToolDlg::OnBnClickedFile)
	ON_BN_CLICKED(IDC_BUTTON2, &CTcpTestToolDlg::OnBnClickedSave)
	ON_BN_CLICKED(IDC_BUTTON4, &CTcpTestToolDlg::OnBnClickedPause)
	ON_BN_CLICKED(IDC_BUTTON5, &CTcpTestToolDlg::OnBnClickedClear)
END_MESSAGE_MAP()


// CTcpTestToolDlg ��Ϣ�������

BOOL CTcpTestToolDlg::OnInitDialog()
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

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	//��ʼ����
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

	//��ʼ��com
	CoInitialize(NULL);


	DWORD dwStyle = m_listOutput.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;
	dwStyle |= LVS_EX_GRIDLINES;
	m_listOutput.SetExtendedStyle(dwStyle);
	m_listOutput.InsertColumn(col_index, _T("���"), LVCFMT_LEFT, 50);
	m_listOutput.InsertColumn(col_threadid, _T("�߳�id"), LVCFMT_LEFT, 80);
	m_listOutput.InsertColumn(col_maxtime, _T("�����Ӧʱ��ms"), LVCFMT_LEFT, 150);
	m_listOutput.InsertColumn(col_mintime, _T("��С��Ӧʱ��ms"), LVCFMT_LEFT, 150);
	m_listOutput.InsertColumn(col_times, _T("���Դ���"), LVCFMT_LEFT, 100);
	m_listOutput.InsertColumn(col_sendonce, _T("���η���"), LVCFMT_LEFT, 100);
	m_listOutput.InsertColumn(col_recvonce, _T("���ν���"), LVCFMT_LEFT, 100);
	m_listOutput.InsertColumn(col_sendall, _T("��������"), LVCFMT_LEFT, 100);
	m_listOutput.InsertColumn(col_recvall, _T("��������"), LVCFMT_LEFT, 80);
	m_listOutput.InsertColumn(col_state, _T("״̬"), LVCFMT_LEFT, 70);

//--------------------OOMS���ƴ���--------------------------
	//�Ȳ���4��Э�� 1��Уʱ  2����¼  3����ȡͣ����  4����ȡ��λ
	m_comboType.InsertString(0, "��ȡ��λ");
	m_comboType.InsertString(0, "��ȡͣ����");
	m_comboType.InsertString(0, "��¼");
	m_comboType.InsertString(0, "Уʱ");
	m_comboType.SetCurSel(0);
//--------------------OOMS���ƴ���end--------------------------

	Log("��ӭʹ��tcp���ߣ����úò�����ʼ���԰�!");
	Log("���Ƕ�̬��־���һ���������µ�״̬!");

	// ��ʼ��������������Ϣ Ĭ��ֵ
	SetDlgItemText(IDC_IPADDRESS1, "127.0.0.1");
	SetDlgItemText(IDC_EDIT4, "8104");
	SetDlgItemText(IDC_EDIT5, "50");
	SetDlgItemText(IDC_EDIT3, "100");
	


	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CTcpTestToolDlg::OnSysCommand(UINT nID, LPARAM lParam)
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
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CTcpTestToolDlg::OnPaint()
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
HCURSOR CTcpTestToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CTcpTestToolDlg::OnBnClickedStart()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (bTesting)
	{
		MessageBox("�ϴβ��Ի�δ�������밴����ͣ���ԡ�������");
		return;
	}
	bTesting = true;
	//׼��tcp����
	UpdateData(TRUE);
	//���Ի�ȡip
	DWORD dwIP=0;
	IN_ADDR ia;
	std::string strIP;
	m_ipControl.GetAddress(dwIP);
	ia.S_un.S_addr = htonl(dwIP);
	//strIP = inet_ntoa(ia);
	//inet_ntoa(addr2.sin_addr);
	//����ȼ��滻Ϊ
	char sendBuf[20] = { '\0' };
	inet_ntop(AF_INET, (void*)&ia, sendBuf, 16);
	strIP = sendBuf;
	if (strIP.empty())
	{
		Log("%s", "ip����Ϊ�գ�");
		return;
	}
	if (m_port<=0)
	{
		Log("�˿ںŲ���");
		return;
	}
	if (m_connectNum<=0)
	{
		Log("��������������");
		return;
	}
	if (m_connectNum>=1000)
	{
		Log("����������");
		return;
	}
	if (m_send.IsEmpty())
	{
		Log("���͵����ݲ���Ϊ�գ�");
		return;
	}

	SetTimer(1, 1000, NULL);

	//����n���߳̿�ʼ����
	m_runningClient = m_connectNum;
	m_sendtotal = 0;
	m_revtotal = 0;
	if (pThread!=NULL)
	{
		delete[] pThread;
	}
	pThread = new CWinThread*[m_connectNum];
	// ����б�
	m_listOutput.DeleteAllItems();
	m_run = 0;
	for (int i=0;i<m_connectNum;i++)
	{
		ThreadParam* param = new ThreadParam;
		param->Client.SetParam(param);
		param->index = i;
		param->pThis = this;
		param->ip = strIP;
		param->port = m_port;
		param->cycle = m_cycle;
		param->sleepTime = m_cyclegap;
		pThread[i] = AfxBeginThread(ThreadFun, param);
		pThread[i]->SetThreadPriority(THREAD_PRIORITY_BELOW_NORMAL);
		param->threadid = pThread[i]->m_nThreadID;

		CString strTemp;
		strTemp.Format("%d", i);
		//�����б�
		m_listOutput.InsertItem(i, strTemp);
		strTemp.Format("0x%x", param->threadid);
		m_listOutput.SetItemText(i, col_threadid, strTemp);
		m_listOutput.SetItemText(i, col_state, "����");
	}

	UpdateData(FALSE);
}

UINT CTcpTestToolDlg::ThreadFun(LPVOID pParam)
{
	ThreadParam* param = (ThreadParam*)pParam;
	if (param == NULL)
	{
		goto EXIT;
	}
	::PostMessage(param->pThis->m_hWnd, WM_USER_CONNECTING,(WPARAM)param,0);
	bool bRet = param->Client.ConnectServer(param->ip.c_str(), param->port);
	if (!bRet)
	{
		::PostMessage(param->pThis->m_hWnd, WM_USER_CONNECTFAIL, (WPARAM)param, 0);
		goto EXIT;
	}

	//ѭ�����壬���Ϸ��ͽ���----------------------begin-------------
	for(param->times=1;param->times<=param->cycle;++param->times)
	{
		if (param->pThis->bTesting == false)
		{
			break;
		}

		::PostMessage(param->pThis->m_hWnd, WM_USER_SENDING, (WPARAM)param, 0);

		//-----------------------OOMS������-------------------------------------
		int cmd = 0;
		switch (param->pThis->m_comboType.GetCurSel())
		{
		case 0:
			cmd = PT_CHECK_TIME;
			break;
		case 1:
			cmd = PT_LOGIN_IN;
			break;
		case 2:
			cmd = PT_PARKING_LIST;
			break;
		case 3:
			cmd = PT_BERTH_INFO;
			break;
		default:
			break;
		}

		//-----------------------OOMS������end-------------------------------------

		bRet = param->Client.SendYourDate(param->pThis->m_send, param->pThis->m_send.GetLength(), cmd);
		if (!bRet)
		{
			::PostMessage(param->pThis->m_hWnd, WM_USER_SENDFAIL, (WPARAM)param, 0);
			goto EXIT;
		}

		//��������ɹ������һ�η��������
		::PostMessage(param->pThis->m_hWnd, WM_USER_TIMES, (WPARAM)param, param->times);

		//��΢�����£�������cpu��
		if (param->sleepTime<10)
		{
			param->sleepTime = 10;
		}
		Sleep(param->sleepTime);
	}

	//ѭ�����壬���Ϸ��ͽ���----------------------end-------------
EXIT:
	::PostMessage(param->pThis->m_hWnd, WM_USER_EXIT, (WPARAM)param, 0);   //��Ҫ���ٴ���Դparam
	return 0;
}

// ��ȡ��ǰʱ�䣨Ĭ�ϸ�ʽΪ2019-07-27 12:32:20��
std::string CTcpTestToolDlg::GetNowTime(const char* format)
{
	char currentTime[64] = { 0 };
	SYSTEMTIME st;
	::GetLocalTime(&st);
	sprintf_s(currentTime, format == nullptr ? "%04d-%02d-%02d %02d:%02d:%02d" : format,
		st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

	return currentTime;
}

void CTcpTestToolDlg::Log(const char* format, ...)
{
	//��־���1024�ֽ�
	char log[1024] = { 0 };
	va_list vlist;
	va_start(vlist, format);
	vsprintf_s(log, 1024, format, vlist);
	va_end(vlist);

	// ��ȡ��־���ɵĵ�ǰʱ�䣨��ʽΪ2019-07-27 12:32:20��
	/*char currentTime[64] = { 0 };
	SYSTEMTIME st;
	::GetLocalTime(&st);
	sprintf_s(currentTime,"%04d-%02d-%02d %02d:%02d:%02d:   ",
		st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond );*/
	std::string currentTime = GetNowTime();
	
	//�ײ���ӵ�ǰʱ�䵽��
	std::string logout = currentTime;
	// ׷����־��������
	logout += log;
	m_listLog.InsertString(0, logout.c_str());
}


LRESULT CTcpTestToolDlg::OnUserConnecting(WPARAM wParam, LPARAM lParam)
{
	ThreadParam* param = (ThreadParam*)wParam;
	m_listOutput.SetItemText(param->index, col_state, "��ʼ����");

	return 0;
}

LRESULT CTcpTestToolDlg::OnUserConnectFail(WPARAM wParam, LPARAM lParam)
{
	ThreadParam* param = (ThreadParam*)wParam;
	Log("��%d�߳�����ʧ��", param->index);
	m_listOutput.SetItemText(param->index, col_state, "����ʧ��");
	return 0;
}

LRESULT CTcpTestToolDlg::OnUserSending(WPARAM wParam, LPARAM lParam)
{
	ThreadParam* param = (ThreadParam*)wParam;
	if (lParam == 0)
	{
		m_listOutput.SetItemText(param->index, col_state, "�ȴ�����");
	}
	else
	{
		m_listOutput.SetItemText(param->index, col_state, "��������");
		//��ʾ���͵��ֽ���,Ҫ����֮ǰ���ֽ���
		CString strHasSend = m_listOutput.GetItemText(param->index, col_sendall);
		int hassend = atoi(strHasSend);
		hassend += lParam;
		CString strTemp;
		strTemp.Format("%d", hassend);
		m_listOutput.SetItemText(param->index, col_sendall, strTemp);
		
		m_sendtotal += lParam;
		UpdateData(FALSE);
	}
	
	return 0;
}

LRESULT CTcpTestToolDlg::OnUserSendFail(WPARAM wParam, LPARAM lParam)
{
	ThreadParam* param = (ThreadParam*)wParam;
	Log("��%d�̷߳���ʧ��", param->index);
	m_listOutput.SetItemText(param->index, col_state, "����ʧ��");
	return 0;
}

LRESULT CTcpTestToolDlg::OnUserRecving(WPARAM wParam, LPARAM lParam)
{
	ThreadParam* param = (ThreadParam*)wParam;
	if (lParam==0)
	{
		m_listOutput.SetItemText(param->index, col_state, "�ȴ�����");
	}
	else
	{
		m_listOutput.SetItemText(param->index, col_state, "��������");

		//��ʾ���͵��ֽ���,Ҫ����֮ǰ���ֽ���
		CString strHasRecv = m_listOutput.GetItemText(param->index, col_recvall);
		int hasrecv = atoi(strHasRecv);
		hasrecv += lParam;
		CString strTemp;
		strTemp.Format("%d", hasrecv);
		m_listOutput.SetItemText(param->index, col_recvall, strTemp);
	
		m_revtotal += lParam;
		UpdateData(FALSE);
		
	}
	
	return 0;
}

LRESULT CTcpTestToolDlg::OnUserRecvFail(WPARAM wParam, LPARAM lParam)
{
	ThreadParam* param = (ThreadParam*)wParam;
	Log("��%d�߳̽���ʧ��", param->index);
	m_listOutput.SetItemText(param->index, col_state, "����ʧ��");
	return 0;
}

LRESULT CTcpTestToolDlg::OnUserRecved(WPARAM wParam, LPARAM lParam)
{
	ThreadParam* param = (ThreadParam*)wParam;
	m_listOutput.SetItemText(param->index, col_state, "�������");

	return 0;
}

LRESULT CTcpTestToolDlg::OnUserExit(WPARAM wParam, LPARAM lParam)
{
	ThreadParam* param = (ThreadParam*)wParam;
	m_listOutput.SetItemText(param->index, col_state, "���Խ���");
	delete param;
	m_runningClient--;
	if (m_runningClient==0)
	{
		Log("���β������");
		bTesting = false;
		KillTimer(1);
	}
	UpdateData(FALSE);
	return 0;
}

LRESULT CTcpTestToolDlg::OnUserRecvData(WPARAM wParam, LPARAM lParam)
{
	//ˢ����recv��
	std::string * pstr = (std::string*)wParam;
	m_rec = pstr->c_str();
	UpdateData(FALSE);
	delete pstr;
	return 0;
}

LRESULT CTcpTestToolDlg::OnUserTimes(WPARAM wParam, LPARAM lParam)
{
	//ˢ�²��Դ���
	ThreadParam* param = (ThreadParam*)wParam;
	CString strTemp;
	strTemp.Format("%d", lParam);
	m_listOutput.SetItemText(param->index, col_times, strTemp);
	
	return 0;
}

LRESULT CTcpTestToolDlg::OnUserTimegap(WPARAM wParam, LPARAM lParam)
{
	if (lParam == 0)
	{
		return 0;
	}
	//ˢ�����ʱ������Сʱ��
	ThreadParam* param = (ThreadParam*)wParam;
	//��ȡ֮ǰ�����ʱ��
	int currmax = atoi(m_listOutput.GetItemText(param->index, col_maxtime));
	if (currmax == 0)  //��ʼ��
	{
		currmax = -1;
	}
	if (lParam>currmax)
	{
		//��Ҫ�������ʱ��
		CString strTemp;
		strTemp.Format("%d", lParam);
		m_listOutput.SetItemText(param->index, col_maxtime, strTemp);
	}
	//��ȡ֮ǰ����Сʱ��
	int currmin = atoi(m_listOutput.GetItemText(param->index, col_mintime));
	if (currmin==0)  //��ʼ��
	{
		currmin = 999999;
	}
	if (lParam < currmin)
	{
		//��Ҫ������Сʱ��
		CString strTemp;
		strTemp.Format("%d", lParam);
		m_listOutput.SetItemText(param->index, col_mintime, strTemp);
	}
	
	return 0;
}

LRESULT CTcpTestToolDlg::OnUserSendOnce(WPARAM wParam, LPARAM lParam)
{
	ThreadParam* param = (ThreadParam*)wParam;
	CString strTemp;
	strTemp.Format("%d", lParam);
	m_listOutput.SetItemText(param->index, col_sendonce, strTemp);

	return 0;
}

LRESULT CTcpTestToolDlg::OnUserRecvOnce(WPARAM wParam, LPARAM lParam)
{
	ThreadParam* param = (ThreadParam*)wParam;
	CString strTemp;
	strTemp.Format("%d", lParam);
	m_listOutput.SetItemText(param->index, col_recvonce, strTemp);

	return 0;
}



__int64 CTcpTestToolDlg::TimeDiff(SYSTEMTIME& left, SYSTEMTIME& right)
{
	CTime tmLeft(left.wYear, left.wMonth, left.wDay, 0, 0, 0);
	CTime tmRight(right.wYear, right.wMonth, right.wDay, 0, 0, 0);

	CTimeSpan sp = tmLeft - tmRight;
	long MillisecondsL = (left.wHour * 3600 + left.wMinute * 60 + left.wSecond) * 1000 + left.wMilliseconds;
	long MillisecondsR = (right.wHour * 3600 + right.wMinute * 60 + right.wSecond) * 1000 + right.wMilliseconds;

	return  (__int64)sp.GetDays() * 86400000 + (MillisecondsL - MillisecondsR);//�˴����غ���  
}



void CTcpTestToolDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (nIDEvent == 1)
	{
		UpdateData(TRUE);
		m_run++;
		m_runtime.Format("%02d:%02d:%02d", m_run / 3600, m_run % 3600 / 60, m_run % 60);
		UpdateData(FALSE);
	}

	CDialogEx::OnTimer(nIDEvent);
}


void CTcpTestToolDlg::OnBnClickedFile()
{
	//��ȡ�ļ����ݷŵ�������
	//���ļ��Ի���ѡ���ļ�
	UpdateData(TRUE);
	CString strFile;
	CFileDialog openFileDlg(TRUE);
	INT_PTR result = openFileDlg.DoModal();
	if (result != IDOK)
	{
		return;
	}
	strFile = openFileDlg.GetPathName();
	
	//ֻ���Ĵ��ļ�
	HANDLE hFile = CreateFile(strFile, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		DWORD dw = GetLastError();
		Log("��%s����", strFile);
		Log("������%d", dw);
		return;
	}

	//��ȡ�ļ���С
	DWORD size = GetFileSize(hFile, NULL);
	void* buff = malloc(size + 1);
	memset(buff, 0, size + 1);
	//���ٿռ�
	DWORD readed=0;
	if (!ReadFile(hFile, buff, size, &readed, NULL))
	{
		Log("��ȡ%s����", strFile);
		DWORD dw = GetLastError();
		Log("������%d", dw);
		free(buff);
		CloseHandle(hFile);
		return;
	}

	m_send = (char*)buff;
	free(buff);
	CloseHandle(hFile);
	UpdateData(FALSE);
}

void CTcpTestToolDlg::OnBnClickedPause()
{
	if (!bTesting)
	{
		return;
	}
	//��ͣ����
	bTesting = false;
	for (int i=0;i<m_connectNum;i++)
	{
		CWinThread* p = pThread[i];
		WaitForSingleObject(*p,INFINITE);
	}
	KillTimer(1);
}

void CTcpTestToolDlg::OnBnClickedSave()
{
	UpdateData(TRUE);
	// ���浽excel�ļ�
// 	if (m_listOutput.GetItemCount() ==0)
// 	{
// 		Log("û����Ч�����ܱ��氡");
// 		return;
// 	}

	CString strFile;
	CFileDialog openFileDlg(FALSE);
	//���ù�������Ϊexcel����
	openFileDlg.GetOFN().lpstrFilter = "�ļ�\0*.xls; *.xlsx\0";   //�ļ����ǵ�����
	INT_PTR result = openFileDlg.DoModal();
	if (result != IDOK)
	{
		return;
	}
	
	// ��ȡ��־���ɵĵ�ǰʱ�䣨��ʽΪ20190727123220��
	//std::string currentTime = GetNowTime("%04d%02d%02d%02d%02d%02d");
	//strFile = currentTime.c_str();
	strFile = openFileDlg.GetPathName();

	// �����ܲ��Ա��������Excel�ļ���
	SaveExcelFile(strFile);
}

void CTcpTestToolDlg::SaveExcelFile(CString& fileName)
{
	// ���EXCEL��CLSID  
	CLSID clsid;
	HRESULT hr = CLSIDFromProgID(L"Excel.Application", &clsid);

	if (FAILED(hr)) {
		Log("CLSIDFromProgID() ��������ʧ�ܣ�");
		return;
	}

	// ����ʵ��  
	IDispatch *pXlApp;
	hr = CoCreateInstance(clsid, NULL, CLSCTX_LOCAL_SERVER, IID_IDispatch, (void **)&pXlApp);
	if (FAILED(hr)) {
		Log("�����Ƿ��Ѿ���װEXCEL��");
		return;
	}

	// ��ȡWorkbooks����  
	IDispatch *pXlBooks;
	{
		VARIANT result;
		VariantInit(&result);
		AutoWrap(DISPATCH_PROPERTYGET, &result, pXlApp, L"Workbooks", 0);
		pXlBooks = result.pdispVal;
	}

	// ����Workbooks.Add()����������һ���µ�Workbook  
	IDispatch *pXlBook;
	{
		VARIANT result;
		VariantInit(&result);
		AutoWrap(DISPATCH_PROPERTYGET, &result, pXlBooks, L"Add", 0);
		pXlBook = result.pdispVal;
	}

	// ����һ�����飬���������  
	int line = m_listOutput.GetItemCount();
	line++;                                     //������ͷ
	VARIANT arr;
	CStringW strOleTemp;
	arr.vt = VT_ARRAY | VT_VARIANT;
	SAFEARRAYBOUND sab[2];
	sab[0].lLbound = 1; sab[0].cElements = line;
	sab[1].lLbound = 1; sab[1].cElements = col_total;
	arr.parray = SafeArrayCreate(VT_VARIANT, 2, sab);

	//��ȡ��ͷ
	for (int j = 1; j <= col_total; j++) {
		//ע�⣬excel��1��ʼ������mfc��listcontrol��0��ʼ
		VARIANT tmp;
		LVCOLUMN columnInfo;
		char str[64] = {0};
		columnInfo.mask = LVCF_TEXT;
		columnInfo.pszText = str;
		columnInfo.cchTextMax = 64;
		m_listOutput.GetColumn(j - 1, &columnInfo);
		tmp.vt = VT_BSTR;
		strOleTemp = CA2W(columnInfo.pszText);
		tmp.bstrVal = SysAllocString(strOleTemp);
		// ������ݵ�������  
		long indices[] = { 1,j };
		SafeArrayPutElement(arr.parray, indices, (void *)&tmp);
	}

	// ��ʼ����������
	for (int i = 2; i <= line; i++) {
		for (int j = 1; j <= col_total; j++) {
			VARIANT tmp;
			tmp.vt = VT_BSTR;
			//ע�⣬excel��1��ʼ������mfc��listcontrol��0��ʼ
			strOleTemp = CA2W(m_listOutput.GetItemText(i - 2, j - 1));
			tmp.bstrVal = SysAllocString(strOleTemp);
			// ������ݵ�������  
			long indices[] = { i,j };
			SafeArrayPutElement(arr.parray, indices, (void *)&tmp);
		}
	}

	// ��Application.ActiveSheet���Ի��Worksheet����  
	IDispatch *pXlSheet;
	{
		VARIANT result;
		VariantInit(&result);
		AutoWrap(DISPATCH_PROPERTYGET, &result, pXlApp, L"ActiveSheet", 0);
		pXlSheet = result.pdispVal;
	}

	// ѡ��һ�����ʴ�С��Range  
	IDispatch *pXlRange;
	{
		VARIANT parm;
		parm.vt = VT_BSTR;
		strOleTemp.Format(L"A1:%c%d",'A'+col_total-1, line);
 		parm.bstrVal = SysAllocString(strOleTemp);
		
		VARIANT result;
		VariantInit(&result);
		AutoWrap(DISPATCH_PROPERTYGET, &result, pXlSheet, L"Range", 1, parm);
		VariantClear(&parm);

		pXlRange = result.pdispVal;
	}

	// �����ǵ�����������Range  
	AutoWrap(DISPATCH_PROPERTYPUT, NULL, pXlRange, L"Value", 1, arr);
	pXlRange->Release();
	
	// ���������Ǹñ����ļ��ˣ�����Worksheet.SaveAs()����(������������������в����������ļ���)  
	{
		VARIANT filename;
		filename.vt = VT_BSTR;
		filename.bstrVal = fileName.AllocSysString();
		AutoWrap(DISPATCH_METHOD, NULL, pXlSheet, L"SaveAs", 1, filename);
	}

	// �˳�������Application.Quit()����  
	AutoWrap(DISPATCH_METHOD, NULL, pXlApp, L"Quit", 0);

	// �ͷ����еĽӿ��Լ�����  
	pXlRange->Release();
	pXlSheet->Release();
	pXlBook->Release();
	pXlBooks->Release();
	pXlApp->Release();
	VariantClear(&arr);
}


void CTcpTestToolDlg::OnBnClickedClear()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_listLog.ResetContent();
}
