#include "ThreadServer.h"

SOCKET clnSocks[MAX_CLNT];  // 所有的连接的客户端的socket的组
int clntCnt = 0; // 客户端连接的个数
HANDLE hMutex;  // 互斥量

// 开始函数
void StartServer() {
	std::cout << "ThreadServer start up!" << std::endl;
}

//初始化网络库
int init_networkdir() {
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		std::cerr << "Winsock initialization failed." << std::endl;
		return 1;
	}
	return 0;
}

//创建套接字,绑定监听
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

//线程执行的函数
unsigned __stdcall HandleCln(void* arg) {
	SOCKET hClntSock = *((SOCKET *)arg);  // 先强制转换， 然后再取内容
	int iLen = 0;
	char szMsg[MAX_BUFF_SIZE] = { 0 };
	while (1) {
		iLen = recv(hClntSock,szMsg,sizeof(szMsg),0);
		if (iLen != -1) {
			// 将收到的消息转发给其他客户端
			SendMsg(szMsg,iLen);
		}
		else {
			break;
		}
	}
	std::cout << "此时连接的数目是：" << clntCnt << std::endl;
	//处理下线的过程  类似队列的逻辑
	//1、确定哪一个连接下线
	WaitForSingleObject(hMutex, INFINITE);
	int i;
	for (i = 0; i < clntCnt; i++) {
		if (hClntSock == clnSocks[i]) {
			//找到是哪个连接下线了。
			while (i++ < clntCnt) {
				clnSocks[i] = clnSocks[i + 1];
			}
			break;
		}
	}
	//移除完后总数-1
	clntCnt--;
	std::cout << "断开连接后，连接数是："<< clntCnt << std::endl;
	ReleaseMutex(hMutex);
	closesocket(hClntSock);
	return 0;
}


int main() {
	StartServer();
	if (init_networkdir() == 1)  // 检测网络库是否初始化成功
		return 1;
	hMutex = CreateMutex(NULL,FALSE,NULL);
	//创建服务器套接字
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
		std::cout << "socket初始化错误" << std::endl;
		return -1; // 
	}
	SOCKADDR_IN addrCli;
	int len = sizeof(SOCKADDR_IN);
	while (1) {
		//接收来自客户端的连接
		SOCKET sockConn = accept(socketServer,(SOCKADDR*)&addrCli, &len);
		//下面操作全局变量，要加锁
		WaitForSingleObject(hMutex,INFINITE);
		//没来一个连接，需要一个线程去维护
		clnSocks[clntCnt++] = sockConn;
		ReleaseMutex(hMutex);
		//clntCnt++; // 数目+1
		uintptr_t hTread = _beginthreadex(NULL, 0, &HandleCln, (void*)&sockConn, 0, NULL);
		std::cout << "Connect client IP = " << inet_ntoa(addrCli.sin_addr) <<
		         	"Num= " << clntCnt << std::endl;
	}
	closesocket(socketServer);
	WSACleanup();  // 清理 Winsock
	return 0;
}