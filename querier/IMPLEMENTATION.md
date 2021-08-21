# CS50 TSE QUERIER
## Implementation Spec

Here we focus on the core subset:

-  Data structures
-  Control flow: pseudo code for overall flow, and for each of the functions
-  Detailed function prototypes and their parameters
-  Error handling and recovery
-  Testing plan

## Data structures 

We use data structure: a 'ranking'  which stores docID and count pairs.

We also use some local data structures:

 * a 'doubleCounter' that stores a counter for score keeping and a counter for the counter from index, as well as an array of unwanted keys
 * a 'checker' that stores a boolean and a integer for storing key


## Control flow

The querier is implemented in one file `querier.c`, with fifteen functions. The major functions with major help functions are listed here in detail. For a more detailed description of some of the local struct getters and setters, see the comments in `querier`

### main

The `main` function parses arguments, calls 'parse_andSequence' to parse string. Then calls andSeqeunceAccumulate to gather scores in a counters, then add counters id/count pairs to ranking struct and sort array from max to min and prints result to query.

### parse_andSeqeunce

`parse_andSequence` builds an array of array of andSeqeunces, and is thus the return type is `char***`

Pseudocode:
    
    creates a local array of array of strings
     loops an array of string and add them to a local array of string called andSequence
     whenever encounters an 'and', skip over the word
     whenever encounters and 'or', add the local andSequence to the array of array, and clean out andSequence
     returns the final array of array

### validateQuery

`validateQuery` verifies whether parsed string is proper based on its binary operators locations

Pseudocode:

    takes the query string
      check if the string ends/initializes with binary operator
      loops over the string looking for consequtive binary operators 
      
### calculateScore

`calculateScore` loops through every word and run counters-iterate with help func to accumulate score

Pseudocode:

    loop through every word in the string
     checks whether that word exists in index
     calls counters_iterate with hel_accumulateScore to accumulate score
     
     
### help_accumulateScore

`help_accumulateScore` is a helper function for calculate Score

Pseudocode:
    
     check whether there's intersection between key passed in and key previously checked
     If there's intersection:
         use the lower score
     else:
         just return
         
### andSequenceAccumulate

`andSequenceAccumulate` calculates score for each and seqeunce and merge it into master counter

Pseudocode:

     takes a score counter and an andsequence and an index
     loops through each string in the andsequence
     calculates the intersection between strings within andSequence
     calls counters_cleanup to clean up the local counter
     calls counters_merge to union into the score counter received as parameter
     
### counters_cleanup

`counters_cleanup` loops though the counter with the andsequence string to clean up unwanted keys, returns clean counter that needs to be freed later

Pseudocode:

     intialize a local new counter
     loops through the counter with the andsequence string
     calls counter_iterate with cleanup_help
         adds all wanted key, score pair into the new counter
     frees original counter
     returns new counter
     
### cleanup_help

`cleanup_help` is a helper function for counters_cleanup

Pseudocode:

    loop through the string
    if any of the counters key is not found, insert into an unwanted array
    otherwise, set the new counter with current pair


### rankBoard_sort
`rankBoard_sort` takes a rankboard and total slots and sort it in the order of max to min

Pseudocode:

     takes the array of ranking struct
     steps through the array
     keep copy of the key we want to move
     updates it with the key previous to it and move every key forward one slot
     continues until all keys are sortede
    
### counters_merge
`counters_merge` takes a counter to merge into, and adds all the counts to existing keys or initialize key if it doesn't exist yet

Pseudocode:

     checks whether key exists in the counter
     if exists:
         simply add count together
     otherwise:
         initialize a key and set to current count
## Other modules

### pagedir

We create a re-usable module `pagedir.c` to handles the *pagesaver*  mentioned in the design (writing a page to the pageDirectory), and marking it as a Crawler-produced pageDirectory (as required in the spec).
We chose to write this as a separate module, in `../common`, to encapsulate all the knowledge about how to initialize and validate a pageDirectory, and how to write and read page files, in one place... anticipating future use by the 
Indexer and Querier.

Pseudocode for `pagedir_init`:

	construct the pathname for the .crawler file in that directory
	open the file for writing; on error, return false.
	close the file and return true.


Pseudocode for `pagedir_save`:

	construct the pathname for the page file in pageDirectory
	open that file for writing
	print the URL
	print the depth
	print the contents of the webpage
	close the file

Pseudocode for `pagedir_validate`:

 check if ./crawler file exists in pageDirectory
    if so, return true
    otherwise return false

Pseudocode for `pagedir_load`:

 if fp is NULL, return NULL
    readLine the first line of the file to get the URL
    create a new webpage with that URL
    if webpage_fetch succeeds, return the webpage
    else return NULL


### libcs50

We leverage the modules of libcs50, most notably `counters`, `hashtable`, and `webpage`.
See that directory for module interfaces.
The new `webpage` module allows us to represent pages as `webpage_t` objects, to fetch a page from the Internet, and to scan a (fetched) page for URLs; in that regard, it serves as the *pagefetcher* described in the design.

## Function prototypes

### querier

Detailed descriptions of each function's interface is provided as a paragraph comment prior to each function's implementation in `querier.c` and is not repeated here.

```c
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

```

### pagedir

Detailed descriptions of each function's interface is provided as a paragraph comment prior to each function's declaration in `pagedir.h` and is not repeated here.

```c
bool pagedir_init(const char* pageDirectory);
void pagedir_save(const webpage_t* page, const char* pageDirectory, const int docID);
webpage_t* pagedir_load(const char* dir, const int docID);
bool pagedir_validate(const char* dir);
```

### index

```c
index_t* index_new(void);
bool index_save(index_t* it, const char* key, const int docID);
void index_delete(index_t* it);
void index_write(index_t* it, FILE* fp);
void index_set(index_t* it, const char* key, const int docID, const int count);
counters_t* index_find(index_t* it, const char* key);
index_t* index_loadFile(FILE* in);
```

### word

```c
char** parseString(const char str[]);
char* normalizeWord(const char* in);
```

## Error handling and recovery

All the command-line parameters are rigorously checked before any data structures are allocated or work begins; problems result in a message printed to stderr and a non-zero exit status. In this non-zero scenario, we cannot guarantee memory free.

Out-of-memory errors are handled by a message printed to stderr and a non-zero exit status.
We anticipate out-of-memory errors to be rare and thus allow the program to crash (cleanly) in this way.

All code uses defensive-programming tactics to catch and exit, if a function receives bad parameters.

That said, certain errors are caught and handled internally: for example, `pagedir_load` returns false if there is any trouble loading the index file.

## Testing plan

Here is an implementation-specific testing plan.


### Integration/system testing

*Integration testing*.  The *querier*, as a complete program, will be mainly tested with fuzzquery, and the protocol follows from below
1. Test `querier` with various invalid arguments.
	2. no arguments
	3. one argument
	4. three or more arguments
	5. invalid `pageDirectory` (non-existent path)
	5. invalid `pageDirectory` (not a crawler directory)
	6. invalid `indexFile` (non-existent path)
	7. incompatible `pageDirectory` and `indexFile`
	7. invalid queries with illegal characters
	8. queriers with extra spaces
	9. queries with upper case letters
 
10. Run *fuzzquery* on a variety of pageDirectories and study results
11. Run *valgrind* on *querier*  to ensure no memory leaks or errors.

