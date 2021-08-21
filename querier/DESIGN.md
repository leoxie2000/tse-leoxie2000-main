# CS50 TSE QUERIER
## Design Spec 

According to the [Querier Requirements Spec](REQUIREMENTS.md), the TSE *querier* is a standalone program that reads the document files produced by the TSE crawler and index file built by the indexer, takes query from stdin and writes search results to stdout.  Its companion, the *fuzzquery* generates random inputs for querier based on seedfile for testing. In this document we cover only the *querier*.

### User interface

The querier's interface with the user is on the command-line; Otherwise searches can be pipelined in as well. The querier itself should always have two arguments

```
querier pageDirectory indexFilename
```

For example, if `letters` is a pageDirectory in `../data`,

``` bash
$ querier ../data/letters ../data/letters.index
```

### Inputs and outputs

**Input**: the querier loads the index file into an internal index structure provided by index.c. Then it continues to take inputs from stdin and run the query process. If it is unable to open a document when displaying the search result, it will exit with status 1.



**Output**: We display the serach result with the format shown in demo, containing score, document ID, and URL, ranked based on score.

### Functional decomposition into modules

We anticipate the following modules or functions as well as some helper functions for iterators(see IMPLEMENTATION.md):

 1. *main*, which parses arguments and call other functions to parse string and accumulate counter, at last display ranking;
 2. *validateQuery*， which validates if query's binary operator locations are correct
 3. *calculateScore*, which loops through every word and run counters-iterate with help func to accumulate score
 4. *parse_andSequence*,  which takes a string and parse into an array of array of strings. In the new array, each inner array is an andsequence. Words such as and and or are cleaned out from new array
 5. *andSeqeunceAccumulate*, which loops through each string into the index counter to add to new counter calculates the intersection within string and adds to master counte
 6. *counters_merge*, which unions local counter into master counter
 7. *counters_cleanup*, which loops though the counter with the andsequence string to clean up unwanted keys, returns clean counter that needs to be freed later

 8. *rankBoard_sort*, which takes a rankboard and total slots and sort it in the order of max to min

And some local struct:

 1. *ranking_t*, a struct that wraps up a (document ID, score) pair;

And some module that helps:

 1. *word*, a module that handles string parsing and normalizing words
 

### Pseudo code for logic/algorithmic flow

The querier will run as follows:

    parses arguments and continues to parse query string until eof using parse_andSequence and 
    run andSeqeunceAccumulate to gather scores in a counters
    add counters id/count pairs to ranking struct and sort array from max to min
    prints result to query
    

where *validateQuery:*

      takes the query string
      check if the string ends/initializes with binary operator
      loops over the string looking for consequtive binary operators 

where *calculateScore:*

     loop through every word in the string
     checks whether that word exists in index
     calls counters_iterate with helper function to accumulate score

where *parse_andSequence:*

     creates a local array of array of strings
     loops an array of string and add them to a local array of string called andSequence
     whenever encounters an 'and', skip over the word
     whenever encounters and 'or', add the local andSequence to the array of array, and clean out andSequence
     returns the final array of array
     
     
where *andSequenceAccumulate:*

     takes a score counter and an andsequence and an index
     loops through each string in the andsequence
     calculates the intersection between strings within andSequence
     calls counters_cleanup to clean up the local counter
     calls counterS_merge to union into the score counter received as parameter
     
     
where *counters_merge:* 
    
     simply iterate through the local counter and union it into master counter


where *counters_cleanup:*

     intialize a local new counter
     loops through the counter with the andsequence string
     adds all wanted key, score pair into the new counter
     frees original counter
     returns new counter
     
where *rankBoard_sort:*
    
     takes the array of ranking struct
     steps through the array
     keep copy of the key we want to move
     updates it with the key previous to it and move every key forward one slot
     continues until all keys are sorted

     
     

### Major data structures


The *index* is a *hashtable* keyed by *word* and storing *counters* as items.
The *counters* is keyed by *docID* and stores a count of the number of occurrences of that word in the document with that ID. 


### Testing plan

*Unit testing*.  Since there's no main module for the querier, the unit testing will mainly consist of passing different arguments through the commandline to test parsing.


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

