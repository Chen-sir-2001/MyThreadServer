/*客户端要做的事情，
1、 请求上线，连接
2、客户端等待服务端的消息
3、等待用户自己的关闭，下线
4、客户端中也有多线程，发送线程，和接收线程
*/ 

#pragma once
#include <iostream>
#include <string>
#include <windows.h>
#include <process.h>

#pragma comment(lib,"ws2_32.lib")
#define NAME_SIZE 256  //最大连接量
#define MAX_BUFF_SIZE 1024	// 最大发送接收的数据大小

// 开始函数
void StartClient();
//初始化套接字库
int init_networkdir();

//线程发送函数
unsigned __stdcall SendMsg(void *arg);  //  或者  unsigned WINAPI SendMsg(); 

//线程接收函数
unsigned __stdcall RecvMsg(void* arg);