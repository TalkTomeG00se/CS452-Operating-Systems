#ifndef __BOOGA_H
#define __BOOGA_H
/*
 * booga.h -- definitions for the char module
 *
 */
#ifndef BOOGA_MAJOR
#define BOOGA_MAJOR 0   /* dynamic major by default */
#endif

#ifndef BOOGA_NR_DEVS
#define BOOGA_NR_DEVS 4    /* example0 through example3 */
#endif
/*
 * Split minors in two parts
 */
#define TYPE(dev)   (MINOR(dev) >> 4)  /* high nibble */
#define NUM(dev)    (MINOR(dev) & 0xf) /* low  nibble */

/*
 * The different configurable parameters
 */
struct booga_stats { // updated to match proc entries needed per project specs
	long int numPhrases[4]; // holds our 4 strings
    long int numDriverOpens[4]; // holds our 4 minor numbers [0,1,2,3]
    long int thisBooga; // will be used to identify device booga0, booga1, booga2, or booga3
    long int bytesRead; // tracks bytes read
    long int bytesWritten; // tracks bytes written
    long int numOpens; // tracks number of times drive was opened
	struct semaphore sem;
};
typedef struct booga_stats booga_stats;

/*extern example_stats Example_Device_Stats;*/

#endif /* __EXAMPLE_H */
