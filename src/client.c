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
char *server_res =
    "The concept of progress has shaped human history in profound ways, "
    "influencing how societies organize themselves, how individuals measure "
    "success, and how cultures define their values. From the earliest tools "
    "made of stone to the most advanced digital technologies of today, "
    "progress is often described as a steady march forward. Yet the reality of "
    "progress is rarely simple or evenly distributed. Some communities benefit "
    "rapidly from innovation, while others are left behind, creating gaps in "
    "wealth, education, and opportunity. This uneven distribution challenges "
    "the idea that progress is automatically beneficial for everyone. True "
    "progress requires not only technological advancement, but also ethical "
    "reflection, social responsibility, and long-term thinking. When societies "
    "focus only on speed and efficiency, they risk overlooking the human costs "
    "involved, such as environmental damage, mental health strain, and loss of "
    "cultural identity. At the same time, rejecting progress entirely is "
    "neither realistic nor desirable, as innovation has improved healthcare, "
    "communication, and quality of life for billions of people. The challenge "
    "lies in learning how to guide progress wisely rather than blindly "
    "accelerating it. This requires cooperation between governments, "
    "scientists, educators, and citizens, as well as a willingness to question "
    "assumptions and adapt when consequences become harmful. In this sense, "
    "progress is not a destination but an ongoing conversation between what is "
    "possible and what is responsible. The future will be shaped not only by "
    "what humans can create, but by the choices they make about how those "
    "creations are used.";

int main() {

  char server_buff[INET_ADDRSTRLEN];
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

  int send_len = strlen(server_res);
  int sent_len = send(socket_fd, server_res, send_len, 0);
  if (sent_len == -1) {
    perror("error sending");
  };
}
