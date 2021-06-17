#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <resolv.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAXBUF 1024

int main(int argc, char **argv)
{
    int sockfd, len;
    struct sockaddr_in dest;
    char buffer[MAXBUF + 1];

    if (argc != 3) {
        printf("xxxxxxxxxxx\n\t\t%s IP\n\t:\t%s 127.0.0.1 80\nIP MAXBUF\n", argv[0], argv[0]);
        exit(0);
    }

	/* ����һ�� socket ���� tcp ͨ�� */
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Socket");
		exit(errno);
	}
	printf("socket created\n");

	/* ��ʼ���������ˣ��Է����ĵ�ַ�Ͷ˿���Ϣ */
	bzero(&dest, sizeof(dest));
	dest.sin_family = AF_INET;
	dest.sin_port = htons(atoi(argv[2]));
	if (inet_aton(argv[1], (struct in_addr *) &dest.sin_addr.s_addr) == 0) {
		perror(argv[1]);
		exit(errno);
	}
	printf("address created\n");

	/* ���ӷ����� */
	if (connect(sockfd, (struct sockaddr *) &dest, sizeof(dest)) != 0) {
		perror("Connect ");
		exit(errno);
	}
	printf("server connected\n");

	bzero(buffer, MAXBUF + 1);
	//strcpy(buffer, "���ǿͻ��˷���������˵���Ϣ\n");
	strcpy(buffer, "Init Stpd/1.1\r\nUsrName:xujiecq@163.com\r\nPassWord:123456\r\nMagicNum:MAXMAXMAX\r\n\r\n");
	/* ����Ϣ������� */
	len = send(sockfd, buffer, strlen(buffer), 0);
	if(len < 0) 
		printf("��Ϣ'%s'����ʧ�ܣ����������%d��������Ϣ��'%s'\n", buffer, errno, strerror(errno));
	else 
		printf("��Ϣ'%s'���ͳɹ�����������%d���ֽڣ�\n", buffer, len);

	/* ���նԷ�����������Ϣ�������� MAXBUF ���ֽ� */
	bzero(buffer, MAXBUF + 1);
	/* ���շ�����������Ϣ */
	len = recv(sockfd, buffer, MAXBUF, 0);
	if(len > 0) 
		printf("������Ϣ�ɹ�:'%s'����%d���ֽڵ����\n", buffer, len);
	else 
		printf("��Ϣ����ʧ�ܣ����������%d��������Ϣ��'%s'\n", errno, strerror(errno));

	/* �ر����� */
	close(sockfd);
	return 0;
}


