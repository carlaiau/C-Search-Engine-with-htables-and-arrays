#include <stdio.h>
#include <stdlib.h>
#include "mylib.h"
#include "flexarray.h"


struct flexarrayrec {
    unsigned int capacity;
    unsigned int num_docs;
    listing* listings;
};

struct listing_rec {
    int count;
    long doc_id;
};


/* Initialises new flexarray */
flexarray flexarray_new(){
    flexarray f = emalloc(sizeof(f));
    f->capacity = 2;
    f->num_docs = 0;
    f->listings = emalloc((f->capacity) * sizeof(listing));
    return f;
}

/* Updates count of term in document */
void flexarray_updatecount(flexarray f) {

    (f->listings[f->num_docs - 1].count)++;

}

/* Returns the last written doc_id for this term */
long flexarray_get_last_id(flexarray f) {

    return f->listings[f->num_docs - 1].doc_id;

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

/* Prints this flexarray */
void flexarray_print(flexarray f) {
    unsigned int i;
    for (i = 0; i < f->num_docs; i++) {
        printf("%d: %lu\t", f->listings[i].count, f->listings[i].doc_id);

    }
}

void flexarray_save(flexarray f, FILE* listings_file_pointer){
    unsigned int i;
    qsort(f->listings, f->num_docs, sizeof(listing), flexarray_compare_docid);
    for (i = 0; i < f->num_docs; i++) {
        fprintf(
            listings_file_pointer,
            "%d %lu\t", 
            f->listings[i].count, 
            f->listings[i].doc_id
        );
    }
    fprintf(listings_file_pointer, "\n");
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