#!/bin/bash

mkdir ../data
mkdir ../data/letters

#testing with wrong url
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.hml ../data/letters 2

#testing with wrong folder path
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../dat/letters 2

#testing with wrong depth
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../data/letters 11

#testing with missing arguments
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../data/letters

#testing with external url
./crawler https://en.wikipedia.org/wiki/Algorithm ../data/letters 2


#valgrind test with depth 1 on toscrape/index.html
mkdir ../data/toscrape-1
valgrind --leak-check=full --show-leak-kinds=all ./crawler http://cs50tse.cs.dartmouth.edu/tse/toscrape/index.html ../data/toscrape-1 1


#testing with letters/index.html depth 0,1,2,10
mkdir ../data/letters-0
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../data/letters-0 0

mkdir ../data/letters-1
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../data/letters-1 1

mkdir ../data/letters-2
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../data/letters-2 2

mkdir ../data/letters-10
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../data/letters-10 10

#testing with toscrape depth 0,2
mkdir ../data/toscrape-0
./crawler http://cs50tse.cs.dartmouth.edu/tse/toscrape/index.html ../data/toscrape-0 0

mkdir ../data/toscrape-2
./crawler http://cs50tse.cs.dartmouth.edu/tse/toscrape/index.html ../data/toscrape-2 2

#testing with wikipedia depth 0,1
mkdir ../data/wikipedia-0
./crawler http://cs50tse.cs.dartmouth.edu/tse/wikipedia/index.html ../data/wikipedia-0 0
mkdir ../data/wikipedia-1
./crawler http://cs50tse.cs.dartmouth.edu/tse/wikipedia/index.html ../data/wikipedia-1 1
