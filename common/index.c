/*
 * Yi Xie Date: May,8 2021
 *
 * leoxie2000
 *
 * index.c
 *
 * implements hashtable of counters, for details, see index.h
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "hashtable.h"
#include "counters.h"
#include <string.h>
#include "file.h"


/***************** struct type*****************/
typedef struct index{
    hashtable_t* hashtable;
} index_t;
/*************************************************************/

/***************** function prototypes *****************/
index_t* index_new(void);
bool index_save(index_t* it, const char* key, const int docID);
void index_delete(index_t* it);
static void itemdelete(void* item);
static void itemprint(void* arg, const char* key, void* item);
static void countprint(void* arg, const int key, const int count);
void index_write(index_t* it, FILE* fp);
void index_set(index_t* it, const char* key, const int docID, const int count);
counters_t* index_find(index_t* it, const char* key);
index_t* index_loadFile(FILE* in);
/********************************************************/


//for details, see index.h comments about usage
index_t*
index_new()
{
    index_t* it = malloc(sizeof(index_t*));

    //initialize hashtable with size 800
    hashtable_t* ht = hashtable_new(800);
    if (it != NULL && ht != NULL){
        it->hashtable = ht;
        return it;
    } else {
        return NULL;
    }
}


//for details, see index.h comments about usage
//only return true when succesfully saved
bool
index_save(index_t* it, const char* key, const int docID)
{

    if (it != NULL && key != NULL){
        hashtable_t* ht = it->hashtable;
        if (ht != NULL){
            //check if the key exists in the hashtable
            void* item = hashtable_find(ht, key);

            //if no item for key, initiate counter
            if (item == NULL){
                counters_t* ctrs = counters_new();
                if (counters_add(ctrs, docID) != 0){
                    return (hashtable_insert(ht, key, ctrs));
                      
                } else {
                    return false;
                }
            //if counter exists, just add
            } else {
                if (counters_add(item, docID) != 0){
                    return true;
                } else {
                    return false;
                }
            }
        } else {
            return false;
        }
    } else {
        return false;
    }

}



//for details, see index.h comments about usage
void
index_set(index_t* it, const char* key, const int docID, const int count)
{
    if (it == NULL || key == NULL || docID < 1 || count <= 0){
        return;
    }

    hashtable_t* ht = it->hashtable;
    if (ht == NULL){
        return;
    }

    //see if key already exists
    counters_t* ctrs = hashtable_find(ht, key);

    //if key exists
    if (ctrs != NULL){
        counters_set(ctrs, docID, count);
    } else {
        counters_t* ctrs1 = counters_new();
        counters_set(ctrs1, docID, count);
        hashtable_insert(ht, key, ctrs1);
    }
}


//for details, see index.h comments about usage
index_t* 
index_loadFile(FILE* in)
{
    if (in == NULL){
        return NULL;
    }

    index_t* it = index_new();
    if (it == NULL){
        return NULL;
    }

    int docID;
    int count;
    char* line;
    char* str;
    
    while ((line = file_readLine(in)) != NULL){

        str = strtok(line, " ");
        //tokenize line by space
        char* key = calloc(strlen(str)+1, sizeof(char));
        strcpy(key, str);

        int loopNum = 1; //keep track of whether we are looping to doc id or count

        while ((str = strtok(NULL, " ")) != NULL){

            //odd numbers are docID, even numbers are count
            if ((loopNum % 2) != 0){
                docID = atoi(str);
            } else {
                count = atoi(str);
                index_set(it, key, docID, count);
            }
            loopNum++;
        }

        free(key);
        free(line);

    }
    return it;

}


//for details, see index.h comments about usage
void
index_delete(index_t* it)
{
    if (it != NULL){

        hashtable_t* ht = it->hashtable;
        
        if (ht != NULL){
            hashtable_delete(ht, &itemdelete);
            free(it);
            return;
        }
        free(it);

    }



}

//for details, see index.h comments about usage
counters_t*
index_find(index_t* it, const char* key)
{
    if (it != NULL && key != NULL){
        hashtable_t* ht = it->hashtable;
        if (ht != NULL){
            counters_t* ctrs = hashtable_find(ht, key);
            if (ctrs !=  NULL){
                return ctrs;
            } else {
                return NULL;
            }
        }
    }
    return NULL;
}

//for details, see index.h comments about usage
void
index_write(index_t* it, FILE* fp)
{
    if (it != NULL && fp != NULL){
        hashtable_t* ht = it->hashtable;
        if (ht != NULL){
            hashtable_iterate(ht, fp, &itemprint);
        }

    }

}

/************ local functions *****************/

//use in iterators to print
static void 
itemprint(void* arg, const char* key, void* item)
{
    if (arg != NULL && key != NULL && item != NULL){
        FILE* fp = (FILE*) arg;
        counters_t* ctrs = (counters_t*) item;

        fputs(key, fp);
        fputs(" ", fp);
        counters_iterate(ctrs, arg, &countprint);
        fputs("\n", fp);
    }


    
}

//used in iterators to print
static void
countprint(void* arg, const int key, const int count)
{
    if (arg != NULL){

        FILE* fp = (FILE*) arg;
        fprintf(fp, "%d %d ", key, count);
    }
    

}

//used in iterators to delete counters
static void
itemdelete(void* item)
{
    counters_delete((counters_t*)item);
}

/************ local functions *****************/
