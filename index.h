#ifndef INDEX_H_
#define INDEX_H_

typedef struct key_pair_rec key_pair;
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mylib.h"
#include "htable.h"

extern int create_index(char* input_file);
extern int compare_docid(const void* first, const void* second);
#endif
