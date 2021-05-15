/**************************************************************************
	> File Name:     common.h
	> Author:        likeqiang
	> mail:          likeqiang2020@gmail.com 
	> Created Time:  2021年05月13日 星期四 15时33分54秒
	> Description:   
 *************************************************************************/

#ifndef EPOLL_H_
#define EPOLL_H_ 

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SERVER_IP "127.0.0.1" 	// 服务端ip
#define SERVER_PORT 8888 				// 服务端port
#define EPOLL_SIZE  500 				// epoll大小
#define BUF_SIZE    1024	  		// 消息大小

/* 设置套接字为非阻塞 */
int setNonBlock(int fd)
{
	int flags = fcntl(fd, F_GETFL, 0);	// 获取当前描述符的属性
	flags |= O_NONBLOCK;
	fcntl(fd, F_SETFL, flags);
	return 0;
}


/* 将文件描述符添加到epoll树上 */
void addFd(int epollfd, int fd)
{
	struct epoll_event ev;
	ev.data.fd = fd;
	ev.events = EPOLLIN | EPOLLET;		// 设置为边缘触发
	epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
	setNonBlock(fd);
}

#endif
