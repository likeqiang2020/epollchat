#include "tcp_client.h"


int main()
{
	TcpClient tclient;

	if(tclient.ConnectToServer("192.168.43.172", 5000) == false)
	{
		printf("Connect 192.168.43.172 fail.\n");
		return -1;
	}

	char strbuffer[1024];
	while(1)
	{
		memset(strbuffer, 0, sizeof(strbuffer));
		printf("你：");
		fgets(strbuffer, sizeof(strbuffer), stdin);
		if(strncmp(strbuffer, "QUIT", 4) == 0)	break;
		if(tclient.Send(strbuffer, sizeof(strbuffer)) <= 0)
		{
			printf("发送失败.\n");
			break;
		}	

		memset(strbuffer, 0, sizeof(strbuffer));
		if(tclient.Recv(strbuffer, sizeof(strbuffer)) <= 0)	
		{
			printf("接收失败.\n");
			break;
		}
		printf("他：%s\n", strbuffer);
	}

	close(tclient.m_sockfd);
}
