#include "stdafx.h"
#include <winsock2.h>
#include <windows.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
struct node
{
	char msg[128];
	int msg_id;
	node *next;
}*flist,*alist,*printid;

struct bufserv{
	
		int userId;
		int forumId;
		int msgId;
		int commentId;
		int choice;
		char *forumname;
		char msg[128];
}buf1;
int isMaze(int *maze, int rows, int columns, int x1, int y1, int x2, int y2);
int isFinal(int *maze, int rows, int columns, int x1, int y1, int x2, int y2, int *arr);
bool flag=true;
int mid = 0;
int count1 =0;
char *Data[100];
int count=1;
int values[100];

int index = 0;
int path[50] = { 0 };
DWORD WINAPI SocketHandler(void*);
void replyto_client(char *buf, int *csock);
int path_exists(int *maze, int rows, int columns, int x1, int y1, int x2, int y2)
{
	int arr[30] = { 0 };
	return isFinal(maze, rows, columns, x1, y1, x2, y2, arr);
}
int isFinal(int *maze, int rows, int columns, int x1, int y1, int x2, int y2, int *arr){

	if (isMaze(maze, rows, columns, x1, y1, x2, y2) == 1)
	{
		if ((x1 == x2 && y1 == y2))
		{
			return 1;
		}
		else if (*((int *)arr + (x1*columns + y1)) == 1)
			return 0;
		if (isFinal(maze, rows, columns, x1, y1 + 1, x2, y2, arr) == 1) // move down
		{
			printf("(%d,%d)->", x1, y1 + 1, x2, y2);
			path[index++] = x1;
			path[index++] = y1 + 1;
			return 1;
		}
		if (isFinal(maze, rows, columns, x1 + 1, y1, x2, y2, arr) == 1) // move right
		{
			printf("(%d,%d)->", x1 + 1, y1, x2, y2);
			path[index++] = x1 + 1;
			path[index++] = y1;
			return 1;
		}
		(*((int *)arr + (x1*columns + y1))) = 1;
		if (isFinal(maze, rows, columns, x1, y1 - 1, x2, y2, arr) == 1) // move up
		{
			printf("(%d,%d)->", x1, y1 - 1, x2, y2);
			path[index++] = x1;
			path[index++] = y1 - 1;
			return 1;
		}
		if (isFinal(maze, rows, columns, x1 - 1, y1, x2, y2, arr) == 1) // move left
		{
			printf("(%d,%d)->", x1 - 1, y1, x2, y2);
			path[index++] = x1 - 1;
			path[index++] = y1;
			return 1;
		}
	}
	return 0;
}
int isMaze(int *maze, int rows, int columns, int x1, int y1, int x2, int y2)
{
	if (rows >0 && columns >0 && x1 >= 0 && x1 < rows && y1 >= 0 && y1 < columns && x2 >= 0 &&
		x2 < rows && y2 >= 0 && y2 < columns && (*((int *)maze + (x1*columns + y1)) == 1))
		return 1;
	return 0;
}

void socket_server() {

	//The port you want the server to listen on
	int host_port= 1101;

	unsigned short wVersionRequested;
	WSADATA wsaData;
	int err;
	wVersionRequested = MAKEWORD( 2, 2 );
 	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 || ( LOBYTE( wsaData.wVersion ) != 2 ||
		    HIBYTE( wsaData.wVersion ) != 2 )) {
	    fprintf(stderr, "No sock dll %d\n",WSAGetLastError());
		goto FINISH;
	}

	//Initialize sockets and set options
	int hsock;
	int * p_int ;
	hsock = socket(AF_INET, SOCK_STREAM, 0);
	if(hsock == -1){
		printf("Error initializing socket %d\n",WSAGetLastError());
		goto FINISH;
	}
	
	p_int = (int*)malloc(sizeof(int));
	*p_int = 1;
	if( (setsockopt(hsock, SOL_SOCKET, SO_REUSEADDR, (char*)p_int, sizeof(int)) == -1 )||
		(setsockopt(hsock, SOL_SOCKET, SO_KEEPALIVE, (char*)p_int, sizeof(int)) == -1 ) ){
		printf("Error setting options %d\n", WSAGetLastError());
		free(p_int);
		goto FINISH;
	}
	free(p_int);

	//Bind and listen
	struct sockaddr_in my_addr;
	my_addr.sin_family = AF_INET ;
	my_addr.sin_port = htons(host_port);
	
	memset(&(my_addr.sin_zero), 0, 8);
	my_addr.sin_addr.s_addr = INADDR_ANY ;
	
	/* if you get error in bind 
	make sure nothing else is listening on that port */
	if( bind( hsock, (struct sockaddr*)&my_addr, sizeof(my_addr)) == -1 ){
		fprintf(stderr,"Error binding to socket %d\n",WSAGetLastError());
		goto FINISH;
	}
	if(listen( hsock, 10) == -1 ){
		fprintf(stderr, "Error listening %d\n",WSAGetLastError());
		goto FINISH;
	}
	
	//Now lets do the actual server stuff

	int* csock;
	sockaddr_in sadr;
	int	addr_size = sizeof(SOCKADDR);
	
 	while(true){
		printf("waiting for a connection\n");
		csock = (int*)malloc(sizeof(int));
		
		if((*csock = accept( hsock, (SOCKADDR*)&sadr, &addr_size))!= INVALID_SOCKET ){
			//printf("Received connection from %s",inet_ntoa(sadr.sin_addr));
			printf("accepted\n");
			CreateThread(0,0,&SocketHandler, (void*)csock , 0,0);
		}
		else{
			fprintf(stderr, "Error accepting %d\n",WSAGetLastError());
		}
	}

FINISH:
;
}


void process_input(char *recvbuf, int recv_buf_cnt, int* csock) 
{

//	char replybuf[1024];
	int index1 = 0;
	int len;
	len = strlen(recvbuf);
	char buf[50] = { '\0' };
	printf("received by server\n");
	printf("%s\n",recvbuf);
	int  m, n, x1, x2, y1, y2,  k = 0;
	m = recvbuf[index1++] - '0';
	n = recvbuf[index1++] - '0';
	x1 = recvbuf[len-5]-'0';
	y1 = recvbuf[len-4]-'0';
	x2 = recvbuf[len-3]-'0';
	y2 = recvbuf[len-2]-'0';
	int *arr = (int *)malloc(m * n * sizeof(int));
	for (int i = 0; i < m; i++)
	for (int j = 0; j < n; j++)
		*(arr + i*n + j) = recvbuf[index1++] - '0';
	for (int i = 0; i < m; i++)
	for (int j = 0; j < n; j++)
		printf("%d ", *(arr + i*n + j));
	printf("%d \n ", path_exists((int*)arr, m, n, x1, y1, x2, y2));
	for (int i = 0; i < index; i++)
	{
		buf[k++] = path[i] + '0';
	}
	buf[k] = '\0';
	printf("reply to buf : %s\n", buf);
	replyto_client(buf, csock);
	
}

void replyto_client(char *buf, int *csock) {
	int bytecount;
	
	if((bytecount = send(*csock, buf, strlen(buf), 0))==SOCKET_ERROR){
		fprintf(stderr, "Error sending data %d\n", WSAGetLastError());
		free (csock);
	}
	//printf("%s\n", buf);
	printf("replied to client: %s\n",buf);
	
}

DWORD WINAPI SocketHandler(void* lp){
    int *csock = (int*)lp;

	char recvbuf[1024];
	int recvbuf_len = 1024;
	int recv_byte_cnt;

	memset(recvbuf, 0, recvbuf_len);
	if((recv_byte_cnt = recv(*csock, recvbuf, recvbuf_len, 0))==SOCKET_ERROR){
		fprintf(stderr, "Error receiving data %d\n", WSAGetLastError());
		free (csock);
		return 0;
	}


	printf("Received bytes %d\nReceived string \"%s\"\n", recv_byte_cnt, recvbuf);
	process_input(recvbuf, recv_byte_cnt, csock);

    return 0;
}