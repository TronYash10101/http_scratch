#ifndef GET_REQUEST
#define GET_REQUEST

#include "message.h"
#include "response.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  const char *name;
  const char *response;
} route;

extern route routes[];
/*
 (MAIN) Handles an HTTP GET request and builds the HTTP response.

 @file_buffer           Buffer to store the requested file contents
 @response_buffer       Buffer to build the HTTP response
 @response_buffer_size  Size of the response buffer
 @status_line           HTTP status line output
 @request_target        Requested resource path
 @response_header       HTTP response headers
*/
void get_request(char *file_buffer, char *response_buffer,
                 int response_buffer_size, Istatus_line *status_line,
                 const char *restrict request_target,
                 response_headers *reponse_header);

#endif
