#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mylib.h"
#include "app.h"

#define NUM_WORDS 900000
int load_indexes(){
    int i = 0; 
    size_t buffer_size = 500;
    int dict_index = 0;
    FILE* dict_file = fopen("index/dictionary", "r");
    char** dictionary = emalloc(NUM_WORDS * sizeof(dictionary[0]));
    
    int* listings_id_per_word =   emalloc(NUM_WORDS * sizeof(int));
    int* docs_per_word =        emalloc(NUM_WORDS * sizeof(int));
    char *temp_line = emalloc(500);
    while (getline(&temp_line, &buffer_size, dict_file) != EOF) {
        dictionary[dict_index] = emalloc((strlen(temp_line) + 1) * sizeof(char));
        sscanf(
            temp_line, 
            "%s %d %d", 
            dictionary[dict_index], 
            &(listings_id_per_word[dict_index]), 
            &(docs_per_word[dict_index])
        );
        dict_index++;
    }
    free(temp_line);

    for(i = 0; i <= dict_index; i++){
        printf("%s, %d, %d\n", dictionary[i], listings_id_per_word[i], docs_per_word[i]);

        if(i == 10){
            return 0;
        }
    }
    
    return 0;

}
