#include "headers/tcp_server.h"
#include "headers/get_request.h"
#include "headers/lower_string.h"
#include "headers/message.h"
#include "headers/parser.h"
#include "headers/sys_includes.h"
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAX_CLIENTS 3
#define RESPONSE_BUFFER_SIZE 2048

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
  char exit = '\0';
  client_responses client_responses;
  int opt = 1;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  if (getaddrinfo("0.0.0.0", "8000", &hints, &res) != 0) {
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

  setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  if (success_addr->ai_family == AF_INET) {
    struct sockaddr_in *addr = (struct sockaddr_in *)success_addr->ai_addr;

    inet_ntop(AF_INET, &addr->sin_addr, ip4_buff, INET_ADDRSTRLEN);

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

      request_headers request_header;
      response_headers response_header;
      Irequest_line request_line;
      Istatus_line status_line;
      char response_buffer[RESPONSE_BUFFER_SIZE];
      char response_header_buffer[RESPONSE_BUFFER_SIZE];
      char file_path[RESPONSE_BUFFER_SIZE];

      if (!(fds[i].revents & POLLIN)) {
        continue;
      }

      int n = recv(fds[i].fd, client_responses.responses[i].buffer,
                   sizeof(client_responses.responses[i].buffer) - 1, 0);
      if (n < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
          continue;
        } else {
          perror("recv");
          close(fds[i].fd);
          continue;
        }
      }

      if (n == 0) {
        printf("Client disconnected\n");
        close(fds[i].fd);
        continue;
      }
      client_responses.responses[i].buffer[n] = '\0';
      request_parser(client_responses.responses[i].buffer, &request_line,
                     &request_header);

      get_request(file_path, response_header_buffer, RESPONSE_BUFFER_SIZE,
                  &status_line, request_line.request_target, &response_header);

      int total_bytes_send = 0;
      int bytes_read = 0;
      int header_length = strlen(response_header_buffer);
      int header_send = 0;

      int file_socket = open(file_path, O_RDONLY);

      if (file_socket == -1) {
        status_line.status_code = 500;
        printf("Could not open file");
        return -1;
      }

      while (header_send < header_length) {
        int header_bytes_send =
            send(fds[i].fd,
                 response_header_buffer +
                     header_send, // moves pointer of first element in response
                                  // header buff to point to first element of
                                  // remaining elements
                 header_length - header_send, 0);
        if (header_bytes_send < 0) {
          break;
        }
        header_send += header_bytes_send;
      }

      while ((bytes_read = read(file_socket, response_buffer,
                                sizeof(response_buffer))) > 0) {

        int bytes_send = 0;
        while (bytes_send < bytes_read) {
          int n = send(fds[i].fd, response_buffer + bytes_send,
                       bytes_read - bytes_send, 0);

          if (n <= 0) {
            close(file_socket);
            return -1;
          }
          bytes_send += n;
        }
      }

      close(fds[i].fd);
      fds[i] = fds[nfds - 1];
      nfds--;
    }

    /* if (exit == 'e') {
      printf("\nServer Closed\n");
      return 0;
    } */
  }

  if (fds[0].events & POLLHUP) {
    freeaddrinfo(res);
    close(socket_fd);
  }
}
