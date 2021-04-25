#include "tcp_client.h"


TcpClient::TcpClient()
{
	m_sockfd = 0;
}


TcpClient::~TcpClient()
{
	if(m_sockfd != 0)
	{
		close(m_sockfd);
		m_sockfd = 0;
	}
}


bool TcpClient::ConnectToServer(const char *serverIP, const int port)
{
	m_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	struct hostent *h;
	h = gethostbyname(serverIP);
	if(h == 0)
	{
		close(m_sockfd);
		m_sockfd = 0;
		return false;
	}

	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	memcpy(&servaddr.sin_addr, h->h_addr, h->h_length);

	int ret = connect(m_sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
	if(ret != 0)
	{
		close(m_sockfd);
		m_sockfd = 0;
		return false;
	}

	return true;
}


int TcpClient::Send(const void *buf, const int buflen)
{
	return send(m_sockfd, buf, buflen, 0);
}


int TcpClient::Recv(void *buf, const int buflen)
{
	return recv(m_sockfd, buf, buflen, 0);
}
