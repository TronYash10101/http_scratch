#include "headers/get_request.h"
#include "headers/errors.h"
#include <stdio.h>
#include <string.h>
// Improve GET Request

void get_request(const Irequest_line *restrict request_line,
                 const request_headers *restrict request_headers,
                 char *file_path_buffer, char *response_header_buffer,
                 int response_buffer_size, int file_path_buffer_size,
                 Istatus_line *status_line, response_headers *response_header) {

  int req_target_word_count = 0;
  int buffer_word_count = 0;
  char target_buffer[1024];
  int content_len = 0;
  struct stat file_stat;

  if (request_line->request_target[0] != '/' &&
      request_line->request_target[0] != '\0') {
    status_line->status_code = 400;
    error_page_path(status_line->status_code, file_path_buffer);
    respond(file_path_buffer, response_header_buffer, response_buffer_size,
            status_line, response_header);
    printf("Request Header Error");
    return;
  }
  if (strstr(target_buffer, "..")) {
    status_line->status_code = 403;
    error_page_path(status_line->status_code, file_path_buffer);
    respond(file_path_buffer, response_header_buffer, response_buffer_size,
            status_line, response_header);
  }

  // Capturing target resource, maybe redundant
  for (req_target_word_count = 0;
       request_line->request_target[req_target_word_count] != '?' &&
       request_line->request_target[req_target_word_count] != '\0';
       req_target_word_count++) {

    target_buffer[buffer_word_count++] =
        request_line->request_target[req_target_word_count];
  }

  target_buffer[buffer_word_count] = '\0';

  // Handling Requests
  if (router(target_buffer, request_line->method, file_path_buffer,
             file_path_buffer_size, response_header) != -1) {
    status_line->status_code = 200;
    respond(file_path_buffer, response_header_buffer, response_buffer_size,
            status_line, response_header);
    printf("GET Success");
    return;
  } else if (router(target_buffer, request_line->method, file_path_buffer,
                    file_path_buffer_size, response_header) == -1) {
    status_line->status_code = 404;
    error_page_path(status_line->status_code, file_path_buffer);
    respond(file_path_buffer, response_header_buffer, response_buffer_size,
            status_line, response_header);
    printf("GET Resource not found");
    return;
  }
}
