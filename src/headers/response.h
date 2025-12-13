#ifndef RESPONSE
#define RESPONSE
#include "message.h"
#include "sys/stat.h"

void format_response(char *buffer, size_t buff_size, Istatus_line *status_line,
                     int content_len, const char *content_type);

void respond(const char *restrict file_path, char *file_buffer,
             char *response_buffer, int response_buffer_size,
             Istatus_line *status_line, response_headers *reponse_header);
#endif
