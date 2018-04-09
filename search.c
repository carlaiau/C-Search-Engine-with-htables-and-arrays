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

flexarray* search_get_listings(long pos, int len){
    /* Tested with awk '{print length}' listings | sort -rn | head -1 */
    FILE *listings_file = fopen("index/listings", "r");
    char buffer[len];
    fseek(listings_file, pos, SEEK_SET);
    fread(buffer, 1, len, listings_file);
    buffer[len - 1] = '\0';
    /* this is the entire read line */
    printf("%s\n", buffer);

    return NULL;
}

