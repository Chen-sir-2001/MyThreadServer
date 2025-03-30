/*必读：
		多线程+网络编程的小项目+队列
		网络编程服务器流程：
			1、初始化网络库
			2、创建socket
			3、bind    ip，端口，协议族
			4、listen   监听
			5、accept   接收客户端的连接
			6、recv   and   send 操作
			7、关闭对等套接字
			8、关闭套接字
			9、关闭网络库
		在上面的操作中在accept处要创建多任务，知道关闭套接字     accept->线程1，线程2，线程3
		对于每一个上线的客户端，服务端会起一个线程去维护
		将收到的消息转发给全部客户端
		当某个客户端断开连接，需要处理断开的连接，比如线程1、2、3对应的客户端1、2、3  假设客户端2下线了，那么这时客户端3改用线程2*/

#pragma once

#ifndef __THREADSERVER_H
#define __THREADSERVER_H

#include <iostream>
#include <string>
#include <windows.h>
#include <process.h>
//#include <winsock2.h>
//#include <ws2tcpip.h>

#define MAX_CLNT 256  //最大连接量
#define MAX_BUFF_SIZE 1024	// 最大发送接收的数据大小

// 开始函数
void StartServer();
//初始化套接字库
int init_networkdir();
//创建套接字
int CreateSoke(SOCKET &socketServer, SOCKADDR_IN &addrServer);

//线程执行的函数
unsigned __stdcall HandleCln(void* arg);
// unsigned WINAPI HandleCln(void* arg);

// 将收到的消息转发给其他客户端
void SendMsg(char* szMsg, int iLen);

#endif
// 尝试使用git
