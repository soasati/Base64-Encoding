//Assignment 1: Base64 encoding system using Client-Server socket programming 
//Group no. : 14

#include<stdio.h>
#include<stdlib.h>
#include<errno.h> // contains the error codes.
#include<string.h>
#include<sys/types.h> //contains definitions of a number of data types used in system calls
#include <sys/socket.h>  //contains a number of definitions of structures needed for sockets.
#include <arpa/inet.h> //contains definitions for internet operations 
#include <unistd.h> //close
#include<math.h>

//constant definition
#define LEN 1025

//Function Declaration
char *convertToBits(char *inp,int padded);
char* encodeThis(char *inp,int padded);
char* encode(char *mg);
int convertToNumber(char *ptr);
void initialise(char *p);


//Driver Function
int main(int argc, char *argv[])
{
    //Initializing variables
	char response[2]; 
    //initializing socket address structure
	struct sockaddr_in server;
    // Creating Client socket
	int csocket = socket(AF_INET,SOCK_STREAM,0);
	if(csocket==-1)
	{
		perror("client socket error");
		exit(-1);
	}
    //converting port number from char array to integer
	int pt = convertToNumber(argv[2]);

    //setting port number
	server.sin_port = htons(pt);

    //Setting Address family - IPv4
	server.sin_family = AF_INET;
	if(inet_aton(argv[1],&server.sin_addr)==0)
	{
		printf("IP Address error\n");
	}
    
    //Connecting to the server
	int len = sizeof(struct sockaddr_in);
	if(connect(csocket,(struct sockaddr *) &server, len)==-1)
	{
		perror("connect ");
		printf("%d\n",errno);
		exit(-1);
	}
	else
	printf("connected to the server..\n");
	char mg[LEN+1];// one extra is for appending null character to the mesage
	while(1)
	{
		initialise(mg); // intiialise the message with null characters
		//checking with user if connection is to be continued or terminated
		write(1,"Do you want to close the connection? - enter y or n and press enter\n",strlen("Do you want to close the connection? - enter y or n and press enter\n"));
		read(0,response,2);
		//If user asks for connection termination
		if(response[0]=='y')
		{
			mg[0] = '2';
			char *ptr = encode(mg);
			write(csocket,ptr,strlen(ptr));
			write(1,"Connection terminated\n",strlen("Connection terminated\n"));
			break;
		}
		//Else ask user to enter the message to be sent to server
		write(1,"Type your message - ",strlen("Type your message - "));
		mg[0] = '1';
		int i=1;
		while(1)
		{
			read(0,response,1);
			if(response[0]=='\n'||i==LEN)// because mg[LEN-1] always contain the null character and this indiates that lenght of 	
				break;		     //message has reached 1024 byes.
			mg[i++] = response[0];
		}
		//encode the message from user 
		char *ptr = encode(mg);
		//send the encoded message to the server
		write(csocket,ptr,strlen(ptr));
		//read the reply message from the server - acknowledgement message
		int st = read(csocket,mg,30);
		write(1,"Message from server ",strlen("Message from server "));
		printf("%s:%d - ",argv[1],pt);
		fflush(stdout);
		//print message at client terminal
		write(1,mg,st);
		write(1,"\n",1);
	}
	//close the client socket in the end
	close(csocket);
	return 0;
}

//Function to convert the data in bytes to bits
char *convertToBits(char *inp,int padded)
{
	//initialize a dynamic char array to store encoded output
	char *out = (char*)malloc(24*sizeof(char)); //convert 25 to 24
	int i,num,j,ind,k;
	//loop to convert 3 bytes to bits 
	for(i=0,k=0; i<3,k<(3-padded); i++,k++)
	{
		num = (int)inp[i];
		ind = i*8;
	    	for (j = 1 << 7; j > 0; j = j / 2)
	    	{
			if(num & j) out[ind++] = '1';
			else out[ind++] = '0';
		}
	}
	for(k=k*8;k<24;k++)
		out[k] = '0';
	return(out);
}

//Function to encode the message to be sent to the server
char* encodeThis(char *inp,int padded)
{
	//declaring the variables
	int it,i,j,ind,num,k;
	//checking for padding
	for(i=0; i<3; i++)
		if(padded == i)
			it = 4 - i;
	//converting the bytes to bits
	char *bits = convertToBits(inp,padded);
	
	char *out = (char*)malloc(4*sizeof(char));
	//Taking 6 bits at a time and converting to corresponding encoding
	for(i=0; i<it; i++)
	{
		ind = i*6;
		num = 0;
		for(j = ind+5 ; j>=ind; j--)
		{
			k = pow(2,ind+5-j);
			num += (bits[j]-'0')*k;
		}
		if(num>=0&&num<=25)
			out[i] = (char)(num + 65);
		if(num>=26&&num<=51)
			out[i] = (char)(num + 71);
		if(num>=52&&num<=61)
			out[i] = (char)(num-4);
		if(num==62)
			out[i] = '+';
		if(num==63)
			out[i] = '/';
	}
	//checking if any padding required and inserting =  accordingly
	while(it<4)
		out[it++] = '=';
		
	return(out);
}

//Main Function to encode the user message
char* encode(char *mg)
{
	//computing the length and variable declaration
	int len = strlen(mg);
	int rem = len%3,p;
	int pad,zeroes;
	//checking for padding required to complete the format of 24 bits
	if(rem==1) pad = 2;
	if(rem==2) pad =1;
	if(rem==3) pad = 0;
	if(rem==0) p = 0;
	else p = 4;  
	int outlen = 4*(len-rem)/3 + p + 1;// extra 1 is for null character indicate the end of encoding

	//creating the dynamic char array to store the output
	char* output = (char*)malloc(outlen*sizeof(char));
	output[outlen] = '\0';
	char *en, arr[3];
	
	int i,j,count=0;
	//checking every 3 bytes of message and converting to corresponding base64 encoding
	for(i=0; i<len; i = i+3)
	{
		//if padding is 2 then insert two = signs
		if(i+1==len)
		{
			arr[0] = mg[i];
			arr[1] = '=';
			arr[2] = '=';
			zeroes = 2;
		}
		//if padding is 1 then insert one  = sign
		else if(i+2==len)
		{
			arr[0] = mg[i];
			arr[1] = mg[i+1];
			arr[2] = '=';
			zeroes = 1;
		}
		//if padding is 0 then no need to insert
		else
		{
			arr[0] = mg[i];
			arr[1] = mg[i+1];
			arr[2] = mg[i+2];
			zeroes = 0;
		}
		//send the array to encode the message
		//this function will first convert the bytes to bits and then encode the message to base64 encoding
		en = encodeThis(arr,zeroes);
		//write the encoded message to output array
		for(j=0; j<4; j++)
			output[count*4+j] = en[j];
		count++;
	}
	
	return(output);
}

//Function to convert the string containing IP address to integer type
int convertToNumber(char *ptr)
{
	int ans=0,i = 0;
	int len = strlen(ptr);
	for(i=0; i<len; i++)
	{
		ans = ans*10 + (ptr[i] - '0');
	}
	return(ans);
}

//Function to initialize all the characters in the array to null characters
void initialise(char *p)
{
	int i;
	for(i=0; i<LEN+1; i++)
	{
		p[i] = '\0';
	}
}
