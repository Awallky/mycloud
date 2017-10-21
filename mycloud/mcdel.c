/* 
   mcdel.cpp 
   NOTE: This is a client-side process.
*/

#include "csapp.h"
#include "mycloud.h"

/*
   deletes the file named filename on the mycloud server 
   identified by MachineName and TCPport using the number 
   SecretKey as the password. 
*/

int main( int argc, char** argv)
{
	int del;

	/* executable name, host name, port #, and secret key */
	if( argc != 5 ){
		printf("<executable> <host> <port> <key> <filename>\n");
		exit(1);
	}
	
	/* run listfiles*/
	if( (del = mycloud_delfile( argv[1], atoi(argv[2]), (unsigned)atoi(argv[3]),
	   			   argv[4])) < 0){
		printf("Error. Invalid delete request.\n");
		exit(1);
	}


	return 0;
}
int mycloud_delfile( char* MachineName, int TCPport, unsigned int SecretKey,
		     char* FileName ){
			int clientfd;
		rio_t rio;
		uint32_t key = htonl(SecretKey);
		uint32_t request = htonl(2);
		int error;
		uint32_t err;	

//		uint32_t size_of_file_names;

//		char* current_file_name;
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
			printf("There was an error deleting file: %s\n", FileName);
			Close(clientfd);
			return -1;
		}/* Other wise the request was succesful */
		else{
			printf("mcdel successful!\n");
		
		}
		return 0;// success		
	}
