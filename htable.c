#include "htable.h"

struct htablerec {
    int capacity;
    int num_words;
    dict* dictionary;
    int* count;
};

struct dict_rec{
    char* word;
    int freq;
    /* Used when generating the index */
    flexarray listings;
    /* Used when loading index from file */
    long pos;
    int len;
};

/* new htable creation */
htable htable_new(int capacity){
    int i;
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
/* free htable */
static void htable_free(htable h){
    int i;
    for(i = 0; i< h->capacity; i++ ){
        if(h->dictionary[i].word != NULL){
            free(h->dictionary[i].word);
        }
    }
    free(h->dictionary);
    free(h->count);
    free(h);
}
/* Hashing functions. Thanks COSC242! */
static unsigned int htable_word_int(char *w){
    unsigned int result = 0;
    while(*w != '\0'){
        result = (*w++ + 31 * result);
    }
    return result;
}
static unsigned int htable_step(htable h,  int i_key){
    return 1 + (i_key % (h->capacity -1));
}

/* 
 * Used during indexing
 * 
 * Parameters: h htable to insert into
 *             token, char* the word
 *             docid, long the docid
 * Returns:    int, a pseudo boolean, 0 for fail, 1 for complete
 * 
 * Procedure:  Use hash function to determine where to insert word
 *             If key at hash is NULL, then this is a new word into the dictionary.
 *             Save the word into the dicitionay and create a new flexarray of listings for it.
 *
 *             If key at hash is the same as token, then we check whether the last inserted docid
 *             is the current docid, if it is (which means another term within the same document), 
 *             we just update the count on this listing array. Otherwise if docid is a new docid, 
 *             we create another listing within the flexarray otherwise we update the count on the current flexarray.
 */ 
int htable_insert(htable h, char *token, long docid){
    int attempts = 0;
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
 * Parameters h the htable to be searched.
 *            search_term the string to search for
 *
 * Returns the index
 */
unsigned int htable_search(htable h, char* search_term){
    int i;
    unsigned int index = htable_word_int(search_term) % h->capacity;
    for (i = 0; i <= h->capacity; i++){  
        if (h->dictionary[index].word == NULL){
            return 0;
        } 
        else if(strcmp( h->dictionary[index].word, search_term) == 0){
            return index;
        } 
        else {
            index = htable_step(h, index);
        }
    }
    return 0;
}

/*
 * Used during indexing 
 * 
 * Paramters:   htable to save
 * Returns:     Int, Pseudo boolean, failure if can't open file pointers. Which will happen if 
 *              index directory does not exist or is not accessible
 * 
 * Procedure:   For every entry in the hash table that is not null we print
 *              into the dictionary file, the hash, the word, frequency and
 *              the position and the length of the corresponding listings
 *              that are been saved to the listings file.
 */
int htable_save(htable h) {
    int i = 0;
    long pos = 0;
    int length = 0;

    FILE *dict_file_pointer = fopen("index/dictionary" , "w");

    /* We're appending this, not overwriting */
    FILE *listings_file_pointer = fopen("index/listings" , "w");
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
            length = flexarray_save(h->dictionary[i].listings, listings_file_pointer);
            fprintf(
                dict_file_pointer, 
                "%d %s %d %lu %d\n", 
                i,
                h->dictionary[i].word, 
                h->dictionary[i].freq,
                pos,
                length
            );
            pos += length;
        }
    }
    htable_free(h);
    return EXIT_SUCCESS;
}

/* 
 * Used during searching.
 * 
 * Parameters:  dict_file, the file pointer of whee the dict is stored
 *              capacity, the size of the hashtable to create 
 * 
 * Returns:     the hashtable
 * 
 * Procedure:   Load Dictionary from index/file into hashtable, 
 *              no need to calculate hash insertion position as this was stored on
 *              index creation.
 *              This hashtable has more fields than the hash table created when indexing
 *              This is because we need to store the positiong and length of the associated
 *              listings flex array, so that we can query the location of the fle array 
 *              straight from disk using fseek
 */
htable htable_load_from_file(FILE* dict_file, int capacity){
    /* Variables to scanf into */
    size_t max_length = 100; /* awk '{print length}' dictionary | sort -rn | head -1 */
    int listing_hash;
    char listing_word[max_length];
    int listing_freq;
    long listing_pos;
    int listing_len;
    char buffer_string[max_length];
    htable h = emalloc(sizeof *h);
    h->capacity = capacity;
    h->dictionary = emalloc(h->capacity * sizeof h->dictionary[0]);
    h->count = calloc(h->capacity, sizeof(int));
    while (fgets(buffer_string, max_length, dict_file) != NULL) {  ;
        sscanf(
            buffer_string,
            "%d %s %d %lu %d",
            &listing_hash,
            listing_word,
            &listing_freq,
            &listing_pos,
            &listing_len
        );
        h->dictionary[listing_hash].word = emalloc((strlen(listing_word) + 1) * sizeof(listing_word[0]));
        strcpy(h->dictionary[listing_hash].word, listing_word);
        h->dictionary[listing_hash].freq = listing_freq;
        h->dictionary[listing_hash].pos = listing_pos;
        h->dictionary[listing_hash].len = listing_len;
    }   
    return h;
}



/* Trivial functions to make other code more readable */
long htable_get_pos(htable h, int hash){
    return h->dictionary[hash].pos;
}
int htable_get_len(htable h, int hash){
    return h->dictionary[hash].len;
}
int htable_get_freq(htable h, int hash){
    return h->dictionary[hash].freq;
}

/*
Debug functions for printing/dumping of htable contents 
*/
void htable_print(htable h){
    int i;
    for (i = 0; i < h->capacity; i++){
        if ( h->dictionary[i].word != NULL) {
            printf("%d: %s\n", i, h->dictionary[i].word);
            flexarray_print(h->dictionary[i].listings);
            printf("\n\n");
        }
    }
    printf("Number of words entered: %d\n\n", h->num_words);
}
void htable_print_loaded(htable h,  int start,  int finish){
    int i;
    for (i = start; i < h->capacity && i < finish; i++){
        if ( h->dictionary[i].word != NULL) {
            printf("%s %d %lu %d\n", 
                h->dictionary[i].word,
                h->dictionary[i].freq,
                h->dictionary[i].pos,
                h->dictionary[i].len
            );
        }
    }
}