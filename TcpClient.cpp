#include "stdafx.h"
#include "TcpClient.h"
#include "TcpTestToolDlg.h"
#include "oomsFunc.h"
#define RECV_SZIE 4096

CTcpClient::CTcpClient():m_param(NULL)
{
	m_sock = socket(AF_INET, SOCK_STREAM, 0);
}


CTcpClient::~CTcpClient()
{
	closesocket(m_sock);
}

bool CTcpClient::ConnectServer(const char* ip, short port)
{
	SOCKADDR_IN ServerAddr = { 0 };

	//�ͻ���Ŀǰֻ����һ��,���ܶ�������
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_addr.s_addr = inet_addr(ip);
	ServerAddr.sin_port = htons(port);

	int nRet = connect(m_sock, (struct sockaddr*)&ServerAddr, sizeof(ServerAddr));
	if (nRet == SOCKET_ERROR)
	{
		return false;
	}
	else
	{
		return true;
	}

}

bool CTcpClient::SendDate(const char* buff, const int len)
{
	//�������˷���
	int nLeft, nWritten;
	const char* pBuf = buff;
	nLeft = len;

	while (nLeft > 0)
	{
		nWritten = send(m_sock,pBuf, nLeft,0);
		if (nWritten == SOCKET_ERROR)
			return false;
		::PostMessage(m_param->pThis->m_hWnd, WM_USER_SENDING, (WPARAM)m_param, nWritten);
		nLeft -= nWritten;
		pBuf += nWritten;
	}
	::GetLocalTime(&m_param->stSend);
	::PostMessage(m_param->pThis->m_hWnd, WM_USER_SENDONCE, (WPARAM)m_param, len);

	//׼�����ܱ���
	::PostMessage(m_param->pThis->m_hWnd, WM_USER_RECVING, (WPARAM)m_param, 0);

	char szRecValue[RECV_SZIE] = { 0 };
	int nRead = 0;
	std::string recvdata;
	//���շ��������ͻ���������(�÷���������, �ڴ˵ȴ������ݽ��յ��ż�������ִ��)
	while (1)
	{
		nRead = recv(m_sock,szRecValue,RECV_SZIE,0);
		if (nRead== SOCKET_ERROR)
		{
			::PostMessage(m_param->pThis->m_hWnd, WM_USER_RECVFAIL, (WPARAM)m_param, 0);
			return false;
		}
		::PostMessage(m_param->pThis->m_hWnd, WM_USER_RECVING, (WPARAM)m_param, nRead);
		std::string temp;
		temp.resize(recvdata.size() + nRead);
		memcpy((void*)temp.data(), recvdata.data(), recvdata.size());
		memcpy((void*)(temp.data() + recvdata.size()), szRecValue, nRead);
		recvdata.swap(temp);

		if (nRead < RECV_SZIE)
		{
			break;
		}
	}
	::GetLocalTime(&m_param->stRev);
	long timegap = CTcpTestToolDlg::TimeDiff(m_param->stRev, m_param->stSend);
	::PostMessage(m_param->pThis->m_hWnd, WM_USER_TIMEGAP, (WPARAM)m_param, timegap);
	::PostMessage(m_param->pThis->m_hWnd, WM_USER_RECVONCE, (WPARAM)m_param, recvdata.size());

	OnRecv(recvdata, recvdata.size());

	::PostMessage(m_param->pThis->m_hWnd, WM_USER_RECVED, (WPARAM)m_param, 0);
	return true;
}

bool CTcpClient::OnRecv(std::string & buff, int  length)
{
	//-----------------------------OOMS���������ƴ���-------------------
	//��������Լ�Э�����Ҫ�޸�����߼�
	//�����±���
	bool bRet = CheckPacket(buff.data(), length);
	if (!bRet)
	{
		::PostMessage(m_param->pThis->m_hWnd, WM_USER_RECVFAIL, (WPARAM)m_param, 0);
		return false;
	}
	//��ȡ���ݱ���
	std::string resjson = ExtractPacket(buff.data(), length);
	//����
	resjson = AES_EnDeCrypt::Decode(resjson.data(), resjson.size());
	//utf-8--->ansi
	resjson = Convert(resjson.c_str(), CP_UTF8, CP_ACP);
	//---------------------------OOMS���������ƴ���end-------------------
	if (m_param->index == 0)   //ֻ��ʾĳһ���ķ������ݣ����ֻ��ʾ�յ��ı��ĳ���
	{
		std::string* data = new std::string;
		*data = resjson;
		::PostMessage(m_param->pThis->m_hWnd, WM_USER_RECVDATA, (WPARAM)data, 0);  //�Է�ɾ������
	}
	return true;
}

void CTcpClient::SetParam(ThreadParam* param)
{
	m_param = param;
}

bool CTcpClient::SendYourDate(const char* buff, const int len, int cmd)
{
	//-----------------------------OOMS���������ƴ���-------------------
	//��������Լ�Э�����Ҫ�޸�����߼�
	//��װ�±���
	//res תutf-8
	std::string temp = buff;     //ooms��buff��һ��json�ַ���
	std::string req = Convert(temp.c_str(), CP_ACP, CP_UTF8);
	//res����
	req = AES_EnDeCrypt::Encode(req.data(), req.size());

	//׼������
	std::string respacket = BuildPacket(req.c_str(), req.length(), cmd);
	return SendDate(respacket.data(), respacket.length());
	//---------------------------OOMS���������ƴ���end-------------------
	
}

