#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mylib.h"
#include "htable.h"
#include "flexarray.h"

#define BUFFER_SIZE 4048
#define NUM_WORDS 100000 /* ~750,000 unique words */

/* first go at indexing */
int create_index(){
	unsigned long current_doc_id = 0 ;
	int word_count = 0;
	/* Flip Flop, whether next token is Doc ID or word */
	int new_doc = 0;
	htable inverted_index;

	FILE *parsed_file_handle;	
	char buffer[BUFFER_SIZE];
	parsed_file_handle = fopen("parsed/ready", "r");

    if (!parsed_file_handle) {
        fprintf(stderr, "Unable to open input!\n");
        return 1;
    }
    /* Instantiate the htable */
    inverted_index = htable_new(NUM_WORDS);
    /* read each line into the buffer */
    while(fgets( buffer, BUFFER_SIZE, parsed_file_handle) != NULL){
		char *token = strtok(buffer, "");		
		while(token != NULL){
			if(new_doc == 1){
				/* Have to use longs beause of document id size */
				current_doc_id = atol(token); 
				new_doc = 0;
			}
			/* Next token will be the ID */
			else if(strcmp(token, "\n") == 0){ 
	    		new_doc = 1;
	    	}
	    	/* this token is a word to add */ 
	    	else{ 
	    		word_count++;
				/* 
					This is to resolve bug from not using delimiter on strtok.
					Without this, the words are indexed with the \n on the end, 
					and strcmp does not work. Nasty, fix if have time. Would not 
					happen if not outputting to intermediate text file for the 
					parsing part of assessment 
				*/
				token[strlen(token) -1 ] = '\0';
        		htable_insert(inverted_index, token, current_doc_id);
			}
    		token = strtok(NULL, "");
    	}
    }
    fclose(parsed_file_handle);  
	/* Create Index files */
	htable_save(inverted_index);
    return 0;
}
