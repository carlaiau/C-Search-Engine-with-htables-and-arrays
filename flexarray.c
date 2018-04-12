#include <stdio.h>
#include <stdlib.h>
#include "mylib.h"
#include "flexarray.h"


/*
 * Return:      flexarray, the newly created flexarray. 
 * Procedure:   emalloc flexarray and set the capacity and item count, 
 *              emalloc items.
 */
flexarray flexarray_new(){
    flexarray f = emalloc(sizeof(f));
    f->capacity = 2;
    f->num_docs = 0;
    f->listings = emalloc((f->capacity) * sizeof(listing));
    return f;
}

/*
 * Parameters:  flexarray, the flexarray to be deleted. 
 * Procedure:   free the listings along with the flexarray itself
 */
void flexarray_free(flexarray f){
    free(f->listings);
    free(f);
}


/* 
 * Used during index creation
 * 
 * Parameters: f, flexarray
 * Returns: long, last added doc_id
 * 
 * Procedure: Returns the last written doc_id for this term
 */
long flexarray_get_last_id(flexarray f) {
    return f->listings[f->num_docs - 1].doc_id;
}

/* 
 * Used during index creation
 * 
 * Parameters:  f, flexarray
 *              id, long doc_id to add
 * 
 * Procedure:   Check if we're at capacity, if so double memory allocation
 *              Add a new listing for this doc_id
 */
void flexarray_append(flexarray f, long id){
    if (f->num_docs == f->capacity){
        f->capacity *= 2;
        f->listings = erealloc(f->listings, (f->capacity) * sizeof(listing));
    }
    f->listings[f->num_docs].count = 1;
    f->listings[f->num_docs++].doc_id = id;
}

/* 
 * Used during index creation
 *
 * Parameters:  flexarray to be updated;
 * Procedure:   increments the last listing's count
 */
void flexarray_updatecount(flexarray f) {
    f->listings[f->num_docs - 1].count++;
}


/* 
 * Used during listings file
 * 
 * Parameters: f flexarray to add to
 *             id, long index of specific id
 *             count int, number of terms in this doc_id
 * 
 * Procedure:  Same as above flexarray append, but because we know the total 
 *             count of the term in each document when loading file
 *             it only needs to get called once per document
 */ 
void flexarray_append_count_known(flexarray f, long id, int count){
    if (f->num_docs == f->capacity){
        f->capacity *= 2;
        f->listings = erealloc(f->listings, (f->capacity) * sizeof(listing));
    }
    f->listings[f->num_docs].count = count;
    f->listings[f->num_docs++].doc_id = id;  
}

/* 
 * Used during listings file
 * 
 * Parameters: f flexarray to add to
 *             index, index within the flexarray to update
 *             id, long index of specific id
 *             tf_idf double, the calculated tf_idf of this specific listing in relation to the term
 * 
 * Procedure:  Same as above flexarray append, but because we kno
 *             count of the term in each document when loading file
 *             it only needs to get called once per document
 */ 
void flexarray_append_merged_result(flexarray f, int index, long id, double tf_idf){
    if (f->num_docs == f->capacity){
        f->capacity *= 2;
        f->listings = erealloc(f->listings, (f->capacity) * sizeof(listing));
    }
    f->listings[index].tf_idf = tf_idf;
    f->listings[index].doc_id = id;  
    f->num_docs++;
}

/*
 * Used duing merging of multiple term listings on a AND logic basis
 * 
 * Parameters:  f, flexarray to be updated;
 *              index, the listing index;
 *              tf_idf, the tf_idf of the term currently seen 
 *
 * Procedure:   This function is called when we know this listing is already in the merged
 *              results flexarray, and we are adding another terms tf_idf to the tf_idf that
 *              is already defined due to previous terms.
 */
void flexarray_update_merged_result(flexarray f, int index, double tf_idf) {
    f->listings[index].tf_idf += tf_idf;
}


/*
 * Used duing creation of index
 * 
 * Parameters:  f, flexarray to be updated;
 *              listings_file_pointer, the file we're writing the listings to
 * 
 * Returns:     int the length written to the file             
 *
 * Procedure:   We get all the listings associated with a specific term
 *              we then sort this array of listings and then compress by only storing
 *              the difference between each corresponding docid, and the first docid of the array
 *              As we are printing, we are keeping track of how many characters we are prining, so that
 *              can be sent back to the terms hash table, and is printed as the length, to be used with fseek.
 */

int flexarray_save(flexarray f, FILE* listings_file_pointer){
    int i;
    int length = 0;
    long first_docid = 0;
    qsort(f->listings, f->num_docs, sizeof(listing), flexarray_compare_docid);
    /* compress listing docids */
    first_docid = f->listings[0].doc_id;

    for (i = 0; i < f->num_docs; i++) {
        if(i > 0){ /* only save listing id difference */
            length += ( long) fprintf(
                listings_file_pointer,
                "%lu %d ", 
                f->listings[i].doc_id - first_docid,
                f->listings[i].count
            );
        }
        else{
            length+= ( long)  fprintf(
                listings_file_pointer,
                "%lu %d ", 
                f->listings[i].doc_id,
                f->listings[i].count
            );
        }
    }
    length += ( long) fprintf(listings_file_pointer, "\n");
    flexarray_free(f);
    return length;
}

/* 
 * 
 * Functions below this are trivial 
 * 
 */



/* docid_comparison used by internal quicksort */
int flexarray_compare_docid(const void* first, const void* second) {
    const listing* first_word_listing = first;
    const listing* second_word_listing = second;
    long first_doc_id = first_word_listing->doc_id;
    long second_doc_id = second_word_listing->doc_id;
    if (first_doc_id > second_doc_id) {
        return 1;
    } else if (first_doc_id < second_doc_id) {
        return -1;
    } else {
        return 0;
    }
}
/* tf_idf_comparison used by internal quicksort */
int flexarray_compare_tf_idf(const void* first, const void* second) {
    const listing* first_word_listing = first;
    const listing* second_word_listing = second;
    double first_doc_idf = first_word_listing->tf_idf;
    double second_doc_idf = second_word_listing->tf_idf;

    if (first_doc_idf < second_doc_idf) {
        return 1;
    } else if (first_doc_idf > second_doc_idf) {
        return -1;
    } else {
        return 0;
    }
}

/* binary searches the docid list and returns the posting count */
int flexarray_get_wordcount(long docid, flexarray f, int start, int finish){
    int midpoint = (finish + start) / 2;
    if(finish < start){
        return -1; 
    }   
    else if((f->listings[midpoint].doc_id > docid)){ 
        return flexarray_get_wordcount(docid, f, start, midpoint - 1); 
    } 
    else if(f->listings[midpoint].doc_id < docid){ 
        return flexarray_get_wordcount(docid, f, midpoint + 1, finish);
    }
    else {
        return f->listings[midpoint].count;
    }   
}

/*
 * Add back the WSJ prefix to the long document 
 * id and also add in the hyphen
 */ 
char* stringify_merged_results_docid(long docid){
    char* decompressed_id = emalloc(15 * sizeof(char));
    sprintf(decompressed_id, "WSJ%lu", docid);
    decompressed_id[13] = decompressed_id[12];
    decompressed_id[12] = decompressed_id[11];
    decompressed_id[11] = decompressed_id[10];
    decompressed_id[10] = decompressed_id[9];
    decompressed_id[9] = '-'; 
    return decompressed_id;
}


/* Print function used for outputting query results */
void flexarray_print_merged_results(flexarray f) {
    int i;
    for (i = 0; i < f->num_docs; i++) {
        printf("%s\t%f\n", stringify_merged_results_docid(f->listings[i].doc_id), f->listings[i].tf_idf);
    }
}

/* Debugging/Development Printing function */
void flexarray_print(flexarray f) {
     int i;
    for (i = 0; i < f->num_docs; i++) {
        printf("%lu: %d\t", f->listings[i].doc_id, f->listings[i].count);
        if(i % 5 == 0){
            printf("\n");        
        }
    }
    printf("\nTotal: %d\n\n", f->num_docs);
}
