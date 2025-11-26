#include "headers/tcp_server.h"
#include "headers/sys_includes.h"
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

struct addrinfo hints;
struct addrinfo
    *res; // res contains the final linked list walk to find the correct value
int socket_fd;
struct sockaddr_storage incoming_req;
struct addrinfo *success_addr;
struct sockaddr *s;
char *server_res = "Hello peer";

int main() {
  // Loads structs needed later and do DNS lookup, etc.

  char ip4_buff[INET_ADDRSTRLEN];
  char peer_buff[INET_ADDRSTRLEN];
  char exit;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  if (getaddrinfo(DOMAIN, "8000", &hints, &res) != 0) {
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
    printf("\nbind succesful at IP %s\n",
           inet_ntop(success_addr->ai_family,
                     (struct sockaddr_in *)success_addr->ai_addr, ip4_buff,
                     success_addr->ai_addrlen));
    printf("socket descriptor %d\n", socket_fd);
  }

  if (listen(socket_fd, 10) == -1) {
    perror("listen error");
    return -1;
  }

  unsigned int incoming_req_size = sizeof(incoming_req);

  // New socket descriptor to actually communicate, old one still used for
  // listening
  int new_socketfd;
  if ((new_socketfd = accept(socket_fd, (struct sockaddr *)&incoming_req,
                             &incoming_req_size)) == -1) {
    perror("accept");
  }

  if (incoming_req.ss_family == AF_INET) {
    struct sockaddr peer;
    unsigned int len = sizeof(struct sockaddr);

    getpeername(new_socketfd, &peer, &len);
    printf("\npeer info: %s\n", inet_ntop(res->ai_family, peer.sa_data,
                                          peer_buff, sizeof(struct sockaddr)));
    int send_len = strlen(server_res);
    int sent_len;
    if ((sent_len = send(new_socketfd, server_res, send_len, 0)) == -1) {
      perror("error sending");
    };

    if (sent_len == send_len) {
      printf("\nComplete Data Sent\n");
    } else {
      printf("\nIncomplete Data Sent\n");
    }
  }
  printf("press e for exit");
  scanf("%c", &exit);
  if (exit == 'e') {
    freeaddrinfo(res);
    close(new_socketfd);
    close(socket_fd);
  }
}
