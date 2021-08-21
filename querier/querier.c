/*
 * Yi Xie  May 10, 2021
 *
 * querier.c
 *
 */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "index.h"
#include "pagedir.h"
#include "word.h"
#include "counters.h"
#include "hashtable.h"
#include "file.h"

/***********local struct********/
typedef struct ranking{
    int score;
    int docID;
} ranking_t;
/***********local struct********/


/**************function prototypes*************/
int fileno(FILE *stream);
void validateQuery(char** array, int len);
void simple_print(void* arg, const int key, const int count);
void calculateScore(index_t* it, counters_t* scoreKeeper, char** str, int len);
void help_accumulateScore(void* arg, const int key, const int count);
static void itemcount(void *arg, const int key,  const int count);
static char*** parse_andSequence(char** str, int* count, int len);
static void andSequenceAccumulate(counters_t* masterScore, char** andSequence, index_t* it);
static void counters_merge(counters_t* master, counters_t* local);
static void merge_help(void* arg, const int key, const int count);
static counters_t* counters_cleanup(counters_t* ctrs, index_t* it, char** str);
static void simple_set(void* arg, const int key, const int count);
static void cleanup_help(void* arg, const int key, const int count);
static void existence_check(void* arg, const int key, const int count);
static void simple_insert(void* arg, const int key, const int count);
static void rankBoard_sort(ranking_t** rankBoard, int slot);
/**************function prototypes*************/


static void
prompt(void)
{
  // print a prompt iff stdin is a tty (terminal)
  if (isatty(fileno(stdin))) {
    printf("Query? ");
  }
}

/************local struct functions************/
ranking_t*
ranking_new(int key, int count)
{
    ranking_t* ranking = malloc(sizeof(ranking_t));
    if (ranking == NULL){
        return NULL;
    } else {
        ranking->score = count;
        ranking->docID = key;
        return ranking;
    }

}

/************local struct functions************/


/*
 * parses arguments and continues to parse query string until eof.
 * run andSeqeunceAccumulate to gather scores in a counters
 * add counters id/count pairs to ranking struct and sort array from max to min
 * prints result to query
 *
 */
int
main(const int argc, const char* argv[])
{
    if (argc != 3){
        fprintf(stderr, "usage: ./querier pageDirectory indexFilename\n");
        exit(1);
    }
    //parses argument
    char* pageDirectory = (char*) argv[1];
    char* indexFilename = (char*) argv[2];

    //validates if directory is legal
    if (pagedir_validate(pageDirectory) != true){
        fprintf(stderr, "Incorrect page directory\n");
        exit(1);
    }

    //validates if index file is correct
    FILE* fp;
    fp = fopen(indexFilename, "r");
    if (fp == NULL){
        fprintf(stderr, "Index file cannot be opened\n");
        exit(1);
    }

    //load file into internal index structure
    index_t* it = index_loadFile(fp);
    fclose(fp);    
    if (it == NULL){
        fprintf(stderr, "Index loading failed\n");
        exit(1);
    }
    //start reading query till EOF or input error

    char data[1000]; //stores read string
    char** parsedString; //stores normalized string
    int i;

    printf("Query? ");
    while ((fgets(data, 1000, stdin) != NULL)){
        parsedString = parseString(data);
        if(parsedString == NULL){
            exit(1);
        }
        printf("Query: ");
        //prints normalized query
        for (i = 0; strcmp(parsedString[i], "\0") != 0; i++){
            printf("%s ", parsedString[i]); 
        }
        printf("\n");

        //validate if the sequence starts/ends with and/or
        validateQuery(parsedString, i);
        int andSequenceCount = 0;
        char*** queryString; //parse normalized string into and sequences
        
        counters_t* scoreKeeper = counters_new();
        //parses the and sequences and put and squence in an array of array of strings
        queryString = parse_andSequence(parsedString, &andSequenceCount, i);

        //loop through andsequence to accumualte
        for (int j = 0; j < andSequenceCount; j++){

            char** temp = queryString[j];
            if (temp == NULL){
                printf("temp is null\n");
            }
            andSequenceAccumulate(scoreKeeper, temp, it);
        }

        //if query is correct, initialize counters to keep scores
        if (scoreKeeper == NULL){
            fprintf(stderr, "out of memory for counter\n");
            exit(1);
        }

        
        int slot = 0; //tracks spots needed

        //calculate how many spots are need in array of ranking
        counters_iterate(scoreKeeper, &slot, itemcount);
        ranking_t** rankBoard = malloc(sizeof(ranking_t*)*slot);
        for(int p = 0; p < slot; p++){
            rankBoard[p] = NULL;
        }

        //stuff them into rankboard
        counters_iterate(scoreKeeper, rankBoard, &simple_insert);
        rankBoard_sort(rankBoard, slot); //sorts array
        if (slot == 0){
            printf("No documents match\n");
        } else {
            printf("Matches %d documents (ranked):\n", slot);
        }
        
        FILE* page;
        char* url;
        int keynum;

        //loop through array of ranking
        for (keynum = 0; keynum < slot; keynum++){

            ranking_t* currentRank = rankBoard[keynum];
            char* buf = malloc(200);
            if (buf == NULL){
                exit(1);
            }
            sprintf(buf, "%s/%d", pageDirectory, currentRank->docID);
            page = fopen(buf,"r");
            //open file for url
            if (page == NULL){
                fprintf(stderr, "File %d cannot be opened\n", currentRank->docID);
                exit(1);
            }
            url = file_readLine(page);

            //prints the current document
            printf("Score   %d  %d: %s\n", currentRank->score, currentRank->docID, url);
            free(buf);
            free(url);
            fclose(page);



        }
    
        //frees waht's inside the array of ranking
        for (keynum = 0; keynum < slot; keynum++){
            if(rankBoard[keynum] != NULL){
                free(rankBoard[keynum]);
            }
        }
        free(rankBoard);
        printf ("--------------------------------\n");
        prompt();
        counters_delete(scoreKeeper);
    
        //frees query string
        for(int h = 0; h < andSequenceCount; h++){

            char** temp = queryString[h];
            free(temp);

        }
        free(queryString);

        //frees parsedstring;
        for(int o = 0; strcmp(parsedString[o], "\0") != 0; o++){
            free(parsedString[o]);
        }

        free(parsedString);
    }
    index_delete(it);
    exit(0);
}

/*
 * takes a rankboard and total slots and sort it
 * in the order of max to min
 */
static void
rankBoard_sort(ranking_t** rankBoard, int slot)
{
    int i;
    int j;
    ranking_t* currentRank;

    for( i = 1; i < slot; i++){
        //keeps copy of current rank
        currentRank = rankBoard[i];
        j = i - 1;
        while (j >= 0 && (rankBoard[j]->score) < (currentRank->score)){
            rankBoard[j+1] = rankBoard[j];
            j = j - 1;
        }
        //move one spot over 
        rankBoard[j+1] = currentRank;
    }
}

/*
 * helper function for counters_iterate
 * simply news a struct based on key and count and adds to array
 * user needs to free it later
 */
static void
simple_insert(void* arg, const int key, const int count)
{
    ranking_t** rankBoard = arg;
    ranking_t* localBoard = ranking_new(key, count);
    int i;
    for(i = 0; rankBoard[i] != NULL; i++){
    }
    rankBoard[i] = localBoard;
}

/*
 * takes a score counter and an andsequence and index
 * verifies parameter 
 * loop through each string into the index counter to add to new counter
 * calculates the intersection within string and adds to master counter 
 * with union 
 *
 */
static void
andSequenceAccumulate(counters_t* masterScore, char** andSequence, index_t* it)
{
    
    if (masterScore == NULL || andSequence == NULL || it == NULL){
        return;
    }

    //local counter for and Sequence
    counters_t* ctrs = counters_new();
    if (ctrs == NULL){
        return;
    }

    //loop through andsequence to iterate with help func
    for (int i = 0; strcmp(andSequence[i], "\0") != 0; i++){
        counters_t* indexCounter = index_find(it, andSequence[i]);
        
        if (indexCounter == NULL){
            counters_delete(ctrs);
            return;
        }
        else {
            //add everything in in first loop
            if (i == 0) {
                //add first string into counter directly
                counters_iterate(indexCounter, ctrs, &simple_set);
            } else{
                //add the rest with help func
                counters_iterate(indexCounter, ctrs, &help_accumulateScore);
            }
        }
    }    
    //clean up the first string and set a clean local counter
    counters_t* cleanCtrs = counters_cleanup(ctrs, it, andSequence);
    counters_delete(ctrs);
    //union local counter with master counter
    counters_merge(masterScore, cleanCtrs);
    counters_delete(cleanCtrs);

}


//unions local counter into master counter
static void
counters_merge(counters_t* master, counters_t* local)
{
    counters_iterate(local, master, &merge_help);

}

//loop through local counter and union into master counter
static void
merge_help(void* arg, const int key, const int count)
{
    counters_t* scoreKeeper = (counters_t*)arg;

    //check the current score of the provided key
    int currentScore;
    currentScore = counters_get(scoreKeeper, key);

    //add new counters key if nonexistent
    if (currentScore == 0){
        if (counters_add(scoreKeeper, key) == 0){
            printf("counter add error\n");
        } else {
            if (counters_set(scoreKeeper, key, count) == false){
                printf("counter set error\n");
            }
        }

    } else {
        //otherwise just simply set current score to current score + count
        int newScore = currentScore + count;
        if (counters_set(scoreKeeper, key, newScore) == false){
                printf("counter set error\n");
        }
    }
}

//a simple struct to keep the index's counter and a scorekeeper with list
//of unwanted keys
typedef struct doubleCounter{
    counters_t* scoreKeep;
    counters_t* indexCount;
    int* unwanted;
} doubleCounter_t;

//loop though the counter with the andsequence string to clean up 
//returns clean counter that needs to be freed later
static counters_t*
counters_cleanup(counters_t* ctrs, index_t* it, char** str)
{
    counters_t* cleanCtrs = counters_new();
    int i = 1;
    int* unwanted = malloc(sizeof(int)*1000);

    for (int j = 0; j < 1000; j++){
        unwanted[j] = 0;
    }
    if(strcmp(str[i], "\0") == 0){
        counters_iterate(ctrs, cleanCtrs, &simple_set);
    }

    //loop up string from slot one and only add key we want to new counter
    for (i = 1; strcmp(str[i], "\0") != 0; i++){
        counters_t* ctrs1= index_find(it, str[i]);
        doubleCounter_t* doubleC = malloc(sizeof(doubleCounter_t));
        doubleC->scoreKeep = cleanCtrs;
        doubleC->indexCount = ctrs1;
        doubleC->unwanted = unwanted;
        counters_iterate(ctrs, doubleC, &cleanup_help);

        free(doubleC);
    }
    free(unwanted);
    return cleanCtrs;
}

//simple helper function to set a counter with a key and count
static void
simple_set(void* arg, const int key, const int count){
    counters_t* scoreKeeper = (counters_t*)arg;
    counters_set(scoreKeeper, key, count);
}
//a wrapper for a checking bool and key
typedef struct checker{
    bool check;
    int key;
} checker_t;

//a helper function for adding to the array of unwanted keys
static void 
doubleC_insert(doubleCounter_t* doubleC,int key)
{
    int* array = doubleC->unwanted;
    int i = 0;
    for(i = 0; array[i] != 0; i++){
        
    }
    array[i] = key;
}

//check if key is in the unwanted array
static bool
doubleC_exist(doubleCounter_t* doubleC, int key){
     int* array = doubleC->unwanted;
    int i = 0;
    for(i = 0; array[i] != 0;){
        
        if (array[i] == key){
            return true;
        }
        i++;
    }
    return false;
}

//loop through the string from slot 1, not original node if unfound key
//helper func called for counters_iterate
static void
cleanup_help(void* arg, const int key, const int count)
{
    doubleCounter_t* doubleC = arg;
    counters_t* scoreKeeper = doubleC->scoreKeep;
    counters_t* indexCounter = doubleC->indexCount;
    checker_t* checking = malloc(sizeof(checker_t));
    checking->check = false;
    checking->key = key;
    counters_iterate(indexCounter, checking, &existence_check);

    //only add to clean counter in key is not unwanted
    if ((checking->check) == true && doubleC_exist(doubleC, key) == false){
        counters_set(scoreKeeper, key, count);
        
    } else {
        //if key is unwanted, add to the unwanted array to keep track
        doubleC_insert(doubleC, key); 
        counters_iterate(scoreKeeper, NULL, &simple_print);
    }

    free(checking);
    

}

//updates bool to track whether key exists
static void
existence_check(void* arg, const int key, const int count)
{
    checker_t* checking = arg;
    if(key == (checking->key)){
        checking->check = true;
    }

}

/*
 * takes a str and parse into an array of array of string
 * In the new array, each inner array is andsequence
 * Words such as and and or are cleaned out from new array
 * Within each inner array is an intersection, and between inner array
 * is union.
 */ 
static char***
parse_andSequence(char** str, int* count, int len)
{
    if (str == NULL || count == NULL){
        return NULL;
    }
    char*** finalString = malloc((len) *sizeof(char**)); 
    if (finalString == NULL){
        return NULL;
        fprintf(stderr, "No space for final string\n");
    }
    char** andSequence = malloc(sizeof(char*)*(len+1));
    int i;
    int andSequenceSlot = 0;
    int finalSlot = 0;
    for (i = 0; i < len; i++){
        if (strcmp(str[i], "and") == 0){
            //skip if word is and
            continue;
        }
        if (strcmp(str[i], "or") == 0){
            //adds andsequence to final string if word is or
            andSequence[andSequenceSlot] = "\0";
            finalString[finalSlot] = andSequence;
            andSequenceSlot = 0;
            finalSlot++;
            andSequence = malloc(sizeof(char*)*(len+1));
            continue;
        }   
        andSequence[andSequenceSlot] = str[i];
        andSequenceSlot++;
    }
    //properly terminate each andsequence string
    andSequence[andSequenceSlot] = "\0";
    finalString[finalSlot] = andSequence;
    *count = finalSlot + 1;
    return finalString;
}

//simple help function to count number of keys in counter
static
void itemcount(void *arg, const int key,  const int count)
{
    int *nitems = arg;
    if(nitems != NULL && key >= 0 && count >= 0){
        (*nitems)++;
    }
}
//simple help function for printing counter
void 
simple_print(void* arg, const int key, const int count)
{
    printf("DocId = %d, Score = %d\n", key, count);
}

//loop through every word and run counters-iterate with help func to accumulate score
void 
calculateScore(index_t* it, counters_t* scoreKeeper, char** str, int len)
{
    if (it == NULL || scoreKeeper == NULL || str == NULL){
        fprintf(stderr,"calculate score wrong parameter\n");
        exit(1);
    }
    counters_t* ctrs;
    int i;
    for (i = 0; i < len; i++){

        ctrs = index_find(it, str[i]);
        //if index doesn't have this word, continue searching for next word
        if (ctrs == NULL){
            continue;
        }
        //if ctrs is found, accumulate scores
        counters_iterate(ctrs, (void*)scoreKeeper, &help_accumulateScore);
    }
}

//helper function that calculates intersection
void
help_accumulateScore(void* arg, const int key, const int count)
{
    counters_t* scoreKeeper = (counters_t*)arg;
    //check the current score of the provided key
    int currentScore;
    currentScore = counters_get(scoreKeeper, key);
    //ignore if there's no intersection
    if (currentScore == 0){
        return;
    } else {
        //keep the lower score of the intersection
        if (count < currentScore){
            counters_set(scoreKeeper, key, count);
        } else {
            return;
        }

    }   
}

//validates if query's binary operator locations are correct
void
validateQuery(char** array, int len)
{
    //chekcs for initial/final and/or
    if (strcmp(array[0], "and") == 0){
        fprintf(stderr, "Error: 'and' cannot be first\n");
        exit(1);
    }
    if (strcmp(array[0], "or") == 0){
        fprintf(stderr, "Error: 'or' cannot be first\n");
        exit(1);
    }

    if (strcmp(array[len-1], "and") == 0){
        fprintf(stderr, "Error: 'and' cannot be last\n");
        exit(1);

    }

    if (strcmp(array[len-1], "or") == 0){
        fprintf(stderr, "Error: 'or' cannot be last\n");
        exit(1);
    }
    int i;

    //check if two and and ors are consecutive
    for (i = 1; i < len-1; i++){
        if (strcmp(array[i], "and") == 0 || strcmp(array[i], "or") == 0){

        if (strcmp(array[i], "and") == 0){

            if (strcmp(array[i+1], "and") == 0){
                fprintf(stderr, "Error: 'and' and 'and' cannot be consecutive\n");
                exit(1);
            }

            if (strcmp(array[i+1], "or") == 0){
                fprintf(stderr, "Error: 'and' and 'or' cannot be consecutive\n");
                exit(1);
            }
        }
        if (strcmp(array[i], "or") == 0){

            if (strcmp(array[i+1], "and") == 0){
                fprintf(stderr, "Error: 'or' and 'and' cannot be consecutive\n");
                exit(1);
            }

            if (strcmp(array[i+1], "or") == 0){
                fprintf(stderr, "Error: 'or' and 'or' cannot be consecutive\n");
                exit(1);
            }
        }
        }
    }
}

