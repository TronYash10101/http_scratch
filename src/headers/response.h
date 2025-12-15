#ifndef RESPONSE
#define RESPONSE
#include "message.h"
#include "sys/stat.h"

void format_response_headers(char *buffer, size_t buff_size,
                             Istatus_line *status_line, int content_len,
                             const char *content_type);

/* Creates response for methods and also fills file_path_buffer
 */
void respond(const char *restrict file_path, char *response_buffer,
             int response_buffer_size, Istatus_line *status_line,
             response_headers *reponse_header);
#endif
