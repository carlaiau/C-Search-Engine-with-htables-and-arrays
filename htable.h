#ifndef HTABLE_H_
#define HTABLE_H_

typedef struct htablerec *htable;
typedef struct listing_rec listing;

extern int 		htable_insert(htable h, char *token, long docid);
extern htable 	htable_new(int capacity);
extern void		htable_print(htable h);
extern int      htable_search(htable h, char *search_term);
extern int      htable_save(htable h);

#endif
