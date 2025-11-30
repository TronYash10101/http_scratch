#include "headers/tcp_server.h"
#include "headers/message.h"
#include "headers/parser.h"
#include "headers/sys_includes.h"
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define MAX_CLIENTS 3

struct addrinfo hints;
struct addrinfo
    *res; // res contains the final linked list walk to find the correct value
int socket_fd;
struct sockaddr_storage incoming_req;
struct addrinfo *success_addr;
struct sockaddr *s;
// struct pollfd connection_socket[0];
struct pollfd fds[MAX_CLIENTS];

typedef struct {
  char buffer[1024];
} response;

typedef struct {
  response responses[MAX_CLIENTS];
} client_responses;

void non_block(int socket_fd) {
  int flags = fcntl(socket_fd, F_GETFL, 0);
  fcntl(socket_fd, F_SETFL, flags | O_NONBLOCK);
}

int main() {

  char ip4_buff[INET_ADDRSTRLEN];
  char peer_buff[INET_ADDRSTRLEN];
  char server_rep[1024];
  char exit;
  client_responses client_responses;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  if (getaddrinfo("127.0.0.1", "8000", &hints, &res) != 0) {
    perror("getaddrinfo");
    return 1;
  }

  for (struct addrinfo *p = res; p != NULL; p = p->ai_next) {
    if ((socket_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) ==
        -1) {
      perror("socker error");
      continue;
    }
    if (bind(socket_fd, p->ai_addr, p->ai_addrlen) == -1) {
      perror("bind error");
      continue;
    }
    success_addr = p;
    break;
  }
  if (success_addr->ai_family == AF_INET) {
    s = success_addr->ai_addr;
    inet_ntop(success_addr->ai_family,
              (struct sockaddr_in *)success_addr->ai_addr, ip4_buff,
              success_addr->ai_addrlen);
    printf("\nbind succesful at IP %s\n", ip4_buff);
    printf("socket descriptor %d\n", socket_fd);
  }

  if (listen(socket_fd, 10) == -1) {
    perror("listen error");
    return -1;
  }
  non_block(socket_fd);

  fds[0].events = POLLIN;
  fds[0].fd = socket_fd;

  int nfds = 1;
  while (1) {
    int events = poll(fds, nfds, 2500);

    // New socket descriptor to actually communicate, old one still used for
    // listening
    if (fds[0].revents & POLLIN) {
      unsigned int incoming_req_size = sizeof(incoming_req);
      int new_socketfd;
      if ((new_socketfd = accept(socket_fd, (struct sockaddr *)&incoming_req,
                                 &incoming_req_size)) == -1) {
        perror("accept");
        return -1;
      }

      non_block(new_socketfd);

      // Check for new clients
      if (incoming_req.ss_family == AF_INET) {
        struct sockaddr peer;
        unsigned int len = sizeof(struct sockaddr);
        struct sockaddr_in *peer_addr = (struct sockaddr_in *)&incoming_req;
        getpeername(new_socketfd, &peer, &len);
        printf("\npeer info: %s\n",
               inet_ntop(res->ai_family, &peer_addr->sin_addr, peer_buff,
                         sizeof(struct sockaddr)));

        if (nfds < MAX_CLIENTS) {
          fds[nfds].fd = new_socketfd;
          fds[nfds].events = POLLIN;
          nfds += 1;
        }
      }
    }

    // Check on existing sockets for action
    for (int i = 1; i < nfds; i++) {
      if (fds[i].revents & POLLIN) {

        field_values request_headers;
        Irequest_line request_line;
        Istatus_line status_line;

        int n = recv(fds[i].fd, client_responses.responses[i].buffer,
                     sizeof(client_responses.responses[i].buffer) - 1, 0);
        if (n == 0) {
          printf("\nClinet Disconnected");
        } else {
          client_responses.responses[i].buffer[n] = '\0';
        }
        request_parser(client_responses.responses[i].buffer, &request_line,
                       &status_line, &request_headers);
        printf("%s", request_line.request_target);
        printf("%s", request_line.method);
        printf("%c", request_headers.field_value[1]);
      }

      break;
    }

    /* int scan_fd = scanf("%c", &exit);
    int flags = fcntl(scan_fd, F_GETFL, 0);
    fcntl(scan_fd, F_SETFL, flags | O_NONBLOCK); */
    if (exit == 'e') {
      printf("\nServer Closed\n");
      return 0;
    }
  }

  if (fds[0].events & POLL_HUP) {
    freeaddrinfo(res);
    for (int i = nfds; i >= 0; i--) {
      close(fds[i].fd);
    }
    close(socket_fd);
  }
}
