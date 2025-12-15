#ifndef POST_REQUEST
#define POST_REQUEST
#include "message.h"
#include "response.h"
#include "unistd.h"

void post_request(const Irequest_line *restrict request_line,
                  const request_headers *restrict request_headers,
                  char *file_path_buffer, char *response_header_buffer,
                  int response_header_buffer_size, Istatus_line *status_line,
                  response_headers *reponse_header, const char *body);

#endif
