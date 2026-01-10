#include "headers/job_queue.h"

void job_queue_init(job_queue_t *queue) {

    queue->head = NULL;
    queue->tail = NULL;

    pthread_mutexattr_init(&queue->qattr);
    pthread_mutexattr_settype(&queue->qattr, PTHREAD_MUTEX_ERRORCHECK);
    pthread_mutex_init(&queue->qlock, &queue->qattr);
    pthread_cond_init(&queue->qcond, NULL);
}

void job_queue_push(job_queue_t *queue, job_t *job) {
    pthread_mutex_lock(&queue->qlock);

    job->next_job = NULL;

    if (queue->tail == NULL) {
        queue->head = job;
        queue->tail = job;
    } else {
        queue->tail->next_job = job;
        queue->tail = job;
    }

    pthread_cond_signal(&queue->qcond);
    pthread_mutex_unlock(&queue->qlock);
}

job_t *job_queue_pop(job_queue_t *queue) {

    pthread_mutex_lock(&queue->qlock);

    while (queue->head == NULL) {
        pthread_cond_wait(&queue->qcond, &queue->qlock);
    }
    job_t *curr_job = queue->head;
    queue->head = queue->head->next_job;

    if (queue->head == NULL) {
        queue->tail = NULL;
    }

    pthread_mutex_unlock(&queue->qlock);

    return curr_job;
}
