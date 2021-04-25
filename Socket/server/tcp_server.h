#ifndef __SERVERCLA_H_
#define __SERVERCLA_H_

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>


class TcpServer
{
	public:
		int m_listenfd;		// 监听
		int m_clientfd;		// 客户端socket

		TcpServer();
		~TcpServer();

		bool InitServer(int port);		// 初始化服务端
		bool Accept();							  // 等待客户端链接

		int Send(const void *buf, const int buflen);		// 发送数据
		int Recv(void *buf, const int buflen);					// 接收数据

		void CloseListen();		// 关闭服务端的监听
		void CloseClient();		// 关闭客户端socket
};

#endif
