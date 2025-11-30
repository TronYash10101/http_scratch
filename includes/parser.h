#ifndef PARSER
#define PARSER

#include "message.h"
#include "sys_includes.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void request_line_parser(const char *restrict request_line_ptr,
                         Irequest_line *request_line);

void response_line_parser(const char *restrict http_version, int status_code,
                          const char *restrict reason_pharse,
                          Istatus_line *status_line);

int hash_key(char *field_name);

void request_headers_parser(const char *restrict header, field_values *headers);

void request_parser(const char *restrict req_get, Irequest_line *request_line,
                    Istatus_line *status_line, field_values *headers);

#endif
