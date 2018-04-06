#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mylib.h"
#include "htable.h"
#include "flexarray.h"


struct htablerec {
    unsigned int capacity;
    unsigned int num_words;
    dict* dictionary;
    int* count;
};

struct dict_rec{
    char* word;
    flexarray listings;
    int freq;
    unsigned int pos;
    unsigned int len;
};

htable htable_new(int capacity){
    unsigned int i;
    htable h = emalloc(sizeof *h);
    h->capacity = capacity;
    h->num_words = 0;

    h->dictionary = emalloc(h->capacity * sizeof h->dictionary[0]);
    h->count = calloc(h->capacity, sizeof(int));

    for(i = 0; i< h->capacity; i++ ){
        h->dictionary[i].word = NULL;
    }
    return h;
}


static unsigned int htable_word_int(char *w){
    unsigned int result = 0;
    while(*w != '\0'){
        result = (*w++ + 31 * result);
    }
    return result;
}

static unsigned int htable_step(htable h, unsigned int i_key){
    return 1 + (i_key % (h->capacity -1));
}



int htable_insert(htable h, char *token, long docid){
    unsigned int attempts = 0;
    unsigned int insert_index = 0;
    insert_index = htable_word_int(token) % h->capacity;
    for(attempts = 0; attempts < h->capacity; attempts++){
        /* Possibility 1: Nothing at word key */
        if(h->dictionary[insert_index].word == NULL){
            h->dictionary[insert_index].word = emalloc((strlen(token) + 1) * sizeof(token[0]));
            /* Copy token into the word key at dictionary position */
            strcpy((h->dictionary[insert_index]).word, token);    
            /* Make new flex array for these listings */
            h->dictionary[insert_index].listings = flexarray_new();
            /* Append this docid to the flex array that already exists at this position */
            flexarray_append( h->dictionary[insert_index].listings, docid);
            h->dictionary[insert_index].freq = 1;
            h->num_words++;
            h->count[insert_index]++;
            return 1;
        }
        /* Possibility 2: Same word  at key */
        if( strcmp(h->dictionary[insert_index].word, token) == 0 ){
            h->count[insert_index]++;
            if (flexarray_get_last_id(h->dictionary[insert_index].listings) != docid) {
                flexarray_append(h->dictionary[insert_index].listings, docid);    
                h->dictionary[insert_index].freq++;
            } 
            else{        
                flexarray_updatecount(h->dictionary[insert_index].listings);
            }
            return 1;
        }
        
        insert_index = htable_step(h, insert_index);
    }
    return 0;
}
/**
 * Searches the hash table for the given value.
 *
 * @param h the htable to be searched.
 * @param s the value to search the hash table with.
 * @return whether the value was found or not.
 */
int htable_search(htable h, char* search_term){
    unsigned int i;
    unsigned int index = htable_word_int(search_term) % h->capacity;
    unsigned int step =  htable_step(h, htable_word_int(search_term));

    for (i = 0; i <= h->capacity; i++){  
        printf("Index looking at: %d\n", index);
        if (h->dictionary[index].word == NULL){
            return 0;
        } 
        else if(strcmp( h->dictionary[index].word, search_term) == 0){
            return 1;
        } 
        else {
            index = (index + step) % h->capacity;
        }
    }
    return 0;
    
}

/**
 * Prints out all the dictionary stored in the hashtable and the frequency of
 * each of them.
 *
 * @param h the htable to get the  from.
 * @param stream the output stream to write to.
 */
void htable_print(htable h){
    unsigned int i;
    for (i = 0; i < h->capacity; i++){
        if ( h->dictionary[i].word != NULL) {
            printf("%d: %s\n", i, h->dictionary[i].word);
            flexarray_print(h->dictionary[i].listings);
            printf("\n\n");
        }
    }
    printf("Number of words entered: %d\n\n", h->num_words);
}

/* 
    Dictionay will be saved to a text file in the format
    hash word freq pos len
    
    pos corresponds to the position that this words listings start in the 
    listings file.
    len corresponds to the length of this words listings 

    pos and len are based on what is returned from the flex array print 
    function.
    */
int htable_save(htable h) {
    unsigned int line = 0;
    unsigned int i = 0;
    
    FILE *dict_file_pointer = fopen("index/dictionary" , "w");
    /* We're appending this, not overwriting */
    FILE *listings_file_pointer = fopen("index/listings" , "a");
    if(dict_file_pointer == NULL){
        fprintf(stderr, "dictionary open fail");
        return EXIT_FAILURE;
    }
    if(listings_file_pointer == NULL){
        fprintf(stderr, "dictionary open fail");
        return EXIT_FAILURE;
    }

    for(i = 0; i < h->capacity; i++){
        if( h->dictionary[i].word != NULL){
            fprintf(
                dict_file_pointer, 
                "%s %d %d\n", 
                h->dictionary[i].word, 
                line++, 
                h->dictionary[i].freq
            );
            flexarray_save(h->dictionary[i].listings, listings_file_pointer);
        }
    }
    return EXIT_SUCCESS;
}
