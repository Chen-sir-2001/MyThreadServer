/*�ͻ���Ҫ�������飬
1�� �������ߣ�����
2���ͻ��˵ȴ�����˵���Ϣ
3���ȴ��û��Լ��Ĺرգ�����
4���ͻ�����Ҳ�ж��̣߳������̣߳��ͽ����߳�
*/ 

#pragma once
#include <iostream>
#include <string>
#include <windows.h>
#include <process.h>

#pragma comment(lib,"ws2_32.lib")
#define NAME_SIZE 256  //���������
#define MAX_BUFF_SIZE 1024	// ����ͽ��յ����ݴ�С

// ��ʼ����
void StartClient();
//��ʼ���׽��ֿ�
int init_networkdir();

//�̷߳��ͺ���
unsigned __stdcall SendMsg(void *arg);  //  ����  unsigned WINAPI SendMsg(); 

//�߳̽��պ���
unsigned __stdcall RecvMsg(void* arg);