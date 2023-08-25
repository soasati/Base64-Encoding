//Assignment 1: Base64 encoding system using Client-Server socket programming 
//Group no. : 14

#include<stdio.h>
#include<stdlib.h>
#include<errno.h> // contains the error codes.
#include<netinet/in.h> //contains constants and structures needed for internet domain addresses
#include<string.h>
#include <sys/types.h> //contains definitions of a number of data types used in system calls
#include <sys/socket.h> //contains a number of definitions of structures needed for sockets.
#include <unistd.h> //provides access to the POSIX operating system API
#include<signal.h>
#include<math.h>
#include <arpa/inet.h>//contains definitions for internet operations 
#include<sys/wait.h>

#define LEN 1500 // because lenght will now be 4/3 * lenght of message allowed to the user. For safe side this is taken to be 1500

//Function Declaration
void sigchld_handler(int s);
int findValue(char a);
char *convertToBits(char *inp);
char *decode(char *mg,int len);
void handleClient(int sockfd,struct sockaddr_in *client);

//Driver Function
int main(int argc, char *argv[])
{
	struct sigaction sa;
	struct sockaddr_in client, server;
	int sock = socket(AF_INET,SOCK_STREAM,0);
	if(sock==-1)
	{
		perror("socket");
		exit(-1);
	}
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(atoi(argv[1]));
	bzero(&server.sin_zero,8);
	int len = sizeof(struct sockaddr_in);
    
	//bind()- assigns a local protocol address to a socket(the address is the combination of an IPv4 (32-bit) address along with a 16 bit TCP port number).
    if(bind(sock,(struct sockaddr *) &server, len)==-1)
	{
		perror("bind");
		exit(-1);
	}
	else
	printf("Socket successfully binded..\n");
    //listen()-converting an unconnected socket into a passive socket, indicating that the kernel should accept incoming connection requests directed to this socket. 
	if(listen(sock,5)==-1)
	{
		perror("listen");
		exit(-1);
	}
	else
	printf("Server listening..\n");
	
	sa.sa_handler = sigchld_handler; // reap all dead processes
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1)
	{
		perror("sigaction");
		exit(1);
    	}

        /*To allow the server to handle multiple simultaneous connections, we make the following changes to the code:

          1.Put the accept statement and the following code in an infinite loop.
          2.After a connection is established, call fork() to create a new process.
          3.The child process will close sockfd and call handleclient, passing the new socket file descriptor as an argument. 
              When the two processes have completed their conversation, as indicated by handleclient() returning, this process simply exits.
          4.The parent process closes newsockfd. Because all of this code is in an infinite loop, it will return to the accept statement to wait for the next connection. 
          */

	while(1)
	{
        //accept()-retrieve a connect request and convert that into a request.
		int clientsocket = accept(sock,(struct sockaddr *) &client, (socklen_t*)&len);
		if(clientsocket==-1)
		{
			perror("accept");
			exit(-1);
		}
		else
		printf("server accept the client...\n");
		//checking for handling client process
		switch(fork()) //
		{
			case -1:
				printf("error in fork\n");
				break;
			case 0:
				close(sock);
				handleClient(clientsocket,&client);
				break;
			default:
				close(clientsocket);
		}
	}
	return 0;
}

// handling the child process
void sigchld_handler(int s)
{
    while(waitpid(-1, NULL, WNOHANG) > 0);
}

//Function for Finding the corresponding ASCII values for the encoded letters
int findValue(char a)
{
	int k = (int)a;
	if(k>=65&&k<=90) // A-Z
	{
		return(k-65); // returning the ASCII value (A-Z)
	}
	if(k>=97&&k<=122) //a-z
	{
		return(k-71); // returning the ASCII value (a-z)
	}
	if(k>=48&&k<=57) //0-9
	{
		return(k+4); // returning the ASCII value(0-9)
	}
	if(k==43) /* + */
		return(62);
	if(k==47) /* / */
		return(63);
}

// converting decimal(ASCII) number into bits.
char *convertToBits(char *inp)
{
	char *out = (char*)malloc(24*sizeof(char)); 
	int i,num,j,ind,k;
	for(i=0; i<4; i++)
	{
		ind = 6*i;
		num = findValue(inp[i]);// getting ascii value for the input character by calling findValue().
		for(j = 1<<5; j>0; j=j/2)
		{
			if(num&j) out[ind++] = '1';
			else out[ind++] = '0'; 
		}
	}
	return(out); 
}

//Function for decoding the encoded message from client
char *decode(char *mg,int length)
{
	int k = length - 4,len1,len2,i,j,x,num,m,it;
	char arr[4];
	if(k>0) len1 = (k*3)/4, it=k/4;
	else len1 = 0, it= 0;
	if(mg[length-1]=='='&&mg[length-2]=='=')
		len2 = 1;
	else if(mg[length-1]=='=')
		len2 = 2;
	else
		len2 = 3;
	int outlen=len1 + len2 + 1; // one extra byte is kept for null character
	char *out = (char*)malloc(outlen*sizeof(char));
	out[outlen-1] = '\0';
	//considering 4 bytes at a time
	for(i=0; i<it; i++)
	{
		int y = i*4;
		for(j=y; j< y + 4; j++) 
		{
			arr[j-y] = mg[j];
		}
		//converting bytes to bits
		char *bits= convertToBits(arr); 
		//decoding the bits
		for(x=0;x<3;x++)
		{
			num = 0;
			m = x*8;
			for(j = 7+m; j>=m; j--)
			{
				num += (bits[j]-'0')*(pow(2,7+m-j));
			}
			out[3*i+x] = (char)num;
		}
	}
	// now we have to look after the last 4 bytes of the mg which may contain =
	j = it*4;
	for(i=j;i<length;i++)
	{
		arr[i-j] = mg[i];
	} 
	char *bits= convertToBits(arr);
	for(x=0;x<len2;x++)
	{
		m = x*8;
		num = 0;
		for(j = 7+m; j>=m; j--)
		{
			num += (bits[j]-'0')*(pow(2,7+m-j));
		}
		char r = num;
		out[it*3+x] = r;
	}
	return(out);
	
}
/*handle client() is a separate instance of this function 
 for each connection.  It handles all communication
 once a connnection has been established.
 */

void handleClient(int sockfd,struct sockaddr_in *client)
{
	char mg[LEN];
	int length;
	char *ip = inet_ntoa(client->sin_addr);
	int port = client->sin_port;
	while(1)
	{
		int i = 0;
		int st = read(sockfd,mg,LEN);
		write(1,"Message Received from client ",strlen("Message Received from client "));
		printf("%s:%d - ",ip,port);
		fflush(stdout);
		write(1,mg,st);
		write(1,"\n",1);
		char *dMsg = decode(mg,st);
		
		if(dMsg[0]=='2')
		{
			write(1,"Decoded Message from client ",strlen("Decoded Message from client "));
			printf("%s:%d - ",ip,port);
			fflush(stdout);
			write(1,"Terminate connection",strlen("Terminate connection"));
			write(1,"\n",1);
			break;
		}
		else
		{
			write(1,"Decoded Message from client ",strlen("Decoded Message from client "));
			printf("%s:%d - ",ip,port);
			fflush(stdout);
			length = strlen(dMsg);
			dMsg[0] = '\0';
			write(1,dMsg,length);
			write(1,"\n",1);
		}
		write(sockfd,"Message received",strlen("Message received"));
	}
	close(sockfd);
	exit(0);
}
