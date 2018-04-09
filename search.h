#ifndef SEARCH_H_
#define SEARCH_H_
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include "htable.h"
#include "flexarray.h"

extern htable search_load_index();

extern flexarray* search_get_listings(long pos, int len);

#endif
