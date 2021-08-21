# CS50 TSE Indexer
## Implementation Spec

Here we focus on the core subset:

-  Data structures
-  Control flow: pseudo code for overall flow, and for each of the functions
-  Detailed function prototypes and their parameters
-  Error handling and recovery
-  Testing plan

## Data structures 

We use data structure: an 'index' serving as wrapper for hashtable, which stores counters that have docID and count pairs.

## Control flow

The indexer is implemented in one file `indexer.c`, with three functions.

### main

The `main` function parses arguments, then simply calls `indexPage` and `indexBuild`, then exits zero.

### indexBuild

`indexBuild` builds index using `indexPage` on every file in directory

Pseudocode:
    creates a new 'index' object
    loops over document ID numbers, counting from 1
        loads a webpage from the document file 'pageDirectory/id'
        if successful, 
             passes the webpage and docID to indexPage

### indexPage

`indexPage` loads the file page into an index

Pseudocode:

    steps through each word of the webpage,
        skips trivial words (less than length 3),
        normalizes the word (converts to lower case),
        looks up the word in the index,
            adding the word to the index if needed
        increments the count of occurrences of this word in this docID

## Other modules

### pagedir

We create a re-usable module `pagedir.c` to handles the *pagesaver*  mentioned in the design (writing a page to the pageDirectory), and marking it as a Crawler-produced pageDirectory (as required in the spec).
We chose to write this as a separate module, in `../common`, to encapsulate all the knowledge about how to initialize and validate a pageDirectory, and how to write and read page files, in one place... anticipating future use by the Indexer and Querier.

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

### indexer

Detailed descriptions of each function's interface is provided as a paragraph comment prior to each function's implementation in `indexer.c` and is not repeated here.

```c
int main(const int argc, char* argv[]);
void indexBuild(const char* pageDirectory, FILE* fp);
bool indexPage(index_t* it, webpage_t* page, const int docID);
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
char* normalizeWord(const char* in);
```

## Error handling and recovery

All the command-line parameters are rigorously checked before any data structures are allocated or work begins; problems result in a message printed to stderr and a non-zero exit status.

Out-of-memory errors are handled by variants of the `mem_assert` functions, which result in a message printed to stderr and a non-zero exit status.
We anticipate out-of-memory errors to be rare and thus allow the program to crash (cleanly) in this way.

All code uses defensive-programming tactics to catch and exit (using variants of the `mem_assert` functions), e.g., if a function receives bad parameters.

That said, certain errors are caught and handled internally: for example, `pagedir_init` returns false if there is any trouble creating the `.crawler` file, allowing the Crawler to decide what to do; the `webpage` module returns false when URLs are not retrievable, and the Crawler does not treat that as a fatal error.

## Testing plan

Here is an implementation-specific testing plan.

### Unit testing

There are units: indexer, pagedir, index, word.
The indexer represents the whole system and is covered below.
The pagedir unit is tiny; it could be tested using a small C 'driver' to invoke its functions with various arguments, but it is likely sufficient to observe its behavior during the system test.



### Integration/system testing

Unit testing. A program indextest will serve as a unit test for the index module; it reads an index file into the internal index data structure, then writes the index out to a new index file.

Integration testing. The indexer, as a complete program, will be tested by building an index from a pageDirectory, and then the resulting index will be validated by running it through the indextest to ensure it can be loaded.

Test indexer with various invalid arguments. 2. no arguments 3. one argument 4. three or more arguments 5. invalid pageDirectory (non-existent path) 5. invalid pageDirectory (not a crawler directory) 6. invalid indexFile (non-existent path) 7. invalid indexFile (read-only directory) 7. invalid indexFile (existing, read-only file)
Run indexer on a variety of pageDirectories and use indextest as one means of validating the resulting index.
Run valgrind on both indexer and indextest to ensure no memory leaks or errors.
