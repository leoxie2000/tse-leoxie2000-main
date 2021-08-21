/*
 * Yi Xie  Date: April 30, 2021
 *
 * crawler.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "webpage.h"
#include "pagedir.h"
#include "bag.h"
#include "hashtable.h"
#include <unistd.h>

/**********function prototypes***********/
int main(const int argc, char* argv[]);
static void parseArgs(const int argc, char* argv[],
                      char** seedURL, char** pageDirectory, int* maxDepth);
static void crawl(char* seedURL, char* pageDirectory, const int maxDepth);
static void pageScan(webpage_t* page, bag_t* pagesToCrawl, hashtable_t* pagesSeen);
/*******************************************/

/*
 * Main function to be called.
 * Simply calls parseargs and crawl
 */
int 
main(const int argc, char* argv[])
{
    char* seedURL;
    char* pageDirectory;
    int maxDepth;

    parseArgs(argc, argv, &seedURL, &pageDirectory, &maxDepth);
    crawl(seedURL, pageDirectory, maxDepth);

}


/*
 * Parses arguments passed from command lines
 * If arguments are improper, exit 1 and print error message
 */
static void 
parseArgs(const int argc, char* argv[],
                      char** seedURL, char** pageDirectory, int* maxDepth)
{
    if (argc == 4){

        //parse the arguments
        char* url = argv[1];
        url = normalizeURL(url);

        char* pD = argv[2];
        int mD;
        mD = atoi(argv[3]);

        //if all fits the criteria, proceed, otherwise exit
        if (isInternalURL(url) && pD != NULL && mD >= 0 && mD <= 10){
            if(pagedir_init(pD)){
                *seedURL = url;
                *pageDirectory = pD;
                *maxDepth = mD;
            } else {
                fprintf(stderr, "Arguments provided incorrect 1!\n");
                exit(1);
            }
            
        } else {
            fprintf(stderr, "Arguments provided incorrect 2!\n");
            exit(1);
        }

    } else {
        fprintf(stderr, "usage: crawler seedURL pageDirectory maxDepth\n");
        exit(1);
    }
}

/*
 * Crawls the page from seed URL and exits 1 if out of memory.
 * Otherwise, call pagescan to scan and pagedir_save to save pages to directory
 */
static void 
crawl(char* seedURL, char* pageDirectory, const int maxDepth)
{
    //initialize hashtable of pages seen and insert current seed url
    hashtable_t* pagesSeen = hashtable_new(200);
    if (pagesSeen != NULL){
        hashtable_insert(pagesSeen, seedURL, "exist");

    } else {
        exit(1);
    }
    
    //initialize bag of pages to vist and add current page
    bag_t* pagesToCrawl = bag_new();
    if (pagesToCrawl != NULL){
        webpage_t* currentPage = webpage_new(seedURL, 0, NULL);
        if (currentPage == NULL){
            exit(1);
        } else {
            bag_insert(pagesToCrawl, currentPage);
        }
    } else {
        exit(1);
    }

    //extracting pages from bag and start crawling
    int pos = 1;
    webpage_t* page;
    while ((page = bag_extract(pagesToCrawl)) != NULL){
        sleep(1);
        if (webpage_fetch(page)){
            printf("%d  Fetched: %s\n", webpage_getDepth(page), webpage_getURL(page));
            pagedir_save(page, pageDirectory, pos);
            if (webpage_getDepth(page) < maxDepth){
                pageScan(page, pagesToCrawl, pagesSeen);             
            }

        } else {
            fprintf(stderr, "webpage_fetch failed\n");
        }
        webpage_delete(page);
        pos++;
    }
    hashtable_delete(pagesSeen, NULL);
    bag_delete(pagesToCrawl, NULL);

}


/*
 * Scans the webpage given, add new URL to future crawls, and seen url to hashtable.
 * Logs the process and print to stdout
 */
static void 
pageScan(webpage_t* page, bag_t* pagesToCrawl, hashtable_t* pagesSeen)
{
    int pos = 0;
    char* url;
    char* url0;

    printf("%d  Scanning: %s\n", webpage_getDepth(page), webpage_getURL(page));

    //looping through the urls in webpage
    while ((url0 = webpage_getNextURL(page,&pos)) != NULL){
        printf("%d   Found: %s\n", webpage_getDepth(page), url0);
        url = normalizeURL(url0);
        free(url0); 
        if (isInternalURL(url)){
           //if not in hashtable
           if (hashtable_find(pagesSeen, url) == NULL){
               //if insert is successful
               char* tempURL = malloc(strlen(url)+1);
               strcpy(tempURL, url);

               //inserting into seen pages in hashtable
               if(hashtable_insert(pagesSeen, tempURL, "exist")){
                   webpage_t* newPage = webpage_new(tempURL, webpage_getDepth(page) + 1, NULL);
                   if (newPage != NULL){
                       //insert new pages in to bag
                       bag_insert(pagesToCrawl, newPage);
                       printf("%d   Added: %s\n", webpage_getDepth(page), tempURL);
                   }
               }
           } else {  
               printf("%d  IgnDupl: %s\n", webpage_getDepth(page), url);
           }
       } else { 
           printf("%d  IgnExtrn: %s\n", webpage_getDepth(page), url);
       }
       free(url);
    }
}
