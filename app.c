#include "app.h"


/* 
    This is the main application file where 
    everything is going to be called from
*/
int main(int argc, char **argv){
    const char *optstring = "rs:pih";
    char option;
    unsigned int hash;
    htable dict;
    while((option = getopt(argc, argv, optstring)) != EOF){
        switch(option){
            case 'p':
                fprintf(stderr, "Parsing! input file: %s\n\n", argv[2]);
                parse(argv[2]);
                break;
            case 'i':
                fprintf(stderr, "Indexing!\ninput file: %s, this may take a while!\n\n", argv[2]);
                create_index(argv[2]);
                break;
            case 's':
                dict = search_load_index();                
                hash = htable_search(dict, argv[2]);
                if(hash){
                    search_get_listings(htable_get_pos(dict, hash), htable_get_len(dict, hash));
                }
                else{
                    printf("%s Not Found!\n", argv[2]);
                }
                break;
            case 'h':                            
            default:
                fprintf(stderr, "\nUsage:\n-p\tfile_to_parse > output_file\n"); 
                fprintf(stderr, "-i\tfile_to_index (Indexes will be created in index folder)\n");
                fprintf(stderr, "-s\tQuery to search for (Indexes will be loaded from index folder)\n");  
                fprintf(stderr, "\n"); 
                return EXIT_SUCCESS;
        }
    }




    return EXIT_SUCCESS;
} 
