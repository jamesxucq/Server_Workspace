// multi-sockets.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"



#include <WINSOCK2.H>
#pragma comment(lib,"WS2_32.LIB")

DWORD WINAPI TestThreadOne(LPVOID lparameter)
{
    WSAData wsa;
    WSAStartup(MAKEWORD(2,2),&wsa);

    SOCKET m_socket=socket(AF_INET,SOCK_STREAM,0);
    
    SOCKADDR_IN addr;
    addr.sin_family=AF_INET;
    addr.sin_port=htons(80);
    addr.sin_addr.S_un.S_addr=inet_addr("202.108.22.5");
    
    if(connect(m_socket,(SOCKADDR *)&addr,sizeof(SOCKADDR))!=SOCKET_ERROR)
    {
        printf("连接百度成功! thread one..\n");
    }
    else
    {
        printf("连接百度失败! thread one..\n");
    }
    closesocket(m_socket);

    WSACleanup();
    return 1;
}

DWORD WINAPI TestThreadTwo(LPVOID lparameter)
{
   // WSAData wsa;
   // WSAStartup(MAKEWORD(2,2),&wsa);

    SOCKET m_socket=socket(AF_INET,SOCK_STREAM,0);
    
    SOCKADDR_IN addr;
    addr.sin_family=AF_INET;
    addr.sin_port=htons(80);
    addr.sin_addr.S_un.S_addr=inet_addr("202.108.22.5");
    
    if(connect(m_socket,(SOCKADDR *)&addr,sizeof(SOCKADDR))!=SOCKET_ERROR)
    {
        printf("连接百度成功! thread two..\n");
    }
    else
    {
        printf("连接百度失败! thread two..\n");
    }
    closesocket(m_socket);

   // WSACleanup();
    return 1;
}

int _tmain(int argc, _TCHAR* argv[])
{
    WSAData wsa;
    WSAStartup(MAKEWORD(2,2),&wsa);
    
    CloseHandle(CreateThread(NULL,NULL,TestThreadOne,NULL,NULL,NULL));
    
    SOCKET m_socket=socket(AF_INET,SOCK_STREAM,0);
    
    SOCKADDR_IN addr;
    addr.sin_family=AF_INET;
    addr.sin_port=htons(80);
    addr.sin_addr.S_un.S_addr=inet_addr("202.108.22.5");
    
    if(connect(m_socket,(SOCKADDR *)&addr,sizeof(SOCKADDR))!=SOCKET_ERROR)
    {
        printf("连接百度成功! main thread..\n");
    }
    else
    {
        printf("连接百度失败! main thread..\n");
    }
    closesocket(m_socket);
    // WSACleanup();

    Sleep(5000);
    CloseHandle(CreateThread(NULL,NULL,TestThreadTwo,NULL,NULL,NULL));

    Sleep(20000);
	WSACleanup();

    return 0;
}


