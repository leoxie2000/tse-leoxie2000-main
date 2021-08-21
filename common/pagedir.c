/*
 * Yi Xie April 30, 2021
 *
 * pagedir.c
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "webpage.h"
#include "file.h"

/*********function prototypes**/
bool pagedir_init(const char* pageDirectory);
void pagedir_save(const webpage_t* page, const char* pageDirectory, const int docID);
bool pagedir_validate(const char* dir);
webpage_t* pagedir_load(const char* dir, const int docID);
/******************/


/*
    construct the pathname for the .crawler file in that directory
open the file for writing; on error, return false.
close the file and return true.
*/
bool 
pagedir_init(const char* pageDirectory)
{

    char* pathname = malloc(50);

    //opening file based on concatenated pathname
    sprintf(pathname, "%s/.crawler", pageDirectory);
    if ((pathname) != NULL){
        FILE* fp = fopen(pathname, "w");
        if (fp != NULL){

            fclose(fp);
            free(pathname);
            return true;
        } else {
            return false;
        }

    } else {
        return false;
    }


}

/*
    construct the pathname for the page file in pageDirectory
open that file for writing
print the URL
print the depth
print the contents of the webpage
close the file
*/
void 
pagedir_save(const webpage_t* page, const char* pageDirectory, const int docID)
{
    char* buf = malloc(50);
    sprintf(buf, "%s/%d", pageDirectory, docID);
    if (buf != NULL){
        FILE* fp = fopen(buf,"w");
        free(buf);
        if (fp != NULL && page != NULL){
            char* url = webpage_getURL(page);
            char* html = webpage_getHTML(page);
            int depth = webpage_getDepth(page);

            if (url != NULL && html != NULL){

                //print url, depth, and html to file
                fprintf(fp, "%s\n%d\n%s\n", url, depth, html);

            }

        }
        fclose(fp);
    }
}

//for details, see pagedir.h
bool
pagedir_validate(const char* dir)
{
    FILE* fp;
    char pathname[100];

    //opening file based on concatenated pathname
    sprintf(pathname, "%s/.crawler", dir);
    if ((fp = fopen(pathname, "r")) != NULL){
        fclose(fp);
        return true;
    } else {
        
        return false;
    }
}

//for details, see pagedir.h
webpage_t* 
pagedir_load(const char* dir, const int docID)
{
    if (dir != NULL && docID >= 1){
        char* pathname = malloc(300);
        if (pathname == NULL){
            fprintf(stderr, "out of memory for loading pathname\n");
            return NULL;
        }
        //opening file based on concatenated pathname
        sprintf(pathname, "%s/%d", dir, docID);
        FILE* fp;
        fp = fopen(pathname, "r");
        free(pathname);

        if (fp == NULL){
            return NULL;
        } else {
            char* line;
            int depth = -1;
            char* url;

            //getting url
            url = file_readLine(fp);

            //getting depth
            line = file_readLine(fp);
            depth = atoi(line);
            free(line);

            //getting html
            line = file_readFile(fp);

            if (url != NULL && line != NULL && depth >= 0){
                webpage_t* page = webpage_new(url, depth, line);

                if (page != NULL){
                    fclose(fp);
                    return page;
                } else {
                    fclose(fp);
                    return NULL;
                }
            } else {
                fclose(fp);
                return NULL;
            }
        }

    } else {
        return NULL;
    }

}

