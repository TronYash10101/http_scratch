#include "headers/tcp_server.h"
#include "headers/parser.h"
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <unistd.h>

// Array storing pollfd structs
struct pollfd fds[MAX_CLIENTS];
struct alive_struct alive_connections[MAX_CLIENTS];

void non_block(int socket_fd) {
  int flags = fcntl(socket_fd, F_GETFL, 0);
  fcntl(socket_fd, F_SETFL, flags | O_NONBLOCK);
}

int main() {

  char ip4_buff[INET_ADDRSTRLEN];
  char peer_buff[INET_ADDRSTRLEN];
  // char exit = '\0';
  // client_responses client_responses;
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

    LOG_INFO("\nbind succesful at IP %s\n", ip4_buff);
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

    // Check for new clients
    if (fds[0].revents & POLLIN) {
      unsigned int incoming_req_size = sizeof(incoming_req);
      int new_socketfd;
      if ((new_socketfd = accept(socket_fd, (struct sockaddr *)&incoming_req,
                                 &incoming_req_size)) == -1) {
        perror("accept");
        return -1;
      }

      non_block(new_socketfd);

      if (incoming_req.ss_family == AF_INET) {
        struct sockaddr peer;
        unsigned int len = sizeof(struct sockaddr);
        struct sockaddr_in *peer_addr = (struct sockaddr_in *)&incoming_req;
        getpeername(new_socketfd, &peer, &len);

        if (nfds < MAX_CLIENTS) {
          fds[nfds].fd = new_socketfd;
          fds[nfds].events = POLLIN;
          alive_connections[nfds].last_active = time(NULL);
          alive_connections[nfds].handled_requests = 0;
          alive_connections[nfds].keep_alive = true;
          nfds += 1;
          LOG_INFO("ACCEPT fd=%d ip=%s", new_socketfd,
                   inet_ntop(res->ai_family, &peer_addr->sin_addr, peer_buff,
                             sizeof(peer_buff)));
        }
      }
    }

    // Check on existing sockets for action
    for (int i = 1; i < nfds; i++) {
      time_t start_time = time(NULL);

      if (!(fds[i].revents & POLLIN)) {
        if (start_time - alive_connections[i].last_active > 10) {
          LOG_WARN("TIMEOUT fd=%d last_active=%ld now=%ld", fds[i].fd,
                   alive_connections[i].last_active, start_time);
          close(fds[i].fd);
          fds[i] = fds[nfds - 1];
          alive_connections[i] = alive_connections[nfds - 1];
          nfds--;
          i--;
        }
        continue;
      }

      request_headers request_header;
      response_headers response_header;
      Irequest_line request_line;
      Istatus_line status_line;
      char response_buffer[RESPONSE_BUFFER_SIZE];
      char response_header_buffer[RESPONSE_BUFFER_SIZE];
      char file_path[RESPONSE_BUFFER_SIZE];
      char body[RESPONSE_BUFFER_SIZE];

      memset(&request_line, 0, sizeof(request_line));
      memset(&request_header, 0, sizeof(request_header));
      memset(file_path, 0, sizeof(file_path));
      memset(body, 0, sizeof(body));
      memset(body, 0, sizeof(body));
      // memset(client_responses.responses[i].buffer, 0,
      //        sizeof(client_responses.responses[i].buffer));

      alive_connections[i].per_connection_buffer_len = 0;
      int n = recv(fds[i].fd,
                   &alive_connections[i].per_connection_buffer +
                       alive_connections[i].per_connection_buffer_len,
                   sizeof(alive_connections[i].per_connection_buffer) - 1, 0);
      alive_connections[i].per_connection_buffer_len += n;

      // printf("%s", client_responses.responses[i].buffer);
      // Look out for this
      alive_connections[i].last_active = time(NULL);
      if (n < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
          LOG_ERROR("RECV fd=%d errno=%d", fds[i].fd, errno);
          continue;
        } else {
          perror("recv");
          close(fds[i].fd);
          continue;
        }
      }

      if (n == 0) {
        LOG_INFO("CLIENT CLOSED fd=%d", fds[i].fd);
        close(fds[i].fd);
        continue;
      }

      alive_connections[i].per_connection_buffer
          [alive_connections[i].per_connection_buffer_len] = '\0';

      int one_request_len = 0;

      if (is_complete_request(alive_connections[i].per_connection_buffer,
                              &one_request_len) == 0) {
        request_parser(alive_connections[i].per_connection_buffer,
                       &request_line, &request_header, body);

        LOG_INFO("REQUEST fd=%d %s %s %s", fds[i].fd, request_line.method,
                 request_line.request_target, request_line.http_version);

        if (strcmp("GET", request_line.method) == 0) {
          get_request(&request_line, &request_header, file_path,
                      response_header_buffer, RESPONSE_BUFFER_SIZE,
                      RESPONSE_BUFFER_SIZE, &status_line, &response_header);
        } else if (strcmp("PUT", request_line.method) == 0) {
          put_request(&request_line, &request_header, file_path,
                      response_header_buffer, RESPONSE_BUFFER_SIZE,
                      RESPONSE_BUFFER_SIZE, &status_line, &response_header,
                      body);
        }
        int total_bytes_send = 0;
        int bytes_read = 0;
        int header_length = strlen(response_header_buffer);
        int header_send = 0;
        int file_socket = open(file_path, O_RDONLY);

        if (file_socket == -1) {
          status_line.status_code = 500;
          LOG_ERROR("Could not open file");
          return -1;
        }
        while (header_send < header_length) {
          int header_bytes_send =
              send(fds[i].fd,
                   response_header_buffer +
                       header_send, // moves pointer of first element in
                                    // response header buff to point to first
                                    // element of remaining elements
                   header_length - header_send, 0);
          LOG_DEBUG("RESPONSE fd=%d status=%d content_len=%d", fds[i].fd,
                    status_line.status_code, header_length);
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
        if (strncasecmp(request_header.Connection, "Close", strlen("Close")) ==
            0) {
          alive_connections[i].keep_alive = false;
          LOG_DEBUG("KEEP-ALIVE fd=%d disabled", fds[i].fd);
        }
        if (!(alive_connections[i].keep_alive) ||
            alive_connections[i].handled_requests > 10) {
          close(fds[i].fd);
          fds[i] = fds[nfds - 1];
          alive_connections[i] = alive_connections[nfds - 1];
          nfds--;
          i--;
          LOG_INFO("CLOSE fd=%d reason=%s handled=%d", fds[i].fd,
                   alive_connections[i].handled_requests > 3
                       ? "max_requests"
                       : "connection_close",
                   alive_connections[i].handled_requests);

        } else {
          fds[i].events = POLLIN;
          alive_connections[i].handled_requests += 1;
        }
        printf("\n");
        LOG_INFO("Buffer State:\n%s",
                 alive_connections[i].per_connection_buffer);
        memmove(&alive_connections[i].per_connection_buffer,
                &alive_connections[i].per_connection_buffer + one_request_len,
                alive_connections[i].per_connection_buffer_len -
                    one_request_len);
      };
    }
  }

  if (fds[0].events & POLLHUP) {
    freeaddrinfo(res);
    close(socket_fd);
  }
}
