#ifndef GET_REQUEST
#define GET_REQUEST

#include "message.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  const char *name;
  const char *response;
} route;

extern route routes[];
void format_response(char *buffer, size_t buff_size, Istatus_line *status_line,
                     const char *response, const char *content_type);
void get_request(char *response_buffer, Istatus_line *status_line,
                 const char *restrict request_target);
#endif
