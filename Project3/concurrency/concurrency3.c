#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include "mt19937ar.c"

struct linkedList list;
int searchers, inserters, deleters, num;

pthread_mutex_t smutex;
pthread_mutex_t imutex;
pthread_mutex_t dmutex;

struct linkedList {
    int val;
    struct linkedList *next;
};
struct linkedList *head;

int randnum(int low, int high) {
    int i = 0;
    i = (int)genrand_int32();
    i = abs(i);
    i %= (high - low);
    if (i < low) {
        i = low;
    }

    return i;
}


int getLinkedListSize() {
    struct linkedList *size;
    size = head;
    int i = 0;
    while (size != NULL) {
        size = size->next;
        i++;
    }
	return i;
}

void *searcher() {
    struct linkedList *searchlist;

    while (1) {
        if (!pthread_mutex_trylock(&smutex)) {
            searchlist = head;
            if (searchlist == NULL) {
                printf("List is empty\n");
                continue;
            } else {
	       	printf("searcher is printing: ");
                while (searchlist != NULL) {
                    printf("%d ", searchlist->val);
                    searchlist = searchlist->next;
                }
                printf("\n");
            }
            pthread_mutex_unlock(&smutex);
        }
        sleep(1);
    }
}
void *inserter() {
    int randomnum;
    struct linkedList *insertlist, **tail;

    while (1) {
        if (getLinkedListSize() < 25) {
            if (!pthread_mutex_trylock(&imutex)) {

                randomnum = randnum(1, 10);
                insertlist = (struct linkedList *)malloc(sizeof(struct linkedList));

                printf("Insert val: %d\n", randomnum);
                insertlist->val = randomnum;
                insertlist->next = NULL;
                tail = &head;

                if (head == NULL) {
                    head == insertlist;
                } else {
                    while (*tail != NULL) {
                        tail = &((*tail)->next);
                    }
                    *tail = insertlist;
                }
                pthread_mutex_unlock(&imutex);
                sleep(1);
            }
        }
    }
}

void *deleter() {
    int deleteNode;
    struct linkedList *deletelist, *previous;

    while (1) {

        if (getLinkedListSize() > 1) {

             if (!pthread_mutex_trylock(&imutex)) {
                if (!pthread_mutex_trylock(&smutex)) {

                    deletelist = head;

                    deleteNode = randnum(1, 10);

                    while (deletelist != NULL) {

                        if (deletelist->val == deleteNode) {
                            printf("Delete val: %d\n", deleteNode);

                            if (deletelist == head) {
                                head = deletelist->next;
                                free(deletelist);
                                break;
                            } else {
                                previous->next = deletelist->next;
                                free(deletelist);
                                break;
                            }
                        } else {
                            previous = deletelist;
                            deletelist = deletelist->next;
                        }
                    }

                    pthread_mutex_unlock(&smutex);
                }

                pthread_mutex_unlock(&imutex);
            }
            
            sleep(1);
        }
    }
}

int main(int argc, char const *argv[]) {

    pthread_t searchThread[3], insertThread[3], deleteThread[3];
	struct linkedList *theLinkedList;
    theLinkedList = (struct linkedList *)malloc(sizeof(struct linkedList));
    theLinkedList->val = randnum(1, 10);
    head = theLinkedList;
    head->next = NULL;

    int i;
    for (i = 0; i < 3; i++) {
        pthread_create(&searchThread[i], NULL, searcher, NULL);
        pthread_create(&insertThread[i], NULL, inserter, NULL);
        pthread_create(&deleteThread[i], NULL, deleter, NULL);
    }

    for (i = 0; i < 3; i++) {
        pthread_join(searchThread[i], NULL);
        pthread_join(insertThread[i], NULL);
        pthread_join(deleteThread[i], NULL);
    }

    return 0;
}
