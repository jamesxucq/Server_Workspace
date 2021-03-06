#pragma once

//使用到的结构 
#pragma	pack(1)

struct Sock4Req
{ 
	char VN; 
	char CD; 
	unsigned short Port; 
	unsigned long IPAddr; 
	char other[1]; 
}; 

struct Sock4Ans
{ 
	char VN; 
	char CD; 
}; 

struct Sock5Req1
{ 
	char Ver; 
	char nMethods; 
	char Methods[255];
}; 

struct Sock5Ans1
{ 
	char Ver; 
	char Method; 
}; 

struct Sock5Req2
{ 
	char Ver; 
	char Cmd; 
	char Rsv; 
	char Atyp; 
	char other[1]; 
}; 

struct Sock5Ans2
{ 
	char Ver; 
	char Rep; 
	char Rsv; 
	char Atyp; 
	char other[1]; 
}; 

struct AuthReq 
{ 
	char *Ver; 
	char *Ulen; 
	char *Name; 
	char *PLen; 
	char *Pass; 
}; 

struct AuthAns 
{ 
	char Ver; 
	char Status; 
}; 

#pragma	pack()


namespace NProxyConnect
{
///////////////////////////////////////////////////////////////////////	

//return value:0=success; -1=connect failed; -2=wrong usr and psw
	int ConnectAuthHttp(SOCKET ProxySocket,char *sDestiServ,int iDestiPort,char *usr,char *psw);
	int ConnectHttp(SOCKET ProxySocket,char *sDestiServ,int iDestiPort);

//return value:0=success; -1=connect failed;
	int ConnectSocks4(SOCKET ProxySocket,char *sDestiServ,int iDestiPort);

//return value:0=success; -1=connect failed; -2=wrong usr and psw
	int ConnectSocks5(SOCKET ProxySocket,char *sDestiServ,int iDestiPort,char *usr,char *psw);
};

namespace NDetectProxy
{
	int GetIEPorxyServ(TCHAR *sProxyServ, int *iProxyPort); //return 0 succ; -1 err;
};