#include <stdio.h>
#include <stdlib.h>
#include "mylib.h"
#include "flexarray.h"


/* Initialises new flexarray */
flexarray flexarray_new(){
    flexarray f = emalloc(sizeof(f));
    f->capacity = 2;
    f->num_docs = 0;
    f->listings = emalloc((f->capacity) * sizeof(listing));
    return f;
}
static void flexarray_free(flexarray f){
    free(f->listings);
    free(f);
}
/* Updates count of term in document */
void flexarray_updatecount(flexarray f) {
    (f->listings[f->num_docs - 1].count)++;
}

/* Returns the last written doc_id for this term */
long flexarray_get_last_id(flexarray f) {
    return f->listings[f->num_docs - 1].doc_id;
}

int flexarray_get_num_docs(flexarray f){
    return f->num_docs;
}
/* Adds a new doc_id for this term */
void flexarray_append(flexarray f, long id){
    if (f->num_docs == f->capacity){
        f->capacity *= 2;
        f->listings = erealloc(f->listings, (f->capacity) * sizeof(listing));
    }
    f->listings[f->num_docs].count = 1;
    f->listings[f->num_docs++].doc_id = id;
}

void flexarray_append_count_known(flexarray f, long id, int count){
    if (f->num_docs == f->capacity){
        f->capacity *= 2;
        f->listings = erealloc(f->listings, (f->capacity) * sizeof(listing));
    }
    f->listings[f->num_docs].count = count;
    f->listings[f->num_docs++].doc_id = id;  
}

/* Prints this flexarray */
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

 int flexarray_save(flexarray f, FILE* listings_file_pointer){
    int i;
    long length = 0;
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



/* Compare doc_ids. Used in qsort */
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
