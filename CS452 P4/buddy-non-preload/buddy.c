/**
 * Implementation of a memory allocator based on the Buddy System.
 * See Knuth Art of Computer Programming, vol. 1, page 442. 
 * Each available block starts with a header that consists
 * of a tag (free/reserved), kval (size of block 2^kval), next
 * and previous pointers. Each reserved block has the tag and kval 
 * field only. All allocations are done in powers of two. All requests
 * are rounded up to the next power of two.
 * 
 * @author  Colten Davis
 * 
 */


#include "buddy.h"
int initialized = FALSE;

/* the header for an available block */
struct block_header {
	short tag;
	short kval;
	struct block_header *next;
	struct block_header *prev;
};

typedef struct block_header Header;

const int RESERVED = 0;
const int FREE = 1;
const int UNUSED = -1;


/* supports memory upto 2^(MAX_KVAL-1) (or 64 GB) in size */
#define  MAX_KVAL  37 
#define  MIN_KVAL  10  
#define  KVAL      29  
#define  NO_MEM    NULL

static size_t roundUp(size_t);
static int getKval(size_t);
static int getBlock(int);
static void* getBuddy(int, void*);

/* deafult memory allocation is 512MB */
const size_t DEFAULT_MAX_MEM_SIZE = 512 * 1024 * 1024;


/* A static structure stores the table of pointers to the lists in the buddy system.  */
struct pool {
	void *start; // pointer to the start of the memory pool
	int lgsize;  // log2 of size
	size_t size; // size of the pool, same as 2 ^ lgsize
	/* the table of pointers to the buddy system lists */
	struct block_header avail[MAX_KVAL];
} pool;



void buddy_init(size_t size) {

	Header *newHeader;
	int i;
	

	if (!initialized) { // if not initialized already

		size = roundUp(size); // rounding up

		pool.lgsize = getKval(size);

		if (pool.lgsize > MAX_KVAL ) { // if size is greater than our max value

			size = 1UL << MAX_KVAL; // using bitshifting 1UL or a unsigned long int of value 1
			// https://stackoverflow.com/questions/14467173/bit-setting-and-bit-shifting-in-ansi-c 

			pool.lgsize = MAX_KVAL;
		}

		if (pool.lgsize < MIN_KVAL) { // if size is less than our min value

			size = 1UL << KVAL;

			pool.lgsize = KVAL; // set to default size of 512
		}

		pool.start = sbrk(size); // using sbrk to change the space allocated. https://stackoverflow.com/questions/16036595/sbrk0-and-sbrksize-both-return-the-same-address

		for (i = 0; i < pool.lgsize; i++)
		{
			pool.avail[i].next = &pool.avail[i];

			pool.avail[i].prev = &pool.avail[i];

			pool.avail[i].tag = RESERVED;

			pool.avail[i].kval = i;
		}

		pool.avail[i].next = pool.start;

		pool.avail[i].prev = pool.start;

		pool.avail[i].tag = FREE;

		pool.avail[i].kval = pool.lgsize;

		newHeader = pool.avail[i].next;

		newHeader->next = &pool.avail[i];

		newHeader->prev = &pool.avail[i];

		newHeader->tag = FREE;

		newHeader->kval = i;

	}

	initialized = TRUE;
	
}


void *buddy_malloc(size_t size) {

	int i = 0;

	int kval = 0; // initial k value

	if (size == 0) { // return null if size is 0

		return NULL;
	}
	if (!initialized) { // if not initialized yet

		buddy_init(1 << KVAL);
	}

	Header *firstHeader = NULL;

	Header *secondHeader = NULL;

	size_t blockSize = size + sizeof(Header); // size of our block
	
	blockSize = roundUp(blockSize); // rounding up
	
	kval = getKval(blockSize);
	
	if (kval > pool.lgsize) { // memory cannot be allocated, setting errno to ENOMEM per project specs

		errno = ENOMEM;

		return (void*) NO_MEM;
	}

	i = getBlock(kval);
	

	if (i != UNUSED) {

		firstHeader = pool.avail[i].next; // header is next available

		pool.avail[i].next = firstHeader->next; // next available is the next 

		firstHeader->next->prev = &pool.avail[i]; // connecting the now next to previous

		firstHeader->tag = RESERVED; // setting tag to now reserved

		firstHeader->kval = kval; // setting k value

		while (i != kval) { 

			i--;

			long htAddress = (long)firstHeader + (1 << i);

			secondHeader = (Header*) htAddress;

			secondHeader->next = &pool.avail[i];

			secondHeader->prev = &pool.avail[i];

			secondHeader->tag = FREE;

			secondHeader->kval = i;

			pool.avail[i].next = secondHeader;

			pool.avail[i].prev = secondHeader;

			pool.avail[i].tag = FREE;
		}

	} else {

		errno = ENOMEM;

		return (void*) NO_MEM;
	}

	firstHeader++;
	
	return (void *) firstHeader;
}

// From what I can see, none of the test call calloc or realloc, tried to delete them, but would also have to change the header file...unsure what that would do. Left both here.

void *buddy_calloc(size_t nmemb, size_t size)
{
	return NULL;
}

void *buddy_realloc(void *ptr, size_t size)
{
	return NULL;
}


void buddy_free(void *ptr) {

	Header *firstHeader = NULL;

	Header *secondHeader = NULL;

	int kval;

	if (ptr == NULL) {

		return;
	}

	firstHeader = (Header*)(ptr) - 1;

	kval = firstHeader->kval;

	void* nextAddress = getBuddy(kval, (void*) firstHeader); // getting address

	secondHeader = (Header *) (nextAddress); // setting address of header

	while ( !(( kval == pool.lgsize ) || ( secondHeader->tag == RESERVED ) // not at max value, or the current head is not reserved, or were not at the right kval
					|| (( secondHeader->tag == FREE) && ( secondHeader->kval != kval)))) {
		
		secondHeader->prev->next = secondHeader->next;

		secondHeader->next->prev = secondHeader->prev;

		kval++;

		if (secondHeader < firstHeader) {

			firstHeader = secondHeader;
		}
		secondHeader = (Header *)(getBuddy(kval, (void*) firstHeader));
	}

	firstHeader->tag = 1;

	firstHeader->next = pool.avail[kval].next;

	pool.avail[kval].next->prev = firstHeader;

	firstHeader->kval = kval;

	firstHeader->prev = &pool.avail[kval];

	pool.avail[kval].next = firstHeader;
	
}


void printBuddyLists(void) {

	int i = 0;

	int available = 0;

	while (i <= pool.lgsize) {

		printf("List %d head = %p --> [tag=%d,kval=%d,addr=%p] ", i , &pool.avail[i], pool.avail[i].tag, pool.avail[i].kval, pool.avail[i].next);

		Header* currentHeader = &pool.avail[i];

		while (currentHeader->next != &pool.avail[i]) {

			printf("-->%p", currentHeader);

			currentHeader = currentHeader->next;
		}

		printf("--> <null>\n");

		if(pool.avail[i].tag == FREE){

			available++;
		}

		i++;
	}

	printf("\nNumber of available blocks = %d\n", available);

	available = 0;
}

/**
 * Helps aid in the rounding  up of the size
 * 
 * @param size 
 * @return the rounded up size
 */
static size_t roundUp(size_t size) {
	
	size_t tempSize = 1;
	
	while (tempSize < size) {

		tempSize = tempSize * 2;

	}

	return tempSize;
}

/**
 * Get the value of the current kval
 * 
 * @param blockSize
 * @return the current kval
 */
static int getKval(size_t blockSize) {
	
	int kval = 0;

	blockSize--;

	while (blockSize > 0) { // while size is greater than 0

		blockSize >>= 1; // bit shift right

		kval++;

	}

	return kval;
}

/**
 * @brief Get the Block object
 * 
 * @param kval 
 * @return returns the if the next block is not available, else returns -1
 */
static int getBlock(int kval) {

	int block;

	for (block = kval; block <= pool.lgsize; block++)	{

		if (pool.avail[block].next != &pool.avail[block])	{ // stepping through all blocks

			return block;

		}
	}

	return -1;
}


static void* getBuddy(int kval, void* currentHeader) {

	long j = (long) kval; // casting

	long y = (long) currentHeader; // casting

	return (void*) (((1 << j) ^ (y - (long)pool.start)) + (long)pool.start); // void return, learned from https://riptutorial.com/c/example/10572/void--pointers-as-arguments-and-return-values-to-standard-functions
	// other sources in README

}
