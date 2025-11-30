#include "sys_includes.h"

#ifndef MESSAGE
#define MESSAGE

#define MAX_HEADERS 4

/* typedef struct {
  char Host[1024];
  int Content_Length;
  char Content_Type[1024];
} request_headers;

typedef struct {
  int Content_Length;
  char Content_Type[1024];
} response_headers; */

typedef struct {
  char method[10];
  char request_target[1024];
  char http_version[50];
} Irequest_line;

typedef struct {
  char http_version[50];
  int status_code;
  char reason_phrase[1024];
} Istatus_line;

typedef struct {
  char field_value[1024];
} field_values;

#endif
