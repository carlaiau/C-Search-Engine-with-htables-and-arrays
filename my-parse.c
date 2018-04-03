#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define BUFFER_SIZE 4048

int not_tag(char* token);


int main(int argc, char *argv []) {


	FILE *file_handle;
	const char *new_line_char_delim = " \n.,";


	char buffer[BUFFER_SIZE];

	file_handle = fopen(argv[1], "r");
    if (!file_handle) {
        fprintf(stderr, "Unable to open input!\n");
        return 1;
    }

    int lines_to_skip = 0;
    /* read each line into the buffer */
    while(fgets( buffer, BUFFER_SIZE, file_handle) != NULL){

    	/* splits based on delimitering strings */
    	char *token = strtok(buffer, new_line_char_delim);
		
		while(token != NULL){

			if(lines_to_skip == 0){ // We aren't in line skipping mode
				
				/* Output new line */
				if(strcmp(token, "<DOC>") == 0) { /* This is a new doc */
					printf("\n");
				}
				else if(strcmp(token, "<SO>") == 0){ /* Set us to line skipping mode */
					lines_to_skip = 4;
				}
				else if( not_tag(token) ) {
					printf( "%s\n", token);
				}
			}
			else{
				lines_to_skip--;
			}
			token = strtok(NULL, new_line_char_delim);

		}
    }
    // closing files.
    fclose(file_handle);  
    return 0;
}

/* Easy removal of tags that are on seperate lines */
int not_tag(char* token){
	if(strcmp(token, "</DOC>") == 0){
		return 0;
	}
	if(strcmp(token, "<DOCNO>") == 0){
		return 0;
	}
	if(strcmp(token, "</DOCNO>") == 0){
		return 0;
	}	
	if(strcmp(token, "<HL>") == 0){
		return 0;
	}
	if(strcmp(token, "<DD>") == 0){
		return 0;
	}
	if(strcmp(token, "</HL>") == 0){
		return 0;
	}		
	if(strcmp(token, "<IN>") == 0){
		return 0;
	}
	if(strcmp(token, "</IN>") == 0){
		return 0;
	}
	if(strcmp(token, "<DATELINE>") == 0){
		return 0;
	}
	if(strcmp(token, "</DATELINE>") == 0){
		return 0;
	}	
	if(strcmp(token, "<TEXT>") == 0){
		return 0;
	}
	if(strcmp(token, "</TEXT>") == 0){
		return 0;
	}					
	return 1;
}

