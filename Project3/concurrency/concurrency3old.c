#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "mt19937ar.c"

struct entry
{
	int num;
	struct entry* next;
};

struct buffer
{
	struct entry* head;
	struct entry* current;
	int counter;
};

struct buffer buff;
int num;
int deleters, searchers, inserters;

void checker(int signal)
{
	kill(0,signal);
	exit(0);
}

int twisterrand(int high, int low)
{
	int i = 0;
	i = (int) genrand_int32();
	i = abs(i);
	i %= (high-low);
	if (i < low)
	{
		i = low;
	}
	return i;
}

void searcher()
{
	int i;
	struct entry *start = buff.head;

	while (deleters != 0)
	{
		sleep(1);
	}
	printf("Searcher searching!\n");
	searchers++;
	if(num < buff.counter)
	{
		printf("Node not found.\n");
		return;
	}

	for (i = 0; i < num; i++)
	{
		start = start->next;
	}
	printf("Num is %d\n", start->num);
	searchers--;
}

void inserter()
{
	struct entry *item;
	printf("Item is being inserted now\n");

	while(deleters != 0)
	{
		sleep(1);
	}
	while(inserters != 0)
	{
		sleep(1);
	}
	inserters = 1;
	item.num = twisterrand(100,1);
	while(buff.current->next != NULL)
	{
		buff.current = buff.current->next;
	}
	buff.current->next = &item;
	buff.current = buff.current->next;
	buff.current->next = NULL;
	buff.counter++;
	printf("Item added!\n");
	inserters = 0;
}

void deleter()
{
	int i;
	struct entry *del = buff.head;
	struct entry *temp;

	while(deleters != 0)
	{
		sleep(1);
	}
	deleters = 1;
	while (inserters != 0)
	{
		sleep(1);
	}
	while (searchers != 0)
	{
		sleep(1);
	}
	printf("deleters is deleting\n");

	if(num == 1)
	{
		printf("Only head can be deleted so not deleting it\n");
		return;
	}
	else if(num < buff.counter)
    {
        printf("No node\n");
    }
    for(i = 0; i < num; i++)
    {
        if(i == (num - 2))
        {
            temp = del;
        }
        del = del->next;          
    }
 
    temp->next = del->next;
    free(del);
    printf("Done\n");
    deleters = 0;
}

int main()
{
	struct sigaction signal;
	struct entry initial;
	pthread_t t1, t2, t3, t4;
	void *d = deleter;
	void *i = inserter;
	void *s = searcher;

	sigemptyset(&signal.sa_mask);
	signal.sa_flags = 0;
	sig.sa_handler = sig_catch;
	sigaction(SIGINT, &signal, NULL);

	initial.next = NULL;
	initial.num = 1;
	buff.head = &initial;

	deleters = 0;
	searchers = 0;
	inserters = 0;
	number = 2;

	pthread_create(&t1, NULL, i, NULL);
    pthread_create(&t2, NULL, i, NULL);
    pthread_create(&t3, NULL, s, NULL);
    pthread_create(&t4, NULL, d, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);
    pthread_join(t4, NULL);

	while(true){
	}
}