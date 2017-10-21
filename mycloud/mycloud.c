#include "csapp.h"
#include "mycloud.h"

/* gets data file's contents */
int getFile( char* arg, char** fileData){
	
	/* read data file here */
 	FILE *data_file;
	int file_size = 0;

	/* Read only */
	if( (data_file = Fopen( arg, "r")) < 0 ){
		printf("Error. File Unable to open.\n");
		exit(1);
	}	
	fseek( data_file, 0L, SEEK_END );

	file_size = ftell( data_file );//get the size of the file
	if( file_size > MAX_FILE_SIZE ){
		file_size = MAX_FILE_SIZE;//do not take more than 100 kB size file
	}
	

	/* return file pointer to beginning */
	fseek( data_file, 0L, SEEK_SET );
	/* allocate enough space to hold file contents */
	*fileData = malloc( file_size + 1);
	/* read data_file into fileData */
	Fread( *fileData, file_size, 1, data_file );
	fclose( data_file );
	return file_size;
}

