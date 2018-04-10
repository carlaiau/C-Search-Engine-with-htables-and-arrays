#include "search.h"

typedef struct flexarrayrec {
    int capacity;
    int num_docs;
    listing* listings;
};

typedef struct listing_rec {
    int count;
    long doc_id;
};


struct search_term_rec{
    char* term;
    double idf;
    document* documents;
};

struct document_rec{
    long docid;
    double tf;
};


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

/* Searchs through the dict and returns the flexarrays for each term that is returned */
void search_for_terms(htable dict, char** terms, int term_count){
    int i;
    int smallest_length = INT_MAX;
    int smallest_listing_index;
    unsigned int hash;
    flexarray all_listings[term_count];

    search_term results[term_count];

    /* retrieve the listings arrays */
    for(i = 0; i < term_count; i++){        
        /* for each term search the hash table and get the listings */            
        hash = htable_search(dict, terms[i]);
        if(hash){
            all_listings[i] = search_get_listings(htable_get_pos(dict, hash), htable_get_len(dict, hash));
            //printf("results for %s\n\n", terms[i]);
            //flexarray_print(listings[i]);                        
        }
        else{
            printf("%s Not Found!\n", terms[i]);
        }

        /* Add results term to the results array */
        results[i].term = emalloc( (strlen(terms[i]) + 1) * sizeof(results[i].term[0] ) );
        
        strcpy(results[i].term, terms[i]);
        /* Add IDF */

        results[i].idf = log((double) NUM_DOCS_EXACT / (double) htable_get_freq(dict, hash));
    }
    /* determine smallest listing array */
    for(i = 0; i < term_count; i++){
        if(flexarray_get_num_docs(all_listings[i]) < smallest_length){
            smallest_length = flexarray_get_num_docs(all_listings[i]);
            smallest_listing_index = i;
        } 
    }
    /* Because we will have a maximum merge of the smallest array then this is the maximum 
    amount of space that needs to be malloced 
    */
    for(i = 0; i< term_count; i++){
        results[i].documents = emalloc(smallest_length * sizeof(document));
    }
    
    printf("%s index %d contains %d items\n", 
        terms[smallest_listing_index], 
        smallest_listing_index, smallest_length
    );

    printf("Testing the saving of results as struct\nTerm: %s\tIDF: %f\n", 
        results[smallest_listing_index].term, 
        results[smallest_listing_index].idf
    );

    /* 
     *   iterate through smallest listing array and 
     *   save the docid, and tf into the results array
     */
    int merged_i = 0;
    for(i = 0; i < smallest_length; i++){
        int temp_wc;
        int curr_doc_wc[term_count];
        int inner_i = 0;
        int found_in_all = 1;
        
        //long docid = all_listings[smallest_listing_index]->listings[i].doc_id;
        
        /* 
         * Iterate through all listing arrays, with docids starting from index 0, of the shortest
         * listing array. We store the docids in a local array. If any array does not contain this 
         * docid, then we exit the for loop. If the docid is found in all listing arrays, then it is 
         * added to our merged set along with it's corresponding tf value on a per term basis */
        /*for(inner_i  = 0; inner_i  < term_count && found_in_all == 1; inner_i++){
            temp_wc = flexarray_get_wordcount(docid, all_listings[inner_i], 0, NUM_DOCS_EXACT);
            if(temp_wc != -1){
                found_in_all = 0;
            }
            else{
                curr_doc_wc[term_count] = temp_wc;
                printf("%d\n", curr_doc_wc[term_count]);
            }
        }*/

        
        /* docid found in all listing arrays, compute the tf of each result and save into the results documents array */
        /*if(found_in_all){
            for(inner_i = 0; inner_i < term_count; inner_i++){
                results[++merged_i].documents[inner_i].docid = docid;
                results[merged_i].documents[inner_i].tf =  curr_doc_wc[inner_i];
                // Make sure to make WC actually tf
            } 
        }*/

        
    }
}



/**
 * Retrieves the flexarray associated with the specific dictionary word
 * from the index listing file
 *
 * @param pos, the position of the flexarray within the file.
 * @param len, the length of the flexarray within the file.
 * 
 * @return flexarray. A flexarray of listings containing the docid and occurances
 * of all the documents that match the queried term.
 */
flexarray search_get_listings(long pos, int len){
    /* Tested with awk '{print length}' listings | sort -rn | head -1 */
    FILE *listings_file = fopen("index/listings", "r");
    int i = 0;
    long first_docid;
    long current_docid;
    int times_word_found = 0;
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
        }
        else if(i % 2 == 0){
            sscanf(token, "%lu", &current_docid);
            current_docid += first_docid;
        }
        else{
            sscanf(token, "%d", &times_word_found);
            flexarray_append_count_known(f, current_docid, times_word_found);
        }
        token = strtok(NULL, " ");
        i++;
    }
    return f;
}
