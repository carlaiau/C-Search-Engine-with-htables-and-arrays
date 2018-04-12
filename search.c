#include "search.h"

struct document_rec{
    long docid;
    /* Used for merged results struct */
    double tf;
    /* Used for loading in the overall WC file */
    int count; 
};

/* 
 * Loads htable
 * 
 * returns htable
 * 
 * Procedure    Loads hash table saved at index/dictionary 
 */
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

/*
 * Loads the word count file
 * 
 * returns array of documents
 * 
 * Procedure    Loads the word count document saved at
 *              index/wordcount
 */
document* load_word_count(){
    int counter = 0;
    int index = 0;
    int max_len = 20; /* No document ID and word count exceeds this */
    long doc_id;
    int word_count;
    char buffer[max_len]; 
    char* token;
    document* all_document_wcs = emalloc(NUM_DOCS_EXACT * sizeof(document));
    FILE *wordcount_file = fopen("index/wordcount", "r");
    if (!wordcount_file){
        fprintf(stderr, "Unable to open index!\n Please ensure that the \"index\" is relative to where you're executing \n");
        return NULL;
    }
    while(fgets(buffer, max_len, wordcount_file)){
        token = strtok(buffer, " ");
        while(token != NULL) {
            if(counter % 2 == 0){
                doc_id = atol(token);
            }
            else if(counter % 2 == 1){
                word_count = atoi(token);
                all_document_wcs[index].docid = doc_id;
                all_document_wcs[index++].count = word_count;
            }
            counter++;
            token = strtok(NULL, " ");
        }
    }
    return all_document_wcs;   
}

/* The Big Kahuna: This is where we do the search itself 
 *
 * Parameters   dict, the loaded in hashtable
 *              terms, an array of strings
 *              term count, the number of terms
 * 
 * Procedure    For each term, we do a hash for this term.
 *              If no hash is found for any of the terms we return from the function
 * 
 *              For each hash that is found we obtain all the listings for this term and save
 *              these listings into an array called all_listings.
 *              all_listings is indexed by the index of each term within the query.
 *              We also store the IDF of each term in an array indexed by the 
 *              index of each term within the query.
 *              
 *              Next we determine the smallest listing length, and the index of this term.
 *              We are using AND logic, so if a document does not exist in the smallest array
 *              it does not exist in the merged results.
 *              
 *              for each listing in the smallest listing array we check if it exists in the other 
 *              listing arrays, if it does, then it is added to the merged_results array, and at the same time
 *              calculating and storing it's combined tf_idf from the multiple terms.
 * 
 *              We then quicksort this merged_results array on the tf_idf score and print and free.
 *          
 */
void search_for_terms(htable dict, char** terms, int term_count){
    int i = 0;
    int term_i;
    int merged_i = 0;
    int smallest_length;
    int smallest_listing_index;
    unsigned int hash;
    flexarray all_listings[term_count];
    double search_term_idfs[term_count];

    flexarray merged_results = flexarray_new();
    document* all_document_wcs = load_word_count();

    /* retrieve the listings arrays */
    for(term_i = 0; term_i < term_count; term_i++){        
        /* for each term search the hash table and get the listings */            
        hash = htable_search(dict, terms[term_i]);
        if(hash){
            all_listings[term_i] = search_get_listings(htable_get_pos(dict, hash), htable_get_len(dict, hash));                    
        }
        else{
            /* Andrew this may be redundant */
            printf("%s Not Found!\n", terms[i]);
            return;
        }
        /* Add each terms IDF to the search_terms array */
        
        search_term_idfs[term_i] = log((double) NUM_DOCS_EXACT / (double) htable_get_freq(dict, hash));
    }
    /* determine smallest listing array */
    for(term_i = 0; term_i < term_count; term_i++){
        if(term_i == 0){
            smallest_length = all_listings[term_i]->num_docs;
            smallest_listing_index = term_i;
        }
        else if(all_listings[term_i]->num_docs < smallest_length){
            smallest_length = all_listings[term_i]->num_docs;
            smallest_listing_index = term_i;
        } 
    }

    /* 
     *   iterate through smallest listing array because this is an AND
     *   results will not exist without existing in this array and it is the shortest
     */
    for(i = 0; i < smallest_length; i++){    
        int temp_wc;        
        int curr_doc_wc[term_count];
        
        int found_in_all = 1;
        long doc_id = all_listings[smallest_listing_index]->listings[i].doc_id;
        
        /* 
         * Iterate through all listing arrays, with docids starting from index 0, of the shortest
         * listing array. We store the docids in a local array. If any array does not contain this 
         * docid, then we exit the for loop. 
         * */
        for(term_i  = 0; term_i < term_count && found_in_all == 1; term_i++){
            temp_wc = flexarray_get_wordcount(doc_id, all_listings[term_i], 0, all_listings[term_i]->num_docs);
            if(temp_wc == -1){
                found_in_all = 0;
            }
            else{
                curr_doc_wc[term_i] = temp_wc;
            }
        }        
        /* 
            docid found in all listing arrays, save this to the merged_listings flexarray
            storing the doc_id and tf_idf.
            If the term already exists in the flexarray, then we add to the tf_idf 
        */
        if(found_in_all == 1){
            for(term_i = 0; term_i < term_count; term_i++){

                double tf = (double) curr_doc_wc[term_i] / 
                    (double) documents_get_wordcount(doc_id, all_document_wcs, 0, NUM_DOCS_EXACT);
                
                double tf_idf = tf * search_term_idfs[term_i];
                
                /* this is the first instance of this doc_id */
                if(term_i == 0){ 
                    flexarray_append_merged_result(merged_results, merged_i, doc_id, tf_idf);
                }
                else{
                    flexarray_update_merged_result(merged_results, merged_i, tf_idf);
                }
            }
            merged_i++;
        }
    }
    if(merged_i > 0){
        qsort(merged_results->listings, merged_results->num_docs, sizeof(listing), flexarray_compare_tf_idf);
        flexarray_print_merged_results(merged_results);
        /* Free Everything */
        for(i = 0; i < term_count; i++){
            flexarray_free(all_listings[i]);
        }
        flexarray_free(merged_results);
    }
    else{
        
    }
    /* Andrew this may be redundant */
    printf("\n");
}



/**
 * Retrieves the flexarray associated with the specific dictionary word
 * from the index listing file
 * 
 *  Parameters:  pos, the position of the flexarray within the file.
 *               len, the length of the flexarray within the file.
 * 
 *  Return:      flexarray. listings containing the docid
 *  Procedure:   fseek to the exact location within the fiel, and read the correct length
 *               Use flexarray_append_count_known as we only call it once per document within the array
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

/* 
 * Binary search of document array, 
 * used for getting the total wordcount of a specific document.
 * Needed for tf_idf calculation
 */
int documents_get_wordcount(long docid, document* documents, int start, int finish){
    int midpoint = (finish + start) / 2;

    if(finish < start){
        return -1; 
    }   
    else if((documents[midpoint].docid > docid)){ 
        return documents_get_wordcount(docid, documents, start, midpoint - 1); 
    } 
    else if(documents[midpoint].docid < docid){ 
        return documents_get_wordcount(docid, documents, midpoint + 1, finish);
    }
    else {
        return documents[midpoint].count;
    }   
}
