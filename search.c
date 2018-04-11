#include "search.h"

struct document_rec{
    long docid;
    /* Used for merged results struct */
    double tf;
    /* Used for loading in the overall WC file */
    int count; 
};

struct search_term_rec{
    char* term;
    double idf;
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


/* Searchs through the dict and returns the flexarrays for each term that is returned */
void search_for_terms(htable dict, char** terms, int term_count){
    int i = 0;
    int term_i;
    int merged_i = 0;
    int smallest_length;
    int smallest_listing_index;
    unsigned int hash;
    flexarray all_listings[term_count];
    search_term search_terms[term_count];

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
            printf("%s Not Found!\n", terms[i]);
        }
        /* Add results term to the search_terms array */
        search_terms[term_i].term = emalloc( (strlen(terms[term_i]) + 1) * sizeof(search_terms[term_i].term[0] ) );
        strcpy(search_terms[term_i].term, terms[term_i]);
        /* Add IDF */
        search_terms[term_i].idf = log((double) NUM_DOCS_EXACT / (double) htable_get_freq(dict, hash));
    }
    /* determine smallest listing array */
    for(term_i = 0; term_i < term_count; term_i++){
        if(term_i == 0){
            smallest_length = flexarray_get_num_docs(all_listings[term_i]);
            smallest_listing_index = term_i;
        }
        else if(all_listings[term_i]->num_docs < smallest_length){
            smallest_length = flexarray_get_num_docs(all_listings[term_i]);
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
                
                double tf_idf = tf * search_terms[term_i].idf;
                
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
            free(search_terms[i].term);
        }
        flexarray_free(merged_results);
    }
    else{
        printf("No Results Found\n");
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

/* binary searches the wordcount list for docid and returns the document wordcount  */
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
