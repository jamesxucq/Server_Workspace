#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <resolv.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAXBUF 16

int main(int argc, char **argv)
{
	int sockfd, len;
	struct sockaddr_in dest;
	char buffer[MAXBUF + 1];

	if (argc != 3) {
		printf
			("参数格式错误！正确用法如下：\n\t\t%s IP地址 端口\n\t比如:\t%s 127.0.0.1 80\n此程序用来从某个 IP 地址的服务器某个端口接收最多 MAXBUF 个字节的消息\n",
			argv[0], argv[0]);
			exit(0);
	}

	/* 创建一个 socket 用于 tcp 通信 */
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Socket");
		exit(errno);
	}
	printf("socket created\n");

	/* 初始化服务器端（对方）的地址和端口信息 */
	bzero(&dest, sizeof(dest));
	dest.sin_family = AF_INET;
	dest.sin_port = htons(atoi(argv[2]));
	if (inet_aton(argv[1], (struct in_addr *) &dest.sin_addr.s_addr) == 0) {
		perror(argv[1]);
		exit(errno);
	}
	printf("address created\n");

	/* 连接服务器 */
	if (connect(sockfd, (struct sockaddr *) &dest, sizeof(dest)) != 0) {
		perror("Connect ");
		exit(errno);
	}
	printf("server connected\n");

int iIndex;
for(iIndex = 0; iIndex < 5; iIndex++){
	bzero(buffer, MAXBUF + 1);
	//strcpy(buffer, "这是客户端发给服务器端的消息\n");
	sprintf(buffer, "Init Stpd/1.1:%d\n", iIndex);
	//strcpy(buffer, "Init Stpd/1.1\r\n");
	/* 发消息给服务器 */
	len = send(sockfd, buffer, strlen(buffer), 0);
	if(len < 0) 
		printf("消息'%s'发送失败！错误代码是%d，错误信息是'%s'\n", buffer, errno, strerror(errno));
	else 
		printf("消息'%s'发送成功，共发送了%d个字节！\n", buffer, len);
sleep(3000);
}

printf("xxxxxxxxxxxxxx\n");

for(iIndex = 0; iIndex < 5; iIndex++){
	/* 接收对方发过来的消息，最多接收 MAXBUF 个字节 */
	bzero(buffer, MAXBUF + 1);
	/* 接收服务器来的消息 */
	len = recv(sockfd, buffer, MAXBUF, 0);
	if(len > 0) 
		printf("接收消息成功:'%s'，共%d个字节的数据\n", buffer, len);
	else 
		printf("消息接收失败！错误代码是%d，错误信息是'%s'\n", errno, strerror(errno));
printf("iIndex:%d\n", iIndex);
sleep(3000);
}

	/* 关闭连接 */
	close(sockfd);
	return 0;
}


