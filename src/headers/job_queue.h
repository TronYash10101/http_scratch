#ifndef JOB_QUEUE
#define JOB_QUEUE

#include "message.h"
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <unistd.h>

#define NUM_WORKERS 3
#define NUM_JOBS 10

typedef struct job_t {
    struct alive_struct *socket_descriptor_properties;
    char file_path_buffer[RESPONSE_BUFFER_SIZE];
    Istatus_line *status_line;
    struct job_t *next_job;
} job_t;

#define POISON_PILL ((job_t *)-1)

typedef struct {
    job_t *head;
    job_t *tail;
    pthread_mutex_t qlock;
    pthread_mutexattr_t qattr;
    pthread_cond_t qcond;
} job_queue_t;

void job_queue_init(job_queue_t *queue);
void job_queue_push(job_queue_t *queue, job_t *job);
job_t *job_queue_pop(job_queue_t *queue);

#endif
