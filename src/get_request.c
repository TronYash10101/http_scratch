#include "headers/get_request.h"
#include <stdio.h>
#include <string.h>

route routes[] = {{"/", "Default Page"},
                  {"/home", "Home Page"},
                  {"/about", "About Page"},
                  {"/contact", "Contact Page"}};

void format_response(char *buffer, size_t buff_size, Istatus_line *status_line,
                     const char *response, const char *content_type) {
  size_t buffer_max_size = strlen(buffer);
  int content_len = strlen(response);
  snprintf(buffer, buffer_max_size,
           "%s %d\r\nContent-Length: %d\r\nContent-Type: %s\r\n\r\n%s",
           status_line->http_version, status_line->status_code, content_len,
           content_type, response);
}

void get_request(char *response_buffer, Istatus_line *status_line,
                 const char *restrict request_target) {

  // eg: /home or eg: /home?param=1
  // spliting request target on /
  int req_target_word_count = 0;
  int buffer_word_count = 0;
  char target_buffer[1024];

  strcpy(status_line->http_version, "HTTP/1.1");

  if (!request_target) {
    status_line->status_code = 400;
    format_response(response_buffer, sizeof(response_buffer), status_line,
                    "Invalid Request", "text");
    return;
  }

  if (request_target[0] != '/') {
    status_line->status_code = 400;
    format_response(response_buffer, sizeof(response_buffer), status_line,
                    "Malformed Request", "text");
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
      const char *route_response = routes[i].response;
      format_response(response_buffer, sizeof(response_buffer), status_line,
                      route_response, "text");
      return;
    }
  }
  status_line->status_code = 404;
  format_response(response_buffer, sizeof(response_buffer), status_line,
                  "Resource Not Found", "text");
  return;
}
