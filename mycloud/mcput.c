/*
  ***************mcput.c***************
*/

#include "csapp.h"
#include "mycloud.h"

/*
  This routine sends datalen bytes of data stored in the buffer 
  pointed to by data to the mycloud server, to be stored for 
  later getfile() requests. 
  The return value is be 0 on success or -1 for error.  
*/

int main(int argc, char** argv)
{
	int put, file_size = 0;
	char *fileData = NULL;

	/* execuatable name, hostname, port #, filename, secret key, and datafile */
	if( argc != 5 )
	{
	  printf("<executable> <host> <port> <key> <filename>\n");
	  exit(1);
	}

	/* get file contents and its size */
	file_size = getFile( argv[4], &fileData );//& is C-style pass-by-reference
	if( (put = mycloud_putfile(argv[1], (unsigned)atoi(argv[2]), (unsigned)atoi(argv[3]),
				   argv[4], fileData, file_size)) < 0 ){
	  printf("An error occurred.\n");
	  free((void*) fileData);
	  exit(1);
	}
	
	free((void*) fileData);
	return 0;
}

/* Establishes a connection with server and sends data to it */
int mycloud_putfile( char* MachineName, unsigned int TCPport, unsigned int SecretKey,
		     char* FileName, char* data, int datalen ){

	/* NOW TO SET THE BYTES ACCORDING TO PROTOCOL */	
	int clientfd;
	rio_t rio;
	uint32_t key = htonl(SecretKey);//
	uint32_t request = htonl(0);
	uint32_t datasize = htonl(datalen);
	int error;
	uint32_t err;
	
	clientfd = Open_clientfd(MachineName, TCPport); //return fd after opening
	
	/* Associate fd with a read buffer and reset the buffer */
	Rio_readinitb(&rio, clientfd);

	/* Write the 4-byte SecretKey to the server in network byte order */
	Rio_writen(clientfd, &key, 4);
	/* Write the 4-byte type of request to the server in network byte order */
	Rio_writen(clientfd, &request, 4);
	/* Write the 80-byte file name to the server */
	Rio_writen(clientfd, FileName, 80);
	/* Write 4-byte integer representing the number of bytes in the data */
	Rio_writen(clientfd, &datasize, 4);
	/* write the datalen-size-byte data to the server */ 
	Rio_writen(clientfd, data, datalen);
	/* read the error response from the server */
	Rio_readnb(&rio, &err, 4);

	error = (int) htonl(err);
	if( !error ){
		printf("Success!\n");
		Close(clientfd);
	}
	else{
		printf("Failure!\n");
		Close(clientfd);
		exit(1);
	}

	return 0; // success
}
