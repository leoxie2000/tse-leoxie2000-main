

#ifndef __INDEX_H
#define __INDEX_H


#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

/****** struct for index type, stores hashtable inside********/
typedef struct index index_t;
/*****************************/

typedef struct counters counters_t;


/******* index_new *******/
/* initializes an index and an hashtable with size 800. 
 *
 * We return:
 * NULL if index and hashtable not initialized
 * pointer to index initialized if successful
 *
 * Note:
 * users are responsible for calling index_delete later
 */
index_t* index_new(void);



/******* index_save *******/
/* users provide: non-NULL index pointer, key for storing, and docID
 *
 * We:
 * store key into hashtable in the index,and initialize counters if needed
 * keep separate copies of the key, so the key can be freed.
 *
 * We return:
 * true if successful
 * false otherwise.
 */
bool index_save(index_t* it, const char* key, const int docID);

/******** index_delete *******/
/* users provide: non Null index pointer
 *
 * We promise: delete everything in that index
 *
 */
void index_delete(index_t* it);

/*********** index_write **********/
/* users provide: index pointer, file to write to
 * 
 * We promised:
 * write to file with format : key [docID count]...
 *
 * We stop if file or index is null
 */
void index_write(index_t* it, FILE* fp);


/*********** index_find ************/
/* users provide: non null index pointer, key to find 
 *
 * We return: 
 * null if cannot find key
 * Counter pointer for key otherwise
 */
counters_t* index_find(index_t* it, const char* key);


/******* index_loadFile *******/
/* users provide: proper file to load index from
 *
 * We return:
 * Pointer to index loaded if successful
 * Null otherwise
 */
index_t* index_loadFile(FILE* in);



/********** index_set ***********/
/* users provide: non null index, key, docID, and count pair
 *
 * We promise:
 * store the key as key of hashtable, docID and count into proper counter
 *
 */
void index_set(index_t* it, const char* key, const int docID, const int count);
#endif // __INDEX_H

