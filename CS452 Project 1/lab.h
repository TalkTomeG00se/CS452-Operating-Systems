#ifndef __LAB_H
#define __LAB_H

#ifdef __cplusplus
extern "C" {
#endif

#define TRUE 1
#define FALSE 0
#define MAX_THREADS 10 // limiting number of maximum threads
#define DEFAULT_THREADS 1 // default number of threads

// function prototypes
void serial_mergesort(int A[], int p, int r);
void merge(int A[], int p, int q, int r);
void insertion_sort(int A[], int p, int r);
void* parallel_mergesort(void *input); // adding function prototype for new parallel mergesort function

struct args { // struck for args that will be passed to the parallel mergesort
    int p,r,*A; 
};

const int INSERTION_SORT_THRESHOLD = 100; //based on trial and error

#ifdef __cplusplus
}
#endif
#endif //__LAB_H
