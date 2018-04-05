#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define BUFFER_SIZE 10000

int not_tag(char* token);
char* clean_up_token(char* token);
char* clean_up_docid(char* token);
char* clean_up_date(char* token);


int main(int argc, char *argv []) {
	FILE *file_handle;
	const char *new_line_char_delim = " $.,\"\'\n\r\t();:{}+^#@*";
	char buffer[BUFFER_SIZE];
	file_handle = fopen(argv[1], "r");
    if (!file_handle) {
        fprintf(stderr, "Unable to open input!\n");
        return 1;
    }
    /* read each line into the buffer */
    while(fgets( buffer, BUFFER_SIZE, file_handle) != NULL){
    	/* splits based on delimitering strings */
    	char *token = strtok(buffer, new_line_char_delim);		
		while(token != NULL){
			/* 
			 * New Doc! 
			 * Output a blank line
			 */
			if(strcmp(token, "<DOC>") == 0) { /* This is a new doc */
				printf("\n");
			}

			/* 
			 * Doc ID
			 * Skip inital line and grab Doc ID, and clean it.
			 */
			else if(strcmp(token, "<DOCNO>") == 0){
				printf( "%s\n", token);
				token = strtok(NULL, new_line_char_delim); // Grab the next token
				printf( "Whats this? %s\n", token);
				if(token != NULL){
					token = clean_up_docid(token);
					printf( "%s\n", token);
				}
				
			}

			/* 
			 * This is Date tag
			 * Needs cleaned to ensure only / or number
		 	 *
			 */
			else if(strcmp(token, "<DD>") == 0){ 
					
				token = strtok(NULL, new_line_char_delim); // Grab the next Line
				
				token = clean_up_date(token);
				if(strlen(token) > 0){
					printf( "%s\n", token);
				}
				
			}


			/* 
			 * all other tokens
			 */
			else if( not_tag(token) ) {
				token = clean_up_token(token);
				if(strlen(token) > 1){
					//printf( "%s\n", token);
				}
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
	if(strcmp(token, "<SO>") == 0){ 
		return 0;
	}
	if(strcmp(token, "</SO>") == 0){ 
		return 0;
	}	
	if(strcmp(token, "<LP>") == 0){ 
		return 0;
	}		
	if(strcmp(token, "</LP>") == 0){ 
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
	if(strcmp(token, "<DOCID>") == 0){		
		return 0;
	}
	if(strcmp(token, "</DOCID>") == 0){		
		return 0;
	}	
	if(strcmp(token, "<CO>") == 0){ 
		return 0;
	}
	if(strcmp(token, "</CO>") == 0){ 
		return 0;
	}				
	return 1;
}


// Removes the xml tag at the end of a token.
char* clean_up_token(char* token) {
    char* temp_token = malloc( strlen(token) * sizeof(temp_token[0]) + 1 );
    unsigned long inner_i = 0;
    unsigned long i;
    for (i = 0; i < strlen(token); i++){
    	if(isalpha(token[i])){
    		temp_token[inner_i++] = tolower(token[i]);
    	}
    	/* Strip out the & */
    	else if(token[i] == '&'){
    		i += 4;
    	}
    }
    temp_token[inner_i] = '\0';
    return temp_token;
}

char* clean_up_date(char* token){
	char* temp_token = malloc( strlen(token) * sizeof(temp_token[0]) + 1 );
    unsigned long inner_i = 0;
    unsigned long i;
    for (i = 0; i < strlen(token); i++){
		if(token[i] == '<'){ // Found end tag
			break;
		}
    	else if(isdigit(token[i]) || token[i] == '/' ){
    		temp_token[inner_i++] = tolower(token[i]);
    	}
    }
    temp_token[inner_i] = '\0';
    return temp_token;
}

/* 
 * Use Doc ID as number index
 * but also output the full WSJ ID string for exact searching 
 */
char* clean_up_docid(char* token){
	printf("Token length: %d", strlen(token) );
	char *temp_token = malloc( strlen(token) * 2 * sizeof(temp_token[0]) + 1 );
	unsigned long i;
	unsigned long inner_i = 0;

	for(i = 0; i < strlen(token); i++){
		if( isdigit(token[i] ) ){
			temp_token[inner_i++] = token[i];
		}
	}
	/* Output a new line at end of ID */
	temp_token[inner_i++] = '\n';

	/* Repeat the DocID completely */
	for(i = 0; i < strlen(token); i++){
		temp_token[inner_i++] = token[i];
	}
	temp_token[inner_i] = '\0';
	return temp_token;
}
