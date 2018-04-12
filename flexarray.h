#ifndef FLEXARRAY_H_
#define FLEXARRAY_H_

/* 
 * The listing struct
 * tf_idf is only used when we create a flexarray to contain our merged search results
 */
typedef struct listing_rec{
    int count;
    long doc_id;
    double tf_idf; /* Used for merged flex array */
} listing;

/* 
 * Flexarray strcture
 * with capacity and num_docs to ensure dynamic resizing
 * Thanks COSC242 
 */
typedef struct flexarrayrec{
    int capacity;
    int num_docs;
    listing* listings;
} *flexarray;


extern flexarray flexarray_new();

/* Used when creating index */
extern void flexarray_append(flexarray f, long id);
extern long flexarray_get_last_id(flexarray f);
extern void flexarray_updatecount(flexarray f);
extern int flexarray_save(flexarray f, FILE* listings_file_pointer);

/* Used when loading listing from file */
extern void flexarray_append_count_known(flexarray f, long id, int count);

/* Used when creating merged results */
extern int flexarray_get_wordcount(long docid, flexarray f, int start, int finish);
extern void flexarray_append_merged_result(flexarray f, int index, long id, double tf_idf);
extern void flexarray_update_merged_result(flexarray f, int index, double tf_idf);
extern char* stringify_merged_results_docid(long docid);

/* Functions called by qsort */
extern int flexarray_compare_docid(const void* first, const void* second);
extern int flexarray_compare_tf_idf(const void* first, const void* second);

/* Functions used when developing, or for debugging */
void flexarray_print_merged_results(flexarray f); 
extern void flexarray_print(flexarray f);

extern void flexarray_free(flexarray f);

#endif
