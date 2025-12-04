#include "headers/get_request.h"
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

route routes[] = {{"/", "Default Page"},
                  {"/home", "Home Page"},
                  {"/about", "About Page"},
                  {"/contact", "Contact Page"}};

char file_path[] =
    "/home/yash-jadhav/http_scratch/src/static_folder/index.html";

void format_response(char *buffer, size_t buff_size, Istatus_line *status_line,
                     const char *response, const char *content_type) {

  // suport apending
  int content_len = strlen(response);
  snprintf(buffer, buff_size,
           "%s %d\r\nContent-Length: %d\r\nContent-Type: %s\r\n\r\n%s",
           status_line->http_version, status_line->status_code, content_len,
           content_type, response);

  buffer[buff_size] = '\0';
}

void serve_static_files(char *file_path, char *response_buffer,
                        int response_buffer_size, Istatus_line *status_line,
                        const char *content_type) {

  char chunked_file_part[8192];
  int bytes_read;
  char curr_dir[1024];

  if (getcwd(curr_dir, sizeof(curr_dir)) != NULL) {
    printf("%s", curr_dir);
  } else {
    perror("error getting dir");
  }

  int static_file_fd = open(file_path, O_RDONLY, 0644);

  if (status_line->status_code == 400) {
    format_response(response_buffer, response_buffer_size, status_line,
                    "Malformed or Invalid Request", content_type);
    return;
  } else if (status_line->status_code == 404 || static_file_fd == -1) {

    format_response(response_buffer, response_buffer_size, status_line,
                    "Resource Not Found", content_type);
    return;
  }

  if (status_line->status_code == 200) {
    while ((bytes_read = read(static_file_fd, chunked_file_part,
                              sizeof(chunked_file_part))) > 0) {
      format_response(response_buffer, response_buffer_size, status_line,
                      chunked_file_part, content_type);
    };
    return;
  }
}

void get_request(char *response_buffer, int response_buffer_size,
                 Istatus_line *status_line,
                 const char *restrict request_target) {

  // eg: /home or eg: /home?param=1
  // spliting request target on /
  int req_target_word_count = 0;
  int buffer_word_count = 0;
  char target_buffer[1024];

  strcpy(status_line->http_version, "HTTP/1.1");

  if (!request_target) {
    status_line->status_code = 400;
    serve_static_files(file_path, response_buffer, response_buffer_size,
                       status_line, "text");
    return;
  }

  if (request_target[0] != '/') {
    status_line->status_code = 400;
    serve_static_files(file_path, response_buffer, response_buffer_size,
                       status_line, "text");
    return;
  }

  // Capturing target resource
  for (req_target_word_count = 0;
       request_target[req_target_word_count] != '?' &&
       request_target[req_target_word_count] != '\0';
       req_target_word_count++) {

    /* if (request_target[req_target_word_count] == '/') {
      continue;
    } */

    target_buffer[buffer_word_count++] = request_target[req_target_word_count];
  }

  target_buffer[buffer_word_count] = '\0';

  // Handling Requests
  for (int i = 0; i < 3; i++) {
    if (strcmp(target_buffer, routes[i].name) == 0) {
      status_line->status_code = 200;
      // const char *route_response = routes[i].response;
      serve_static_files(file_path, response_buffer, response_buffer_size,
                         status_line, "text");
      return;
    }
  }
  status_line->status_code = 404;
  serve_static_files(file_path, response_buffer, response_buffer_size,
                     status_line, "text");
  return;
}
