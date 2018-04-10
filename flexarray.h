#ifndef FLEXARRAY_H_
#define FLEXARRAY_H_

typedef struct listing_rec{
    int count;
    long doc_id;
} listing;

typedef struct flexarrayrec{
    int capacity;
    int num_docs;
    listing* listings;
} *flexarray;


extern flexarray flexarray_new();
extern void flexarray_append(flexarray f, long id);

extern void flexarray_append_count_known(flexarray f, long id, int count);

extern void flexarray_print(flexarray f);

extern long flexarray_get_last_id(flexarray f);
extern int flexarray_get_num_docs(flexarray f);

extern void flexarray_updatecount(flexarray f);
extern  int flexarray_save(flexarray f, FILE* listings_file_pointer);

extern int flexarray_get_wordcount(long docid, flexarray f, int start, int finish);

extern int flexarray_compare_docid(const void* first, const void* second);
#endif
