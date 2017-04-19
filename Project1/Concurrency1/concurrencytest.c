#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "mt19937ar.c"

#include <time.h>

typedef int bool;
#define true 1
#define false 0

// *** Definition of structs, threadBuf is our global buffer to be shared, initialize mutex to protect our global buffer ***
struct item
{
	int randomNum; // Just a random number
	int waitTime; // Random waiting period between 2 and 9 
}; 

struct buffer
{
	struct item* items[32];
}; 

static struct buffer threadBuf; // Both threads will have access to this data

// Initialize the mutex we will be using to lock access to our global data in order to synchronize threads
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;	

// Condition variables let one thread inform other threads about changes in the state of a shared variable and allows other threads to BLOCK in order to wait for this notification
static pthread_cond_t condition = PTHREAD_COND_INITIALIZER;

// Pthread IDs for identification
static pthread_t consumer; // to identify the consumer thread
static pthread_t producer; // " " "

// Initialize buffer to entirely NULL, for adding and removing purposes
void initializeBuffer()
{
	int i = 0;

	for (i; i < 32; i++)
		threadBuf.items[i] = NULL;
}

// *** CHECK STATUS OF BUFFER ***
bool isBufFull()
{
	int i = 0;

	for (i; i < 32; i++)
	{
		if (threadBuf.items[i] == NULL)
			return false; 
	}

	return true;	
}

bool isBufEmpty()
{
	int i = 0;

	for (i; i < 32; i++)
	{
		if (threadBuf.items[i] != NULL)
			return false;
	}

	return true;
}

// for use with producer stuff, returns the first empty element of array
int returnEmptyIndex()
{
	int i = 0;

	for (i; i < 32; i++)
	{
		if (threadBuf.items[i] == NULL)
			return i;
	}

	return -1;	
}

// for use with consumer, returns first full element of array
int returnFullIndex()
{
	int i = 0;

	for (i; i < 32; i++)
	{
		if (threadBuf.items[i] != NULL)
			return i;
	}

	return -1;
}

// *** ASM STUFF: REGISTERS, RANDOM # GENERATION, ETC ***

// Set up registers for random number generation
unsigned int eax, ebx, ecx, edx;

// Source: http://codereview.stackexchange.com/questions/147656/checking-if-cpu-supports-rdrand
// To check if the processor supports rdrand, see if the 30th bit
// in the ecx register is set.
// If it is, return true. If not, return false.
// Call this function ONLY after preparing the registers
bool checkProcessor()
{
	if (ecx & 1<<30)
		return true; // rdrand can be supported
	else
		return false;
} 

// ** Throw these functions away when the ASM is understood **
int itemFirstVal() // between 0 and 10
{
	int r;
	r = (rand() % (10 + 1));
	return r;
}

int itemSecondVal() // between 2 and 9
{
	int r;
	r = (rand() % (9 + 1 - 2)) + 2;
	return r;
}

int producerVal() // between 3 and 7
{
	int r;
	r = (rand() % (7 + 1 - 3)) + 3;
	return r;
}

/*** FUNCTIONS FOR PRODUCER THREAD ***
a) Wait a random time between 3-7 seconds
b) Generate an item: 1) a random number 2) a random waiting period between 2-9
It's a function pointer in accordance with POSIX standards
If buffer is full, block until consumer removes an item
*/

struct item* createRandomItem()
{
	struct item* i1 = malloc(sizeof(struct item));
	i1->randomNum = itemFirstVal();
	i1->waitTime = itemSecondVal();
	return i1;
}

void addRandomItem(struct item* i1)
{
	int index = returnEmptyIndex();		
	threadBuf.items[index] = i1;
}

void* produceAnItem()
{
 	// Lock the buffer (mutexes)
	pthread_mutex_lock(&mutex);
	 
 	// Use pthread conditions, pthread_cond_wait to wait until 
 	// there's space in the buffer
 	while (isBufFull() == true)
	{
		printf("PRODUCER THREAD: Blocking until item is consumed\n");
		pthread_cond_wait(&condition, &mutex); // let this thread sleep until consumer takes an item
	}
		
 	// Buffer is not full, generate random number between 3-7 secs then wait that amount of time
	int randomWaitTime = producerVal();
	printf("PRODUCER THREAD: Sleeping %d seconds before producing\n", randomWaitTime);
	sleep(randomWaitTime);

	printf("PRODUCER THREAD: I am producing a random item\n");

 	// create random item
	struct item* i1 = createRandomItem(); 

	// add item to buffer
	addRandomItem(i1);

 	// Signal to the consumer there's an item
 	pthread_cond_signal(&condition);
 	
	// Unlock the buffer
 	pthread_mutex_unlock(&mutex);
	
}

/*** FUNCTIONS FOR CONSUMER THREAD ***
*/

void removeItem(int index)
{
	// free memory
	free(threadBuf.items[index]);	

	// turn the index NULL
	threadBuf.items[index] = NULL;
}

struct item* getItem(int* index)
{
	// use returnFullindex
	*(index) = returnFullIndex();

	// return the item
	int indx = *(index);
	return threadBuf.items[indx];
}

void* consumeAnItem()
{
 	// Lock the buffer (mutexes)
 	pthread_mutex_lock(&mutex);
 
 	// Use pthread conditions, pthread_cond_wait to wait until
 	// the buffer is not empty
 	while (isBufEmpty() == true)
	{
		printf("CONSUMER THREAD: Blocking until item is produced.\n");
		pthread_cond_wait(&condition, &mutex); // let this thread sleep until consumer takes an item
	}
		
	// Get a full index for consumption
	int indexToConsume;
	struct item* itemToConsume = getItem(&indexToConsume);

 	// print out first value
	printf("CONSUMER THREAD: Random value in first field is %d\n", itemToConsume->randomNum);  	

 	// wait the second value
	sleep(itemToConsume->waitTime);	 	

 	// print second value
 	printf("CONSUMER THREAD: I just waited a random time of %d\n", itemToConsume->waitTime);

	sleep(2); // for clear printing

 	// write a function that erases that index, adjusts the array/size element
 	removeItem(indexToConsume);	

	// Signal to the producer to make a new item
 	pthread_cond_signal(&condition);
 	 
	// Unlock the buffer
	pthread_mutex_unlock(&mutex);

}

int loopCount(char** argv)
{
	if (argv[1] == NULL)
	{
		printf("No loop counter provided. Exiting...\n");
		exit(1);
	}

	else
	{
		int loopCounter = atoi(argv[1]);
		return loopCounter;
	}

}

int main(int argc, char** argv)
{
	srand(time(NULL));

	initializeBuffer(threadBuf);

	// pthread_create: (ref to ID of thread, pointer to struct with option flags, pointer to function that will be start point of execution for thread, argument passed to the function of execution)
	// For our first thread execution, produce an item.
	
	int loopCounter = loopCount(argv);

	int i = 0;

	printf("# of times to run: %d\n", loopCounter);

	for (i; i < loopCounter; i++)
	{
		pthread_create(&producer, NULL, produceAnItem, NULL);
		pthread_create(&consumer, NULL, consumeAnItem, NULL);
	
	// pthread_join(thread to wait on, return value of terminated thread)
	// Wait on the consumer thread to terminate in some fashion 
		pthread_join(consumer, NULL);
	}

	return 0;
}
