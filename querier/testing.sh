#!/bin/bash




#case when directory opened and index don't match, thus exit 1 and claims file cannot be opened though words are found
./fuzzquery ~/cs50-dev/shared/tse/output/toscrape-1.index 10 11111 | ./querier ~/cs50-dev/shared/tse/output/letters-10 ~/cs50-dev/shared/tse/output/toscrape-1.index

#testing with proper query
echo "search and fourier or tse" | ./querier ~/cs50-dev/shared/tse/output/toscrape-1 ~/cs50-dev/shared/tse/output/toscrape-1.index


#testing with multiple spaces
echo "tab     here    output  " | ./querier ~/cs50-dev/shared/tse/output/toscrape-1 ~/cs50-dev/shared/tse/output/toscrape-1.index


#testing with special characters
echo "two-part" | ./querier ~/cs50-dev/shared/tse/output/toscrape-1 ~/cs50-dev/shared/tse/output/toscrape-1.index

echo "two part'!" | ./querier ~/cs50-dev/shared/tse/output/toscrape-1 ~/cs50-dev/shared/tse/output/toscrape-1.index


echo "two, part" | ./querier ~/cs50-dev/shared/tse/output/toscrape-1 ~/cs50-dev/shared/tse/output/toscrape-1.index

#testing with consecutive and and ors
echo "search and and tse" | ./querier ~/cs50-dev/shared/tse/output/toscrape-1 ~/cs50-dev/shared/tse/output/toscrape-1.index

echo "search or or here" | ./querier ~/cs50-dev/shared/tse/output/toscrape-1 ~/cs50-dev/shared/tse/output/toscrape-1.index

#testing with initial/terminal and and ors
echo "and fourier" | ./querier ~/cs50-dev/shared/tse/output/toscrape-1 ~/cs50-dev/shared/tse/output/toscrape-1.index
echo "or fourier" | ./querier ~/cs50-dev/shared/tse/output/toscrape-1 ~/cs50-dev/shared/tse/output/toscrape-1.index
echo "fourier and" | ./querier ~/cs50-dev/shared/tse/output/toscrape-1 ~/cs50-dev/shared/tse/output/toscrape-1.index
echo "fourier or" | ./querier ~/cs50-dev/shared/tse/output/toscrape-1 ~/cs50-dev/shared/tse/output/toscrape-1.index

#testing with bad folder and bad index
echo "fourier" | ./querier ~/cs50-dev/shared/tse/output/toscrape ~/cs50-dev/shared/tse/output/toscrape-1.index

echo "fourier" | ./querier ~/cs50-dev/shared/tse/output/toscrape ~/cs50-dev/shared/tse/output/toscrape-8.index

#testing with extra arguments
echo "fourier" | ./querier ~/cs50-dev/shared/tse/output/toscrape ~/cs50-dev/shared/tse/output/toscrape-1.index extra

#testing with 1 argument
echo "fourier" | ./querier ~/cs50-dev/shared/tse/output/toscrape
#testing with no argument
echo "fourier" | ./querier
#testing with uppercase letters
echo "fourieR AND tse" | ./querier ~/cs50-dev/shared/tse/output/toscrape ~/cs50-dev/shared/tse/output/toscrape-1.index
#some fuzz testing with different seeds and index
./fuzzquery ~/cs50-dev/shared/tse/output/toscrape-1.index 10 1211 | ./querier ~/cs50-dev/shared/tse/output/toscrape-1 ~/cs50-dev/shared/tse/output/toscrape-1.index


./fuzzquery ~/cs50-dev/shared/tse/output/toscrape-2.index 10 111 | ./querier ~/cs50-dev/shared/tse/output/toscrape-2 ~/cs50-dev/shared/tse/output/toscrape-2.index

./fuzzquery ~/cs50-dev/shared/tse/output/toscrape-2.index 10 131 | ./querier ~/cs50-dev/shared/tse/output/toscrape-2 ~/cs50-dev/shared/tse/output/toscrape-2.index

./fuzzquery ~/cs50-dev/shared/tse/output/toscrape-2.index 10 11 | ./querier ~/cs50-dev/shared/tse/output/toscrape-2 ~/cs50-dev/shared/tse/output/toscrape-2.index
./fuzzquery ~/cs50-dev/shared/tse/output/toscrape-2.index 10 11 | ./querier ~/cs50-dev/shared/tse/output/toscrape-2 ~/cs50-dev/shared/tse/output/toscrape-2.index
./fuzzquery ~/cs50-dev/shared/tse/output/toscrape-2.index 10 151 | ./querier ~/cs50-dev/shared/tse/output/toscrape-2 ~/cs50-dev/shared/tse/output/toscrape-2.index
#some valgrind test
./fuzzquery ~/cs50-dev/shared/tse/output/toscrape-2.index 10 4511 | valgrind --leak-check=full --show-leak-kinds=all ./querier ~/cs50-dev/shared/tse/output/toscrape-2 ~/cs50-dev/shared/tse/output/toscrape-2.index
