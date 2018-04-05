#include <stdio.h>
#include <stdlib.h>
#include "mylib.h"
#include "flexarray.h"


struct flexarrayrec {
    int capacity;
    int num_docs;
    posting* postings;
};

struct posting_rec {
    int count;
    long docid;
};


/* Initialises new flexarray */
flexarray flexarray_new(){
    flexarray f = emalloc(sizeof(f));
    f->capacity = 2;
    f->num_docs = 0;
    f->postings = emalloc((f->capacity) * sizeof(posting));
    return f;
}

/* Updates count of term in document */
void flexarray_updatecount(flexarray f) {

    (f->postings[f->num_docs - 1].count)++;

}

/* Returns the last written docid for this term */
long flexarray_get_last_id(flexarray f) {

    return f->postings[f->num_docs - 1].docid;

}
/* Adds a new docid for this term */
void flexarray_append(flexarray f, long id){
    if (f->num_docs == f->capacity){
        f->capacity *= 2;
        f->postings = erealloc(f->postings, (f->capacity) * sizeof(posting));
    }
    f->postings[f->num_docs].count = 1;
    f->postings[f->num_docs++].docid = id;
}

/* Prints this flexarray */
void flexarray_print(flexarray f) {
    int i;
    for (i = 0; i < f->num_docs; i++) {
        printf("%d: %lu\t", f->postings[i].count, f->postings[i].docid);

    }
}

void flexarray_save(flexarray f, FILE* listings_file_pointer){
    int i;
    for (i = 0; i < f->num_docs; i++) {
        fprintf(
            listings_file_pointer,
            "%d %lu\t", 
            f->postings[i].count, 
            f->postings[i].docid
        );
    }
    fprintf(listings_file_pointer, "\n");
}
