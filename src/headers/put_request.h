#ifndef POST_REQUEST
#define POST_REQUEST
#include "errors.h"
#include "message.h"
#include "response.h"
#include "router.h"
#include "unistd.h"
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void put_request(const Irequest_line *restrict request_line,
                 const request_headers *restrict request_headers,
                 char *file_path_buffer, char *response_header_buffer,
                 int response_header_buffer_size, int file_path_buffer_size,
                 Istatus_line *status_line, response_headers *reponse_header,
                 const char *body);

#endif
