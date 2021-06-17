//===============================================================================================
//工程名称：飞鸽传书
//功能描述： 用ipmsg协议实现网络聊天，用户上、下线、文件传输
//包含文件： command.c usrlist.c filelist.c srfile.c ulits.c
//作者： by dengke.
//=============================================================================================

#include <pthread.h>
#include "commands.h"
#include "usrlist.h"
#include "filelist.h"
#include "srfile.h"
extern const char allHost[];
extern int msgSock;
extern int tcpSock;
extern PFILENODE myFileList;
struct passwd *pwd;
struct utsname sysName;
char usrcmd[1024];

pthread_mutex_t ipmsg_mutex;
//

int initServ(struct sockaddr_in *servaddr)
{
	bzero(servaddr,sizeof(*servaddr));
	msgSock=socket(AF_INET,SOCK_DGRAM,0);//AF_INET-使用ip协议族，sock_dgram使用面向无连接，0具体使用的协议由系统指定

	tcpSock=socket(AF_INET,SOCK_STREAM,0);//

	servaddr->sin_family=AF_INET;
	servaddr->sin_port=htons(IPMSG_DEFAULT_PORT);//端口使用ipmsg默认的69号端口；
	servaddr->sin_addr.s_addr=htonl(INADDR_ANY);

	int broadcast_on=1;//打开广播选项

	if (setsockopt(msgSock, SOL_SOCKET, SO_BROADCAST, &broadcast_on, sizeof(broadcast_on))<0)
	{
		printf("initSvr: Socket options brodcast set error.\n");
		exit(1);
	}

	if(bind(msgSock,(struct sockaddr*)servaddr,sizeof(*servaddr))<0)
	{
		perror("initServ: bind msgsock err!\n");
		exit(1);
	}
	//*

	if(bind(tcpSock,(struct sockaddr*)servaddr,sizeof(*servaddr))<0)
	{
		perror("initServ: bind tcpSock err!\n");
		exit(1);
	}
	if(listen(tcpSock,10)<0)
	{
		perror("initServ: listen tcpSock err!\n");
		exit(1);    
	}
	//*/

	printf("init msgServ finshed!\n");
	return 0;
}
//=========================================================================
//函数名称：
//功能描述：上线提示
//函数参数:
//返回值：
//==========================================================================

void loginBordCast()
{
	MSG loginMsg;
	char brodcastMsg[]="wolf online!";

	makeSendMsg(&loginMsg,brodcastMsg,allHost,IPMSG_BR_ENTRY);
	sendToMsg(msgSock,&loginMsg);

	return ;
}
//===============================================================================================
//函数名称：printOlineUsr()
//功能描述：打印在线用户
//
//================================================================================================

void printOnlineUsr()
{
	printf("---------------------onlie usr info-------------------\n ");
	listUsr(stdout);
	printf("---------------------onlie usr info-------------------\n ");
}
//===================================================================================================
//函数名称：recverPthread(void *sockfd)
//功能描述：接收信息处理线程，生成在线用户列表，显示消息
//函数参数：
//返回值：
//===================================================================================================

void *recverPthread(void *sockfd)
{
	struct sockaddr_in fromServer;
	char recvbuf[MAX_BUF];
	char fromIp[20];
	size_t fromServLen;    
	size_t recvMsgLen;
	int recvSock=*(int*)sockfd; //****

	MSG recvMsg;
	MSG retMsg;

	fromServLen=sizeof(fromServer);

	while(1)
	{                                                //接收客服端信息
		if(recvMsgLen=recvfrom(recvSock,recvbuf,sizeof(recvbuf),0,(struct sockaddr *)&fromServer,&fromServLen));
		{
			inet_ntop(AF_INET,&fromServer.sin_addr.s_addr,fromIp,16);
			if(strcmp(fromIp,"192.168.220.211"))//自己ip不收
			{                    
				//pthread_mutex_lock(&ipmsg_mutex);//----------pthread_mutex_lock---------
				msgParser(&recvMsg,recvbuf);//解释信息，将信息解析成结构体

				if(((recvMsg.commandNo&IPMSG_BR_ENTRY)==1)||((recvMsg.commandNo&IPMSG_ANSENTRY)==3))//报文是用户上线提示或通报在线
				{        
					if(!findUsrByName(recvMsg.senderName))//如果在线用户链表没有该用户则添加
					{
						addUsr(recvMsg.senderName,recvMsg.senderHost,fromIp);//插入一个在线用户
						//printf("%s is online !\n",recvMsg.senderName);//上线提示
					}
				}
				else if((recvMsg.commandNo&IPMSG_BR_EXIT)==2)//用户退出
				{
					if(findUsrByName(recvMsg.senderName)!=NULL)
					{
						delUsr(findUsrByName(recvMsg.senderName));//
					}
				}
				else if(((recvMsg.commandNo&IPMSG_SENDMSG)==4)||(recvMsg.commandNo&0xff)==32)
				{ //显示发送消息
					//printf("%s ",recvMsg.senderName);
					printf("%s(",recvMsg.senderHost);
					printf("%s):",fromIp);
					printf("%s\n",recvMsg.additional);
					makeSendMsg(&retMsg,"收到了",fromIp,IPMSG_RECVMSG);//收到后发回应
					sendToMsg(recvSock,&retMsg);
				}
				else
				{
				}
				//                                printf("%s ",recvMsg.senderName);
				//                                printf("%s ",recvMsg.senderHost);
				//                                printf("%s ",fromIp);
				//                                printf("Sendmsg=%d ",(recvMsg.commandNo&IPMSG_SENDMSG));
				//                                printf("用户上线=%d ",(recvMsg.commandNo&IPMSG_BR_ENTRY));
				//                                printf("tcp请求=%d ",(recvMsg.commandNo&IPMSG_GETFILEDATA));
				//                                printf("传送文件选项=%d ",(recvMsg.commandNo&IPMSG_FILEATTACHOPT));
				//                                printf("附件=%s\n",recvMsg.additional);
			}
			//pthread_mutex_unlock(&ipmsg_mutex); //--------pthread_mutex_unlock----------
		}
	}

}
//=================================================================================================
//函数名称：chat();
//功能描述：实现与客服端聊天
//函数参数：
//返回值：
//================================================================================================

void chat(int chatSock)
{
	char chatbuf[1024];
	char tempbuf[10];
	char destIp[20];
	unsigned int usrNo;
	USERINFO *chatUsr;
	MSG talkMsg;
	while(1)
	{
		pthread_mutex_lock(&ipmsg_mutex);//---------------pthread_mutex_lock-----

		printOnlineUsr();
		pthread_mutex_unlock(&ipmsg_mutex);//-------------pthread_mutex_unlock----

		printf(" 'q'---to go back!\n");
		printf("input a usrNo form usrlist for chat:");
		fgets(tempbuf,sizeof(tempbuf),stdin);
		//pthread_mutex_lock();

		if(!strcmp(tempbuf,"q\n"))
		{
			return;
		}
		if(!isdigit(tempbuf[0]))
		{
			printf("err!please input a usrlist num:\n");    
			continue;
		}
		usrNo=atoi(tempbuf);
		if(!isdigit(usrNo))
		{
			pthread_mutex_lock(&ipmsg_mutex);//--------pthread_mutex_lock-----

			chatUsr=findUsrByNum(usrNo);
			pthread_mutex_unlock(&ipmsg_mutex);//---------pthread_mutex_unlock

			if(chatUsr) //找到聊天的对象
			{
				strcpy(destIp,chatUsr->ipaddr);    
				while(1)
				{ //与选定的usr聊天
					printf("destIp(%s):",destIp);
					memset(chatbuf,0,sizeof(chatbuf));
					//pthread_mutex_unlock();

					fgets(chatbuf,sizeof(chatbuf),stdin);
					//pthread_mutex_lock();
					if(!strcmp(chatbuf,"q\n"))//退出聊天，返回上一级目录
					{
						break;
					}
					pthread_mutex_lock(&ipmsg_mutex);//---------pthread_mutex_lock-------

					makeSendMsg(&talkMsg,chatbuf,destIp,IPMSG_SENDMSG);
					sendToMsg(chatSock,&talkMsg);
					pthread_mutex_unlock(&ipmsg_mutex);//---------pthread_mutex_unlock---
				}
			}
			else
			{
				printf("you select usr is not online\n");
				continue;
			}
		}
		else
		{
			printf("your input usrNo is not digit!\n");
			continue;    
		}
		//pthread_mutex_unlock();
	}    
}
//=================================================================================================
//函数名称：
//功能描述：打印帮助信息
//函数参数：
//返回值：
//==================================================================================================

void printFunMenu()
{
	printf("****************function menu******************\n");
	printf(" 'ls'---list all online usr \n");
	printf(" 'chat'---begin to chat \n");
	printf(" 'sf'---sendfile example: sf file1 file2 \n");
	printf(" 'rf'---recvsfile example: rf file1 file2 \n");
	printf(" 'help'---help info \n");
	printf(" 'exit'---quit system \n");
	printf("****************function menu******************\n");

}

//=================================================================================================
//函数名称：usrTalkPthread(void *sockfd)
//功能描述：线程、接收用户输入
//函数参数：
//返 回 值：
//===================================================================================================

void *getCmdPthread(void *sockfd)
{
	int talkSock=*(int *)sockfd;
	char rfcmdbuf[100];
	char *arg[100];
	char usrno[20];

	USERINFO *selectuser;

	while(1)
	{        
		printf("\r ");
		printf("\r select your command:");
		fgets(usrcmd,sizeof(usrcmd),stdin);
		//pthread_mutex_lock();

		if(!strcmp(usrcmd,"ls\n"))
		{
			pthread_mutex_lock(&ipmsg_mutex);//-----pthread_mutex_lock-----

			printOnlineUsr();
			pthread_mutex_unlock(&ipmsg_mutex); //----pthread_mutex_unlock
		}
		else if(!strcmp(usrcmd,"chat\n"))
		{
			chat(talkSock);//
		}
		else if(!strcmp(usrcmd,"help\n"))
		{
			printFunMenu();
		}
		else if(!strcmp(usrcmd,"exit\n"))
		{
			perror("in exit\n");
			exit(1);
		}
		else if(!strcmp(usrcmd,"sf\n"))
		{
			//----------------------------pthread_mutex_lock-------------

			pthread_mutex_lock(&ipmsg_mutex);
			printOnlineUsr();
			pthread_mutex_unlock(&ipmsg_mutex); //--------prhread_mutex_unlock

			printf("input command:sf usrNo filename\n");
			printf("example:sf 1 file1 file2 ...\n");
			fgets(rfcmdbuf,sizeof(rfcmdbuf),stdin);
			//---------------------------------pthread_mutex_lock---------

			pthread_mutex_lock(&ipmsg_mutex);
			rfcmdbuf[strlen(rfcmdbuf)-1]='\0';
			int count=0;
			count=strSplit(rfcmdbuf," ",arg);    

			if(!isdigit(atoi(arg[1]))&&(count<2))
			{
				printf("command is bad commad!\n");
				continue;
			}
			//printf("arg[1]=%s arg[2]=%s\n",arg[1],arg[2]);
			//printf("before makefilelist\n");
			//                        int i=2;
			//                        while(arg[i])
			//                        {
			//                            printf("arg[%d]=%s\n",i,arg[i]);
			//                            i++;
			//                        }
			makeFileList(arg);//生成文件列表
			//printf("after makefilelist\n");

			listFile(stdout);
			strcpy(usrno,arg[1]);
			selectuser=findUsrByNum(atoi(usrno));//获得选定的用户信息

			sendFileList(talkSock,selectuser,myFileList);
			//---------------------------------pthread_mutex_unlock---------
			pthread_mutex_unlock(&ipmsg_mutex);
		}
		else if(!strcmp(usrcmd,"rf\n"))
		{
		}
		else
		{
			printf("command '%s' is not found!",usrcmd);    
		}
		//pthread_mutex_unlock();
	}
}
//void *recvFilePthread(void *sockfd)
//{
//        int connfd;
//        struct sockaddr_in clientAddr;
//        size_t clientLen=sizeof(clientAddr);
//        char databuf[1024];
//        size_t recvLen;
//        connfd=accept(tcpSock,(struct *)&clientAddr,(socklen_t *)&clientLen);
//        while(1)
//        {
//                if((recvLen=read(tcpSock,databuf,sizeof(databuf)))>0)
//                {
//                        
//                }
//        }
//}

//==================================================================================================
//函数名称：
//功能描述：接收文件传输（完成中......）
//函数参数：
//返回值：
//==================================================================================================

void *recvFilePthread(void *sockfd)
{
	char recvbuf[1024];
	int connfd;
	int clientSock=*(int *)sockfd;
	struct sockaddr_in clientServer;
	size_t client_len=sizeof(clientServer);
	size_t recvLen;

	connfd=accept(clientSock,(struct sockaddr *)&clientServer,&client_len);
	while(1)
	{
		if((recvLen=read(connfd,recvbuf,sizeof(recvbuf)))>0)
		{
			printf("recvbuf=%s\n",recvbuf);    
		}    
		else
		{
			break;    
		}
	}
	close(clientSock);
}
//=============================================================================================
//函数名称：
//功能描述：创建线程实现，聊天、用户上线、下线、及文件传送
//函数参数：
//返回值：
//==============================================================================================

int main(int argc,char *argv[])
{

	struct sockaddr_in msgServer;
	pthread_t recvMsg,usrCmd,recvFile,sendFile;

	pthread_mutex_init(&ipmsg_mutex,NULL);

	initServ(&msgServer);
	initUsrList();

	pwd=getpwuid(getuid());//***
	uname(&sysName); //***

	loginBordCast();//广播通知上线

	printFunMenu();
	memset(usrcmd,0,sizeof(usrcmd));

	pthread_create(&recvMsg,NULL,(void *)*recverPthread,&msgSock);
	pthread_create(&usrCmd,NULL,(void *)*getCmdPthread,&msgSock);
	//pthread_create(&recvFile,NULL,(void *)*recvFilePthread,&tcpSock);
	//pthread_create(&sendFile,NULL,(void *)*sendFilePthread,&tcpSock);

	pthread_join(recvMsg,NULL);
	pthread_join(usrCmd,NULL);
	//pthread_join(recvFile,NULL);
}

//用到的常用命令和全局变量:

#ifndef __COMMANDS_H_
#define __COMMANDS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pwd.h>
#include <sys/utsname.h>

#include "ipmsg.h"

//typedef struct filenode FILENODE;

typedef struct msg
{
	unsigned int version;
	unsigned int packetNo;
	char senderName[MAX_NAMEBUF];
	char senderHost[MAX_NAMEBUF];
	unsigned int commandNo;
	char additional[MAX_BUF];
	struct sockaddr_in ipAddr;
	//struct filenode *list;


}MSG;

int sendToMsg(int sockfd,MSG *sendmsg);
int msgParser(MSG *msgRecv,char *recvbuf);
int makeSendMsg(MSG *msgSend,char *usrInput,char *destip,int comFlag);
#endif

//==================================================================================
//文件名称： commands.c
//功能描述： 用到的常用命令和全局变量
//包含文件： commands.h ulits.h
//维护日期：
//===================================================================================

#include "commands.h"
#include "ulits.h"

struct passwd *pwd;
struct utsname sysName;
const char allHost[]="192.168.220.255";
int msgSock;
int tcpSock;
extern pthread_mutex_t ipmsg_mutex;
//===================================================================================
//函数名称： makeMsg
//功能描述：将字符串生成要发送的报文结构体格式
//函数参数：msg生成的报文结构体存放地址，usrinput用户输入，comflag 命令字
//返 回 值：构造成功返回1，失败返回0；
//===================================================================================

int makeSendMsg(MSG *msgSend,char *usrInput,char *destip,int comFlag)
{
	if(msgSend==NULL)
	{
		return 0;
	}
	bzero(msgSend,sizeof(msgSend));
	msgSend->version=1;
	msgSend->packetNo=(unsigned int)time(NULL);
	strcpy(msgSend->senderName,pwd->pw_name);
	strcpy(msgSend->senderHost,sysName.nodename);
	msgSend->commandNo=comFlag;
	memcpy(msgSend->additional,usrInput,sizeof(msgSend->additional));
	//<input type="radio" name="" value="">

	msgSend->ipAddr.sin_family=AF_INET;
	msgSend->ipAddr.sin_port=htons(IPMSG_DEFAULT_PORT);
	//msgSend->ipAddr.sin_addr.s_addr=inet_addr(destip);

	inet_pton(AF_INET,destip,&msgSend->ipAddr.sin_addr);//loginMsg.ipAddr.sin_addr


	return 1;
}
//===================================================================================
//函数名称：msgParser
//功能描述：将接收到的recvbuf解析，解析结果保存到结构体
//函数参书：msgRecv解析结果保存地址，recvbuf接收到的字符串buf
//返 回 值：int 成功返回1，失败返回0；
//===================================================================================

int msgParser(MSG *msgRecv,char *recvbuf)
{
	char *arg[10];
	char filebuf[1024];
	unsigned int len=0;
	assert(recvbuf!=NULL);

	if(msgRecv==NULL)
	{
		return 0;
	}
	bzero(msgRecv,sizeof(msgRecv));

	trim(recvbuf,1);
	pthread_mutex_lock(&ipmsg_mutex);//--------pthread_mutex_lock();

	strSplit(recvbuf,":",arg);
	pthread_mutex_unlock(&ipmsg_mutex);//------pthread_mutex_unlock;

	msgRecv->version=atoi(arg[0]);
	msgRecv->packetNo=atoi(arg[1]);
	strcpy(msgRecv->senderName,arg[2]);
	strcpy(msgRecv->senderHost,arg[3]);
	msgRecv->commandNo=atoi(arg[4]);
	strcpy(msgRecv->additional,arg[5]);

	return 1;
}
//========================================================================================
//函数名称：sendToMsg(int sockfd,MSG *sendmsg);
//功能描述：将结构体类型的数据发送出去
//函数参数：sockfd发送的sockfd对象，sendmsg发送的数据包结构体
//返 回 值：int
//===========================================================================================

int sendToMsg(int sockfd,MSG *sendmsg)
{
	char sendbuf[2408];    

	if(sendmsg==NULL)
	{
		perror("sendToMsg: send err!\n");
		exit(1);
	}
	sprintf(sendbuf,"%u:%u:%s:%s:%u:%s",sendmsg->version,
		sendmsg->packetNo,
		sendmsg->senderName,
		sendmsg->senderHost,
		sendmsg->commandNo,
		sendmsg->additional);

	//len=strlen(sendbuf);

	//printf("sendbuf=%s\n",sendbuf);

	sendto(sockfd,sendbuf,strlen(sendbuf),0,(struct sockaddr*)&sendmsg->ipAddr,sizeof(sendmsg->ipAddr));
	//port=ntohs(sendmsg->ipAddr.sin_port);

	return 0;                            
}




//=============funtion test----

/*
int main(int argc,char *argv[])
{
char usrinput[60]="hello world!";
char recvbuf[60]="1:1235:user name:host name:32:hello world";

MSG msgSend;
MSG msgRecv;

pwd=getpwuid(getuid());
uname(&sysName);

makeSendMsg(&msgSend,usrinput,33);
printf("----print msgsend-----------\n");
printf("version=%d\n",msgSend.version);
printf("packetNo=%d\n",msgSend.packetNo);
printf("senderName=%s\n",msgSend.senderName);
printf("senderHost=%s\n",msgSend.senderHost);
printf("commandNo=%d\n",msgSend.commandNo);
printf("additional=%s\n",msgSend.additional);

printf("----print msgrecv-----------\n");
msgParser(&msgRecv,recvbuf);
printf("version=%d\n",msgRecv.version);
printf("packetNo=%d\n",msgRecv.packetNo);
printf("senderName=%s\n",msgRecv.senderName);
printf("senderHost=%s\n",msgRecv.senderHost);
printf("commandNo=%d\n",msgRecv.commandNo);
printf("additional=%s\n",msgRecv.additional);
return 0;
}
*/

//用户管理模块：
//=============================================================
// 文件名称：
// 功能描述：用户管理链表
// 维护记录： by dengke
//=============================================================

#pragma once
#ifdef __cplusplus
extern "C" {
#endif

	typedef struct usr_info_t {
		struct usr_info_t *next;
		unsigned int num;
		char *usrname;
		char *hostname;
		char *ipaddr;
	} USERINFO, *PUSERINFO;


	void initUsrList(void);
	int addUsr(char *name, char *hostname,char *ipaddr);
	int delUsr(PUSERINFO usr);
	int listUsr(FILE *file);
	PUSERINFO findUsrByName(char *name);
	PUSERINFO findUsrByNum(unsigned int number);

#ifdef __cplusplus
};
#endif


//=============================================================
// 文件名称：usrlist.c
// 功能描述：通用链表修改成的myUsrList链表, **注释**部分为要注意修改的地方
//
//
//
//包含文件：usrlist.h lnklst.h
// 维护记录：2009-05-08 V1.1 by lijian
//=============================================================

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include "lnklst.h"
#include "usrlist.h"
//===============================================================================

static PUSERINFO myUsrList = NULL;//链表头在这里定义

static unsigned int count;
//========================================================================================
//函数名称：delUsrEx(PUSERINFO usr)
//功能描述：释放节点所占用的堆内存空间
//函数参数：usr要释放的节点指针
//返 回 值：
//===========================================================================================

static void delUsrEx(PUSERINFO usr)
{
	if(usr->usrname)
		free(usr->usrname);
	if(usr->hostname)
		free(usr->hostname);
	if(usr->ipaddr)
		free(usr->ipaddr);
	free(usr);
}
//==========================================================================================
//函数名称：destroyUsrList(void)
//功能描述：链表初始化，所有内存空间清0
//函数参数：void
//返回值：
//=============================================================================================

static void destroyUsrList(void)
{
	PUSERINFO tmpNod = myUsrList; //*****************这里要修改*********************

	while(tmpNod)
	{
		PUSERINFO tmp = tmpNod->next;
		delUsrEx(tmpNod); //清空节点所占的内存

		tmpNod = tmp;
	}
}


/***********************************以下为用户使用接口****************************************/


//=============================================================================================
//函数名称：initUsrList(void)
//功能描述：调用destroyUsrList来清空链表，完成初始化
//函数参数：
//返 回 值：
//============================================================================================

void initUsrList(void)
{
	if(myUsrList) //*************这里要修改**********

	{
		destroyUsrList();
	}
	myUsrList = NULL;
}
//=============================================================================================
//函数名称：addUsr(char *name, char *host,char *ip)
//功能描述：在链表中插入一个节点
//函数参数：name、host、ip节点信息指针
//返回值：
//============================================================================================

int addUsr(char *name, char *host,char *ip) //使用通用链表时此函数必须修改

{
	PUSERINFO newusr = (PUSERINFO)malloc(sizeof(USERINFO));
	if(newusr == NULL)
		return -ENOMEM;

	newusr->usrname = (char *)malloc(strlen(name) + 1);
	if(newusr->usrname == NULL)
	{
		free(newusr);
		return -ENOMEM;
	}

	newusr->hostname = (char *)malloc(strlen(host) + 1);
	if(newusr->hostname == NULL)
	{
		free(newusr->usrname);
		free(newusr);
		return -ENOMEM;
	}

	newusr->ipaddr = (char *)malloc(strlen(ip) + 1);
	if(newusr->ipaddr == NULL)
	{
		free(newusr->ipaddr);
		free(newusr);
		return -ENOMEM;
	}

	newusr->num=count;
	strcpy(newusr->usrname, name);
	strcpy(newusr->hostname, host);
	strcpy(newusr->ipaddr, ip);
	count++;
	//pthread_mutext_lock();

	add_node((void**)&myUsrList, newusr); //*****************这里要修改*****************

	//pthread_mutext_unlock();

	return 0;
}
//=============================================================================================
//函数名称：delUsr(PUSERINFO usr)
//功能描述：调用函数del_node从链表中删除一个节点
//函数参数：usr要从链表中删除节点
//返回值： 成功返回0，失败返回-1
//==============================================================================================

int delUsr(PUSERINFO usr)
{
	if(usr==NULL)
	{
		printf("node is not found!\n");    
		return -1;
	}
	del_node((void**)&myUsrList, usr); //这里要注意修改

	delUsrEx(usr);
	count--;
	return 0;
}
//===============================================================================================
//函数名称：findUsrByName(char *name)
//功能描述：通过name查找节点
//函数参数：name要查找的节点关键字
//返 回 值：找到的节点的结构体指针,没找到为null
//===============================================================================================

PUSERINFO findUsrByName(char *name)
{
	PUSERINFO ret = NULL;
	PUSERINFO tmpNod = myUsrList;
	while(tmpNod)
	{
		if(strcmp(tmpNod->usrname, name) == 0)
		{
			ret = tmpNod;
			break;
		}
		tmpNod = tmpNod->next;
	}
	return ret;
}
//=================================================================================================
//函数名称：
//功能描述：通过num查找节点
//函数参数：
//返回值：
//=================================================================================================

PUSERINFO findUsrByNum(unsigned int number)
{
	//printf("find ok\n");

	PUSERINFO ret = NULL;
	PUSERINFO tmpNod = myUsrList;
	while(tmpNod)
	{
		if(tmpNod->num == number)
		{
			ret = tmpNod;
			break;
		}
		tmpNod = tmpNod->next;
	}
	return ret;
}
//=================================================================================================
//函数名称：
//功能描述：打印输出链表
//函数参数：FIlE *file输出终端，显示器为，stdout
//返 回 值：输出链表个数
//=================================================================================================

int listUsr(FILE *file)
{
	int count = 1;
	char temp[10];
	PUSERINFO usr = myUsrList;
	if(usr == NULL)
		fprintf(file, "User List is empty!\n");
	else
	{
		fprintf(file, "%6s %20s %20s %20s\n", "num", "Name", "hostname","ipaddr");
		while(usr)
		{
			fprintf(file,"%6d %20s %20s %20s\n", usr->num, usr->usrname, usr->hostname,usr->ipaddr);
			usr = usr->next;
		}
		//fprintf(file, "%d users found!\n", count);

	}
	return count;
}
/*
int main()
{
addUsr("usrname1", "host1","192.168.220.1");
listUsr(stdout);
addUsr("usrname2", "host2","192.168.220.2");
addUsr("usrname3", "host3","192.168.220.3");
addUsr("usrname4", "host4","192.168.220.4");
addUsr("usrname5", "host5","192.168.220.5");
listUsr(stdout);
//delUsr(findUsrByName("usrname3"));
delUsr(findUsrByNum(3));
//delUsr("usrname8");
listUsr(stdout);
printf("init list...\n");
initUsrList();
listUsr(stdout);
return 0;
}

*/

#ifndef __SRFILE_H_
#define __SRFILE_H_

#include "ulits.h"
#include "filelist.h"
#include "usrlist.h"
#include "commands.h"
#include <sys/stat.h>

extern int makeFileList(char *arg1[]);
int sendFileList(int sockfd,USERINFO *destusr,FILENODE *filelist);
#endif

#include "srfile.h"

//=========================================================================================
//函数名称：makeFileList(char *rfbuf)
//功能描述：生成发送文件链表
//函数参数：arg1[],发送文件名的指针数组
//返回值：
//=========================================================================================

int makeFileList(char *arg1[])
{
	//char *arg[10];


	char filename[50];
	char filesize[50];
	char mtime[50];
	char filetype[5];
	int i=2;
	struct stat filetypebuf;

	initFileList();
	while(arg1[i])
	{    
		memset(filename,0,sizeof(filename));
		memset(filesize,0,sizeof(filesize));
		memset(mtime,0,sizeof(mtime));
		memset(filetype,0,sizeof(filetype));
		printf("arg1[%i]=%s\n",i,arg1[i]);
		if(lstat(arg1[i],&filetypebuf)<0)
		{
			printf("file:'%s' is err!\n",arg1[i]);    
			i++;
			continue;
		}

		if(S_ISREG(filetypebuf.st_mode))
		{
			sprintf(filetype,"%d",1);
		}
		else if(S_ISDIR(filetypebuf.st_mode))
		{
			sprintf(filetype,"%d",2);
		}
		else
		{
			//filetype=;

			continue;
		}

		strcpy(filename,arg1[i]);
		sprintf(filesize,"%u",filetypebuf.st_size);
		sprintf(mtime,"%u",filetypebuf.st_mtime);
		printf("file[%d]=%s\n",i,filename);

		addFile(filename,filesize,mtime,filetype);
		i++;
	}    
	return i;
}
//============================================================================================
//函数名称：sendFileLIst(int sockfd,FILENODE *filelist)
//功能描述：发送文件链表
//函数参数：发送的sockfd对象，filelist发送的文件链表
//返 回 值：
//============================================================================================

int sendFileList(int sockfd,USERINFO *destusr,FILENODE *filelist)
{
	char sendbuf[600];
	FILENODE *tmpNode;
	int i=0;
	tmpNode=filelist;
	MSG sfMsg;

	while(tmpNode)
	{
		memset(sendbuf,0,sizeof(sendbuf));
		sprintf(sendbuf+1,"%d:%s:%x:%x:%x:\\a",tmpNode->fileno
			,tmpNode->filename
			,tmpNode->filesize
			,tmpNode->mtime
			,tmpNode->filetype);
		//            len=len+strlen(tmpNode->fileno)+1;
		//            strcpy(sendbuf+len,tmpNode->filename);
		//            len=len+strlen(tmpNode->filename)+1;
		//            strcpy(sendbuf+len,tmpNode->filesize);
		//            len=len+strlen(tmpNode->mtime)+1;
		//            strcpy(sendbuf+len,tmpNode->mtime);
		//            len=len+strlen(tmpNode->filetype)+1;
		//            strcpy(sendbuf+len,tmpNode->filetype);

		makeSendMsg(&sfMsg,sendbuf,destusr->ipaddr,IPMSG_SENDMSG|IPMSG_FILEATTACHOPT);//

		sendToMsg(sockfd,&sfMsg);
		tmpNode=tmpNode->next;
		i++;
		printf("i=%d in sendfilelist\n",i);

	}    
	return i;
}

//====================================================================================
//    The information contained herein is the exclusive property of
//    Sunnnorth Technology Co. And shall not be distributed, reproduced,
//    or disclosed in whole in part without prior written permission.
//    (C) COPYRIGHT 2003 SUNNORTH TECHNOLOGY CO.
//    ALL RIGHTS RESERVED
//    The entire notice above must be reproduced on all authorized copies.
//====================================================================================


//=============================================================
// 文件名称：demo.h
// 功能描述：链表使用示例程序
// 维护记录：2009-05-08 V1.1 by lijian
//=============================================================

#pragma once
#ifdef __cplusplus
extern "C" {
#endif

	typedef struct filenode
	{
		struct filenode *next;
		unsigned int fileno;
		char *filename;
		char *filesize;
		char *mtime;
		char *filetype;
	} FILENODE, *PFILENODE;


	extern void initFileList(void);
	int addFile(char *name, char *size,char *time,char *type);
	int delFile(PFILENODE file);
	int listFile(FILE *file);

	PFILENODE findFileByName(char *name);
	PFILENODE findFileByNum(unsigned int number);

#ifdef __cplusplus
};
#endif


//=============================================================
// 文件名称：本通用链表的快速使用方法及步骤（可用查找替换命令）：
// 一、在头文件中根据自己需要增加/删除成员（除整数外，成员变量必须为指针类型）
// 二、把"filelist.h"替换成自己的头文件名
// 三、把"myFileList"替换成自己的文件列表名
// 四、在 根据自己的成员名称修改static void delFileEx()
// 五、在 根据自己的成员名称修改int addFile()
// 六、把其他成员变量修改一直
//包含文件：filelist.h lnklst.h
// 维护记录：2009-05-10 V1.2 by dengke mend some bugs!
//=============================================================

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include "lnklst.h"
#include "filelist.h"
//========================================================================================

PFILENODE myFileList = NULL;//链表头在这里定义

static unsigned int count;
//========================================================================================
//函数名称：delFileEx(PFILENODE file)
//功能描述：释放节点所占用的堆内存空间
//函数参数：file要释放的节点指针
//返 回 值：
//===========================================================================================

static void delFileEx(PFILENODE file)//****此函数要根据成员数目来添加/删除成员

{
	if(file->filename)
		free(file->filename);
	if(file->filesize)
		free(file->filesize);
	if(file->mtime)
		free(file->mtime);
	if(file->filetype)
		free(file->filetype);
	free(file);
}
//==========================================================================================
//函数名称：destroyFileList(void)
//功能描述：链表初始化，所有内存空间清0
//函数参数：void
//返回值：
//=============================================================================================

static void destroyFileList(void)
{
	PFILENODE tmpNod = myFileList;
	while(tmpNod)
	{
		PFILENODE tmp = tmpNod->next;
		delFileEx(tmpNod); //清空节点所占的内存

		tmpNod = tmp;
	}
}


/***********************************以下为用户使用接口****************************************/


//=============================================================================================
//函数名称：initFileList(void)
//功能描述：调用destroyFileList来清空链表，完成初始化
//函数参数：
//返 回 值：
//============================================================================================

void initFileList(void)
{
	if(myFileList)
	{
		destroyFileList();
	}
	myFileList = NULL;
}
//=============================================================================================
//函数名称：addFile(char *name, char *host,char *ip)
//功能描述：在链表中插入一个节点
//函数参数：name、host、ip节点信息指针
//返回值：
//============================================================================================

int addFile(char *name, char *size,char *time,char *type) //使用通用链表时此函数必须修改

{
	PFILENODE newfile = (PFILENODE)malloc(sizeof(FILENODE));
	if(newfile == NULL)
		return -ENOMEM;

	newfile->filename = (char *)malloc(strlen(name) + 1);
	if(newfile->filename == NULL)
	{
		free(newfile);
		return -ENOMEM;
	}

	newfile->filesize = (char *)malloc(strlen(size) + 1);
	if(newfile->filesize == NULL)
	{
		free(newfile->filename);
		free(newfile);
		return -ENOMEM;
	}

	newfile->mtime = (char *)malloc(strlen(time) + 1);
	if(newfile->mtime == NULL)
	{
		free(newfile->mtime);
		free(newfile);
		return -ENOMEM;
	}
	newfile->filetype = (char *)malloc(strlen(type) + 1);
	if(newfile->mtime == NULL)
	{
		free(newfile->filetype);
		free(newfile);
		return -ENOMEM;
	}

	newfile->fileno=count;

	strcpy(newfile->filename, name);
	strcpy(newfile->filesize, size);
	strcpy(newfile->mtime, time);
	strcpy(newfile->filetype,type);
	count++;
	//pthread_mutext_lock();

	add_node((void**)&myFileList, newfile); //*****************这里要修改*****************

	//pthread_mutext_unlock();

	return 0;
}
//=============================================================================================
//函数名称：delFile(PFILENODE file)
//功能描述：调用函数del_node从链表中删除一个节点
//函数参数：file要从链表中删除节点
//返回值： 成功返回0，失败返回-1
//==============================================================================================

int delFile(PFILENODE file)
{
	if(file==NULL)
	{
		printf("node is not found!\n");    
		return -1;
	}
	del_node((void**)&myFileList, file); //这里要注意修改

	delFileEx(file);
	count--;
	return 0;
}
//===============================================================================================
//函数名称：findFileByName(char *name)
//功能描述：通过name查找节点
//函数参数：name要查找的节点关键字
//返 回 值：找到的节点的结构体指针,没找到为null
//===============================================================================================

PFILENODE findFileByName(char *name)
{
	PFILENODE ret = NULL;
	PFILENODE tmpNod = myFileList;
	while(tmpNod)
	{
		if(strcmp(tmpNod->filename, name) == 0)
		{
			ret = tmpNod;
			break;
		}
		tmpNod = tmpNod->next;
	}
	return ret;
}
//=================================================================================================
//函数名称：
//功能描述：通过num查找节点
//函数参数：
//返回值：
//=================================================================================================

PFILENODE findFileByNum(unsigned int number)
{
	//printf("find ok\n");

	PFILENODE ret = NULL;
	PFILENODE tmpNod = myFileList;
	while(tmpNod)
	{
		if(tmpNod->fileno == number)
		{
			ret = tmpNod;
			break;
		}
		tmpNod = tmpNod->next;
	}
	return ret;
}
//=================================================================================================
//函数名称：
//功能描述：打印输出链表
//函数参数：FIlE *file输出终端，显示器为，stdout
//返 回 值：输出链表个数
//=================================================================================================

int listFile(FILE *file)
{
	int counter = 1;
	char temp[10];
	PFILENODE fnode = myFileList;
	if(fnode == NULL)
		fprintf(file, "User List is empty!\n");
	else
	{
		fprintf(file, "%6s %20s %20s %20s %20s\n", "fileNo", "fileName", "filesize","mtime","filetype");
		while(fnode)
		{
			fprintf(file,"%6d %20s %20s %20s %20s\n", fnode->fileno, fnode->filename, fnode->filesize,fnode->mtime,fnode->filetype);
			fnode = fnode->next;
		}
		//fprintf(file, "%d users found!\n", count);

	}
	return counter;
}
/*
int main()
{
addFile("filename0", "size0","mtime1","0");
listFile(stdout);
addFile("filename1", "size1","mtime1","1");
addFile("filename2", "size2","mtime1","2");
addFile("filename3", "size3","mtime1","3");
addFile("filename4", "size4","mtime1","4");
addFile("filename5", "size5","mtime1","5");

listFile(stdout);
//delFile(findFileByName("filename3"));
delFile(findFileByNum(3));
delFile(findFileByName("filename4"));
listFile(stdout);
printf("init list...\n");
initFileList();
listFile(stdout);
return 0;
}

*/

//字符串处理函数：

#ifndef __ULITS_H_
#define __ULITS_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

extern void trim(char *dest, int flag);
extern int strSplit(char *split,const char *sour,char *result[]);

#endif

//////////////////////////////////////////////////

#include "ulits.h"
//===============================================================================
//函数名称：trim
//功能描述：字符串处理，去掉2端的空格，变成全小写(flag==1)
//函数参数：dest被处理的字符串指针变量，flag标志(flag==1)将字符串转换为小写
//返回值： 无
//================================================================================

void trim(char *dest, int flag)
{
	char *ptr;
	int len;
	assert(dest!=NULL);
	ptr = dest;

	while (isspace(*ptr))
		ptr++;

	len = strlen(ptr);
	if (ptr > dest)
		memmove(dest, ptr, len+1);

	ptr = dest+len-1;

	while (isspace(*ptr))
		ptr--;

	*(ptr+1) = '\0';

	ptr = dest;

	if (flag == 1)
		while (*ptr!='\0')
		{
			*ptr = tolower(*ptr);
			ptr++;
		}
}
//=================================================================================
//函数名称：strSplit

//功能描述：将一个被字符串split,按分割符sour，分割成段，每段首地址存放于指针数组result
//函数参数：split被分割字符串的指针变量，sour分割符指针变量，result分割结果存放的指针数组
//返回值： int 分割的段数；
//==================================================================================

int strSplit(char *split,const char *sour,char *result[])
{
	char *pos,*start;
	int i=0,counter=0;

	assert((split!=NULL)&&(sour!=NULL));

	pos=start=split;
	while(pos!=NULL)
	{
		pos=strstr(start,sour);//ls |

		if(pos==NULL)
		{
			result[i]=start;
			counter++;
			break;
		}

		*pos='\0';
		if(*start)
		{
			result[i]=start;
			i++;
			counter++;
		}
		start=pos+strlen(sour);
	}

	result[counter]=NULL;
	return counter;
}

//通用链表：
//====================================================================================
//=============================================================
// 文件名称：lnklst.h
// 功能描述：通用链表管理程序
// 维护记录：2008-12-25     V1.0
// 2009-05-08 V1.1 by dengek fix some bug
//=============================================================

#pragma once
#ifdef __cplusplus
extern "C" {
#endif

	typedef enum {NOD_BEFORE = 0, NOD_AFTER = 1} NOD_POSITION;

	void add_node_to(void **head, void *node, void *to, NOD_POSITION before_or_after);
	void move_node(void **head, void *moved, void *to, NOD_POSITION before_or_after);
	void add_node(void **head, void *node);
	void del_node(void **head, void *node);
	typedef int (*CMP_FUNC)(void *t1, void *t2);
	void sort_list(void **head, CMP_FUNC nodcmp);
	void add_node_sorted(void **head, void *node, CMP_FUNC nodcmp);
	int get_node_count(void **head);
	void *next_node(void *current);
	void *get_node_byID(void *head, int ID);

#ifdef __cplusplus
};
#endif


//=============================================================
// 文件名称：lnklst.c
// 功能描述：通用链表管理程序
// 维护记录：2008-12-25     V1.0
// 2009-05-08 V1.1 by dengke fix some bug
//=============================================================

#include <stdlib.h>
#include "lnklst.h"

#define ONE_WAY_LINKED_LIST
//#define TWO_WAY_LINKED_LIST


typedef struct basic_nod_t {
#ifdef TWO_WAY_LINKED_LIST
	struct basic_nod_t *prev;
#endif
	struct basic_nod_t *next;
} BASICNOD, *PBASICNOD;

static inline struct basic_nod_t *prev_node(PBASICNOD head, PBASICNOD nod)
{
#ifdef ONE_WAY_LINKED_LIST
	PBASICNOD n = head;
	if((nod == NULL) || ( nod == head))
		return NULL;
	while(n)
	{
		if(n->next == nod)
			return n;
		n = n->next;
	}
	return NULL;
#endif
#ifdef TWO_WAY_LINKED_LIST
	return nod->prev;
#endif
}

void *next_node(void *current)
{
	return (void *)((PBASICNOD)current)->next;
}

void *last_node(void *head)
{
	PBASICNOD p = (PBASICNOD)head;
	while(p && p->next)
	{
		p = p->next;
	}
	return p;
}

void add_node_to(void **head, void *node, void *to, NOD_POSITION before_or_after)
{
	struct basic_nod_t **h = (struct basic_nod_t**)head;
	struct basic_nod_t *n = (struct basic_nod_t*)node;
	struct basic_nod_t *tt = (struct basic_nod_t*)to;
	if(node == NULL)
		return;
	if(*h == NULL)
	{
		// the link list is empty

		*h = n;
#ifdef TWO_WAY_LINKED_LIST
		n->prev = NULL;
#endif
		n->next = NULL;
	}
	else if(before_or_after == NOD_BEFORE)
	{
		// move the 'moved' node to the place before 'to'

		// if 'to' is NULL, then move the 'moved' node to the head

#ifdef ONE_WAY_LINKED_LIST
		PBASICNOD prev_nod;
#endif
		if(tt == NULL)
			tt = *h;
		if(*h == tt)
			*h = n;
#ifdef TWO_WAY_LINKED_LIST
		n->prev = tt->prev;
#endif
		n->next = tt;
#ifdef TWO_WAY_LINKED_LIST
		tt->prev = n;
		if(n->prev != NULL)
		{
			n->prev->next = n;
		}
#endif
#ifdef ONE_WAY_LINKED_LIST
		prev_nod = prev_node(*h, tt);
		if(prev_nod)
		{
			prev_nod->next = n;
		}
#endif
	}
	else if(before_or_after == NOD_AFTER)
	{
		//move the 'moved' node to the place after 'to'

		if(tt == NULL)
			tt = last_node(*h);
		n->next = tt->next;
#ifdef TWO_WAY_LINKED_LIST
		n->prev = tt;
#endif
		tt->next = n;
#ifdef TWO_WAY_LINKED_LIST
		if(n->next != NULL)
			n->next->prev = n;
#endif
	}
}

void add_node(void **head, void *node)
{
	add_node_to(head, node, NULL, NOD_AFTER);
}

void del_node(void **head, void *node)
{
	PBASICNOD *h = (struct basic_nod_t**)head;
	PBASICNOD n = (struct basic_nod_t*)node;
#ifdef ONE_WAY_LINKED_LIST
	PBASICNOD tmp;
#endif
	if(h == NULL)
		return;
	if(*h == NULL)
		return;
	if(n == *h)
	{
		*h = n->next;
	}
#ifdef TWO_WAY_LINKED_LIST
	if(n->prev)
		n->prev->next = n->next;
	if(n->next)
		n->next->prev = n->prev;
#endif
#ifdef ONE_WAY_LINKED_LIST
	tmp = prev_node(*h, n);
	if(tmp)
		tmp->next = n->next;
#endif
}

void move_node(void **head, void *moved, void *to, NOD_POSITION before_or_after)
{
	PBASICNOD *h = (PBASICNOD *)head;
	PBASICNOD m = (PBASICNOD)moved;
	PBASICNOD tt = (PBASICNOD)to;
	if(    (h == NULL) || (*h == NULL) || (m == tt))
		return;
	del_node(head, moved);
	add_node_to(head, moved, to, before_or_after);
}

void sort_list(void **head, CMP_FUNC nodcmp)
{
	PBASICNOD *h = (PBASICNOD *)head;
	PBASICNOD nod1 = *h;
	PBASICNOD nod2 = nod1->next;
	int swaped = 1;
	if(nod2 == NULL)
		return;
	while(swaped)
	{
		swaped = 0;
		while(1)
		{
			if((*nodcmp)(nod1, nod2) > 0)
			{
				move_node(head, nod1, nod2, NOD_AFTER);
				nod2 = nod1->next;
				swaped = 1;
			}
			else
			{
				nod1 = nod2;
				nod2 = nod2->next;
			}
			if(nod2 == NULL)
			{
				nod1 = *h;
				nod2 = nod1->next;
				break;
			}
		}
	}
}

void add_node_sorted(void **head, void *node, CMP_FUNC nodcmp)
{
	int added = 0;
	PBASICNOD *h = (PBASICNOD *)head;
	PBASICNOD n = (PBASICNOD)node;
	PBASICNOD tmp = *h;
	if(*h == NULL)
	{
		add_node_to(head, node, NULL, NOD_AFTER);
		return;
	}
	if((*nodcmp)(n, *h) < 0)
	{
		add_node_to(head, node, *h, NOD_BEFORE);
		return;
	}
	//    if((*nodcmp)(n, last_node(*head,*h)) >= 0)

	//    {

	//        add_node(head, node);

	//        return;

	//    }

	while(tmp)
	{
		if((*nodcmp)(n, tmp) < 0)
		{
			add_node_to(head, node, tmp, NOD_BEFORE);
			added = 1;
			break;
		}
		else
			tmp = tmp->next;
	}
	if(added == 0)
		add_node_to(head, node, NULL, NOD_AFTER);
}

int get_node_count(void **head)
	{
	PBASICNOD *h = (PBASICNOD *)head;
	PBASICNOD tmp = *h;
	int count = 0;
	if(h == NULL)
		return 0;
	if(*h == NULL)
		return 0;
	while(tmp)
	{
		tmp = tmp->next;
		count++;
	}
	return count;
}

void *get_node_byID(void *head, int ID)
{
	PBASICNOD h = (PBASICNOD)head;
	PBASICNOD tmp;
	int count = 0;
	if(ID == 0)
		return head;
	for(tmp = h; tmp && (count < ID); tmp = tmp->next, count++);
	return tmp;
}