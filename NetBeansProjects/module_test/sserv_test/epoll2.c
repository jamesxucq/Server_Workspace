#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define MAXLINE 16
#define OPEN_MAX 100
#define LISTENQ 20
#define SERV_PORT 5555
#define INFTIM 1000

void setnonblocking(int sock)
{
     int opts;

     opts=fcntl(sock,F_GETFL);
     if(opts<0) {
          perror("fcntl(sock,GETFL)");
          return;
     }

     opts = opts|O_NONBLOCK;
     if(fcntl(sock,F_SETFL,opts)<0) {
          perror("fcntl(sock,SETFL,opts)");
          return;
     }   
}

int main()
{
int iIndex;
     int i, maxi, listenfd, connfd, sockfd,epfd,nfds;
     ssize_t n;
     char line[MAXLINE];
     socklen_t clilen;
     //����epoll_event�ṹ��ı���,ev����ע���¼�,�������ڻش�Ҫ������¼�
     struct epoll_event ev,events[20];

	 //�������ڴ���accept��epollר�õ��ļ�������
     epfd=epoll_create(256);

     struct sockaddr_in clientaddr;
     struct sockaddr_in serveraddr;

     listenfd = socket(AF_INET, SOCK_STREAM, 0);

     //��socket����Ϊ��������ʽ
     setnonblocking(listenfd);

     //������Ҫ������¼���ص��ļ�������
     ev.data.fd=listenfd;

     //����Ҫ������¼�����
     ev.events=EPOLLIN|EPOLLET;

     //ע��epoll�¼�
     epoll_ctl(epfd,EPOLL_CTL_ADD,listenfd,&ev);

     bzero(&serveraddr, sizeof(serveraddr));
     serveraddr.sin_family = AF_INET;
	 serveraddr.sin_addr.s_addr = INADDR_ANY;
     //char *local_addr="200.200.200.204";
     //inet_aton(local_addr,&(serveraddr.sin_addr));//htons(SERV_PORT);
     serveraddr.sin_port=htons(SERV_PORT);

     if(bind(listenfd,(const struct sockaddr*)&serveraddr, sizeof(serveraddr)) == -1){
		printf("bind error!!!\n");
		return 0;
	 }

     if(listen(listenfd, LISTENQ) == -1){
		printf("listen error!!!\n");
		return 0;
	 }


     maxi = 0; 
iIndex = 0;
     for ( ; ; ) {
          //�ȴ�epoll�¼��ķ���
          nfds=epoll_wait(epfd,events,20,500);

          //�����������������¼�     
          for(i=0;i<nfds;++i)
          {
               if(events[i].data.fd==listenfd)
               {
                    connfd = accept(listenfd,(struct sockaddr *)&clientaddr, &clilen);
                    if(connfd<0){
                         perror("connfd<0");
                         exit(1);
                    }

                    setnonblocking(connfd);
                    char *str = inet_ntoa(clientaddr.sin_addr);
                    printf("connect from \n");

                    //�������ڶ��������ļ�������
                    ev.data.fd=connfd;

                    //��������ע��Ķ������¼�
                    ev.events=EPOLLIN;

                    //ע��ev
                    epoll_ctl(epfd,EPOLL_CTL_ADD,connfd,&ev);
               }

               else if(events[i].events&EPOLLIN)
               {
					bzero(line, MAXLINE);
                    if ( (sockfd = events[i].data.fd) < 0) continue;
                    if ( (n = read(sockfd, line, MAXLINE)) < 0) {
                         if (errno == ECONNRESET) {
                              close(sockfd);
                              events[i].data.fd = -1;
                         } else
                              printf("readline error\n");
                    } else if (n == 0) {
                         close(sockfd);
                         events[i].data.fd = -1;
                    }
printf("recv:%s", line);
iIndex++;
if(iIndex >= 5){
                    //��������д�������ļ�������
                    ev.data.fd=sockfd;

                    //��������ע���д�����¼�
                    ev.events=EPOLLOUT;

                    //�޸�sockfd��Ҫ������¼�ΪEPOLLOUT
                    epoll_ctl(epfd,EPOLL_CTL_MOD,sockfd,&ev);
iIndex = 0;
}
               }
               else if(events[i].events&EPOLLOUT)
               {   
					sprintf(line, "hello world!!!\n");
					n = strlen(line);
                    sockfd = events[i].data.fd;
                    write(sockfd, line, n);
printf("%d send:%s", iIndex, line);
iIndex++;
if(iIndex >= 5){
                    //�������ڶ��������ļ�������
                    ev.data.fd=sockfd;

                    //��������ע��Ķ������¼�
                    ev.events=EPOLLIN;

                    //�޸�sockfd��Ҫ������¼�ΪEPOLIN
                    epoll_ctl(epfd,EPOLL_CTL_MOD,sockfd,&ev);
iIndex = 0;
}
               }
          }
     }
}