#ifndef PARSER
#define PARSER

#include "leading_whitespace.h"
#include "message.h"
#include "sys_includes.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

void request_line_parser(const char *restrict request_line_ptr,
                         Irequest_line *request_line);

/* void response_line_parser(const char *restrict http_version, int status_code,
                          const char *restrict reason_pharse,
                          Istatus_line *status_line); */

/**
 Fill request_header struct, internal method for request_headers_parser

 @param header headers struct to fill
 @param field_name_buffer field_name
 @param field_value_buffer field_value
 **/
void fill_headers(request_headers *headers, const char *field_name_buffer,
                  const char *field_value_buffer);

/* Checks if request is complete, if yes returns 0 else -1 */
int is_complete_request(const char *buffer, int *one_request_len);

/**
 extract headers, internal method for request_parser

 @param header headers recieved from request_parser
 @param headers headers struct to fill
 **/
void request_headers_parser(const char *restrict header,
                            request_headers *headers);

/**
 (IN MAIN) Fill request_line and request_header

 @param req_get headers recieved from client
 @param req_alive struct for keep-alive to fill
 @param request_line request_line struct to fill
 @param headers headers struct to fill

 **/
void request_parser(const char *restrict req_get, Irequest_line *request_line,
                    request_headers *headers, char body[2048]);

#endif
