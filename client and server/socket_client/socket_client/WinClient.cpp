#include <winsock2.h>
#include "stdafx.h"
#include <windows.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <conio.h>


int getsocket()
{
	int hsock;
	int * p_int ;
	hsock = socket(AF_INET, SOCK_STREAM, 0);
	if(hsock == -1){
		printf("Error initializing socket %d\n",WSAGetLastError());
		return -1;
	}
	
	p_int = (int*)malloc(sizeof(int));
	*p_int = 1;
	if( (setsockopt(hsock, SOL_SOCKET, SO_REUSEADDR, (char*)p_int, sizeof(int)) == -1 )||
		(setsockopt(hsock, SOL_SOCKET, SO_KEEPALIVE, (char*)p_int, sizeof(int)) == -1 ) ){
		printf("Error setting options %d\n", WSAGetLastError());
		free(p_int);
		return -1;
	}
	free(p_int);

	return hsock;
}
void socket_client()
{

	//The port and address you want to connect to
	int host_port= 1101;
	char* host_name="127.0.0.1";

	//Initialize socket support WINDOWS ONLY!
	unsigned short wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD( 2, 2 );
 	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 || ( LOBYTE( wsaData.wVersion ) != 2 ||
		    HIBYTE( wsaData.wVersion ) != 2 )) {
	    fprintf(stderr, "Could not find sock dll %d\n",WSAGetLastError());
		goto FINISH;
	}

	//Initialize sockets and set any options

	//Connect to the server
	struct sockaddr_in my_addr;

	my_addr.sin_family = AF_INET ;
	my_addr.sin_port = htons(host_port);
	
	memset(&(my_addr.sin_zero), 0, 8);
	my_addr.sin_addr.s_addr = inet_addr(host_name);

	//if( connect( hsock, (struct sockaddr*)&my_addr, sizeof(my_addr)) == SOCKET_ERROR ){
	//	fprintf(stderr, "Error connecting socket %d\n", WSAGetLastError());
	//	goto FINISH;
	//}

	//Now lets do the client related stuff
	char buffer[1024], buffer1[1024];
	int buffer_len = 1024, buffer_len1 = 1024;
	int bytecount, len;
	int c;
	FILE *fp;
	fp = fopen("C://Users//Lenovo//Desktop//output.txt", "a+");
	int j = 0;
	char ch;


	while(true) {

		int hsock = getsocket();
		//add error checking on hsock...
		if( connect(hsock, (struct sockaddr*)&my_addr, sizeof(my_addr)) == SOCKET_ERROR ){
			fprintf(stderr, "Error connecting socket %d\n", WSAGetLastError());
			goto FINISH;
		}

		memset(buffer, '\0', buffer_len);
		/*printf("Enter your message to send here\n");
		for(char* p=buffer ; (c=getch())!=13 ; p++){
			printf("%c", c);
			*p = c;
		}*/
		fseek(fp, 0, SEEK_SET);
		char ch;
		/*if (!(fp = fopen("C://Users//Lenovo//Desktop//output.txt", "r"))){
			printf("File could not be opened to retrieve your data from it.\n");
		}*/
		while (!feof(fp))
		{
			ch = fgetc(fp);
			
			if (ch != '\n' && ch != ' ' && ch!=',')
				buffer[j++] = ch;
		}
		buffer[j] = '\0';
		printf("%s", buffer);
		printf("sending to server\n");
		if( (bytecount=send(hsock, buffer, strlen(buffer),0))==SOCKET_ERROR){
			fprintf(stderr, "Error sending data %d\n", WSAGetLastError());
			goto FINISH;
		}
		printf("Sent bytes %d\n", bytecount);
		memset(buffer, '\0', buffer_len);
		if((bytecount = recv(hsock, buffer, buffer_len, 0))==SOCKET_ERROR){
			fprintf(stderr, "Error receiving data %d\n", WSAGetLastError());
			goto FINISH;
		}
		printf("Recieved bytes %d\nReceived string \"%s\"\n", bytecount, buffer);
		//printf("%s\n",buffer);
		fseek(fp, 0, SEEK_END);
		len = strlen(buffer);
		for (int i = 0; i < len; i = i + 2)
			fprintf(fp, "\n(%d,%d)\n", buffer[i]-'0', buffer[i + 1]-'0');

		fclose(fp);
		closesocket(hsock);
	}

	//closesocket(hsock);
FINISH:
;
}