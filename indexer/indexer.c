/*
 * Yi Xie May 8th, 2021
 *
 * indexer.c
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "index.h"
#include "word.h"
#include "webpage.h"
#include "pagedir.h"
#include "counters.h"


/******** function prototypes **********/
void indexBuild(const char* pageDirectory, FILE* fp);
bool indexPage(index_t* it, webpage_t* page, const int docID);
/******** function prototypes **********/


/*
 * parses args and call indexBUild and index page
 *
 */
int
main(const int argc, const char* argv[])
{
    //defensive checks
    if (argc != 3){
        fprintf(stderr, "usage: ./indexer pageDirectory indexFilename\n");
        exit(1);
    } else {
        
        char* pageDirectory = (char*) argv[1];
        char* indexFilename = (char*) argv[2];

        if (pagedir_validate(pageDirectory) != true){
            fprintf(stderr, "Incorrect page directory\n");
            exit(1);
        }
        FILE* fp;
        fp = fopen(indexFilename, "w");

        //checking whether file can be opened
        if (fp == NULL){
            fprintf(stderr, "Index file cannot be opened\n");
            exit(1);
        }

        //
        indexBuild(pageDirectory, fp);
        fclose(fp);
        exit(0);
    }

}


/*
 * Users provide: proper directory, file to write to
 * 
 * build index using indexpage on every file in directory
 *
 */
void
indexBuild(const char* pageDirectory, FILE* fp)
{
    index_t* it = index_new();
    int docNum = 1;
    webpage_t* page;
    page = pagedir_load(pageDirectory, docNum);

    if (page == NULL){
        fprintf(stderr, "loading file with ID 1 failed\n");
        exit(1);
    }

    //continue to load pages and call indexpage on it
    while (page != NULL) {
        if (indexPage(it, page, docNum) == false){
            fprintf(stderr, "indexPage failed for docNum %d in directory %s\n", docNum, pageDirectory);

        }

        docNum++;
        webpage_delete(page);
        page = pagedir_load(pageDirectory, docNum);
    }

    webpage_delete(page);
    index_write(it, fp);
    index_delete(it);

}

/*
 * Takes input: index, webpage, and doc ID
 * loads the page into an index
 *
 * We return:
 * true if successful
 * false otherwise
 *
 */
bool 
indexPage(index_t* it, webpage_t* page, const int docID)
{
    if (page == NULL){
        return false;
    }
    int pos = 0;
    char* result;
        
    //continue reading from webpage and store in into index struct
    while ((result = webpage_getNextWord(page, &pos)) != NULL) {
        if (strlen(result) < 3){
            free(result);
        } else {
            char* result1;
            result1  = normalizeWord(result);
            free(result);

            if (result1 != NULL){
                if (index_save(it, result1, docID) != true){
                    free(result1);
                    return false;
                } else {
                    free(result1);
                }
            }
        }    
    }
    return true;
}
