To compile parser:
gcc -Wall -pedantic parse.c -lm -o built/parser


To compile indexer:

gcc -Wall -ansi -pedantic flexarray.c htable.c index.c mylib.c -lm -o built/indexer


