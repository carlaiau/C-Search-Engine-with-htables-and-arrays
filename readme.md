
  
First attempt at building a Search Engine in C. Specifically written for a set of WSJ articles from the 80s. 

Simple Data structures used are:

- hashtable - dict/vocab
- flexarray - listings/postcount



## Compile using:
    gcc -W -Wall -ansi -o2 -lm *.c -o executable_name


## Parse
    ./executable_name parse file_name

will parse the filename to stdout.

Approximate execution time 1 minutes.

## Index
    ./executable_name index parsed_file_name

will create three index files from this parsed input file:

 - index/dictionary
 - index/listings
 - index/wordcount
 

You need to ensure there is a folder relative to where you're executing called index for the indexes to be written too.
Approximate execution time 2 minutes.

## Search
    ./executable_name search

Will listen on stdin until EOF, therefore we can pipe a line separated query file to this command. The executable must be run with the indexes relative to the executable file. Please change directory to **/home/cshome/c/caiau/info-ass** and run the executable from here. Index folder is set as 775, and index files within as 644.

execution time, 3 seconds to perform the 50 queries in test-search.txt file
    
## Notes

I would like to add better program management around the creation and loading of indexes. Ideally you can index to a specific dir, and also search against a specific dir rather than having to cd but I got lazy.

Potential Bug, when running via hex some of the DOC_IDs are spitting out two ?? after them. I can't replicate this issue when running on local Mac OSX, but this will break automated testing, if it still present. I am compiling with 

|e-dir=/usr/include/c++/4.2.1
|Apple LLVM version 8.1.0 (clang-802.0.42)

If that changes anything?