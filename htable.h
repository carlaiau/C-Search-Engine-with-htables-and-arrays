#ifndef HTABLE_H_
#define HTABLE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mylib.h"
#include "flexarray.h"

typedef struct htablerec *htable;
typedef struct dict_rec dict;

extern int 		htable_insert(htable h, char *token, long docid);
extern htable 	htable_new(int capacity);
extern void		htable_print(htable h);
extern void		htable_print_loaded(htable h,  int start,  int finish);

extern unsigned int      htable_search(htable h, char *search_term);
extern int      htable_save(htable h);
extern int      htable_compare_words(const void* first, const void* second);
extern htable   htable_load_from_file(FILE* dict_file, int capacity);

extern long htable_get_pos(htable h, int hash);
extern int htable_get_len(htable h, int hash);
extern int htable_get_freq(htable h, int hash);
#endif
