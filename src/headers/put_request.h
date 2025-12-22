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

/**
 * @brief Handle an HTTP PUT request and prepare the HTTP response.
 *
 * This function processes an HTTP PUT request by interpreting the request
 * line and headers, writing or updating the target resource using the
 * provided request body, and constructing the appropriate HTTP status line
 * and response headers.
 *
 * No data is written directly to the network socket; instead, all response
 * metadata is written to the provided output structures and buffers.
 *
 * @param request_line               Parsed HTTP request line (method, target,
 * version)
 * @param request_headers            Parsed HTTP request headers
 * @param file_path_buffer           Buffer used to construct the resolved file
 * system path
 * @param response_header_buffer     Buffer used to build serialized HTTP
 * response headers
 * @param response_header_buffer_size Size of the response header buffer
 * @param file_path_buffer_size      Size of the file path buffer
 * @param status_line                Output structure for the HTTP status line
 * @param response_header            Output structure for HTTP response headers
 * @param body                       Request body data to be written to the
 * target resource
 */
void put_request(const Irequest_line *restrict request_line,
                 const request_headers *restrict request_headers,
                 char *file_path_buffer, char *response_header_buffer,
                 int response_header_buffer_size, int file_path_buffer_size,
                 Istatus_line *status_line, response_headers *reponse_header,
                 const char *body);

#endif
