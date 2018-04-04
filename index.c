#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mylib.h"
#include "htable.h"
#include "flexarray.h"

#define BUFFER_SIZE 4048
#define NUM_WORDS 155 /* ~750,000 unique words */
#define MAX_DOCS 1000000 /* ~750,000 unique words */

/* first go at indexing */
int main(int argc, char *argv []) {
	
	unsigned long current_doc_id = 0 ;
	int word_count = 0;

	htable inverted_index;

	FILE *file_handle;	
	char buffer[BUFFER_SIZE];

	file_handle = fopen(argv[1], "r");

	/* Flip Flop, whether next token is Doc ID or word */
	int new_doc = 0;

    if (!file_handle) {
        fprintf(stderr, "Unable to open input!\n");
        return 1;
    }

    /* Instantiate the htable */
    inverted_index = htable_new(NUM_WORDS);


    /* read each line into the buffer */
    while(fgets( buffer, BUFFER_SIZE, file_handle) != NULL){

    	char *token = strtok(buffer, "");		
		while(token != NULL){

			if(new_doc == 1){
				printf("Token:\t%s", token);
				current_doc_id = atol(token); // This is above the maximum allowed
				new_doc = 0;

			}

			// There is a new document coming!
			else if(strcmp(token, "\n") == 0){ 
	    		new_doc = 1;
	    	}
	    	// Add word to the dict hash table 
	    	else{ 
	    		word_count++;
        		htable_insert(inverted_index, token, current_doc_id);
			}
    		token = strtok(NULL, "");
    	}
    }
    // closing files.
    fclose(file_handle);  


    //htable_print(inverted_index);
    return 0;
}