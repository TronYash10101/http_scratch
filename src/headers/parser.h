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

void fill_headers(request_headers *headers, const char *field_name_buffer,
                  const char *field_value_buffer);

void request_headers_parser(const char *restrict header,
                            request_headers *headers);

void request_parser(const char *restrict req_get, Irequest_line *request_line,
                    request_headers *headers);

#endif
