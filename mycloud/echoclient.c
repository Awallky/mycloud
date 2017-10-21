/* $begin echoclientmain */
#include "csapp.h"

int main(int argc, char **argv) 
{
    int clientfd, port;
    char *host, buf[MAXLINE];
    rio_t rio;

    if (argc != 3) {
	fprintf(stderr, "usage: %s <host> <port>\n", argv[0]);
	exit(0);
    }
    /* get host name */
    host = argv[1];
    /* get TCP port # */
    port = atoi(argv[2]);

    clientfd = Open_clientfd(host, port); //return fd after opening

    /* Associate fd with a read buffer and rest the buffer */
    Rio_readinitb(&rio, clientfd);

    /* get user input until ^c or other signal entered */
    while(Fgets(buf, MAXLINE, stdin) != NULL){
    	Rio_writen(clientfd, buf, strlen(buf));//write n bytes to server
	Rio_readnb(&rio, buf, MAXLINE);//Modified from readlineb to readnb
    	Fputs(buf, stdout);
    }
    
    Close(clientfd); //line:netp:echoclient:close
    exit(0);
}
/* $end echoclientmain */
