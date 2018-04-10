#include "search.h"


struct search_term_rec{
    char* term;
    double idf;
    document* documents;
};

struct document_rec{
    long docid;
    /* Used for merged results struct */
    double tf;

    /* Used for loading in the overall WC file */
    int count; 
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
    int i;
    int inner_i;
    int smallest_length = INT_MAX;
    int smallest_listing_index;
    
    unsigned int hash;
    flexarray all_listings[term_count];
    search_term results[term_count];
    int merged_i = 0;
    int merged_results = 0;

    document* all_document_wcs = load_word_count();

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
    for(i = 0; i < term_count; i++){
        results[i].documents = emalloc(smallest_length * sizeof(document));
    }
    

    /* 
     *   iterate through smallest listing array and 
     *   save the docid, and tf into the results array
     */
    merged_i = 0;
    for(i = 0; i < smallest_length; i++){
        
        int temp_wc;
        int curr_doc_wc[term_count];
        int inner_i = 0;
        int found_in_all = 1;
        long doc_id = all_listings[smallest_listing_index]->listings[i].doc_id;
        
        /* 
         * Iterate through all listing arrays, with docids starting from index 0, of the shortest
         * listing array. We store the docids in a local array. If any array does not contain this 
         * docid, then we exit the for loop. If the docid is found in all listing arrays, then it is 
         * added to our merged set along with it's corresponding tf value on a per term basis 
         * */
        for(inner_i  = 0; inner_i < term_count && found_in_all == 1; inner_i++){
            temp_wc = flexarray_get_wordcount(doc_id, all_listings[inner_i], 0, all_listings[inner_i]->num_docs);
            if(temp_wc == -1){
                found_in_all = 0;
            }
            else{
                curr_doc_wc[inner_i] = temp_wc;
            }
        }        
        /* 
            docid found in all listing arrays, compute the tf of each result and 
            save into the results term based  documents array 
        */

       
        if(found_in_all == 1){
            for(inner_i = 0; inner_i < term_count; inner_i++){
                results[inner_i].documents[merged_i].docid = doc_id;
                results[inner_i].documents[merged_i].tf = 
                    (double) curr_doc_wc[inner_i] / (double) documents_get_wordcount(doc_id,all_document_wcs, 0, NUM_DOCS_EXACT);
                merged_results++;
                // Make sure to make WC actually tf
            }
            merged_i++;
        }
    }
    
   
    
    if(merged_results > 0){
        printf("Merged Results: %d\n", merged_results);
        for(i = 0; i < term_count; i++){
            printf("\n\n\nTerm: %s\n", results[i].term);
            printf("IDF: %f\n", results[i].idf);
            for(inner_i = 0; inner_i < merged_results; inner_i++){
                if(results[i].documents[inner_i].tf != 0){
                    printf("%lu: %f\n",results[i].documents[inner_i].docid, results[i].documents[inner_i].tf);
                }

            }
        }
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
