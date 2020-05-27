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

static pthread_mutex_t mutex= PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond=PTHREAD_COND_INITIALIZER; 
struct sockaddr_in serverAddr;
char data[1024];
char output[1024];

struct user{

	int serial , socket_num , length , flag;
	struct sockaddr_in socketaddressClient;

};



void success(){
	printf("listening is successful, enjoy...\n");
}


int clientCount=0;
struct user Client[1024];
pthread_t thread[1024];

int checkWhetherString(int l, int i, char *output){
	l += snprintf(output + l,1024,"Client %d is at socket %d.\n",i + 1,Client[i].socket_num);
		return l;	

}

int findidentity(int givenSocket,struct user *abc , int count){

	int result;
	for(int i=0;i<count;i++){
		if(abc[i].socket_num==givenSocket){
			result=i+1;
			return result;
		}
	}
	return 0;
}

// _Bool checkinputnum(char * data , int read){

// 	for(int i=0;i<read;i++){
// 				if(isdigit(data[i]==0)){
// 					continue;
// 				}
// 				else{
// 					return 0;
// 				}
// }

// return 1;
// }

void checkeer(int read){
	data[read] = '\0';
}

void * connectToNetwork(void * ClientDetail){

	struct user * clientDetail = (struct user*) ClientDetail;
	int serial = clientDetail->serial;
	int clientSocket = clientDetail->socket_num;

	printf("Client %d connected.\n",serial+ 1);
	while(1){


		
		int read = recv(clientSocket,data,1024,0);
		checkeer(read);
		

		

		if(strcmp(data,"ONLINE") == 0){

			int l = 0;

			int i=0;
			while(i<clientCount){

					if(i != serial)
					l = checkWhetherString(l,i,output);
				i++;
			}

			send(clientSocket,output,1024,0);
			continue;

		}
		if(data[0]=='S' && data[1]=='E' && data[2]=='N' && data[3]=='D' && data[4]=='M' && data[5]=='S' && data[6]=='G'){

			read = recv(clientSocket,data,1024,0); // Here clientsocket is the socket number of the incoming message sender
			checkeer(read);
			
			int id = *data - '0';
			id=id-1;

			read = recv(clientSocket,data,1024,0);
			checkeer(read);

			char buf[1024];

			if(Client[id].socket_num==clientSocket){
				snprintf(buf,1024,"Heyy man , get a life! Don't message yourself :p ");
				send(Client[id].socket_num,buf,1024,0);
			}

			else{
				snprintf(buf,1024,"Personal Message sent to you by %d : %s",findidentity(clientSocket,Client,clientCount),data);

			send(Client[id].socket_num,buf,1024,0);
			}
						

			// 	else{
			// 		char buf[1024];
			// 		snprintf(buf,1024,"Please send the recievers number");
			// send(clientSocket,buf,1024,0);		
			// 	}

		}

		if(strcmp(data,"SENDALL") == 0){

			// /id=0; 
			pthread_mutex_lock(&mutex);
			read = recv(clientSocket,data,1024,0);
			data[read] = '\0';
				for(int i=0;i<clientCount;i++)
				{
					if(Client[i].socket_num!=clientSocket){
					char buf[1024];
					snprintf(buf,1024,"Global Message sent to you by %d : %s",findidentity(clientSocket,Client,clientCount),data);
					send(Client[i].socket_num,buf,1024,0);
						}
					}
					pthread_mutex_unlock(&mutex);		

		}

	}

		// if(strcmp(data,"S0ND") != 0 && strcmp(data,"ONLINE") != 0 && strcmp(data,"SEND") != 0){ // Error condition (if message is typed without any of the three commands)
		// 	char buf[1024];
		// 	//printf("Hello\n");
		// 	//snprintf(buf,1024,"Heyy man , if you wanna send message send us a valid command!:p ");
		// 	send(clientSocket,buf,1024,0);

		// }
		return NULL;
	}



int main(){

	int serverSocket = socket(PF_INET, SOCK_STREAM, 0);



	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(8080);
    serverAddr.sin_addr.s_addr = htons(INADDR_ANY);

	int bindnum = bind(serverSocket,(struct sockaddr *) &serverAddr , sizeof(serverAddr));
	if(bindnum==-1){
		printf("Error in binding\n" );
		return 0;
	}
	else{
		printf("binding is successful\n");
	}	
	int listennum = listen(serverSocket,1024);
	if(listennum==-1){
		printf("Error in listening\n" );
		return 0;
	}
	else{
		success();
	}


	while(1){

		Client[clientCount].socket_num = accept(serverSocket, (struct sockaddr*) &Client[clientCount].socketaddressClient, &Client[clientCount].length);
		Client[clientCount].serial= clientCount;
		//Client[clientCount].flag= 0;
		pthread_create(&thread[clientCount], NULL, connectToNetwork, (void *) &Client[clientCount]);

		clientCount ++;

 
	}

	int i=0;
	while(i<clientCount){
		pthread_join(thread[i],NULL);
		i++;
	}


}

