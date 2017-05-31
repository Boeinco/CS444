#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include "mt19937ar.c"

#define SEATS 10
#define CUST 20

sem_t bsem;
sem_t csem;
pthread_mutex_t open;
int extrainit = SEATS;

void *customer(void* arg) { 
     int id = *((int *)arg);
     while(1)
     {
     	pthread_mutex_lock(&open);

     	if(extrainit > 0)
     	{
     		sleep(genrand_int32() % 5);
     		printf("customer %d entering barbs\n", id);
     		extrainit--;
     		sem_post(&csem);
     		pthread_mutex_unlock(&open);
     		sem_wait(&bsem);
     		printf("cutting customer %d's hair\n", id);
     		sleep(10);
     		printf("customer %d done\n", id);
     		printf("customer %d may or may not be back\n", id);
     		sleep(10);
     	}
     	else
     	{
     		pthread_mutex_unlock(&open);
     		printf("customer %d leaving beibs sucks\n", id);
     		sleep(10);
     	}
     }
} 

void *barber(void* arg) 
{
	while(1)
	{
		printf("the beibs is waiting\n");
		sem_wait(&csem);
		pthread_mutex_lock(&open);
		extrainit++;
		sem_post(&bsem);
		pthread_mutex_unlock(&open);
		printf("beibs is cutting\n");
		sleep(1);
	}
}

int main(int argc, char *argv[]) 
{
	pthread_t bthread;
	pthread_t cthread[CUST];

	int i;
	init_genrand(time(NULL));
	int num[CUST];

	pthread_mutex_init(&open, NULL);
	sem_init(&bsem, 0, 0);
	sem_init(&csem, 0, 0);

	pthread_create(&bthread, NULL, barber, NULL);

	for(i = 0; i < CUST; i++)
	{
		num[i] = i;
		pthread_create(&cthread[i], NULL, customer, &num[i]);
	}

	for(i = 0; i < CUST; i++)
	{
		pthread_join(cthread[i], NULL);
	}

	return 0;
}