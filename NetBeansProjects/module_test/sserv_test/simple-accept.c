#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAXBUF 1024

int main(int argc, char **argv)
{
	int sockfd, new_fd;
	socklen_t len;
	struct sockaddr_in my_addr, their_addr;
	unsigned int myport, lisnum;
	char buf[MAXBUF + 1];

	if (argv[1])
		myport = atoi(argv[1]);
	else
		myport = 8080;

	if (argv[2])
		lisnum = atoi(argv[2]);
	else
		lisnum = 2;

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}
	else printf("socket created\n");

	bzero(&my_addr, sizeof(my_addr));
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(myport);
	if(argv[3]) 
		my_addr.sin_addr.s_addr = inet_addr(argv[3]);
	else 
		my_addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(sockfd, (struct sockaddr *) &my_addr, sizeof(struct sockaddr)) == -1) {
		perror("bind");
		exit(1);
	}
	else printf("binded\n");

	if (listen(sockfd, lisnum) == -1) {
		perror("listen");
		exit(1);
	}
	else printf("begin listen\n");

	while(1) {
		len = sizeof(struct sockaddr);
		if ((new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &len)) == -1) {
			perror("accept");
			exit(errno);
		}
		else printf("server: got connection from %s, port %d, socket %d\n",inet_ntoa(their_addr.sin_addr), ntohs(their_addr.sin_port), new_fd);

		/* 开始处理每个新连接上的数据收发 */
		bzero(buf, MAXBUF + 1);
		/* 接收客户端的消息 */
		len = recv(new_fd, buf, MAXBUF, 0);
		if(len > 0) 
			printf("接收消息成功:'%s'，共%d个字节的数据\n", buf, len);
		else 
			printf("消息接收失败！错误代码是%d，错误信息是'%s'\n", errno, strerror(errno));

		bzero(buf, MAXBUF + 1);
		//strcpy(buf, "这是在连接建立成功后向客户端发送的第一个消息\n只能向new_fd这个用accept函数新建立的socket发消息，不能向sockfd这个监听 socket发送消息，监听socket不能用来接收或发送消息\n");
		strcpy(buf, "Stpd/1.1 200 Ok\r\nMagicNum:MAXMAXMAX\r\nServHost:192.168.0.2\r\nServPort:8989\r\n\r\n");
		/* 发消息给客户端 */
		len = send(new_fd, buf, strlen(buf), 0);
		if(len < 0) {
			printf("消息'%s'发送失败！错误代码是%d，错误信息是'%s'\n", buf, errno, strerror(errno));
		}
		else 
			printf("消息'%s'发送成功，共发送了%d个字节！\n", buf, len);

		/* 处理每个新连接上的数据收发结束 */
	}

	close(sockfd);
	return 0;
}