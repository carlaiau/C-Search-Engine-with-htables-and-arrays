#include "app.h"
/* 
    This is the main application file where 
    everything is going to be called from
*/
int main(int argc, char **argv){
    char buffer[BUFFER_SIZE];
    char* token;
    char* temp_token;
    int term_count = 0;
    int term_max = 10;
    char** terms;
    int i;
    int len;
    int inner_i;

    htable dict;
    if(argc > 0){
        if(strcmp(argv[1], "parse") == 1){
            fprintf(stderr, "Parsing! input file: %s\n\n", argv[2]);
            parse(argv[2]);
        
        }
        else if(strcmp(argv[1], "index") == 0){
            fprintf(stderr, "Indexing!\ninput file: %s, this may take a while!\n\n", argv[2]);
            create_index(argv[2]);
        }
        else if(strcmp(argv[1], "search") == 0){
            /* Search from std in */
            dict = search_load_index();
            while(fgets(buffer, BUFFER_SIZE, stdin)){
                terms = emalloc(sizeof(terms[0]) * term_max);
                token = strtok(buffer, " ");			
                while(token != NULL) {
                    /* Test to make sure this is valid first */
                    /* Remove Non Alphabetical */
                    len = strlen(token);
                    temp_token = emalloc( ( len + 1) * sizeof(token[0]));
                    inner_i = 0;
			        for(i = 0; i < len; i++){   
				        if(isalpha(token[i])){
				            temp_token[inner_i++] = tolower(token[i]);
			            }
                    }
                    temp_token[inner_i] = '\0';
                    if(term_count == term_max - 1){
                        term_max *= 2;
                        terms = erealloc(terms, sizeof(terms[0]) * term_max);
                    }
                    terms[term_count] = emalloc( (len + 1) * sizeof(temp_token[0]));
                    strcpy(terms[term_count++], temp_token);    
                    token = strtok(NULL, " ");
                }
                if(term_count > 0){
                    search_for_terms(dict, terms, term_count);
                    for(i = 0; i < term_count; i++){
                        free(terms[i]);
                    }
                }
                free(temp_token);
                free(terms);
                term_max = 10;
                term_count = 0;
            }
        }
        else if( strcmp(argv[1], "help") == 0 
            || strcmp(argv[1], "-h") == 0
            || strcmp(argv[1], "man") == 0 
            ){

            fprintf(stderr, "\nUsage:\nparse\tfile_to_parse > output_file\n"); 
            fprintf(stderr, "index\tfile_to_index (Indexes will be created in index folder)\n");
            fprintf(stderr, "search\tQuery to search for will be taken from stdin (Indexes will be loaded from index folder)\n");  
            fprintf(stderr, "\n"); 
            
        }
        return EXIT_SUCCESS;
    }
    else{
        printf("What");
    }




    return EXIT_SUCCESS;
} 

