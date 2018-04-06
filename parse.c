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

static char* output_clean(char *token){
	unsigned int i;
	unsigned int inner_i = 0;
	/* local var rather than double look */
	unsigned int len = strlen(token);
	char* temp_token = malloc( (len + 1 )* sizeof(temp_token[0]));

	/* Make sure token is not an & */	
	if(strcmp(token, "&amp") != 0 && len > 1){
		/* we have a tag to remove */
		if(token[len - 1] == '>'){ 
			for(i = 0; token[i] != '<'; i++){
				if(isalpha(token[i])){
					temp_token[inner_i++] =  tolower(token[i]);
				}
			}
		}
		else{
			/* Remove Non Alphabetical */
			for(i = 0; i < len; i++){
				if(isalpha(token[i])){
					temp_token[inner_i++] =  tolower(token[i]);
				}
			}
		}
	}
	temp_token[inner_i] = '\0';
	return temp_token;
}




int parse(char* input_filename){
	int id = 0; /* expecting id, date bool */
	FILE *file_handle;
	const char *delim = " $.,\"\'\n();:{}+^#@*";
	char buffer[BUFFER_SIZE];
	char* temp_token;
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
				temp_token = output_clean(token);
				if(temp_token[0] != '\0'){
					printf("%s\n", temp_token);
				}
			}
			token = strtok(NULL, delim);
		}
	}
	
    fclose(file_handle);  
	return 0;
}

