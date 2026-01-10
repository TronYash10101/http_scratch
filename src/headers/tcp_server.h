#include "get_request.h"
#include "job_queue.h"
#include "lower_string.h"
#include "message.h"
#include "parser.h"
#include "put_request.h"
#include "sys_includes.h"
#include "workers.h"
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <poll.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#ifndef TCP_SERVER
#define TCP_SERVER

#define PORT 5000
#define MAX_CLIENTS 50
#define MAX_WORKERS 50

struct addrinfo hints;
struct addrinfo
    *res; // res contains the final linked list walk to find the correct value
int socket_fd;
struct sockaddr_storage incoming_req;
struct addrinfo *success_addr;
struct sockaddr *s;

typedef struct {
    char buffer[1024];
} response;

typedef struct {
    response responses[MAX_CLIENTS];
} client_responses;

#endif // TCP_SERVER
