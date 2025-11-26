#include "headers/sys_includes.h"
#include "headers/tcp_server.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

struct addrinfo hints;
struct addrinfo
    *res; // res contains the final linked list walk to find the correct value
int socket_fd;

int main() {

  char server_buff[INET_ADDRSTRLEN];
  char server_rep[1024];
  char host_name[1024];
  int yes = 1;

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

    if (gethostname(host_name, sizeof(host_name)) == -1) {
      perror("hostname");
      continue;
    }
    break;
  }
  setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes);

  struct sockaddr *s = res->ai_addr;
  unsigned int len = sizeof(struct sockaddr);
  if (connect(socket_fd, s, len) == -1) {
    perror("connect");
  };
  struct sockaddr_in *peer = (struct sockaddr_in *)s;
  printf("\nserver information: %s\n",
         inet_ntop(res->ai_family, &peer->sin_addr, server_buff,
                   sizeof(struct sockaddr_in)));

  int n = recv(socket_fd, server_rep, sizeof(server_rep) - 1, 0);
  if (n == 0) {
    printf("Connection Closed");
  } else {
    server_rep[n] = '\0';
  }

  printf("%s\n", server_rep);
}
