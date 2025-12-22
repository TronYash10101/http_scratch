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

/**
 * @brief Handle an HTTP GET request and prepare the HTTP response.
 *
 * This function processes an HTTP GET request by resolving the requested
 * resource path, reading the corresponding file from disk if it exists,
 * and constructing the appropriate HTTP status line and response headers.
 *
 * The function does not send the response directly; it populates the
 * provided output buffers and structures for later transmission.
 *
 * @param request_line               Parsed HTTP request line (method, target,
 * version)
 * @param request_headers            Parsed HTTP request headers
 * @param file_path_buffer           Buffer used to construct the resolved file
 * system path
 * @param response_header_buffer     Buffer used to build serialized HTTP
 * response headers
 * @param response_buffer_size       Size of the response header buffer
 * @param file_path_buffer_size      Size of the file path buffer
 * @param status_line                Output structure for the HTTP status line
 * @param response_header            Output structure for HTTP response headers
 */
void get_request(const Irequest_line *restrict request_line,
                 const request_headers *restrict request_headers,
                 char *file_path_buffer, char *response_header_buffer,
                 int response_buffer_size, int file_path_buffer_size,
                 Istatus_line *status_line, response_headers *reponse_header);

#endif
