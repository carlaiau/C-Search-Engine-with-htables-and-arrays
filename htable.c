#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mylib.h"
#include "htable.h"
#include "flexarray.h"


struct htablerec {
    int capacity;
    int num_words;
    listing *listings;
    int* count;
};

struct listing_rec{
    char* word;
    flexarray postings;
    int freq;
};

htable htable_new(int capacity){
    int i;
    htable h = emalloc(sizeof *h);
    h->capacity = capacity;
    h->num_words = 0;

    h->listings = emalloc(h->capacity * sizeof h->listings[0]);
    h->count = calloc(h->capacity, sizeof(int));

    for(i = 0; i< capacity; i++ ){
        h->listings[i].word = NULL;
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
    int attempts = 0;
    int insert_index = 0;
    insert_index = htable_word_int(token) % h->capacity;

    /* Possibility 1: Nothing at word key */
    if(h->listings[insert_index].word == NULL){
        (h->listings[insert_index]).word = emalloc((strlen(token) + 1) * sizeof(token[0]));
        /* Copy token into the word key at listings position */
        strcpy((h->listings[insert_index]).word, token);    
        /* Make new flex array for these postings */
        (h->listings[insert_index]).postings = flexarray_new();

        /* Append this docid to the flex array that already exists at this position */
        flexarray_append( (h->listings[insert_index]).postings, docid);
        
        h->listings[insert_index].freq = 1;

        h->num_words++;
        h->count[insert_index]++;
        return 1;
    }

    /* Possibility 2: Same word  at key */
    if( strcmp(h->listings[insert_index].word, token) == 0 ){
        h->count[insert_index]++;
        
        if (flexarray_get_last_id((h->listings[insert_index]).postings) != docid) {
            
            flexarray_append((h->listings[insert_index]).postings, docid);    
            h->listings[insert_index].freq++;
        } 

        else{        
            flexarray_updatecount((h->listings[insert_index]).postings);
        }
        return 1;
    }

    /* Possiblity 3: Some other word at key */
    for(attempts = 0; h->listings[insert_index].word != NULL && attempts < h->capacity; attempts++){
        insert_index = htable_step(h, insert_index);
        /* Possibility 1: Nothing at word key */
        if(h->listings[insert_index].word == NULL){
            (h->listings[insert_index]).word = emalloc((strlen(token) + 1) * sizeof(token[0]) );
            
            /* Copy word into the word key at listings position */
            strcpy( (h->listings[insert_index]).word , token);    
            
            /* Make new flex array for these postings */
            (h->listings[insert_index]).postings = flexarray_new();

            /* Append this docid to the flex array that already exists at this position */
            flexarray_append((h->listings[insert_index]).postings, docid);
            
            h->listings[insert_index].freq = 1;

            h->num_words++;
            h->count[insert_index]++;
            return 1;
        }
        /* Possibility 2: Same word at key */
        if( strcmp(h->listings[insert_index].word, token) == 0 ){
            h->count[insert_index]++;
            if (flexarray_get_last_id((h->listings[insert_index]).postings) != docid) {
                flexarray_append((h->listings[insert_index]).postings, docid);    
                h->listings[insert_index].freq++;
            } 
            else{        
                flexarray_updatecount((h->listings[insert_index]).postings);
            }
            return 1;
        }

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
    int i;
    int index = htable_word_int(search_term) % h->capacity;
    int step =  htable_step(h, htable_word_int(search_term));

    for (i = 0; i <= h->capacity; i++){  
        printf("Index looking at: %d\n", index);
        if (h->listings[index].word == NULL){
            return 0;
        } 
        else if(strcmp( h->listings[index].word, search_term) == 0){
            return 1;
        } 
        else {
            index = (index + step) % h->capacity;
        }
    }
    return 0;
    
}

/**
 * Prints out all the words stored in the hashtable and the frequency of
 * each of them.
 *
 * @param h the htable to get the words from.
 * @param stream the output stream to write to.
 */
void htable_print(htable h){
    int i;
    for (i = 0; i < h->capacity; i++){
        if ( (h->listings[i]).word != NULL) {
            printf("%d: %s\n", i, (h->listings[i]).word);
            flexarray_print((h->listings[i]).postings);
            printf("\n\n");
        }
    }
    printf("Number of words entered: %d\n\n", h->num_words);
}

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
        if( h->listings[i].word != NULL){
            fprintf(
                dict_file_pointer, 
                "%s %d %d\n", 
                h->listings[i].word, 
                line++, 
                h->listings[i].freq
            );
            flexarray_save(h->listings[i].postings, listings_file_pointer);
        }
    }
    return EXIT_SUCCESS;

}
