/**************************************************************************
> File Name:     client.cpp
> Author:        likeqiang
> mail:          likeqiang2020@gmail.com 
> Created Time:  2021年05月13日 星期四 17时33分23秒
> Description:   
*************************************************************************/

#include "common.h"


int main(int argc, char *argv[])
{
	// 服务端结构
	struct sockaddr_in server_addr;
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd == -1)
	{ perror("socket"); return -1; }

	if( connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) ==-1)
	{ perror("connect"); return -1; }

	int pipe_fd[2];												// 创建管道,[0]父进程读,[1]子进程写
	if(pipe(pipe_fd) < 0)
	{ perror("pipe"); return -1; }

	int epfd = epoll_create(EPOLL_SIZE);	// 创建epoll
	if(epfd < 0)
	{ perror("epoll"); return -1; }
	static struct epoll_event events[2];
	addFd(epfd, sockfd);
	addFd(epfd, pipe_fd[0]);

	bool isNormal_client = true;					// 客户端是否正常
	char message[BUF_SIZE];

	int pid = fork();
	if(pid < 0)
	{ perror("fork"); return -1; }

	if(pid == 0)													// 子进程只负责写	
	{
		close(pipe_fd[0]);

		while(isNormal_client) {
			fgets(message, BUF_SIZE, stdin);

			if(strncmp(message, "EXIT", strlen("EXIT")) == 0)
					isNormal_client = false;
			else {
				if(write(pipe_fd[1], message, strlen(message)) < 0)
				{ perror("write"); exit(-1); }
			}
		}
	}

	if(pid > 0)													 // 父进程只负责读
	{
		close(pipe_fd[1]);
		while(isNormal_client) {
			int event_read = epoll_wait(epfd, events, 2, -1); 

			for(int i = 0; i < event_read; ++i) {
				bzero(&message, sizeof(message));

				if(events[i].data.fd == sockfd) {
					int ret = recv(sockfd, message, BUF_SIZE, 0);
					if(ret == 0) {
						printf("server closed.\n");
						close(sockfd);
						isNormal_client = false;
					} else
						printf("%s", message);
				}	
				else {
					int ret = read(events[i].data.fd, message, BUF_SIZE);
					if(ret == 0)	isNormal_client = false;
					else	send(sockfd, message, BUF_SIZE, 0);
				}
			}
		}
	}

	if(pid)	{ close(pipe_fd[0]); close(sockfd); }
	else { close(pipe_fd[1]); }

	return 0;
}
