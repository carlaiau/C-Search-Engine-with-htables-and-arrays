#ifndef SEARCH_H_
#define SEARCH_H_
#include <math.h>
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include "mylib.h"
#include "htable.h"
#include "flexarray.h"

#define NUM_WORDS 250000 
#define NUM_DOCS_EXACT 173252

typedef struct search_term_rec search_term;
typedef struct document_rec document;

extern htable search_load_index();
extern void search_for_terms(htable dict, char** terms, int term_count);

extern flexarray search_get_listings(long pos, int len);

extern documents_get_wordcount(long docid, document* documents, int start, int finish);
#endif
