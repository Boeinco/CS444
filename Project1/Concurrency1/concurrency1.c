/* Hello this is Austin working on concurrency for CS444 */

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include "mt19937ar.c"
#include <time.h>
#include <iostream>
using namespace std;

pthread_mutex_t lock;

struct Item
{
   int print;
   int wait;
};

struct Item product[32];

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
			  printf("Consumer from index %d is %d\n", i, product[i].print);
			  printf("Consumer at index %d for %d\n", i, product[i].wait);
			  t = product[i].wait;
			  product[i].print = -1;
			  product[i].wait = -1;
		  }
		  pthread_mutex_unlock(&lock);
		  sleep(t);
	  }
	  i++;
   }
}

int main()
{
    int i;
    int producer_wait;
    for (i = 0; i < 32; ++i){
        product[i].print=-1;
        product[i].wait=-1;
    }

    //Prints all the data in the array.
    //for (i = 0; i < 32; ++i){
    //    printf("Print: %d\n", product[i].print);
    //    printf("Wait: %d\n", product[i].wait);
    //}
    pthread_t consumer_thread;
    if(pthread_create(&consumer_thread, NULL, (void *)consumerthread, (void *)NULL)) {

        fprintf(stderr, "Error creating thread\n");
        return 1;

    }
    while (1){

        if (i >= 32 ) {
            i = 0;
        }
        
        producer_wait = 0;
        
        if (!checkmutex()){
            //printf("Mutex Aquired by Producer\n");
            if (checkproductempty(i)) {
                product[i].print = rdrand_func(3);
                product[i].wait = rdrand_func(1);
                printf("Producer Inserting Value %d and Wait Time %d at Index: %d\n", product[i].print, product[i].wait, i);
                producer_wait = rdrand_func(2);
                printf("Producer is now sleeping for %d seconds\n", producer_wait);
            }
            pthread_mutex_unlock(&lock);
            //printf("Mutex released by Producer\n");
            sleep(producer_wait);
        } 
        i++;
    }
    if(pthread_join(consumer_thread, NULL)) {
        fprintf(stderr, "Error joining thread\n");
        return 2;
    }
    return 0;

}
