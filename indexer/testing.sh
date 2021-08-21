#!/bin/bash

# testing with faulty directory name
./indexer ../dat ../data/letters-0.index

# testing with faulty index outfile
./indexer ../data/letters-0 ../da/letters-0.index

# testing with extra arguments
./indexer ../data/letters-0 ../data/letters-0.index abc

# testing with directory without ./crawler
./indexer ../data/letters ../data/letters-0.index

#testing with no argument
./indexer

#testing with readonly file

./indexer ../data/letters-0 ../data/readOnly/1

#testing with readonly directory as file

./indexer ../data/letters-0 ../data/readOnly

# testing with correct directory letters-0
./indexer ../data/letters-0 ../data/letters-0.index
./indextest ../data/letters-0.index ../data/letters-0cmp.index
~/cs50-dev/shared/tse/indexcmp ../data/letters-0.index ../data/letters-0cmp.index

# testing with letters-1
./indexer ../data/letters-1 ../data/letters-1.index
./indextest ../data/letters-1.index ../data/letters-1cmp.index
~/cs50-dev/shared/tse/indexcmp ../data/letters-1.index ../data/letters-1cmp.index


# testing with wikipedia-0
./indexer ../data/wikipedia-0 ../data/wikipedia-0.index
./indextest ../data/wikipedia-0.index ../data/wikipedia-0cmp.index
~/cs50-dev/shared/tse/indexcmp ../data/wikipedia-0.index ../data/wikipedia-0cmp.index

# testing with valgrind on indexer
valgrind --leak-check=full --show-leak-kinds=all ./indexer ../data/letters-1 ../data/letters-1.index

#testing with valgrind on indextest
valgrind --leak-check=full --show-leak-kinds=all ./indextest ../data/letters-1.index ../data/letters-1cmp.index
