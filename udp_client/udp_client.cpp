#include "stdafx.h"  
#define _WINSOCK_DEPRECATED_NO_WARNINGS 
#pragma warning (disable:4996)
#include <stdio.h>  
#include <winsock2.h>  
#include <iostream>
#define  DEFAULT_BUFLEN 1024
using namespace std;

#pragma comment(lib, "ws2_32.lib")   

int getFileSizeSystemCall(char * strFileName)
{
	struct stat temp;
	stat(strFileName, &temp);
	return temp.st_size;
}

int main(int argc, char* argv[])
{
	WSADATA wsaData;
	WORD socketVersion = MAKEWORD(2, 2);
	if (WSAStartup(socketVersion, &wsaData) != 0)
	{
		return 0;
	}
	SOCKET sclient = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(8888);
	sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	int len = sizeof(sin);
	int iResult;
	char sendbuf[DEFAULT_BUFLEN];
	char recvbuf[DEFAULT_BUFLEN];

	char  sendData[DEFAULT_BUFLEN];// = "来自客户端的数据包.\n";

	char  * sayhello = "Hello 开始传送数据.\n";
	iResult=sendto(sclient, sayhello, strlen(sayhello), 0, (sockaddr *)&sin, len);
	if (iResult > 0)
	{
		cout << "客户端：Hello 开始传送数据." << endl;
	}

	iResult = recvfrom(sclient, recvbuf, DEFAULT_BUFLEN, 0, (sockaddr *)&sin,&len);
	if (iResult > 0)
	{
		cout << "服务器：Hello 数据连接已开启，可以发送！" << endl;
	}

	while (1)
	{
		FILE *file = NULL;
		char filename[100];
		int filelen = 0;
		char buf[1];
		while (file == NULL)
		{
			memset(filename, 0, strlen(filename));
			memset(buf, 0, strlen(buf));
			printf("输入文件名：  ");
			cin >> filename;
			filename[strlen(filename)] = '\0';
			sendto(sclient, filename, strlen(filename)+1, 0, (sockaddr *)&sin, len);//发送文件名
			cout << "文件名长度为" << strlen(filename) << endl;
			file = fopen(filename, "rb");
		}


		filelen = getFileSizeSystemCall(filename);
		cout << "数据长度为  " << filelen << endl;

		int invfilelen = htonl(filelen);
		cout << "数据长度转换为网络字节序为  " << invfilelen << endl;

		sendto(sclient, (char *)(&invfilelen), 4, 0, (sockaddr *)&sin, len);//发送文件长度

		int tosendlen = filelen;

		// 发送缓冲区中的测试数据
		while (tosendlen > 0)
		{
			cout << "还有" << tosendlen << "个字节需要发送" << endl;
			fread(sendbuf, 1, DEFAULT_BUFLEN, file);
			int iSend = DEFAULT_BUFLEN;
			if (tosendlen < DEFAULT_BUFLEN) iSend = tosendlen;
			iResult = sendto(sclient, sendbuf, iSend, 0, (sockaddr *)&sin, len);
			printf("装填成功！\n");

			if (iResult == SOCKET_ERROR) {
				printf("发送失败！错误编号: %d\n", WSAGetLastError());
				closesocket(sclient);
				WSACleanup();
				return 1;
			}
			//printf("发射成功: %s(%ld)\n\n", sendbuf, iResult);
			tosendlen -= iResult;
		}

		fclose(file);

		char recvData[255];
		int ret = recvfrom(sclient, recvData, 255, 0, (sockaddr *)&sin, &len);
		if (ret > 0)
		{
			recvData[ret] = 0x00;
			printf(recvData);
		}
	}

	closesocket(sclient);
	WSACleanup();
	return 0;
}