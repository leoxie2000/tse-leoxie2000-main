




#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

//prototype
char* normalizeWord(const char* in);


/****** normalizeWord *******/
/*
 * Takes a word input and returns copy for it
 *
 * Needs to be freed later
 *
 */

char* 
normalizeWord(const char* in)
{
    //malloc space for word copy
    char* out = calloc(strlen(in)+1, sizeof(char));
    int i = 0;
    if (in == NULL){
        return NULL;
    }
    char ch;
    strcpy(out, in);

    while (out[i] != '\0'){
        ch = out[i];
        out[i] = tolower(ch);
        i++;

    }
    return out;
}

//parses string into normalized string with white space as delimiter
char**
parseString(const char str[])
{
    int i; //keep track of location in old string
    int j = 0; //keep track of location of string in new string
    int k = 0; //keep track of location of char in newstring[]
    int NUMBER_OF_STRINGS = 25;
    int STRING_LENGTH = 25;

    char** newString = malloc(sizeof(char*) * NUMBER_OF_STRINGS);
    newString[0] = malloc(sizeof(char)*STRING_LENGTH);
    for (i = 0; str[i] != '\0'; i++){

        
        //if letter is not letter or white space, it's invalid
        if (isalpha(str[i]) == 0 && isspace(str[i]) == 0){
            printf("Error: bad character '%c' in query\n", str[i]);
            return NULL;
        }
        if (isspace(str[i]) != 0){

            //check if two spaces in a row
            if (isspace(str[i+1]) != 0){
                
                continue;
            } else {
                
                newString[j][k] = '\0';
                k = 0;
                j++;

                newString[j] = malloc(sizeof(char)*STRING_LENGTH);
                continue;
            }
        }
        newString[j][k] = tolower(str[i]);
        k++;
    }

    free(newString[j]);
    newString[j] = "\0";

    return newString;
}
