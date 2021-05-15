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

	int pipe_fd[2];                       // 创建管道,[0]父进程读,[1]子进程写
	if(pipe(pipe_fd) < 0)
	{ perror("pipe"); return -1; }

	int epfd = epoll_create(2);  // 创建epoll,Linux 2.6.8后,参数只要大于0就行了
	if(epfd < 0)
	{ perror("epoll"); return -1; }
	static struct epoll_event events[2];
	addFd(epfd, sockfd);
	addFd(epfd, pipe_fd[0]);

	bool isNormal_client = true;          // 客户端是否正常
	char message[BUF_SIZE];

	pid_t pid = fork();
	if(pid < 0)
	{ perror("fork"); return -1; }

	if(pid == 0)                          // 子进程负责写	
	{
		close(pipe_fd[0]);

		while(isNormal_client) {
			fgets(message, BUF_SIZE, stdin); // 获取屏幕输入

			if(strncmp(message, "EXIT", strlen("EXIT")) == 0)
					isNormal_client = false;
			else {
				if(write(pipe_fd[1], message, strlen(message)) < 0)
				{ perror("write"); exit(-1); }
			}
		}
	}

	if(pid > 0)                           // 父进程负责读
	{
		close(pipe_fd[1]);
		while(isNormal_client) {
			int event_read = epoll_wait(epfd, events, 2, -1); // -1代表阻塞 

			for(int i = 0; i < event_read; ++i) {
				bzero(&message, sizeof(message));

				if(events[i].data.fd == sockfd) {	// 服务端发来消息
					int ret = recv(sockfd, message, BUF_SIZE, 0);
					if(ret == 0) {
						printf("server closed.\n");
						close(sockfd);
						isNormal_client = false;
					} else
						printf("%s", message);
				}	
				else {                            // 写事件触发
					int ret = read(events[i].data.fd, message, BUF_SIZE); // 获取管道消息
					if(ret == 0)	isNormal_client = false;
					else	send(sockfd, message, BUF_SIZE, 0);             // 发给服务端
				}
			} //for
		} //while
	} //if

	if(pid)	{ close(pipe_fd[0]); close(sockfd); }
	else { close(pipe_fd[1]); }

	return 0;
}
