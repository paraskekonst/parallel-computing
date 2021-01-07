PAGERANK 

parallel implementation: pagerank_gauss_seidel.c
serial   implementation: serial.c

In order to compile the programs run the following commands:
for parallel: gcc -O3 pagerank_gauss_seidel.c -o gs -lpthread
for serial : gcc -O3 se.c -o se

Arguments:
parallel progmam needs two arguments : first argument is dataset's filename and second argument is number of threads 
serial program needs one argument : dataset's filename

In order to excute the programs run 
for parallel: ./gs filename number_of_threads
for example : ./gs  web-Stanford.txt 2

for serial : ./se filename 
for example : ./se  web-Stanford.txt


test.c : this program compares the results of pagerank_gauss_seidel.c and matlabs scrip
to compile : gcc -O3 test.c -o test
arguments : needs one argument : 1 for set1 with 643 nodes or 2 for set2 with 2426 nodes
to excute  : >/test 1 
or : ./test 2
