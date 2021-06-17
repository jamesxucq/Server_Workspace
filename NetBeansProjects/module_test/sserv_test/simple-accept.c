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

		/* ��ʼ����ÿ���������ϵ������շ� */
		bzero(buf, MAXBUF + 1);
		/* ���տͻ��˵���Ϣ */
		len = recv(new_fd, buf, MAXBUF, 0);
		if(len > 0) 
			printf("������Ϣ�ɹ�:'%s'����%d���ֽڵ�����\n", buf, len);
		else 
			printf("��Ϣ����ʧ�ܣ����������%d��������Ϣ��'%s'\n", errno, strerror(errno));

		bzero(buf, MAXBUF + 1);
		//strcpy(buf, "���������ӽ����ɹ�����ͻ��˷��͵ĵ�һ����Ϣ\nֻ����new_fd�����accept�����½�����socket����Ϣ��������sockfd������� socket������Ϣ������socket�����������ջ�����Ϣ\n");
		strcpy(buf, "Stpd/1.1 200 Ok\r\nMagicNum:MAXMAXMAX\r\nServHost:192.168.0.2\r\nServPort:8989\r\n\r\n");
		/* ����Ϣ���ͻ��� */
		len = send(new_fd, buf, strlen(buf), 0);
		if(len < 0) {
			printf("��Ϣ'%s'����ʧ�ܣ����������%d��������Ϣ��'%s'\n", buf, errno, strerror(errno));
		}
		else 
			printf("��Ϣ'%s'���ͳɹ�����������%d���ֽڣ�\n", buf, len);

		/* ����ÿ���������ϵ������շ����� */
	}

	close(sockfd);
	return 0;
}