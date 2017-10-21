#ifndef __MYCLOUD_H__
#define __MYCLOUD_H__

// Libraries to include
#include "csapp.h"

#define MAX_DNS_NAME_SIZE 40 // no more than 40 characters long
#define MAX_FILENAME_SIZE 80 // no more than 80 characters long
#define MAX_FILE_SIZE 100000 // 100 kBytes
unsigned int secretKey; // global secretKey varaible

/* Tracks the files containd in the server */
unsigned int NUM_OF_FILES;
char** filenames;
/* Tracks the files containd in the server */


/* **************** My Cloud Funcitons ******************** */
int mycloud_putfile(char *MachineName, unsigned int TCPport, unsigned int SecretKey,
		    char *Filename, char *data, int datalen);

int mycloud_getfile(char *MachineName, int TCPport, unsigned int SecretKey,
		    char *Filename, char* data, int datalen);

int mycloud_delfile(char *MachineName, int TCPport, unsigned int SecretKey,
		    char *Filename);

int mycloud_listfiles(char *MachineName, int TCPport, unsigned int SecretKey,
		      char *listbuf, int listbuflen);

/* gets data file's contents */
int getFile( char* arg, char** fileData);

/* **************** End My Cloud Funcitons ******************** */

#endif
