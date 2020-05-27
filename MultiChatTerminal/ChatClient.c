#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <signal.h>

char data[1024];
char input[1024];

void process(int read){
	data[read] = '\0';
	printf("%s\n",data);
}

void wrongExitError(){

	printf("Hey if you wanna exit please exit using the words/command 'EXIT' \n");

}

void * doRecieving(void * sockID){

	int clientSocket = *((int *) sockID);

	for(int i=10; i>0; i=i-1){

		
		int read = recv(clientSocket,data,1024,0);
		process(read);
		i++;
	}

}

void get_input(int clientSocket){

	send(clientSocket,input,1024,0);
	scanf("%[^\n]s",input);
	send(clientSocket,input,1024,0);
}

int main(){

	int clientSocket = socket(PF_INET, SOCK_STREAM, 0);

	struct sockaddr_in serverAddr;

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(8080);
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	//serverAddr.sin_addr.s_addr =inet_addr("127.0.0.1");

	int connectnum = connect(clientSocket, (struct sockaddr*) &serverAddr, sizeof(serverAddr));
		if(connectnum==-1){
			printf("Connection Failed");
			return 0;
		}
		else{
			printf("Connection Succcessful");
		}

	printf("Connection is established , Welcome to V-Chat ............\n");
	//printf("%d\n",clientSocket );
	pthread_t thread;
	pthread_create(&thread, NULL, doRecieving, (void *) &clientSocket );
	int f=0;
	while(1){
		//memset(input,0,1024);

		signal(SIGINT,wrongExitError);

		
		scanf("%s",input);

		signal(SIGINT,wrongExitError);

		if(strcmp(input,"ONLINE") == 0){
			//printf("%d\n",clientSocket );
			send(clientSocket,input,1024,0); // Here clientSocket is the server address

		}
		else if(input[0]=='S' && input[1]=='E' && input[2]=='N' && input[3]=='D' && input[4]=='M' && input[5]=='S' && input[6]=='G'){
			send(clientSocket,input,1024,0);
			scanf("%s",input);
			get_input(clientSocket);
		}


		else if(input[0]=='S' && input[1]=='E' && input[2]=='N' && input[3]=='D' && input[4]=='A' && input[5]=='L' && input[6]=='L'){
			get_input(clientSocket);
		}

		else if(strcmp(input,"EXIT") == 0){

			printf("Exiting from V-Chat\n");
			send(clientSocket,input,1024,0);
			exit(0);
		}

		else if(strcmp(input,"SENDALL") != 0 || strcmp(input,"ONLINE") != 0 || strcmp(input,"SENDMSG") != 0){ // Error condition (if message is typed without any of the three commands)
			//char buf[1024];
			printf("Heyy man , if you wanna send message send us a valid command!:p\n");
			//snprintf(buf,1024,"Heyy man , if you wanna send message send us a valid command!:p ");
			//send(clientSocket,buf,1024,0);

		}

		signal(SIGINT,wrongExitError );

	}
// 	if(f==1){
// 		exit(0);
// 	}
}