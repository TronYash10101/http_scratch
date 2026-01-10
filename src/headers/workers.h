#ifndef WORKERS
#define WORKERS

#include "job_queue.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>

#include <time.h>
#include <unistd.h>

#define ERROR ((void *)0)
void *static_serve(void *arg);
#endif
