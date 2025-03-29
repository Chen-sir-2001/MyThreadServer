//客户端生成的exe文件，需要用命令行的方式启动， 需要传参

#include "MyThreadClient.h"

//默认名称
char szName[NAME_SIZE] = "[DEFAULT]";  
//定义消息变量
char szMsg[MAX_BUFF_SIZE];

// 开始函数
void StartClient() {
	std::cout << "ThreadClient start up!" << std::endl;
}

//线程发送函数
unsigned __stdcall SendMsg(void* arg)  //  或者  unsigned WINAPI SendMsg(); 
{
	SOCKET hClntSock = *((SOCKET*)arg);  // 先强制转换， 然后再取内容
	int iLen = 0;
	char szNameMsg[NAME_SIZE +MAX_BUFF_SIZE] = { 0 };  // 既要有名字，又要有消息

	while (1) {
		memset(szMsg, 0, MAX_BUFF_SIZE);  //全部请0
		fgets(szMsg, MAX_BUFF_SIZE,  stdin);  // 从控制台等待数据，阻塞；
		// 客户端下线
		if (!strcmp(szMsg, "Q\n") || !strcmp(szMsg,"Q\n")) {
			closesocket(hClntSock);
			exit(0);
		}
		//拿到消息后，拼接包，发送给服务器
		sprintf(szNameMsg,"%s %s", szName,szMsg);
		send(hClntSock, szNameMsg,strlen(szNameMsg), 0);
	}
}

//线程接收函数
unsigned __stdcall RecvMsg(void* arg) {
	//SOCKET hClntSock = *((SOCKET*)arg);
	//int iLen = 0;
	//char szNameMsg[NAME_SIZE + MAX_BUFF_SIZE] = { 0 };  // 既要有名字，又要有消息

	//while (1) {
	//	memset(szNameMsg, 0, NAME_SIZE + MAX_BUFF_SIZE);  //全部请0
	//	iLen = recv(hClntSock, szNameMsg, sizeof(szNameMsg), 0);
	//	if (iLen != -1) {
	//		// 将收到的消息转发给其他客户端
	//		return 2;
	//	}
	//	szNameMsg[iLen] = 0;
	//	fputs(szNameMsg,stdout);
	//}
	SOCKET hClntSock = *((SOCKET*)arg);
	int iLen = 0;
	char szNameMsg[NAME_SIZE + MAX_BUFF_SIZE] = { 0 };  // 既要有名字，又要有消息

	while (1) {
		memset(szMsg, 0, MAX_BUFF_SIZE);  // 全部清 0
		iLen = recv(hClntSock, szNameMsg, sizeof(szNameMsg) - 1, 0); // 防止溢出，最后一位留给 '\0'

		if (iLen > 0) {
			szNameMsg[iLen] = '\0'; // 确保字符串正确终止
			std::cout << szNameMsg; // 正确打印消息
		}
		else if (iLen == 0) {
			// 服务器关闭连接，退出
			std::cout << "服务器已断开连接，退出接收线程" << std::endl;
			break;
		}
		else {
			std::cerr << "recv 发生错误: " << WSAGetLastError() << std::endl;
			break;
		}
	}

	return 0;
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

int main(int argc, char* argv[]) {
	if (argc != 2) {
		std::cout << "必须输入两个参数,一个是文件名.exe,一个是名称" << std::endl
			<< "启动方式：exe文件目录下，shift+鼠标右键, 点击在此处打开powershll窗口" << std::endl
			<< "例如：./MyThreadClient.exe xiaoyige" << std::endl;
		system("pause");
		return -1;
	}
	sprintf(szName,"[%s]",argv[1]);

	HANDLE hSendThread,hRecvThread; // 两个线程，发送和接收线程
	StartClient();
	if (init_networkdir() != 0) {
		return -1;  // 退出
	}
	//建立socket
	SOCKET socketClient = socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == socketClient) {
		std::cout << "socket errorno  = " << GetLastError() << std::endl;
	}

	SOCKADDR_IN addrClient;
	addrClient.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	addrClient.sin_family = AF_INET;
	addrClient.sin_port = htons(8080);

	// 连接服务器
	if (connect(socketClient, (SOCKADDR*)&addrClient, sizeof(SOCKADDR))) {
		std::cout << "connect errorno = " << GetLastError() << std::endl;
		return -1;
	}

	//创建两个线程，一个发送数据，一个接收数据
	hSendThread = (HANDLE)_beginthreadex(nullptr,0,&SendMsg,(void*)&socketClient,0,nullptr);
	hRecvThread = (HANDLE)_beginthreadex(nullptr, 0, &RecvMsg, (void*)&socketClient, 0, nullptr);
	WaitForSingleObject(hRecvThread, INFINITE);
	WaitForSingleObject(hSendThread, INFINITE);

	closesocket(socketClient);
	WSACleanup();  // 清理 Winsock
	return 0;
}