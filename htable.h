#ifndef HTABLE_H_
#define HTABLE_H_

typedef struct htablerec *htable;
typedef struct dict_rec dict;

extern int 		htable_insert(htable h, char *token, long docid);
extern htable 	htable_new(int capacity);
extern void		htable_print(htable h);
extern int      htable_search(htable h, char *search_term);
extern int      htable_save(htable h);
extern int      htable_compare_words(const void* first, const void* second);
#endif
