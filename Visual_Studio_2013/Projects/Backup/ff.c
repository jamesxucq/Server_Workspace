//===============================================================================================
//�������ƣ��ɸ봫��
//���������� ��ipmsgЭ��ʵ���������죬�û��ϡ����ߡ��ļ�����
//�����ļ��� command.c usrlist.c filelist.c srfile.c ulits.c
//���ߣ� by dengke.
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
	msgSock=socket(AF_INET,SOCK_DGRAM,0);//AF_INET-ʹ��ipЭ���壬sock_dgramʹ�����������ӣ�0����ʹ�õ�Э����ϵͳָ��

	tcpSock=socket(AF_INET,SOCK_STREAM,0);//

	servaddr->sin_family=AF_INET;
	servaddr->sin_port=htons(IPMSG_DEFAULT_PORT);//�˿�ʹ��ipmsgĬ�ϵ�69�Ŷ˿ڣ�
	servaddr->sin_addr.s_addr=htonl(INADDR_ANY);

	int broadcast_on=1;//�򿪹㲥ѡ��

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
//�������ƣ�
//����������������ʾ
//��������:
//����ֵ��
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
//�������ƣ�printOlineUsr()
//������������ӡ�����û�
//
//================================================================================================

void printOnlineUsr()
{
	printf("---------------------onlie usr info-------------------\n ");
	listUsr(stdout);
	printf("---------------------onlie usr info-------------------\n ");
}
//===================================================================================================
//�������ƣ�recverPthread(void *sockfd)
//����������������Ϣ�����̣߳����������û��б���ʾ��Ϣ
//����������
//����ֵ��
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
	{                                                //���տͷ�����Ϣ
		if(recvMsgLen=recvfrom(recvSock,recvbuf,sizeof(recvbuf),0,(struct sockaddr *)&fromServer,&fromServLen));
		{
			inet_ntop(AF_INET,&fromServer.sin_addr.s_addr,fromIp,16);
			if(strcmp(fromIp,"192.168.220.211"))//�Լ�ip����
			{                    
				//pthread_mutex_lock(&ipmsg_mutex);//----------pthread_mutex_lock---------
				msgParser(&recvMsg,recvbuf);//������Ϣ������Ϣ�����ɽṹ��

				if(((recvMsg.commandNo&IPMSG_BR_ENTRY)==1)||((recvMsg.commandNo&IPMSG_ANSENTRY)==3))//�������û�������ʾ��ͨ������
				{        
					if(!findUsrByName(recvMsg.senderName))//��������û�����û�и��û������
					{
						addUsr(recvMsg.senderName,recvMsg.senderHost,fromIp);//����һ�������û�
						//printf("%s is online !\n",recvMsg.senderName);//������ʾ
					}
				}
				else if((recvMsg.commandNo&IPMSG_BR_EXIT)==2)//�û��˳�
				{
					if(findUsrByName(recvMsg.senderName)!=NULL)
					{
						delUsr(findUsrByName(recvMsg.senderName));//
					}
				}
				else if(((recvMsg.commandNo&IPMSG_SENDMSG)==4)||(recvMsg.commandNo&0xff)==32)
				{ //��ʾ������Ϣ
					//printf("%s ",recvMsg.senderName);
					printf("%s(",recvMsg.senderHost);
					printf("%s):",fromIp);
					printf("%s\n",recvMsg.additional);
					makeSendMsg(&retMsg,"�յ���",fromIp,IPMSG_RECVMSG);//�յ��󷢻�Ӧ
					sendToMsg(recvSock,&retMsg);
				}
				else
				{
				}
				//                                printf("%s ",recvMsg.senderName);
				//                                printf("%s ",recvMsg.senderHost);
				//                                printf("%s ",fromIp);
				//                                printf("Sendmsg=%d ",(recvMsg.commandNo&IPMSG_SENDMSG));
				//                                printf("�û�����=%d ",(recvMsg.commandNo&IPMSG_BR_ENTRY));
				//                                printf("tcp����=%d ",(recvMsg.commandNo&IPMSG_GETFILEDATA));
				//                                printf("�����ļ�ѡ��=%d ",(recvMsg.commandNo&IPMSG_FILEATTACHOPT));
				//                                printf("����=%s\n",recvMsg.additional);
			}
			//pthread_mutex_unlock(&ipmsg_mutex); //--------pthread_mutex_unlock----------
		}
	}

}
//=================================================================================================
//�������ƣ�chat();
//����������ʵ����ͷ�������
//����������
//����ֵ��
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

			if(chatUsr) //�ҵ�����Ķ���
			{
				strcpy(destIp,chatUsr->ipaddr);    
				while(1)
				{ //��ѡ����usr����
					printf("destIp(%s):",destIp);
					memset(chatbuf,0,sizeof(chatbuf));
					//pthread_mutex_unlock();

					fgets(chatbuf,sizeof(chatbuf),stdin);
					//pthread_mutex_lock();
					if(!strcmp(chatbuf,"q\n"))//�˳����죬������һ��Ŀ¼
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
//�������ƣ�
//������������ӡ������Ϣ
//����������
//����ֵ��
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
//�������ƣ�usrTalkPthread(void *sockfd)
//�����������̡߳������û�����
//����������
//�� �� ֵ��
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
			makeFileList(arg);//�����ļ��б�
			//printf("after makefilelist\n");

			listFile(stdout);
			strcpy(usrno,arg[1]);
			selectuser=findUsrByNum(atoi(usrno));//���ѡ�����û���Ϣ

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
//�������ƣ�
//���������������ļ����䣨�����......��
//����������
//����ֵ��
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
//�������ƣ�
//���������������߳�ʵ�֣����졢�û����ߡ����ߡ����ļ�����
//����������
//����ֵ��
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

	loginBordCast();//�㲥֪ͨ����

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

//�õ��ĳ��������ȫ�ֱ���:

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
//�ļ����ƣ� commands.c
//���������� �õ��ĳ��������ȫ�ֱ���
//�����ļ��� commands.h ulits.h
//ά�����ڣ�
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
//�������ƣ� makeMsg
//�������������ַ�������Ҫ���͵ı��Ľṹ���ʽ
//����������msg���ɵı��Ľṹ���ŵ�ַ��usrinput�û����룬comflag ������
//�� �� ֵ������ɹ�����1��ʧ�ܷ���0��
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
//�������ƣ�msgParser
//���������������յ���recvbuf����������������浽�ṹ��
//�������飺msgRecv������������ַ��recvbuf���յ����ַ���buf
//�� �� ֵ��int �ɹ�����1��ʧ�ܷ���0��
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
//�������ƣ�sendToMsg(int sockfd,MSG *sendmsg);
//�������������ṹ�����͵����ݷ��ͳ�ȥ
//����������sockfd���͵�sockfd����sendmsg���͵����ݰ��ṹ��
//�� �� ֵ��int
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

//�û�����ģ�飺
//=============================================================
// �ļ����ƣ�
// �����������û���������
// ά����¼�� by dengke
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
// �ļ����ƣ�usrlist.c
// ����������ͨ�������޸ĳɵ�myUsrList����, **ע��**����ΪҪע���޸ĵĵط�
//
//
//
//�����ļ���usrlist.h lnklst.h
// ά����¼��2009-05-08 V1.1 by lijian
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

static PUSERINFO myUsrList = NULL;//����ͷ�����ﶨ��

static unsigned int count;
//========================================================================================
//�������ƣ�delUsrEx(PUSERINFO usr)
//�����������ͷŽڵ���ռ�õĶ��ڴ�ռ�
//����������usrҪ�ͷŵĽڵ�ָ��
//�� �� ֵ��
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
//�������ƣ�destroyUsrList(void)
//���������������ʼ���������ڴ�ռ���0
//����������void
//����ֵ��
//=============================================================================================

static void destroyUsrList(void)
{
	PUSERINFO tmpNod = myUsrList; //*****************����Ҫ�޸�*********************

	while(tmpNod)
	{
		PUSERINFO tmp = tmpNod->next;
		delUsrEx(tmpNod); //��սڵ���ռ���ڴ�

		tmpNod = tmp;
	}
}


/***********************************����Ϊ�û�ʹ�ýӿ�****************************************/


//=============================================================================================
//�������ƣ�initUsrList(void)
//��������������destroyUsrList�����������ɳ�ʼ��
//����������
//�� �� ֵ��
//============================================================================================

void initUsrList(void)
{
	if(myUsrList) //*************����Ҫ�޸�**********

	{
		destroyUsrList();
	}
	myUsrList = NULL;
}
//=============================================================================================
//�������ƣ�addUsr(char *name, char *host,char *ip)
//�����������������в���һ���ڵ�
//����������name��host��ip�ڵ���Ϣָ��
//����ֵ��
//============================================================================================

int addUsr(char *name, char *host,char *ip) //ʹ��ͨ������ʱ�˺��������޸�

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

	add_node((void**)&myUsrList, newusr); //*****************����Ҫ�޸�*****************

	//pthread_mutext_unlock();

	return 0;
}
//=============================================================================================
//�������ƣ�delUsr(PUSERINFO usr)
//�������������ú���del_node��������ɾ��һ���ڵ�
//����������usrҪ��������ɾ���ڵ�
//����ֵ�� �ɹ�����0��ʧ�ܷ���-1
//==============================================================================================

int delUsr(PUSERINFO usr)
{
	if(usr==NULL)
	{
		printf("node is not found!\n");    
		return -1;
	}
	del_node((void**)&myUsrList, usr); //����Ҫע���޸�

	delUsrEx(usr);
	count--;
	return 0;
}
//===============================================================================================
//�������ƣ�findUsrByName(char *name)
//����������ͨ��name���ҽڵ�
//����������nameҪ���ҵĽڵ�ؼ���
//�� �� ֵ���ҵ��Ľڵ�Ľṹ��ָ��,û�ҵ�Ϊnull
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
//�������ƣ�
//����������ͨ��num���ҽڵ�
//����������
//����ֵ��
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
//�������ƣ�
//������������ӡ�������
//����������FIlE *file����նˣ���ʾ��Ϊ��stdout
//�� �� ֵ������������
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
//�������ƣ�makeFileList(char *rfbuf)
//�������������ɷ����ļ�����
//����������arg1[],�����ļ�����ָ������
//����ֵ��
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
//�������ƣ�sendFileLIst(int sockfd,FILENODE *filelist)
//���������������ļ�����
//�������������͵�sockfd����filelist���͵��ļ�����
//�� �� ֵ��
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
// �ļ����ƣ�demo.h
// ��������������ʹ��ʾ������
// ά����¼��2009-05-08 V1.1 by lijian
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
// �ļ����ƣ���ͨ������Ŀ���ʹ�÷��������裨���ò����滻�����
// һ����ͷ�ļ��и����Լ���Ҫ����/ɾ����Ա���������⣬��Ա��������Ϊָ�����ͣ�
// ������"filelist.h"�滻���Լ���ͷ�ļ���
// ������"myFileList"�滻���Լ����ļ��б���
// �ġ��� �����Լ��ĳ�Ա�����޸�static void delFileEx()
// �塢�� �����Լ��ĳ�Ա�����޸�int addFile()
// ������������Ա�����޸�һֱ
//�����ļ���filelist.h lnklst.h
// ά����¼��2009-05-10 V1.2 by dengke mend some bugs!
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

PFILENODE myFileList = NULL;//����ͷ�����ﶨ��

static unsigned int count;
//========================================================================================
//�������ƣ�delFileEx(PFILENODE file)
//�����������ͷŽڵ���ռ�õĶ��ڴ�ռ�
//����������fileҪ�ͷŵĽڵ�ָ��
//�� �� ֵ��
//===========================================================================================

static void delFileEx(PFILENODE file)//****�˺���Ҫ���ݳ�Ա��Ŀ�����/ɾ����Ա

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
//�������ƣ�destroyFileList(void)
//���������������ʼ���������ڴ�ռ���0
//����������void
//����ֵ��
//=============================================================================================

static void destroyFileList(void)
{
	PFILENODE tmpNod = myFileList;
	while(tmpNod)
	{
		PFILENODE tmp = tmpNod->next;
		delFileEx(tmpNod); //��սڵ���ռ���ڴ�

		tmpNod = tmp;
	}
}


/***********************************����Ϊ�û�ʹ�ýӿ�****************************************/


//=============================================================================================
//�������ƣ�initFileList(void)
//��������������destroyFileList�����������ɳ�ʼ��
//����������
//�� �� ֵ��
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
//�������ƣ�addFile(char *name, char *host,char *ip)
//�����������������в���һ���ڵ�
//����������name��host��ip�ڵ���Ϣָ��
//����ֵ��
//============================================================================================

int addFile(char *name, char *size,char *time,char *type) //ʹ��ͨ������ʱ�˺��������޸�

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

	add_node((void**)&myFileList, newfile); //*****************����Ҫ�޸�*****************

	//pthread_mutext_unlock();

	return 0;
}
//=============================================================================================
//�������ƣ�delFile(PFILENODE file)
//�������������ú���del_node��������ɾ��һ���ڵ�
//����������fileҪ��������ɾ���ڵ�
//����ֵ�� �ɹ�����0��ʧ�ܷ���-1
//==============================================================================================

int delFile(PFILENODE file)
{
	if(file==NULL)
	{
		printf("node is not found!\n");    
		return -1;
	}
	del_node((void**)&myFileList, file); //����Ҫע���޸�

	delFileEx(file);
	count--;
	return 0;
}
//===============================================================================================
//�������ƣ�findFileByName(char *name)
//����������ͨ��name���ҽڵ�
//����������nameҪ���ҵĽڵ�ؼ���
//�� �� ֵ���ҵ��Ľڵ�Ľṹ��ָ��,û�ҵ�Ϊnull
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
//�������ƣ�
//����������ͨ��num���ҽڵ�
//����������
//����ֵ��
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
//�������ƣ�
//������������ӡ�������
//����������FIlE *file����նˣ���ʾ��Ϊ��stdout
//�� �� ֵ������������
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

//�ַ�����������

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
//�������ƣ�trim
//�����������ַ�������ȥ��2�˵Ŀո񣬱��ȫСд(flag==1)
//����������dest��������ַ���ָ�������flag��־(flag==1)���ַ���ת��ΪСд
//����ֵ�� ��
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
//�������ƣ�strSplit

//������������һ�����ַ���split,���ָ��sour���ָ�ɶΣ�ÿ���׵�ַ�����ָ������result
//����������split���ָ��ַ�����ָ�������sour�ָ��ָ�������result�ָ�����ŵ�ָ������
//����ֵ�� int �ָ�Ķ�����
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

//ͨ������
//====================================================================================
//=============================================================
// �ļ����ƣ�lnklst.h
// ����������ͨ������������
// ά����¼��2008-12-25     V1.0
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
// �ļ����ƣ�lnklst.c
// ����������ͨ������������
// ά����¼��2008-12-25     V1.0
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