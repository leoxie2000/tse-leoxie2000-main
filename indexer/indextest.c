/* indextest.c
 *
 * Yi Xie  May 9th
 *
 * loads data from index file and write out to a new index
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <index.h> 


// main function for testing

int
main(const int argc , const char* argv[])
{
    if (argc != 3){
        fprintf(stderr, "usage: ./indextest oldIndexFilename newIndexFilename\n");
        exit(1);
    }
    

    char* oldIndexFilename = (char*)argv[1];
    char* newIndexFilename = (char*)argv[2];

    FILE* oldFile = fopen(oldIndexFilename, "r");
    FILE* newFile = fopen(newIndexFilename, "w");

    if (oldFile == NULL || newFile == NULL){
        fprintf(stderr, "Incorrect filenames\n.");
        exit(1);
    }
    index_t* it = index_loadFile(oldFile);

    if (it == NULL){
        fprintf(stderr, "Index cannot be initialized\n");
        exit(1);
    }

    index_write(it, newFile);
    index_delete(it);
    fclose(oldFile);
    fclose(newFile);
    
}
  
