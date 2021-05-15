#ifndef __TCPCLIENT_H_
#define __TCPCLIENT_H_

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>


class TcpClient
{
	public:
		int m_sockfd;

		TcpClient();
		~TcpClient();

		bool ConnectToServer(const char *serverIP, const int port);		// 连接服务端
		int Send(const void *buf, const int buflen);		// 向服务端发送数据
		int Recv(void *buf, const int buflen);					// 从服务端接收数据
};


#endif
