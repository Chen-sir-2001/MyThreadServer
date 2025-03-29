//�ͻ������ɵ�exe�ļ�����Ҫ�������еķ�ʽ������ ��Ҫ����

#include "MyThreadClient.h"

//Ĭ������
char szName[NAME_SIZE] = "[DEFAULT]";  
//������Ϣ����
char szMsg[MAX_BUFF_SIZE];

// ��ʼ����
void StartClient() {
	std::cout << "ThreadClient start up!" << std::endl;
}

//�̷߳��ͺ���
unsigned __stdcall SendMsg(void* arg)  //  ����  unsigned WINAPI SendMsg(); 
{
	SOCKET hClntSock = *((SOCKET*)arg);  // ��ǿ��ת���� Ȼ����ȡ����
	int iLen = 0;
	char szNameMsg[NAME_SIZE +MAX_BUFF_SIZE] = { 0 };  // ��Ҫ�����֣���Ҫ����Ϣ

	while (1) {
		memset(szMsg, 0, MAX_BUFF_SIZE);  //ȫ����0
		fgets(szMsg, MAX_BUFF_SIZE,  stdin);  // �ӿ���̨�ȴ����ݣ�������
		// �ͻ�������
		if (!strcmp(szMsg, "Q\n") || !strcmp(szMsg,"Q\n")) {
			closesocket(hClntSock);
			exit(0);
		}
		//�õ���Ϣ��ƴ�Ӱ������͸�������
		sprintf(szNameMsg,"%s %s", szName,szMsg);
		send(hClntSock, szNameMsg,strlen(szNameMsg), 0);
	}
}

//�߳̽��պ���
unsigned __stdcall RecvMsg(void* arg) {
	//SOCKET hClntSock = *((SOCKET*)arg);
	//int iLen = 0;
	//char szNameMsg[NAME_SIZE + MAX_BUFF_SIZE] = { 0 };  // ��Ҫ�����֣���Ҫ����Ϣ

	//while (1) {
	//	memset(szNameMsg, 0, NAME_SIZE + MAX_BUFF_SIZE);  //ȫ����0
	//	iLen = recv(hClntSock, szNameMsg, sizeof(szNameMsg), 0);
	//	if (iLen != -1) {
	//		// ���յ�����Ϣת���������ͻ���
	//		return 2;
	//	}
	//	szNameMsg[iLen] = 0;
	//	fputs(szNameMsg,stdout);
	//}
	SOCKET hClntSock = *((SOCKET*)arg);
	int iLen = 0;
	char szNameMsg[NAME_SIZE + MAX_BUFF_SIZE] = { 0 };  // ��Ҫ�����֣���Ҫ����Ϣ

	while (1) {
		memset(szMsg, 0, MAX_BUFF_SIZE);  // ȫ���� 0
		iLen = recv(hClntSock, szNameMsg, sizeof(szNameMsg) - 1, 0); // ��ֹ��������һλ���� '\0'

		if (iLen > 0) {
			szNameMsg[iLen] = '\0'; // ȷ���ַ�����ȷ��ֹ
			std::cout << szNameMsg; // ��ȷ��ӡ��Ϣ
		}
		else if (iLen == 0) {
			// �������ر����ӣ��˳�
			std::cout << "�������ѶϿ����ӣ��˳������߳�" << std::endl;
			break;
		}
		else {
			std::cerr << "recv ��������: " << WSAGetLastError() << std::endl;
			break;
		}
	}

	return 0;
}

//��ʼ�������
int init_networkdir() {
	
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		std::cerr << "Winsock initialization failed." << std::endl;
		return 1;
	}
	return 0;
}

int main(int argc, char* argv[]) {
	if (argc != 2) {
		std::cout << "����������������,һ�����ļ���.exe,һ��������" << std::endl
			<< "������ʽ��exe�ļ�Ŀ¼�£�shift+����Ҽ�, ����ڴ˴���powershll����" << std::endl
			<< "���磺./MyThreadClient.exe xiaoyige" << std::endl;
		system("pause");
		return -1;
	}
	sprintf(szName,"[%s]",argv[1]);

	HANDLE hSendThread,hRecvThread; // �����̣߳����ͺͽ����߳�
	StartClient();
	if (init_networkdir() != 0) {
		return -1;  // �˳�
	}
	//����socket
	SOCKET socketClient = socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == socketClient) {
		std::cout << "socket errorno  = " << GetLastError() << std::endl;
	}

	SOCKADDR_IN addrClient;
	addrClient.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	addrClient.sin_family = AF_INET;
	addrClient.sin_port = htons(8080);

	// ���ӷ�����
	if (connect(socketClient, (SOCKADDR*)&addrClient, sizeof(SOCKADDR))) {
		std::cout << "connect errorno = " << GetLastError() << std::endl;
		return -1;
	}

	//���������̣߳�һ���������ݣ�һ����������
	hSendThread = (HANDLE)_beginthreadex(nullptr,0,&SendMsg,(void*)&socketClient,0,nullptr);
	hRecvThread = (HANDLE)_beginthreadex(nullptr, 0, &RecvMsg, (void*)&socketClient, 0, nullptr);
	WaitForSingleObject(hRecvThread, INFINITE);
	WaitForSingleObject(hSendThread, INFINITE);

	closesocket(socketClient);
	WSACleanup();  // ���� Winsock
	return 0;
}