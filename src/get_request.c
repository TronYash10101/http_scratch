#include "headers/get_request.h"
#include "headers/response.h"
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

char home_path[] =
    "/home/yash-jadhav/http_scratch/src/static_folder/index.html";
char about_path[] =
    "/home/yash-jadhav/http_scratch/src/static_folder/about.html";
char not_found_path[] =
    "/home/yash-jadhav/http_scratch/src/static_folder/not_found.html";
char malformed_path[] =
    "/home/yash-jadhav/http_scratch/src/static_folder/malformed.html";
char big_path[] = "/home/yash-jadhav/http_scratch/src/static_folder/big.html";

route routes[] = {{"/", home_path}, {"/about", about_path}, {"/big", big_path}};

void get_request(char *file_buffer, char *response_buffer,
                 int response_buffer_size, Istatus_line *status_line,
                 const char *restrict request_target,
                 response_headers *reponse_header) {

  const int MAX_ENDPOINTS = sizeof(routes) / sizeof(routes[0]);

  int req_target_word_count = 0;
  int buffer_word_count = 0;
  char target_buffer[1024];
  int content_len = 0;
  struct stat file_stat;

  strcpy(status_line->http_version, "HTTP/1.1");

  if (!request_target) {
    respond(malformed_path, file_buffer, response_buffer, response_buffer_size,
            status_line, reponse_header);
    return;
  }

  if (request_target[0] != '/') {
    respond(malformed_path, file_buffer, response_buffer, response_buffer_size,
            status_line, reponse_header);
    return;
  }

  // Capturing target resource
  for (req_target_word_count = 0;
       request_target[req_target_word_count] != '?' &&
       request_target[req_target_word_count] != '\0';
       req_target_word_count++) {

    target_buffer[buffer_word_count++] = request_target[req_target_word_count];
  }

  target_buffer[buffer_word_count] = '\0';

  // Handling Requests
  for (int i = 0; i < MAX_ENDPOINTS; i++) {
    if (strcmp(target_buffer, routes[i].name) == 0) {
      respond(routes[i].response, file_buffer, response_buffer,
              response_buffer_size, status_line, reponse_header);
      return;
    }
  }

  respond(not_found_path, file_buffer, response_buffer, response_buffer_size,
          status_line, reponse_header);
  return;
}
