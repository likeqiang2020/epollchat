#include "tcp_server.h"

TcpServer::TcpServer()
{
	m_listenfd = 0;
	m_clientfd = 0;
}


TcpServer::~TcpServer()
{
	if(m_listenfd != 0)	close(m_listenfd);
	if(m_clientfd != 0)	close(m_clientfd);
}


bool TcpServer::InitServer(int port)
{
	m_listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if(m_listenfd == -1)
	{
		perror("socket fail.\n");
		close(m_listenfd);
		m_listenfd = 0;
		return false;
	}

	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	int bind_ret = bind(m_listenfd, (const struct sockaddr *)&servaddr, sizeof(servaddr));
	if(bind_ret == -1)
	{
		perror("bind fail.\n");
		close(m_listenfd);
		m_listenfd = 0;
		return false;
	}

	int listend_ret = listen(m_listenfd, 5);
	if(listend_ret == -1)
	{
		perror("listed fail.\n");
		close(m_listenfd);
		m_listenfd = 0;
		return false;
	}

	return true;
}


bool TcpServer::Accept()
{
	m_clientfd = accept(m_listenfd, 0, 0);
	if(m_clientfd <= 0)
	{
		perror("accept fali.\n");
		return false;
	}
	return true;
}


int TcpServer::Send(const void *buf, const int buflen)
{
	return send(m_clientfd, buf, buflen, 0);
}


int TcpServer::Recv(void *buf, const int buflen)
{
	return recv(m_clientfd, buf, buflen, 0);
}


void TcpServer::CloseListen()
{
	if(m_listenfd != 0)
	{
		close(m_listenfd);
		m_listenfd = 0;
	}
}


void TcpServer::CloseClient()
{
	if(m_clientfd != 0)
	{
		close(m_clientfd);
		m_clientfd = 0;
	}
}
