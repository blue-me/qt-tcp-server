#include <stdio.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

//#include <winsock.h>
#ifdef _WIN64
//	#define WIN32_LEAN_AND_MEAN
	#include <winsock2.h>
//	#include <windows.h>
#elif __linux__
	#include <arpa/inet.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
#else
#endif	

//#pragma comment(lib, "ws2_32.lib") 
//#pragma comment(lib, "wsock32.lib")

#include "interaction.h"
#include "message.h"
#include "sql.h"
#include "oneIO.h"

#define INTER_MAXSIZE 1024

int usedIO = 0;

void *workPart(void *user)
{
	char receiveBuffer[INTER_MAXSIZE];
	char sendBuffer[INTER_MAXSIZE];
	SOCKET i_connfd = ((OneIO*)user)->i_connfd;
	MYSQL mysql = ((OneIO*)user)->mysql;
	//char userName[4]="123";
	//char userPassword[4]="123";
	double amount = 0;
	int nrecvSize=0;
	int accumulateError = 0;
	
	while(1){
        memset(receiveBuffer,0,sizeof(receiveBuffer));
        memset(sendBuffer,0,sizeof(sendBuffer));
        
        nrecvSize = recv(i_connfd,receiveBuffer,INTER_MAXSIZE,0);
        Message *receiveMessage = (Message*)receiveBuffer;
        Message *sendMessage= (Message*)sendBuffer;
        
        if(nrecvSize<0) 
        {
            printf("accept error: %s (errno: %d)\n",strerror(errno),errno);
            if(accumulateError < 10){
            	++accumulateError;
            	continue;
			}
			else {
				printf("Connection Wrong!\n");
				break;
			}
            
        }
        else if(nrecvSize == 0)
        {
            printf("client has disconnected!\n");
            close(i_connfd);
            --usedIO; 
            break;
        }
        else
        {
        	accumulateError=0;
        	
        	char* userName = receiveMessage->info_name;
			char* userPassword = receiveMessage->info_password;
			bool loginSig = false;

        	switch(receiveMessage->type){
        		//��½�׶Σ��ʺŻ�������������⣬�����������ֱ�ӷ��� 
        		case '0':{
        			//�ȼ���ʺŴ治���ڣ�Ȼ���ټ������Բ���
        			++usedIO;
					loginSig = userIsExist(userName,mysql);
					if(!loginSig){
						sendMessage->type='5';
						printf("User Wrong!\n"); 
		        		sendMessage->status=false;
					}
					//�ʺŴ��ڣ�������Դ��ѯ 
					else{
						loginSig = passwordIsRight(userName,userPassword,mysql);
						if(!loginSig){
							sendMessage->type='5';
							printf("User Wrong!\n"); 
			        		sendMessage->status=false;
						}
						else{
							//�ʺ�������ȷ�����ص�ǰ��� 
							printf("Logined Success!\n");
							memcpy(sendMessage->info_name,userName,16);
							amount = checkAmount(userName,mysql);
							
							sendMessage->type='0';
							sendMessage->number = amount;
							sendMessage->status=true;
							
						}
					}
					break;
				}
				
				//�����˻����� 
				case '1':{
					loginSig = userIsExist(userName,mysql);
					if(loginSig){
						//�ʺ��Ѵ��� 
						sendMessage->type='5';
						printf("The account exists!\n"); 
		        		sendMessage->status=false;
					}
					else{
						printf("Create the account\n");
						createAccount(userName,userPassword,mysql);
						sendMessage->type='2';
						sendMessage->status=true;
					}
					break;
				}
				
				//��ѯ��ǰ������ 
				case '2':{
					memcpy(sendMessage->info_name,userName,16);
					amount = checkAmount(userName,mysql);
					
					sendMessage->type='2';
					sendMessage->number = amount;
					sendMessage->status=true;
	
	        		printf("The Check Amount:%f\n",amount);
					break;
				}
				
				//�����˻������� 
				case '3':{
					double bias = receiveMessage->number;
					amount = checkAmount(userName,mysql);
					amount += bias;
					//���ĳɹ� 
					if(changeAmount(userName,amount,mysql) && amount != (-1+bias) ){
						sendMessage->type='3';
	        			sendMessage->status=true;
	        			sendMessage->number=amount;
	        			printf("The changeAmount success:%f\n",amount); 
					}
					//����ʧ�� 
					else{
						sendMessage->type='3';
	        			sendMessage->status=false;
	        			sendMessage->number=bias;
						printf("The changeAmount failed:%f\n",bias);
					}	
					break;
				}
				
				//������
				case '7':{
					loginSig = userIsExist(userPassword,mysql);
					if(!loginSig){
						sendMessage->type='5';
						printf("Target Wrong!\n"); 
		        		sendMessage->status=false;
					}
					else{
						double hostAmount = checkAmount(userName,mysql);
						double targetAmount = checkAmount(userPassword,mysql);
						double bias = receiveMessage->number;
						hostAmount -= bias;
						targetAmount += bias;
						if(changeAmount(userName,hostAmount,mysql) && changeAmount(userPassword,targetAmount,mysql)){
							sendMessage->type='7';
		        			sendMessage->status=true;
		        			sendMessage->number=hostAmount;
		        			printf("The transfer success\n"); 
						}
						else{
							sendMessage->type='5';
		        			sendMessage->status=false;
		        			sendMessage->number=bias;
							printf("The changeAmount failed\n");
						}
					}
					break;
				}		
			}
			
			memcpy(sendBuffer,sendMessage,sizeof(Message));
			send(i_connfd,sendBuffer,sizeof(sendBuffer),0);
			printf("client sending %d bytes\n",nrecvSize);
        }  	
    }
    
}

