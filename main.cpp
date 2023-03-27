#include <stdio.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <pthread.h>



#ifdef _WIN64
//	#pragma comment(lib, "ws2_32.lib") 
//	#pragma comment(lib, "wsock32.lib")
//	#define WIN32_LEAN_AND_MEAN
	#include <winsock2.h>	
	//self computer ip address 
	#define IP_ADDR "127.0.0.1"
	//#include <windows.h>
#elif __linux__
	#include <arpa/inet.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	//wireless hotspot ip address
	#define IP_ADDR "192.168.4.1"

#else
#endif

#include "mysql.h"
#include "interaction.h"
#include "oneIO.h"

#define MAXSIZE 1024

#define IP_PORT 8888
//#define MY_ADDR "172.17.0.2"
//extern int usedIO = 0;

int main()
{
		//打开数据库 
		MYSQL mysql;
		mysql_init(&mysql);
		
		//详情请查看mysql_real_connect()接口说明，主机，主机用户名，密码，数据库名称，接口 
		if(!mysql_real_connect(&mysql,"xxxxxxxxx","xxxx","xxxxxxxxxxxxxxxx","xxxxxxxxx",3306,nullptr,0)){
			printf("The database connection failed.\n");
			return -1;
		}
		else
			printf("The database connection success.\n");
			
		//服务器开启侦听端口 
#ifdef _WIN64
		WORD sockVersion = MAKEWORD(2,2);
		WSADATA wsaData;  
		printf("Main function in WIN64\n");
    	if(WSAStartup(sockVersion, &wsaData)!=0)  
    	{  
        	return 0;  
    	}  
#else
#endif   	
        SOCKET i_listenfd,i_connfd;
        struct sockaddr_in my_sersock;
        char msg[MAXSIZE];
        int nrecvSize = 0;

        if((i_listenfd = socket(AF_INET,SOCK_STREAM,0) )<0)
        {
                printf("socket Error: %s (errno: %d)\n", strerror(errno), errno);
                exit(0);
        }
        
        //设置地址重用
		int opt;
		opt = 1;
		setsockopt(i_listenfd,SOL_SOCKET,SO_REUSEADDR,(char*)&opt,sizeof(opt)); 

		//配置地址结构 
        memset(&my_sersock, 0, sizeof(my_sersock));
        my_sersock.sin_family = AF_INET;
        my_sersock.sin_addr.s_addr = inet_addr(IP_ADDR);
        my_sersock.sin_port = htons(IP_PORT);

        if(bind(i_listenfd,(struct sockaddr*)&my_sersock,sizeof(my_sersock))<0)
        {
                printf("bind Error: %s (errno: %d)\n", strerror(errno), errno);
                exit(0);
        }

        if(listen(i_listenfd, 20) < 0)
        {
                printf("listen Error: %s (errno: %d)\n", strerror(errno), errno);
                exit(0);
        }

        
        
        
        while(1){
        	//等待连接 
        	printf("======waiting for client[%d] request======\n",usedIO);
        	
        	struct sockaddr_in clientAddr;
        	int addrLen = sizeof(clientAddr);
        	memset(&clientAddr,0,sizeof(clientAddr));
        	i_connfd = accept(i_listenfd,(struct sockaddr*)&clientAddr,&addrLen);
        	
        	if(i_connfd < 0){
                printf("accept error: %s (errno: %d)\n",strerror(errno),errno);
                exit(0);
        	}
        	//成功连接 
        	else{
        		
        		pthread_t tid;
        		OneIO* user;
        		
                printf("Client[%d],welcome!\n",i_connfd);
                printf("remote address:%s,port:%d\n",
				inet_ntoa(clientAddr.sin_addr),ntohs(clientAddr.sin_port));
				
				user = (OneIO*)malloc(sizeof(OneIO));
	        	user->mysql =mysql;
	        	user->i_connfd = i_connfd;
	        	//创建新的线程 
	        	pthread_create(&tid,NULL,workPart,(void*)user);
	        	pthread_detach(tid);
        	}
        		
			//workPart(i_connfd,mysql);
        	
		}
		mysql_close(&mysql);
        close(i_connfd);
        close(i_listenfd);

        return 0;
}


