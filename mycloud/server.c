/* 
   ***************server.c***************
*/
#include "csapp.h"
#include "mycloud.h"

/* Tracks the files containd in the server */
unsigned int NUM_OF_FILES = 0;
char** filenames;
/* Tracks the files containd in the server */

void runServer(int connfd);
void printStatus( unsigned int key, unsigned int requestType, char* filename, int error );

/* Begin servermain */
int main(int argc, char **argv) 
{
    int listenfd, connfd, port, key; // secretKeyinit;
    socklen_t clientlen;
    struct sockaddr_in clientaddr;
    struct hostent *hp;
    char *haddrp;

    if( argc != 3 ){
	fprintf(stderr, "usage: %s <port> <key>\n", argv[0]);
	exit(1);
    }
    port = atoi(argv[1]);

/*   Check for secret key for opening the server */

    key = atoi(argv[2]);
    secretKey = key;

    listenfd = Open_listenfd(port);
    /* allocate array of filenames */
    int i;
    filenames =(char**) malloc(40);
    for( i = 0; i < 40; i += 1 ){
	filenames[i] = (char*)malloc(MAX_FILE_SIZE);
    }

    while (1) {
	clientlen = sizeof(clientaddr);
	/* Open socket fd with any eligible clients */
	connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);

	/* Determine the domain name and IP address of the client */
	hp = Gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr, 
			   sizeof(clientaddr.sin_addr.s_addr), AF_INET);
	haddrp = inet_ntoa(clientaddr.sin_addr);
	printf("server connected to %s (%s)\n", hp->h_name, haddrp);
	
	runServer(connfd);
	Close(connfd);
    }
    exit(0);
}
/* $end servermain */


/* Server Implementation handles put, get, del, list requests */
void runServer(int connfd){
	size_t n;
	char dat[MAX_FILE_SIZE];
	rio_t rio;
	unsigned int error, key, request, data_size;
	char *filename =  (char*)malloc(81);	
	FILE* data_file;

	int buffer;//holds the integers sent over network
	uint32_t netToHost, network_error;
	Rio_readinitb(&rio, connfd);//initialize rio struct
	error = 0;
	n = Rio_readnb(&rio, &buffer, 4);
	if( n != 4 ){
		printf("Error. Did not receive 4 bytes from secret key.\n");
		error = 1;
	}

	/* Get the secret key into unsigned int data type */
	netToHost = ntohl( (uint32_t)buffer );
	key = (unsigned) netToHost;
//	printf("Key:\t%u\n", key);

	if( key != secretKey ){
		printf("Error. Invalid key entry.\n");
		error = 1;
	}

	/* Get the request type into unsigned int data type */
	n = Rio_readnb(&rio, &buffer, 4);
	if( n != 4 ){
		printf("Error. Did not receive 4 bytes from request type.\n");
		error = 1;
	}
	netToHost = ntohl( (uint32_t)buffer );
	request = (unsigned) netToHost;
//	printf("Request:\t%u\n", request);

	
	unsigned i;
	/* Determine the ype of request and handle each protocol accordingly */
	switch( request ){
/* **************************************PUT************************************* */
		case 0:{
			/* read the filename into the filename string */
			n = Rio_readnb(&rio, filename, 80);
			//strcat(filename, "\0"); 			
			if( !error ){
				/* read the size of the file */
				n = Rio_readnb(&rio, &buffer, 4);
				if( n != 4){
					printf("Error. 4 bytes not read into the integer.\n");
					error = 1;
				}
				netToHost = ntohl( (uint32_t)buffer );
				data_size = (unsigned) netToHost;
	//			printf("Data size:\t%u\n", data_size);
				/* read the data into the dat buffer and create the file */
				n = Rio_readnb(&rio, &dat, data_size);
	//			printf("here:\n");
	//			strcat(filename, ".dat");
				if( (data_file = Fopen(filename, "wb")) != NULL ){
					printf("here:\n");
					Fwrite(filename, data_size, 1, data_file);

					Fclose(data_file);
					network_error = htonl( error );
//				
					Rio_writen(connfd, &network_error, 4);	
					//strcat(filename, "\0");
					printStatus( key, 0, filename, error);
			//		strcat(filename, "\0");
					filenames[NUM_OF_FILES] = filename;
					NUM_OF_FILES += 1;
				}
			}
			else{
				error = 1;
				printStatus(key, 0, filename, error);
			}
			break;}
/* ***************************************GET************************************ */
		case 1:{
			n = Rio_readnb(&rio, filename, 80);//read 
			if( !error ){
				/* If the open file is successful, send its data */
				if( (data_file = Fopen(filename, "rb")) != NULL ){

					/* Now to send the error message */
					network_error = htonl( (uint32_t)error );
					Rio_writen(connfd, &network_error, 4);

					/* Find/write size of file to the client */
					int file_size;
					fseek( data_file, 0L, SEEK_END );
					file_size = ftell( data_file );
					fseek( data_file, 0L, SEEK_SET );
					uint32_t network_filesize;
					network_filesize = htonl( (uint32_t)file_size );
					Rio_writen(connfd, &network_filesize, 4);

					/* Send the file contents over network */
					char* file_data =(char*) malloc( file_size+1 );
					Fread( file_data, file_size, 1, data_file );
					Rio_writen(connfd, file_data, file_size);
					printStatus(key, 1, filename, error);
					
					/* Free memory and close file */
					fclose( data_file );
					free( (void*) file_data);
				}
			}/* Otherwise send error to client */
			else{
				error = 1;
				network_error = htonl( (uint32_t)error );
				Rio_writen(connfd, &network_error, 4);
				printStatus(key, 1, filename, error);
			}
			break;}
/* ***************************************DEL************************************ */
		case 2:{
			/* read the filename into the filename string */
			n = Rio_readnb(&rio, filename, 80);
			
			/* if the filename to be deleted does not exist */
			if( (error = unlink(filename)) < 0 ){
				printf("Unable to delete file!\n\n");
				error = 1;
				network_error = htonl( (uint32_t)error );
				Rio_writen(connfd, &network_error, 4);
				printStatus(key, 2, filename, error);
			}
			else{
				printf("File exists!\n\n");
				network_error = htonl( (uint32_t)error );
				Rio_writen(connfd, &network_error, 4);
				printStatus(key, 2, filename, error);
				if( NUM_OF_FILES == 0 ){
					;
				}/* remove filename from the list and decrement */
				else{
					int i;
					for( i = 0; i < 40; i += 1 ){
						if( filenames[i] == filename ){
							filenames[i] = "";
							int j;
							/* shift left one position  */
							for( j = i; j < 39; j += 1){
								filenames[j] = filenames[j+1];
							}
							if( NUM_OF_FILES != 0 ){
								NUM_OF_FILES -= 1;
							}
							break;
						}
					}
				}
			}
			break;}
/* ***************************************LIST*********************************** */
		case 3:{
			char* file_names_buff = (char*)calloc(1,80*(NUM_OF_FILES+1));
			/* Get cumulative size of filenames */
			for( i = 0; i < NUM_OF_FILES; i += 1 ){
				strcat(file_names_buff, filenames[i]);
				strcat(file_names_buff, "\n");
				//files_len += strlen( filenames[i] ); 
			}
			strcat(file_names_buff, "\0");
			uint32_t len_files = (uint32_t) htonl( strlen(file_names_buff) );
			Rio_writen(connfd, &len_files, 4);

		/* send the filenames to the client */
/*			for( i = 0; i < NUM_OF_FILES; i += 1 ){
				printf("\n\n filename[%d]= %s, made of %d characters.", i, filenames[i],
				strlen(filenames[i]));
		//		Rio_writen(connfd, filenames[i], strlen( filenames[i]) );
			}
			*/
			/***** Used for Testing ***********/
		/*	printf("\n\nList: %s\nSize:%u\n", file_names_buff, strlen(file_names_buff));	
			int k=0;
			while (k < 81){
				printf("%d: %c\n", k, file_names_buff[k]);
				if (file_names_buff[k] == '\0')
					break;
				k++;
			} */
			/***** End Testing ****************/
			Rio_writen(connfd, file_names_buff, strlen(file_names_buff));
			printStatus(key, 3, "", error);
			break;}
		default:{
			printf("Invalid request\n");//Invalid request
		}
	}
}


/* Print Results of the server being run */
void printStatus( unsigned int key, unsigned int requestType, char* filename, int error){
	printf("Secret Key:\t%u\n", key);
	switch(requestType){
		case 0:
			printf("Request Type:\t%s\n", "mcput");
			break;
		case 1:	
			printf("Request Type:\t%s\n", "mcget");
			break;
		case 2:
			printf("Request Type:\t%s\n", "mcdel");
			break;
		case 3:
			printf("Request Type:\t%s\n", "mclist");
			break;
		default:
			printf("Error. Invalid Request Entry\n");
	}

	if( requestType == 3 ){
		printf("Filename:\tNONE\n");
	}
	else{
		printf("File Name:\t%s\n", filename);
	}

	if( !error ){
	  printf("Operation Status:\tSuccess\n");
	}
	else{
	  printf("Operation Status:\tFailure\n");
	}
	int i;
	for( i = 0; i < 75; i += 1 ){
		printf("=");
	}
	printf("\n\n");
	return;
}
