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
	long int booga0_Opens;
    long int booga1_Opens;
    long int booga2_Opens;
    long int booga3_Opens;
    long int numBytesRead;
    long int numBytesWrite;
    long int numBoogas;
    long int numNekas;
    long int numGoogoos;
    long int numWoogas;
	struct semaphore sem;
};
typedef struct booga_stats booga_stats;

/*extern example_stats Example_Device_Stats;*/

#endif /* __EXAMPLE_H */