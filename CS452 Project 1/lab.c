/**
 * Name: Colten Davis
 * Email: coltendavis@u.boisestate.edu
 * Github Username: TalkTomeG00se
 */
#include "lab.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <pthread.h> // adding pthread library


/*
 * insertion_sort(int A[], int p, int r):
 *
 * description: Sort the section of the array A[p..r].
 */
void insertion_sort(int A[], int p, int r)
{
	int j;

	for (j=p+1; j<=r; j++) {
		int key = A[j];
		int i = j-1;
		while ((i > p-1) && (A[i] > key)) {
			A[i+1] = A[i];
			i--;
		}
		A[i+1] = key;
	}
}


/*
 * serial_mergesort(int A[], int p, int r):
 *
 * description: Sort the section of the array A[p..r].
 */
void serial_mergesort(int A[], int p, int r)
{
	if (r-p+1 <= INSERTION_SORT_THRESHOLD)  {
			insertion_sort(A,p,r);
	} else {
		int q =  r + (p - r)/2;
		serial_mergesort(A, p, q);
		serial_mergesort(A, q+1, r);
		merge(A, p, q, r);
	}
}

void* parallel_mergesort(void *userInput) {

	struct args *param = (struct args*) userInput; // creating a struct for user inputs

	serial_mergesort(param->A, param->p, param->r); // calling mergesort on the inputs

	pthread_exit(NULL); // Note: Didn't have this at first, got warning that a resource wasn't closed.

	// source: https://stackoverflow.com/questions/36382444/why-do-we-use-pthread-exit-when-we-can-use-return

	
}

/*
 * merge(int A[], int p, int q, int r):
 *
 * description: Merge two sorted sequences A[p..q] and A[q+1..r]
 *              and place merged output back in array A. Uses extra
 *              space proportional to A[p..r].
 */
void merge(int A[], int p, int q, int r)
{
	int *B = (int *) malloc(sizeof(int) * (r-p+1));

	int i = p;
	int j = q+1;
	int k = 0;
	int l;

	// as long as both lists have unexamined elements
	// this loop keeps executing.
	while ((i <= q) && (j <= r)) {
		if (A[i] < A[j]) {
			B[k] = A[i];
			i++;
		} else {
			B[k] = A[j];
			j++;
		}
		k++;
	}

	// now only at most one list has unprocessed elements.

	if (i <= q) {
		// copy remaining elements from the first list
		for (l=i; l<=q; l++) {
			B[k] = A[l];
			k++;
		}
	} else {
		// copy remaining elements from the second list
		for (l=j; l<=r; l++) {
			B[k] = A[l];
			k++;
		}
	}

	// copy merged output from array B back to array A
	k=0;
	for (l=p; l<=r; l++) {
		A[l] = B[k];
		k++;
	}

	free(B);
}
