#include "ThreadServer.h"

SOCKET clnSocks[MAX_CLNT];  // ���е����ӵĿͻ��˵�socket����
int clntCnt = 0; // �ͻ������ӵĸ���
HANDLE hMutex;  // ������

// ��ʼ����
void StartServer() {
	std::cout << "ThreadServer start up!" << std::endl;
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

//�����׽���,�󶨼���
int CreateSoke(SOCKET& socketServer, SOCKADDR_IN& addrServer) {
	socketServer = socket(AF_INET, SOCK_STREAM, 0);

	addrServer.sin_addr.s_addr = htonl(INADDR_ANY);
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(8080);

	if (SOCKET_ERROR == bind(socketServer, (SOCKADDR*)&addrServer, sizeof(SOCKADDR))) {
		printf("bind errorno = %d\n", GetLastError());
		return -1;
	}

	if (SOCKET_ERROR == listen(socketServer, 5)) {
		printf("bind errorno = %d\n", GetLastError());
		return -1;
	}
	std::cout << "start lisent" << std::endl;
	return 0;
}

void SendMsg(char* szMsg, int iLen) {
	WaitForSingleObject(hMutex, INFINITE);
	int i = 0;
	for (i = 0; i < iLen; i++) {
		send(clnSocks[i],szMsg, iLen, 0);
	}
	ReleaseMutex(hMutex);
}

//�߳�ִ�еĺ���
unsigned __stdcall HandleCln(void* arg) {
	SOCKET hClntSock = *((SOCKET *)arg);  // ��ǿ��ת���� Ȼ����ȡ����
	int iLen = 0;
	char szMsg[MAX_BUFF_SIZE] = { 0 };
	while (1) {
		iLen = recv(hClntSock,szMsg,sizeof(szMsg),0);
		if (iLen != -1) {
			// ���յ�����Ϣת���������ͻ���
			SendMsg(szMsg,iLen);
		}
		else {
			break;
		}
	}
	std::cout << "��ʱ���ӵ���Ŀ�ǣ�" << clntCnt << std::endl;
	//�������ߵĹ���  ���ƶ��е��߼�
	//1��ȷ����һ����������
	WaitForSingleObject(hMutex, INFINITE);
	int i;
	for (i = 0; i < clntCnt; i++) {
		if (hClntSock == clnSocks[i]) {
			//�ҵ����ĸ����������ˡ�
			while (i++ < clntCnt) {
				clnSocks[i] = clnSocks[i + 1];
			}
			break;
		}
	}
	//�Ƴ��������-1
	clntCnt--;
	std::cout << "�Ͽ����Ӻ��������ǣ�"<< clntCnt << std::endl;
	ReleaseMutex(hMutex);
	closesocket(hClntSock);
	return 0;
}


int main() {
	StartServer();
	if (init_networkdir() == 1)  // ���������Ƿ��ʼ���ɹ�
		return 1;
	hMutex = CreateMutex(NULL,FALSE,NULL);
	//�����������׽���
	SOCKET socketServer;
	SOCKADDR_IN addrServer;
	/*SOCKET socketServer = socket(AF_INET, SOCK_STREAM, 0);
	SOCKADDR_IN addrServer;

	addrServer.sin_addr.s_addr = htonl(INADDR_ANY);
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(8080);
	
	if (SOCKET_ERROR == bind(socketServer, (SOCKADDR*)&addrServer, sizeof(SOCKADDR))) {
		printf("bind errorno = %d\n", GetLastError());
		return -1;
	}

	if (SOCKET_ERROR == listen(socketServer, 5)) {
		printf("bind errorno = %d\n", GetLastError());
		return -1;
	}
	std::cout << "start lisent" << std::endl;*/
	if (CreateSoke(socketServer, addrServer) != 0) {
		std::cout << "socket��ʼ������" << std::endl;
		return -1; // 
	}
	SOCKADDR_IN addrCli;
	int len = sizeof(SOCKADDR_IN);
	while (1) {
		//�������Կͻ��˵�����
		SOCKET sockConn = accept(socketServer,(SOCKADDR*)&addrCli, &len);
		//�������ȫ�ֱ�����Ҫ����
		WaitForSingleObject(hMutex,INFINITE);
		//û��һ�����ӣ���Ҫһ���߳�ȥά��
		clnSocks[clntCnt++] = sockConn;
		ReleaseMutex(hMutex);
		//clntCnt++; // ��Ŀ+1
		uintptr_t hTread = _beginthreadex(NULL, 0, &HandleCln, (void*)&sockConn, 0, NULL);
		std::cout << "Connect client IP = " << inet_ntoa(addrCli.sin_addr) <<
		         	"Num= " << clntCnt << std::endl;
	}
	closesocket(socketServer);
	WSACleanup();  // ���� Winsock
	return 0;
}