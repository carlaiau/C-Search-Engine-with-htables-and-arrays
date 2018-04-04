#include <stdio.h>
#include <stdlib.h>
#include "mylib.h"
#include "htable.h"
#include <string.h>

/* emalloc with error checking */
void *emalloc(size_t s){
   void *result = malloc(s);
   if (NULL == result){
      fprintf(stderr, "memory allocation failed.\n");
      exit(EXIT_FAILURE);
   }
   return result;
}

/* erealloc with error checking */
void *erealloc(void *p, size_t s){
   void *result = realloc(p, s);
   if (NULL == result){
      fprintf(stderr, "memory allocation failed.\n");
      exit(EXIT_FAILURE);
   }
   return result;
}

/* binary searches for a given word */
int search(char *word, char **dict, int start, int finish){
    int m =(finish + start) / 2;
    if(finish < start){
        return -1;
    }
    else if(strcmp(dict[m], word) > 0){
        return search(word, dict, start, m - 1);
    } else if(strcmp(dict[m],word) < 0){
        return search(word, dict, m + 1, finish);
    }else {
        return m;
    }
}