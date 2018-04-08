#include "app.h"


/* 
    This is the main application file where 
    everything is going to be called from
*/
int main(int argc, char **argv){
    const char *optstring = "rs:pih";
    char option;
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
                dict = load_indexes();
                 
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
