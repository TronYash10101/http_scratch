#include "sys_includes.h"

#ifndef MESSAGE
#define MESSAGE

#define MAX_FIELD_LINES 4

typedef struct {
  char Host[256];
  char Content_Length[32];
  char Content_Type[128];
  char User_Agent[512];
  char Accept[512];
  char Connection[32];
  char Accept_Encoding[128];
  char Accept_Language[128];
} request_headers;

typedef struct {
  int Content_Length;
  char Content_Type[1024];
} response_headers;

typedef struct {
  char method[10];
  char request_target[1024];
  char http_version[500];
} Irequest_line;

typedef struct {
  char http_version[500];
  int status_code;
} Istatus_line;

typedef struct {
  char field_value[1024];
} field_values;

#endif
