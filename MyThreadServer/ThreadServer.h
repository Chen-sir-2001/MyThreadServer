/*�ض���
		���߳�+�����̵�С��Ŀ+����
		�����̷��������̣�
			1����ʼ�������
			2������socket
			3��bind    ip���˿ڣ�Э����
			4��listen   ����
			5��accept   ���տͻ��˵�����
			6��recv   and   send ����
			7���رնԵ��׽���
			8���ر��׽���
			9���ر������
		������Ĳ�������accept��Ҫ����������֪���ر��׽���     accept->�߳�1���߳�2���߳�3
		����ÿһ�����ߵĿͻ��ˣ�����˻���һ���߳�ȥά��
		���յ�����Ϣת����ȫ���ͻ���
		��ĳ���ͻ��˶Ͽ����ӣ���Ҫ����Ͽ������ӣ������߳�1��2��3��Ӧ�Ŀͻ���1��2��3  ����ͻ���2�����ˣ���ô��ʱ�ͻ���3�����߳�2*/

#pragma once

#ifndef __THREADSERVER_H
#define __THREADSERVER_H

#include <iostream>
#include <string>
#include <windows.h>
#include <process.h>
//#include <winsock2.h>
//#include <ws2tcpip.h>

#define MAX_CLNT 256  //���������
#define MAX_BUFF_SIZE 1024	// ����ͽ��յ����ݴ�С

// ��ʼ����
void StartServer();
//��ʼ���׽��ֿ�
int init_networkdir();
//�����׽���
int CreateSoke(SOCKET &socketServer, SOCKADDR_IN &addrServer);

//�߳�ִ�еĺ���
unsigned __stdcall HandleCln(void* arg);
// unsigned WINAPI HandleCln(void* arg);

// ���յ�����Ϣת���������ͻ���
void SendMsg(char* szMsg, int iLen);

#endif
// ����ʹ��git
