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
     //声明epoll_event结构体的变量,ev用于注册事件,数组用于回传要处理的事件
     struct epoll_event ev,events[20];

	 //生成用于处理accept的epoll专用的文件描述符
     epfd=epoll_create(256);

     struct sockaddr_in clientaddr;
     struct sockaddr_in serveraddr;

     listenfd = socket(AF_INET, SOCK_STREAM, 0);

     //把socket设置为非阻塞方式
     setnonblocking(listenfd);

     //设置与要处理的事件相关的文件描述符
     ev.data.fd=listenfd;

     //设置要处理的事件类型
     ev.events=EPOLLIN|EPOLLET;

     //注册epoll事件
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
          //等待epoll事件的发生
          nfds=epoll_wait(epfd,events,20,500);

          //处理所发生的所有事件     
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

                    //设置用于读操作的文件描述符
                    ev.data.fd=connfd;

                    //设置用于注测的读操作事件
                    ev.events=EPOLLIN;

                    //注册ev
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
                    //设置用于写操作的文件描述符
                    ev.data.fd=sockfd;

                    //设置用于注测的写操作事件
                    ev.events=EPOLLOUT;

                    //修改sockfd上要处理的事件为EPOLLOUT
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
                    //设置用于读操作的文件描述符
                    ev.data.fd=sockfd;

                    //设置用于注测的读操作事件
                    ev.events=EPOLLIN;

                    //修改sockfd上要处理的事件为EPOLIN
                    epoll_ctl(epfd,EPOLL_CTL_MOD,sockfd,&ev);
iIndex = 0;
}
               }
          }
     }
}