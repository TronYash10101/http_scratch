#include "headers/tcp_server.h"
#include "headers/get_request.h"
#include "headers/job_queue.h"
#include "headers/parser.h"
#include "headers/put_request.h"
#include "headers/workers.h"
#include "headers/ws_build_frames.h"
#include "headers/ws_recieve_response.h"
#include "headers/ws_send_response.h"
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

// Array storing pollfd structs
struct pollfd fds[MAX_CLIENTS];
struct alive_struct alive_connections[MAX_CLIENTS];

void non_block(int socket_fd) {
    int flags = fcntl(socket_fd, F_GETFL, 0);
    fcntl(socket_fd, F_SETFL, flags | O_NONBLOCK);
}

void create_thread_pool(void *(*worker_function)(void *), job_queue_t *queue,
                        pthread_t worker_pool[MAX_WORKERS]) {

    for (int i = 0; i < MAX_WORKERS; i++) {
        pthread_create(&worker_pool[i], NULL, worker_function, queue);
    }
}

void upgrade_connection(const char *sec_websocket_key,
                        char response_header_buffer[RESPONSE_BUFFER_SIZE]) {

    char buffer[RESPONSE_BUFFER_SIZE];

    snprintf(buffer, RESPONSE_BUFFER_SIZE,
             "%s258EAFA5-E914-47DA-95CA-C5AB0DC85B11", sec_websocket_key);

    unsigned char sha1_hash[SHA_DIGEST_LENGTH];
    SHA1((const unsigned char *)buffer, strlen(buffer), sha1_hash);

    char sec_websocket_accept[RESPONSE_BUFFER_SIZE];
    EVP_EncodeBlock((unsigned char *)sec_websocket_accept, sha1_hash,
                    SHA_DIGEST_LENGTH);

    snprintf(response_header_buffer, RESPONSE_BUFFER_SIZE,
             "HTTP/1.1 101 Switching Protocols\r\n"
             "Upgrade: websocket\r\n"
             "Connection: Upgrade\r\n"
             "Sec-WebSocket-Accept: %s\r\n\r\n",
             sec_websocket_accept);
}

int main() {

    char ip4_buff[INET_ADDRSTRLEN];
    char peer_buff[INET_ADDRSTRLEN];
    // char exit = '\0';
    // client_responses client_responses;
    int opt = 1;
    job_queue_t job_queue;
    pthread_t worker_pool[MAX_WORKERS];

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if (getaddrinfo("0.0.0.0", "8000", &hints, &res) != 0) {
        perror("getaddrinfo");
        return 1;
    }

    for (struct addrinfo *p = res; p != NULL; p = p->ai_next) {
        if ((socket_fd =
                 socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
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

    if (listen(socket_fd, MAX_CLIENTS) == -1) {
        perror("listen error");
        return -1;
    }
    non_block(socket_fd);

    fds[0].events = POLLIN;
    fds[0].fd = socket_fd;

    int nfds = 1;

    job_queue_init(&job_queue);
    create_thread_pool(static_serve, &job_queue, worker_pool);

    while (1) {
        int events = poll(fds, nfds, 2500);

        // Check for new clients
        if (fds[0].revents & POLLIN) {
            unsigned int incoming_req_size = sizeof(incoming_req);
            int new_socketfd;
            if ((new_socketfd =
                     accept(socket_fd, (struct sockaddr *)&incoming_req,
                            &incoming_req_size)) == -1) {
                perror("accept");
                return -1;
            }

            non_block(new_socketfd);

            if (incoming_req.ss_family == AF_INET) {
                struct sockaddr peer;
                unsigned int len = sizeof(struct sockaddr);
                struct sockaddr_in *peer_addr =
                    (struct sockaddr_in *)&incoming_req;
                getpeername(new_socketfd, &peer, &len);

                if (nfds < MAX_CLIENTS) {
                    fds[nfds].fd = new_socketfd;
                    fds[nfds].events = POLLIN;
                    alive_connections[nfds].fd = new_socketfd;
                    alive_connections[nfds].last_active = time(NULL);
                    alive_connections[nfds].handled_requests = 0;
                    alive_connections[nfds].keep_alive = true;
                    alive_connections[nfds].protocol_type = CONN_HTTP;
                    nfds += 1;
                    LOG_INFO("ACCEPT fd=%d ip=%s", new_socketfd,
                             inet_ntop(res->ai_family, &peer_addr->sin_addr,
                                       peer_buff, sizeof(peer_buff)));
                }
            }
        }

        // Check on existing sockets for action
        time_t start_time = time(NULL);
        for (int i = 1; i < nfds; i++) {

            if (!(fds[i].revents & POLLIN)) {
                if (start_time - alive_connections[i].last_active > 10 &&
                    alive_connections[i].is_busy == false &&
                    alive_connections[i].protocol_type == CONN_HTTP) {
                    LOG_WARN("TIMEOUT fd=%d last_active=%ld "
                             "now=%ld",
                             fds[i].fd, alive_connections[i].last_active,
                             start_time);
                    close(fds[i].fd);
                    fds[i] = fds[nfds - 1];
                    alive_connections[i] = alive_connections[nfds - 1];
                    nfds--;
                    i--;
                } else if (start_time - alive_connections[i].last_active > 10 &&
                           alive_connections[i].protocol_type ==
                               CONN_WEBSOCKET) {
                    // handle ping-pong
                    ws_send_response(fds[i].fd, WS_OP_PING, "Are You there");
                    LOG_INFO("SENT PING");
                    if (!alive_connections[i].sent_ping) {
                        alive_connections[i].sent_ping = true;
                        alive_connections[i].sent_ping_time = time(NULL);
                    }
                }
                continue;
            }

            int n;
            alive_connections[i].per_connection_buffer_len = 0;
            n = recv(fds[i].fd,
                     alive_connections[i].per_connection_buffer +
                         alive_connections[i].per_connection_buffer_len,
                     sizeof(alive_connections[i].per_connection_buffer) -
                         alive_connections[i].per_connection_buffer_len - 1,
                     0);

            if (n > 0) {
                alive_connections[i].per_connection_buffer_len += n;
                alive_connections[i].last_active = time(NULL);
            }

            alive_connections[i].per_connection_buffer
                [alive_connections[i].per_connection_buffer_len] = '\0';

            switch (alive_connections[i].protocol_type) {

            case CONN_HTTP:;

                request_headers request_header;
                response_headers response_header;
                Irequest_line request_line;
                Istatus_line status_line;
                // char response_buffer[RESPONSE_BUFFER_SIZE];
                char response_header_buffer[RESPONSE_BUFFER_SIZE];
                char file_path_buffer[RESPONSE_BUFFER_SIZE];
                char body[RESPONSE_BUFFER_SIZE];

                memset(&request_line, 0, sizeof(request_line));
                memset(&request_header, 0, sizeof(request_header));
                memset(file_path_buffer, 0, sizeof(file_path_buffer));
                memset(body, 0, sizeof(body));

                if (n < 0) {
                    if (errno != EAGAIN && errno != EWOULDBLOCK) {
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

                int one_request_len = 0;
                if (is_complete_request(
                        alive_connections[i].per_connection_buffer,
                        &one_request_len) == 0) {
                    request_parser(alive_connections[i].per_connection_buffer,
                                   &request_line, &request_header, body);

                    LOG_INFO("REQUEST fd=%d %s %s %s", fds[i].fd,
                             request_line.method, request_line.request_target,
                             request_line.http_version);

                    if (strcmp("GET", request_line.method) == 0) {
                        if (strncasecmp(request_header.Connection, "Upgrade",
                                        strlen("Upgrade")) == 0) {
                            char websocket_response[RESPONSE_BUFFER_SIZE];
                            upgrade_connection(
                                leading_whitespace(
                                    request_header.Sec_WebSocket_Key),
                                websocket_response);
                            int header_bytes_sent = 0;
                            int header_bytes_tosend =
                                strlen(websocket_response);

                            while (header_bytes_sent < header_bytes_tosend) {
                                int n = send(
                                    fds[i].fd,
                                    websocket_response + header_bytes_sent,
                                    header_bytes_tosend - header_bytes_sent, 0);
                                header_bytes_sent += n;
                            }
                            alive_connections[i].protocol_type = CONN_WEBSOCKET;
                            memset(alive_connections[i].per_connection_buffer,
                                   0,
                                   sizeof(alive_connections[i]
                                              .per_connection_buffer));
                            i--;
                            continue;
                        }
                        get_request(&request_line, &request_header,
                                    file_path_buffer, response_header_buffer,
                                    RESPONSE_BUFFER_SIZE, RESPONSE_BUFFER_SIZE,
                                    &status_line, &response_header);
                    } else if (strcmp("PUT", request_line.method) == 0) {
                        put_request(&request_line, &request_header,
                                    file_path_buffer, response_header_buffer,
                                    RESPONSE_BUFFER_SIZE, RESPONSE_BUFFER_SIZE,
                                    &status_line, &response_header, body);
                    }
                    int total_bytes_send = 0;
                    int bytes_read = 0;
                    int header_length = strlen(response_header_buffer);
                    int header_send = 0;

                    while (header_send < header_length) {
                        int header_bytes_send = send(
                            fds[i].fd, response_header_buffer + header_send,
                            header_length - header_send, 0);
                        LOG_DEBUG("RESPONSE fd=%d status=%d "
                                  "content_len=%d",
                                  fds[i].fd, status_line.status_code,
                                  header_length);
                        if (header_bytes_send < 0) {
                            break;
                        }
                        header_send += header_bytes_send;
                    }

                    // pass file path to worker
                    job_t *job = malloc(sizeof(job_t));
                    job->socket_descriptor_properties = &alive_connections[i];
                    snprintf(job->file_path_buffer,
                             sizeof(job->file_path_buffer), "%s",
                             file_path_buffer);
                    job->status_line = &status_line;
                    job_queue_push(&job_queue, job);

                    if (strncasecmp(request_header.Connection, "Close",
                                    strlen("Close")) == 0) {
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
                        LOG_INFO("CLOSE fd=%d reason=%s "
                                 "handled=%d",
                                 fds[i].fd,
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

                    memmove(alive_connections[i].per_connection_buffer,
                            alive_connections[i].per_connection_buffer +
                                one_request_len,
                            alive_connections[i].per_connection_buffer_len -
                                one_request_len);

                    alive_connections[i].per_connection_buffer_len -=
                        one_request_len;
                };
                break;

            case CONN_WEBSOCKET:;
                time_t last_time = time(NULL);
                if ((fds[i].revents & (POLLERR | POLLHUP | POLLNVAL))) {
                    // handle close
                    close(fds[i].fd);
                    fds[i] = fds[nfds - 1];
                    alive_connections[i] = alive_connections[nfds - 1];
                    nfds--;
                    i--;
                    continue;
                }

                if (n < 0) {
                    if (errno != EAGAIN && errno != EWOULDBLOCK) {
                        perror("recv");
                        close(fds[i].fd);
                        continue;
                    }
                }

                if (n == 0) {
                    LOG_INFO("WEBSOCKET CLIENT CLOSED fd=%d", fds[i].fd);
                    close(fds[i].fd);
                    continue;
                }

                if (alive_connections[i].sent_ping &&
                    last_time - alive_connections[i].sent_ping_time > 10) {
                    LOG_WARN("PONG NOT RECIEVED CLOSING "
                             "CONNECTION");
                    close(fds[i].fd);
                    fds[i] = fds[nfds - 1];
                    alive_connections[i] = alive_connections[nfds - 1];
                    nfds--;
                    i--;
                    continue;
                }

                uint8_t client_message[1024];
                ws_opcode_t request_opcode = 0;
                memset(client_message, 0, sizeof(client_message));

                const char response[] = "Hello client";

                request_opcode = ws_recieve_response(
                    alive_connections[i].per_connection_buffer, client_message);

                if (request_opcode == WS_OP_TEXT ||
                    request_opcode == WS_OP_CONTINUATION) {
                    if ((alive_connections[i].last_active - start_time) % 5 ==
                        0) {
                        ws_send_response(fds[i].fd, WS_OP_TEXT, response);
                        alive_connections[i].last_active = time(NULL);
                    }

                } else if (request_opcode == WS_OP_PING) {
                    ws_send_response(
                        fds[i].fd, WS_OP_PONG,
                        alive_connections[i].per_connection_buffer);

                } else if (request_opcode == WS_OP_PONG) {
                    LOG_INFO("PONG RECIEVED");
                    alive_connections[i].sent_ping = false;

                } else if (request_opcode == -1) {
                    LOG_ERROR("Error Occured(Incorrect opcode or "
                              "unmasked data recieved)");
                } else if (request_opcode == WS_OP_CLOSE) {
                    close(fds[i].fd);
                    fds[i] = fds[nfds - 1];
                    alive_connections[i] = alive_connections[nfds - 1];
                    nfds--;
                    i--;
                    LOG_INFO("Closing Websocket Connection");
                }

                LOG_INFO("%s", client_message);
                fflush(stdout);
                break;

            default:
                LOG_ERROR("COULD NOT UNDERSTAND PROTOCOL");
            }
        }
    }

    if (fds[0].events & POLLHUP) {
        job_queue_push(&job_queue, POISON_PILL);
        for (int i = 0; i < MAX_WORKERS; i++) {
            pthread_join(worker_pool[i], NULL);
        }
        freeaddrinfo(res);
        close(socket_fd);
    }
}
