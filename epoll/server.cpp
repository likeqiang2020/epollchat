/**************************************************************************
> File Name:     server.cpp
> Author:        likeqiang
> mail:          likeqiang2020@gmail.com 
> Created Time:  2021年05月13日 星期四 16时29分38秒
> Description:   
*************************************************************************/

#include "common.h"
#include <iostream>
#include <list>

using namespace std;


/* 链表保存客户端fd */
list<int> clients_list;


/* 发送广播 */
int sendBroadcast(int cfd)
{
	char buf[BUF_SIZE];									// 接收消息
	char message[BUF_SIZE];							// 显示消息
	const char say[] = " : ";

	bzero(buf, BUF_SIZE);
	bzero(message, BUF_SIZE);

	// 接收消息
	printf("read from client[%d]\n", cfd);
	ssize_t len = recv(cfd, buf, BUF_SIZE, 0);

	if(len == 0 || len == -1)         	// 对端关闭或出错
	{
		printf("[%d] closed.\nNow clients:[%d]\n", cfd, (int)clients_list.size()- 1);
		clients_list.remove(cfd);
		close(cfd);
	}
	else				 												// 正常接收
	{
		if(clients_list.size() == 1) {			// 如果房间内只有自己 
			char onlyYou[] = "目前房间内只有自己!\n";
			send(cfd, onlyYou, strlen(onlyYou), 0);
			return len;
		}
		sprintf(message, "%d : ", cfd);
		strncat(message, buf, strlen(buf) + 1);
		write(STDOUT_FILENO, message, strlen(message) + 1);

		list<int>::iterator it;
		for(it = clients_list.begin(); it != clients_list.end(); ++it) {
			if(*it != cfd) {								// 广播给其他客户端
				if( send(*it, message, BUF_SIZE, 0) < 0)
				{ perror("send"); return -1; }
			}
		}
	}
	return len;
}


int main(int argc, char *argv[])
{
	// 服务端地址结构
	struct sockaddr_in server_addr;
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	// 客户端地址结构
	struct sockaddr_in client_addr;
	socklen_t client_addr_len = sizeof(client_addr);
	bzero(&client_addr, client_addr_len);

	int listenfd = socket(AF_INET, SOCK_STREAM, 0);			// 服务端套接字
	if(listenfd == -1)
	{ perror("socket"); return -1; }

	if( bind(listenfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)	
	{ perror("bind"); return -1; }

	if( listen(listenfd, 10) == -1)
	{ perror("listen"); return -1; }

	int epfd = epoll_create(EPOLL_SIZE);								// 创建epoll树根
	if(epfd < 0)
	{ perror("epoll_create"); return -1; }

	struct epoll_event events[EPOLL_SIZE];							// epoll表
	addFd(epfd, listenfd);															// 先将listenfd挂树上

	char welcome[] = "Welcome you! Your chat ID is: [%d]\nYou can use 'EXIT' to exit.\n";	// 客户端连接后,服务器先发送一条欢迎信息
	while(1)
	{
		// epoll表中有多少触发事件
		int event_read = epoll_wait(epfd, events, EPOLL_SIZE, -1);
		if(event_read < 0)
		{ perror("epoll_wait"); break; }

		for(int i = 0; i < event_read; ++i) {
			int sockfd = events[i].data.fd;

			if(sockfd == listenfd) {												// 客户端连接请求
				int clientfd = accept(listenfd, (struct sockaddr *)&client_addr, &client_addr_len);
				if(clientfd == -1)
				{ perror("accept"); break; }
				
				addFd(epfd, clientfd);												// 把这个cfd挂树上
				clients_list.push_back(clientfd);							// 加入到客户端list
				printf("Client [%s] connected.	Now clients[%d]\n", inet_ntoa(client_addr.sin_addr), (int)(clients_list.size()) - 1);	// 打印客户端IP地址

				char message[BUF_SIZE];
				bzero(message, BUF_SIZE);
				sprintf(message, welcome, clientfd);					// 打印欢迎信息
				if( send(clientfd, message, BUF_SIZE, 0) < 0)
				{ perror("send"); return -1; }
			}
			else {																					// 广播
				if( sendBroadcast(sockfd) < 0)
				{ perror("sendBroadcast"); return -1; }
			}
		}
	}

	close(listenfd);
	close(epfd);
	return 0;
}
