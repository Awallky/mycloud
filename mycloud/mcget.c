/* 
   mcget.c 
   NOTE: This is a client-side process.
*/

#include "csapp.h"
#include "mycloud.h"

/* retrieves a file named filename from the mycloud server 
   identified by MachineName and TCPport using the number SecretKey 
   as the password. 
   It then writes the file to standard output. 
*/

int main( int argc, char** argv )
{
	int get, data_len = 0;
	char* file_data = NULL;

	/* executable name, host name, port #, filename, and secret key */
	if( argc != 6 ){
		printf("<executable> <host> <port> <filename> <datalen> <key>\n");
		exit(1);
	}
	
	/* run getfile */
	if( (get = mycloud_getfile( argv[1], atoi(argv[2]), (unsigned)atoi(argv[5]),
	   			    argv[3] ,file_data, data_len)) < 0){
		printf("Error. Invalid get request.\n");
		exit(1);
	}

	return 0;	
}

int mycloud_getfile( char* MachineName, int TCPport, unsigned int SecretKey,
		     char* FileName, char* data, int datalen){	
		int clientfd;
		rio_t rio;
		uint32_t key = htonl(SecretKey);
		uint32_t request = htonl(1);
		int error;
		uint32_t err;	
		
		/* return fd after opening */
		clientfd = Open_clientfd(MachineName, TCPport);

		/* Associate fd with read buffer and reset the buffer */
		Rio_readinitb(&rio, clientfd);

		/* Write 4-byte secret key to the server in network byte order */
		Rio_writen(clientfd, &key, 4);
		/* Write the 4-byte type of request in network byte order */
		Rio_writen(clientfd, &request, 4);
		/* Write the 80-byte file name to the server */
		Rio_writen(clientfd, FileName, 80);
		/* Retrieve the return status of the get request */
		Rio_readnb(&rio, &err, 4);
		/* convert to host byte ordering and handle cases */
		error = (int) ntohl( err );
		if( error != 0 ){
			Close(clientfd);
			return -1;
		}/* Other wise the request was succesful */
		else{

			uint32_t file_size;
			int dl;

			/* get the file size and allocate the space */
			Rio_readnb(&rio, &file_size, 4);
			dl = ntohl( file_size );
			/* saturate the datalen */
			if( datalen > dl ){
				datalen = dl;
			}
			
			data = malloc( datalen + 1 );
			/* Read the file into data */
			Rio_readnb(&rio, data, datalen);
			printf(" Data:\t%s\n", data);
		}
		
		Close(clientfd);
		return 0; // success
}
