#ifndef __pagedir_H
#define __pagedir_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "webpage.h"




bool pagedir_init(const char* pageDirectory);
void pagedir_save(const webpage_t* page, const char* pageDirectory, const int docID);

/******* pagedir_validate **********/
/* 
 * We promise: 
 * To return true if directory has ./crawler file in it
 * false if otherwise
 */
bool pagedir_validate(const char* dir);

/******** pagedir_load *************/
/*
 * users provide:
 * correct diretory name and docID
 *
 * We promise:
 * To return pointer to webpage if loaded
 * Null otherwise
 *
 * Note:
 * Users should call webpage_delete on returned pointer later
 *
 */
webpage_t* pagedir_load(const char* dir, const int docID);







#endif // __pagedir_H

