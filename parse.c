#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define BUFFER_SIZE 10000

static void output_id(char* token){
	unsigned int i;
	unsigned int len = strlen(token);
	if(len > 0){
		printf("\n");
	}
	for(i = 0; i < len && token[i] != '<'; i++){
		if(isdigit(token[i])){
			printf("%c", token[i]);
		}
	}
	printf("\n");
}

static void output_clean(char *token){
	unsigned int i;
	/* local var rather than double look */
	unsigned int len = strlen(token);
	char previous_printed_char = '\n';
	/* Make sure token is not an & */	
	if(strcmp(token, "&amp") != 0 && len > 1){
		/* we have a tag to remove */
		if(token[len - 1] == '>' || token[len - 2] == '>'){ 
			for(i = 0; token[i] != '<'; i++){
				if(isalpha(token[i])){
					printf("%c", tolower(token[i]));
					previous_printed_char = token[i];
				}
				else if(token[i] == '/' || token[i] == '-' || token[i] == '('){
					if(previous_printed_char != '\n'){
						printf("\n");
						previous_printed_char = '\n';
					}
				}
			}
		}
		else{
			/* Remove Non Alphabetical */
			for(i = 0; i < len; i++){
				if(isalpha(token[i])){
					printf("%c", tolower(token[i]));
					previous_printed_char = token[i];
				}
				else if(token[i] == '/' || token[i] == '-' || token[i] == '('){
					if(previous_printed_char != '\n'){
						printf("\n");
						previous_printed_char = '\n';
					}
				}			
			}
		}
	}
	if(previous_printed_char != '\n'){
		printf("\n");
	}
}




int parse(char* input_filename){
	int id = 0; /* expecting id, date bool */
	FILE *file_handle;
	const char *delim = " $.,\"\'\n();:{}+^#@*";
	char buffer[BUFFER_SIZE];
	file_handle = fopen(input_filename, "r");
    if (!file_handle) {
        fprintf(stderr, "Unable to open input!\n");
        return 1;
    }
	
	/* read each line into the buffer */
    while(fgets( buffer, BUFFER_SIZE, file_handle) != NULL){
    	/* splits based on delimitering strings */
    	char *token = strtok(buffer, delim);		
		while(token != NULL) {
            if(id == 1) {
				output_id(token);
                id = 0;
            }
			/* Is a Tag */
            else if(token[0] == '<'){
				/* Next Token will be ID */
				if(strcmp(token, "<DOCNO>") == 0) {
                    id = 1;
                }
            }
			/* This is normal content */
			else{
				output_clean(token);
			}
			token = strtok(NULL, delim);
		}
	}
	
    fclose(file_handle);  
	return 0;
}

