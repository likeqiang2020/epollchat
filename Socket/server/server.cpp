#include "tcp_server.h"

int main()
{
	signal(SIGCHLD, SIG_IGN);

	TcpServer tserver;
	if((tserver.InitServer(5000)) == false)
	{
		printf("server initialize fail.\n");
		return -1;
	}

	while(1)
	{
		if(tserver.Accept() == false)
		{
			printf("Accet fail.\n");
			continue;
		}
		if(fork() > 0)
		{
			tserver.CloseClient();
			continue;
		}
	

	tserver.CloseListen();
	printf("客户端连接.\n");

	char strbuffer[1024];
	while(1)
	{
		memset(strbuffer, 0, sizeof(strbuffer));
		if(tserver.Recv(strbuffer, sizeof(strbuffer)) <= 0)	
		{
			printf("接收失败.\n");
			break;
		}
		printf("他：%s\n", strbuffer);
		
		memset(strbuffer, 0, sizeof(strbuffer));
		printf("你：");
		fgets(strbuffer, sizeof(strbuffer), stdin);
		if (tserver.Send(strbuffer, strlen(strbuffer)) <= 0)
		{
			printf("发送失败.\n");
			break;
		}
	}

	printf("客户端已断开连接.\n");

		return 0;
	}
}
