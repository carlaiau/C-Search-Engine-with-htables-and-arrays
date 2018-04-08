#include "search.h"

#define NUM_WORDS 250000

htable load_indexes(){
    htable dict;
    FILE *dictionary_file = fopen("index/dictionary", "r");
    if (!dictionary_file){
        fprintf(stderr, "Unable to open index!\n Please ensure that the \"index\" is relative to where you're executing \n");
        return NULL;
    }	
    dict = htable_load_from_file(dictionary_file, NUM_WORDS);
    htable_print_loaded(dict);
    /*

    FILE *listings_file = fopen("index/listings", "r");
	FILE *wordcount_file = fopen("index/wordcount", "r");
    */
	
    
    return dict;
}
