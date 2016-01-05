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

	char  sendData[DEFAULT_BUFLEN];// = "���Կͻ��˵����ݰ�.\n";

	char  * sayhello = "Hello ��ʼ��������.\n";
	iResult=sendto(sclient, sayhello, strlen(sayhello), 0, (sockaddr *)&sin, len);
	if (iResult > 0)
	{
		cout << "�ͻ��ˣ�Hello ��ʼ��������." << endl;
	}

	iResult = recvfrom(sclient, recvbuf, DEFAULT_BUFLEN, 0, (sockaddr *)&sin,&len);
	if (iResult > 0)
	{
		cout << "��������Hello ���������ѿ��������Է��ͣ�" << endl;
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
			printf("�����ļ�����  ");
			cin >> filename;
			filename[strlen(filename)] = '\0';
			sendto(sclient, filename, strlen(filename)+1, 0, (sockaddr *)&sin, len);//�����ļ���
			cout << "�ļ�������Ϊ" << strlen(filename) << endl;
			file = fopen(filename, "rb");
		}


		filelen = getFileSizeSystemCall(filename);
		cout << "���ݳ���Ϊ  " << filelen << endl;

		int invfilelen = htonl(filelen);
		cout << "���ݳ���ת��Ϊ�����ֽ���Ϊ  " << invfilelen << endl;

		sendto(sclient, (char *)(&invfilelen), 4, 0, (sockaddr *)&sin, len);//�����ļ�����

		int tosendlen = filelen;

		// ���ͻ������еĲ�������
		while (tosendlen > 0)
		{
			cout << "����" << tosendlen << "���ֽ���Ҫ����" << endl;
			fread(sendbuf, 1, DEFAULT_BUFLEN, file);
			int iSend = DEFAULT_BUFLEN;
			if (tosendlen < DEFAULT_BUFLEN) iSend = tosendlen;
			iResult = sendto(sclient, sendbuf, iSend, 0, (sockaddr *)&sin, len);
			printf("װ��ɹ���\n");

			if (iResult == SOCKET_ERROR) {
				printf("����ʧ�ܣ�������: %d\n", WSAGetLastError());
				closesocket(sclient);
				WSACleanup();
				return 1;
			}
			//printf("����ɹ�: %s(%ld)\n\n", sendbuf, iResult);
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