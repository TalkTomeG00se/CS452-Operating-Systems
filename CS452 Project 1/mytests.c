#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/times.h> /* for times system call */
#include <sys/time.h>  /* for gettimeofday system call */
#include <unistd.h>
#include <error.h>
#include "lab.h"
#include <pthread.h> // adding pthread library

// function prototypes
int check_if_sorted(int A[], int n);
void generate_random_array(int A[], int n, int seed);
void serial_mergesort(int A[], int p, int r);
void merge(int A[], int p, int q, int r);
void insertion_sort(int A[], int p, int r);
double getMilliSeconds(void);


/*
---------------------------------------------------------------------------
clock_t times(struct tms *buffer);
times() fills the structure pointed to by buffer with
time-accounting information.  The structure defined in
<sys/times.h> is as follows:
struct tms {
    clock_t tms_utime;       user time
    clock_t tms_stime;       system time
    clock_t tms_cutime;      user time, children
    clock_t tms_cstime;      system time, children
The time is given in units of 1/CLK_TCK seconds where the
value of CLK_TCK can be determined using the sysconf() function
with the agrgument _SC_CLK_TCK.
---------------------------------------------------------------------------
*/


float report_cpu_time(void)
{
	struct tms buffer;
	float cputime;

	times(&buffer);
	cputime = (buffer.tms_utime)/ (float) sysconf(_SC_CLK_TCK);
	return (cputime);
}


float report_sys_time(void)
{
	struct tms buffer;
	float systime;

	times(&buffer);
	systime = (buffer.tms_stime)/ (float) sysconf(_SC_CLK_TCK);
	return (systime);
}

double getMilliSeconds(void)
{
    struct timeval now;
    gettimeofday(&now, (struct timezone *)0);
    return (double) now.tv_sec*1000.0 + now.tv_usec/1000.0;
}


/*
 * generate_random_array(int A[], int n, int seed):
 *
 * description: Generate random integers in the range [0,RANGE]
 *              and store in A[1..n]
 */

#define RANGE 1000000

void generate_random_array(int A[], int n, int seed)
{
    int i;

	srandom(seed);
    for (i=1; i<=n; i++)
        A[i] = random()%RANGE;
}


/*
 * check_if_sorted(int A[], int n):
 *
 * description: returns TRUE if A[1..n] are sorted in nondecreasing order
 *              otherwise returns FALSE
 */

int check_if_sorted(int A[], int n)
{
	int i=0;

	for (i=1; i<n; i++) {
		if (A[i] > A[i+1]) {
			return FALSE;
		}
	}
	return TRUE;
}

int main(int argc, char **argv) {

	if (argc < 3) { // there must be at least one command-line argument. Note: changed to 3 from 2, added argument for threads
			fprintf(stderr, "Usage: %s <input size> <number of threads> [<seed>] \n", argv[0]);
			exit(1);
	}

	int n = atoi(argv[1]); // input size, items to be sorted

	int numThreads = atoi(argv[2]); // num threads 

	if(numThreads <= 0){ // if threads less than or equal to 0

		numThreads = DEFAULT_THREADS; // setting default value of 1

	}

	if(numThreads > MAX_THREADS){ // user used too many threads

		fprintf(stderr, "Please limit the number of threads used to 10 or less"); 
		exit(1);

	}

	int makeThreads = n / numThreads; // input size divided by number of threads

	int distributeThreads = n % numThreads; // if n is not able to be split evenly when divided by given num of threads

	int i = 0; // had to add this and change all for loops. initial declaration only allowed in c99 or c11 mode

	struct args *inputs = malloc(numThreads * sizeof(struct args)); // allocating memory for the parameters to be passed to each thread

	int seed = 1;
	if (argc == 3)
		seed = atoi(argv[2]);
	

	int *A = (int *) malloc(sizeof(int) * (n+1)); // n+1 since we are using A[1]..A[n]

	for(i = 0; i < numThreads; i++){ // looping through and adding input size to each thread

		inputs[i].A = A;
	}

	// generate random input

	generate_random_array(A,n, seed);

	double start_time;
	double sorting_time;

	// sort the input (and time it)
	start_time = getMilliSeconds();

	pthread_t givenThreads[numThreads]; // setting the number of threads

	// serial_mergesort(A,1,n); don't need as switching to a threaded mergesort

	for(i = 0; i < numThreads; i++){ // creating the threads for mergesort

		inputs[i].p = (makeThreads * i) + 1; // assinging values to p, beginning of array A

		inputs[i].r = inputs[i].p + makeThreads -1; // assingnig values to r, end of portion to be sorted in array A[p...r]

		if(i == numThreads -1){ // if we're near max threads

			inputs[i].r =inputs[i].r + distributeThreads; // use remainder found above to ensure array end is included
		}

		pthread_create(&givenThreads[i], NULL, parallel_mergesort, &inputs[i]); // creating threads
	}

	for(i = 0; i < numThreads; i++){ // joining the threads

		pthread_join(givenThreads[i], NULL); // waiting for threads to end, NULL value is where status usually is, not needed here
	}

	struct args *mergeThreads = &inputs[0]; // struct for our threads to merge

	for(i = 1; i < numThreads;i++){

		merge(A, mergeThreads->p, inputs[i].p -1, inputs[i].r); // merging threads back together
	}

	sorting_time = getMilliSeconds() - start_time;

	// print results if correctly sorted otherwise cry foul and exit
	if (check_if_sorted(A,n)) {
		printf("Sorting %d elements took %4.2lf seconds.\n", n,  sorting_time/1000.0);
	} else {
		printf("%s: sorting failed!!!!\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	free(inputs);
	free(A); // free the A array

	exit(EXIT_SUCCESS);
}
