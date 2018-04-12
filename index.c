#include "index.h"

#define BUFFER_SIZE 4048
/* 
It's 218943, but indexs quicker with a bit of mem overflow.
This needs to be expanded further to reduce collision probability
*/
#define NUM_WORDS 250000 
#define NUM_DOCS 175000 /* 173252 via grep wc -l */

struct key_pair_rec {
	long docid;
    int word_count;
};

/* 
 * Create the index data files
 */
int create_index(char* input_file){
	htable inverted_index;
	key_pair* list_of_word_counts;
	unsigned long current_doc_id = 0;
	unsigned int docs_entered = 0;
	int words_in_doc = 0;
	unsigned int i = 0;
	
	/* Flip Flop, whether next token is Doc ID or word */
	int new_doc = 0;
	char buffer[BUFFER_SIZE];

	FILE *parsed_file_handle = fopen(input_file, "r");
	FILE *wordcount_file_handle = fopen("index/wordcount", "w");

    if (!parsed_file_handle) {
        fprintf(stderr, "Unable to open input!\n");
        return 1;
    }
    if (!wordcount_file_handle) {
        fprintf(stderr, "Unable to write index!\n Please ensure there is a \"index\" folder relative to where you're executing\n");
        return 1;
    }	
    
    inverted_index = htable_new(NUM_WORDS);
	
	/*Pseduo 2D Array of docid <-> word_count pairs */
	list_of_word_counts = emalloc(NUM_DOCS * sizeof(list_of_word_counts[0]));
	
    /* read each line into the buffer */
    while(fgets( buffer, BUFFER_SIZE, parsed_file_handle) != NULL){
		char *token = strtok(buffer, "");		
		while(token != NULL){
			if(new_doc == 1){
				/* If there has already been a doc_id seen */
				current_doc_id = atol(token); 
				new_doc = 0;
			}
			/* Next token will be the ID */
			else if(strcmp(token, "\n") == 0){ 
	    		new_doc = 1;
				if(docs_entered != 0){
					list_of_word_counts[docs_entered].docid = current_doc_id;
            		list_of_word_counts[docs_entered].word_count = words_in_doc;
					words_in_doc  = 0;
				}
				docs_entered++;
	    	}
	    	/* this token is a word to add */ 
	    	else{ 
	    		words_in_doc++;
				/* 
					This is to resolve bug from not using delimiter on strtok.
					Without this, the words are indexed with the \n on the end, 
					and strcmp does not work. Nasty, can fix if have time. Would not 
					happen if not outputting to intermediate text file for the 
					parsing part of assessment.
				*/
				token[strlen(token) -1 ] = '\0';
        		htable_insert(inverted_index, token, current_doc_id);
			}
    		token = strtok(NULL, "");
    	}
    }
	/* Add final docid word count */
	if(docs_entered != 0){
		list_of_word_counts[docs_entered].docid = current_doc_id;
        list_of_word_counts[docs_entered].word_count = words_in_doc;	
	}

    fclose(parsed_file_handle);  

	/* Create Index files */
	qsort(list_of_word_counts, NUM_DOCS, sizeof(key_pair), compare_docid);

	for (i = 0; i < NUM_DOCS; i++) {
		if(list_of_word_counts[i].docid != 0){
        	fprintf(
				wordcount_file_handle, "%lu %d\n", 
				list_of_word_counts[i].docid, 
				list_of_word_counts[i].word_count
			);
		}
    }
	htable_save(inverted_index);
    return 0;
}


/* Compare docids. Used in qsort */
int compare_docid(const void* first, const void* second) {
    const key_pair* first_aa = first;
    const key_pair* second_aa = second;
    long first_doc_id = first_aa->docid;
    long second_doc_id = second_aa->docid;

    if (first_doc_id > second_doc_id ) {
        return 1;
    } else if (first_doc_id < second_doc_id ) {
        return -1; 
    } else {
        return 0;
    }   
}


