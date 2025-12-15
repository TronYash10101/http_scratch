#ifndef GET_REQUEST
#define GET_REQUEST

#include "errors.h"
#include "message.h"
#include "response.h"
#include "router.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
/*
 (MAIN) Handles an HTTP GET request and builds the HTTP response.

 @file_buffer           Buffer to store the requested file contents
 @response_buffer       Buffer to build the HTTP response
 @response_buffer_size  Size of the response buffer
 @status_line           HTTP status line output
 @request_target        Requested resource path
 @response_header       HTTP response headers
*/
void get_request(const Irequest_line *restrict request_line,
                 const request_headers *restrict request_headers,
                 char *file_path_buffer, char *response_header_buffer,
                 int response_buffer_size, Istatus_line *status_line,
                 response_headers *reponse_header);

#endif
