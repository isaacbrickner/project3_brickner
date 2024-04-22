#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include "eventbuf.h"

struct eventbuf *eb;
sem_t *spaces_sem, *items_sem, *mutex_sem;
int events_per_producer; 

void *producer_thread(void *arg);
void *consumer_thread(void *arg);

sem_t *sem_open_temp(const char *name, int value)
{
    sem_t *sem;
    if ((sem = sem_open(name, O_CREAT, 0600, value)) == SEM_FAILED)
        return SEM_FAILED;
    if (sem_unlink(name) == -1) {
        sem_close(sem);
        return SEM_FAILED;
    }
    return sem;
}

int main(int argc, char *argv[]) {
    if (argc != 5 ) {
        fprintf(stderr, "CLI arguments are as follows: %s num_producers num_consumers events_per_producer max_outstanding\n", argv[0]);
        return 0;
    }
    int num_producers = atoi(argv[1]);
    int num_consumers = atoi(argv[2]);
    events_per_producer = atoi(argv[3]);
    int max_outstanding = atoi(argv[4]);
  
    eb = eventbuf_create();

    spaces_sem = sem_open_temp("spaces_sem", max_outstanding);
  
    items_sem = sem_open_temp("items_sem", 0);

    mutex_sem = sem_open_temp("mutex_sem", 1);
 

    pthread_t producers[num_producers];
    int producer_ids[num_producers];
    for (int i = 0; i < num_producers; i++) {
        producer_ids[i] = i;
        (pthread_create(&producers[i], NULL, producer_thread, &producer_ids[i]));
    }


    pthread_t consumers[num_consumers];
    int consumer_ids[num_consumers];
    for (int i = 0; i < num_consumers; i++) {
        consumer_ids[i] = i;
        pthread_create(&consumers[i], NULL, consumer_thread, &consumer_ids[i]);
    }

    for (int i = 0; i < num_producers; i++) {
        pthread_join(producers[i], NULL);
    }

 
    for (int i = 0; i < num_consumers; i++) {
        sem_post(spaces_sem);
    }

    for (int i = 0; i < num_consumers; i++) {
        pthread_join(consumers[i], NULL);
    }

    sem_close(spaces_sem);
    sem_close(items_sem);
    sem_close(mutex_sem);

    eventbuf_free(eb);

    return 0;
}

void *producer_thread(void *arg) {
    int producer_id = *((int *) arg);
    for (int i = 0; i < events_per_producer; i++) {
        sem_wait(spaces_sem); 
        sem_wait(mutex_sem); // lock the mutex
        eventbuf_add(eb, producer_id * 100 + i); // add event to the buffer
        printf("P%d: adding event %d\n", producer_id, producer_id * 100 + i); 
        sem_post(mutex_sem); // unlock the mutex
        sem_post(items_sem); // signal that an event is available
        sleep(1); // sleep
    }
    printf("P%d: exiting\n", producer_id); 
    pthread_exit(NULL);
}

void *consumer_thread(void *arg) {
    int consumer_id = *((int *) arg);
    while (1) {
        sem_wait(items_sem); // wait if there are no items to consume
        sem_wait(mutex_sem); // lock the mutex
        if (eventbuf_empty(eb)) {
            sem_post(mutex_sem); // unlock the mutex
            break; // rxit the loop if buffer is empty
        }
        int event = eventbuf_get(eb); // get event from the buffer
        printf("C%d: got event %d\n", consumer_id, event); 
        sem_post(mutex_sem); // unlock the mutex
        sem_post(spaces_sem); // signal that a space is available in the buffer
        sleep(1); 
    }
    printf("C%d: exiting\n", consumer_id); 
    pthread_exit(NULL);
}
