/* Hello this is Austin working on concurrency for CS444 
also hello this is Isaac working on concurrency for CS444 */

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include "mt19937ar.c"
#include <time.h>
#include <stdlib.h>

pthread_mutex_t lock;

struct Item
{
   int print;
   int wait;
};

struct Item product[32];
unsigned int ecx;

int processorcheck()
{
   if (ecx & 0x40000000)
      return 1;
   else
      return 0;
}

int checkproductempty(int x)
{
   int a = 0;
   if (product[x].wait == -1 || product[x].print == -1)
   {
      a = 1;
   }
   return a;
}

int checkmutex()
{
   return pthread_mutex_trylock(&lock);
}

void consumerthread()
{
   int i, t;
   while (1)
   {
      if (i >= 32)
      {
		  i = 0;
      }
	  t = 0;
      if (!checkmutex())
      {
		  if (!checkproductempty(i))
		  {
			  printf("Consumer random val from loop %d is %d\n", i, product[i].print);
			  printf("Consumer sleeping at index %d for %d\n", i, product[i].wait);
			  t = product[i].wait;
			  product[i].print = -1;
			  product[i].wait = -1;
		  }
		  pthread_mutex_unlock(&lock);
		  sleep(t); //you are feeling sleepy now
	  }
	  i++;
   }
}

int main(int argc, char *argv[])
{
	if (argc == 1)
	{
		printf("No argument given, exiting now.\n");
		return 0;
	}
	printf("Argv is : %d\n", atoi(argv[1]));
    srand(time(NULL));
    int i, count;
    int prodwait;

    for (i = 0; i < 32; ++i){//just initializating all the items
        product[i].print=-1;
        product[i].wait=-1;
    }

    pthread_t consumer_thread;
    if(pthread_create(&consumer_thread, NULL, (void *)consumerthread, (void *)NULL)) {

        fprintf(stderr, "Error creating thread\n");
        return 1;

    }
    while (1){

        if (i >= 32 ) {
            i = 0;
        }
		prodwait = 0;
        
        if (!checkmutex()){
            if (checkproductempty(i)) {
                product[i].print = (rand() % (11));//between 1-10
                product[i].wait = (rand() % (9 + 1 -2)) + 2;//waiting for somewhere between 2-9 second 
		printf("Producer Inserting Value %d and Wait Time %d in loop: %d\n", product[i].print, product[i].wait, i);
                prodwait = (rand() % (7 + 1 - 3)) + 3;//waiting for somewhere between 3-7 seconds
		printf("Producer is now sleeping for %d seconds\n", prodwait);
            }
            pthread_mutex_unlock(&lock);
            sleep(prodwait);//You are feeling sleepy now.
	    i++;
        } 
        	printf("i is: %d\n", i);
		if (i == atoi(argv[1]))
		{
			return 0;
		}
    }
    if(pthread_join(consumer_thread, NULL)) {
        fprintf(stderr, "Error joining thread\n");
        return 2;
    }
    return 0;

}
