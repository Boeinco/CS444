#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <semaphore.h>
#include <pthread.h>

//there will be print statements to track the state of each philosopher;
//for reference: Thinking = 0, Hungry = 1, Eating = 2
//					   ^this is the important one

//the mutex will handle the forks at the table
sem_t mutex;
sem_t S[5];

//philosopher is an infinitely looping function
void * philospher(void *num);
void take_fork(int);
void put_fork(int);
void test(int);

//this global array makes us aware of every philosopher's state, important for telling who is eating
int state[5];
//this helps us keep track of the threads
int phil_num[5]={0,1,2,3,4};
char* phil_names[5]={"Confucius","Descartes","Plato","Sartre","Aristotle"};
  
int main(){
    //set the rand seed
    srand (time(NULL));
    //create 5 threads, one for each of the philosophers 0-4
    int i;
    pthread_t thread_id[5];
    sem_init(&mutex,0,1);
    for(i=0;i<5;i++)
        sem_init(&S[i],0,0);
    for(i=0;i<5;i++){
        pthread_create(&thread_id[i],NULL,philospher,&phil_num[i]);
        printf("%s is Thinking\n",phil_names[i]);
    }
    for(i=0;i<5;i++)
        pthread_join(thread_id[i],NULL);
}
  
//philosopher function loops infinitely, moving between the different states
void * philospher(void *num){
    while(1){
        //i tracks the number of the thread, determining forks to use and allowing for print statements
        int *i = num;
        //philosophers begin the cycle by thinking for 1-20 seconds
        sleep(rand() % 19 + 1);
	//philosopher takes forks after thinking to begin eating
        take_fork(*i);
        sleep(0);
	//after getting forks and eating they return the forks
        put_fork(*i);
    }
}

//this function is called after thinking, the philosopher will attempt to eat
void take_fork(int num){
    //the mutex locks forks so that they can only be used by one philosopher at a time
    sem_wait(&mutex);
    state[num] = 1;
    printf("%s is Hungry\n",phil_names[num]);
    //philosopher checks for forks
    test(num);
    sem_post(&mutex);
    sem_wait(&S[num]);
    //this short pause is just mostly to look good, it gave more instances of the philosophers eating twice at a time
    sleep(1);
}

void test(int num)
{
    //if the philosopher is hungry and neither adjacent philosopher is eating, they will take their forks and eat
    if (state[num] == 1 && state[(num+4)%5] != 2 && state[(num+1)%5] != 2){
        state[num] = 2;
        printf("%s takes fork %d and %d\n",phil_names[num],((num+4)%5)+1,num+1);
        printf("%s is Eating\n",phil_names[num]);
        //eating takes 2-9 seconds
		sleep(rand() % 7 + 2);
        sem_post(&S[num]);
    }
}

//after eating the philosopher returns their fork
void put_fork(int num){
    sem_wait(&mutex);
    state[num] = 0;
    printf("%s put forks %d and %d down\n",phil_names[num],((num+4)%5)+1,num+1);
    printf("%s is Thinking\n",phil_names[num]);
    test((num+4)%5);
    test((num+1)%5);
    sem_post(&mutex);
}
