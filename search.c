#include "search.h"

#define NUM_WORDS 250000

/* This is the intial loading of the hash table from file */
htable search_load_index(){
    htable dict;
    FILE *dictionary_file = fopen("index/dictionary", "r");
    if (!dictionary_file){
        fprintf(stderr, "Unable to open index!\n Please ensure that the \"index\" is relative to where you're executing \n");
        return NULL;
    }	
    dict = htable_load_from_file(dictionary_file, NUM_WORDS);    
    return dict;
}

flexarray search_get_listings(long pos, int len){
    /* Tested with awk '{print length}' listings | sort -rn | head -1 */
    FILE *listings_file = fopen("index/listings", "r");
    int i = 0;
    long first_docid;
    long current_docid;
    int wordcount = 0;
    char buffer[len];
    char* token;
    flexarray f = flexarray_new();

    fseek(listings_file, pos, SEEK_SET);
    fread(buffer, 1, len, listings_file);
    buffer[len - 1] = '\0';
    token = strtok(buffer, " ");
    
    while(token != NULL){
        if(i == 0){
            sscanf(token, "%lu", &first_docid);
            current_docid = first_docid;
            printf("First Doc ID: %lu\n", first_docid);
        }
        else if(i % 2 == 0){
            sscanf(token, "%lu", &current_docid);
            current_docid += first_docid;
        }
        else{
            sscanf(token, "%d", &wordcount);
            //printf("%lu => %d\n", current_docid, wordcount);
            flexarray_append_count_known(f, current_docid, wordcount);
        }
        token = strtok(NULL, " ");
        i++;
    }
    return f;
}

