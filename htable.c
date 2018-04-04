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



int htable_insert(htable h, char *token, int docid){
    int attempts = 0;
    int insert_index = 0;

    unsigned int long hash = 0;
    hash = htable_word_int(token);
    insert_index = hash % h->capacity;


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
            strcpy((h->listings[insert_index]).word, token);    
            
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
 * Prints out all the words stored in the hashtable and the frequency of
 * each of them.
 *
 * @param h the htable to get the words from.
 * @param stream the output stream to write to.
 */
void htable_print(htable h){
    int i;
    for (i = 0; i < h->num_words; i++){

        if ( (h->listings[i]).word != NULL) {

            printf("\n%d %s ", i, (h->listings[i]).word);
            flexarray_print((h->listings[i]).postings);
            printf("\n");
            
        }
    }
    printf("Number of words entered: %d\n", h->num_words);
}