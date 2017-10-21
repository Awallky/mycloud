/* 
   mclist.cpp 
   NOTE: This is a client-side process
*/

#include "csapp.h"
#include "mycloud.h"

/*
  retrieves a list of the filenames stored on the mycloud server,
  identified by MachineName and TCPport using the number SecretKey as the password.   It then writes the list of filenames to standard output, 
  one filename per line of output. 
*/
extern char** filenames;
extern unsigned int NUM_OF_FILES;
int mycloud_listfiles( char* MachineName, int TCPport, unsigned int SecretKey,
		       char* listbuf, int listbuflen ){
		int clientfd;
		rio_t rio;
		uint32_t key = htonl(SecretKey);
		uint32_t request = htonl(3);


		uint32_t size_of_file_names;

		char* current_file_name;
		/* return fd after opening */
		clientfd = Open_clientfd(MachineName, TCPport);

		/* Associate fd with read buffer and reset the buffer */
		Rio_readinitb(&rio, clientfd);

		/* Write 4-byte secret key to the server in network byte order */
		Rio_writen(clientfd, &key, 4);
		/* Write the 4-byte type of request in network byte order */
		Rio_writen(clientfd, &request, 4);
		
		/* Retrieve the size of all names of files */
		Rio_readnb(&rio, &size_of_file_names, 4);		
	
		if (size_of_file_names < 0){
			printf("There was an unexpected error!\n");
		}
		else{
			/* Ensure the size is in correct host order */
			unsigned temp = (unsigned) ntohl(size_of_file_names);
//			printf("temp: %d\n", temp);

			current_file_name = (char *)malloc(temp+1);
			/* Read the names of the files */
			Rio_readnb(&rio, current_file_name, temp +1);
			

//			printf("Return from RIO\n");

//			current_file_name = (char *)htonl(current_file_name);
			printf("%s\n", current_file_name);
		} 
/*			while (Countup < NUM_OF_FILES)
			{
				Rio_readnb(&rio, &current_file_name, strlen(filenames[Countup]));
				printf("%s\n", current_file_name);
				Countup++;
			}	*/
//		}
		close(clientfd);		
		return 0; // success
	}

/* retrieves a list of files from the mycloud server 
   identified by MachineName and TCPport using the number SecretKey 
   as the password.  
*/

int main( int argc, char** argv )
{
	int list;
	char* file_data = NULL;
	int data_len = 40;
	/* executable name, host name, port #, and secret key */
	if( argc != 4 ){
		printf("<executable> <host> <port> <key>\n");
		exit(1);
	}
	
	/* run listfiles*/
	if( (list = mycloud_listfiles( argv[1], atoi(argv[2]), (unsigned)atoi(argv[3]),
	   			   file_data, data_len)) < 0){
		printf("Error. Invalid list request.\n");
		exit(1);
	}

	return 0;	
}

