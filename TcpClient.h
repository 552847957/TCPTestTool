#pragma once
#include <string>

//ͬ��tcp�⣬send֮��������ȴ���Ӧ��ֱ���յ����ݺ󷵻�
struct ThreadParam;
class CTcpClient
{
public:
	CTcpClient();
	~CTcpClient();

	void SetParam(ThreadParam* param);

	bool ConnectServer(const char* ip, short port);

	//�˽ӿڽ��ܶ��ƣ������Թ��߿��Բο��޸ģ���Ҫ������װ�����뷢�ͱ���
	//�ڲ���ص���SendDate����������������
	//���������Ĳ����Լ����Ͷ����ܶ��ƣ�������������ʹ�á�
	bool SendYourDate(const char* buff, const int len,int cmd);

	bool SendDate(const char* buff, const int len);

	//�˽ӿڽ��ܶ��ƣ������Թ��߿��Բο��޸ģ���Ҫ������ܱ������������
	//�����������޸ģ�һ������������ʾ���ܵ����ı�����
	bool OnRecv(std::string & buff, int length);


private:
	SOCKET m_sock;
	ThreadParam* m_param;
};

