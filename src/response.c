#include "headers/response.h"

void format_response_headers(char *buffer, size_t buff_size,
                             Istatus_line *status_line, int content_len,
                             const char *content_type) {

    snprintf(buffer, buff_size,
             "%s %d\r\nContent-Length: %d\r\nContent-Type: %s\r\n\r\n",
             status_line->http_version, status_line->status_code, content_len,
             content_type);
}

void respond(const char *restrict file_path, char *response_buffer,
             int response_buffer_size, Istatus_line *status_line,
             response_headers *response_header) {

    int content_len = 0;
    struct stat file_stat;

    strcpy(status_line->http_version, "HTTP/1.1");
    stat(file_path, &file_stat);
    content_len = file_stat.st_size;
    format_response_headers(response_buffer, response_buffer_size, status_line,
                            file_stat.st_size, response_header->Content_Type);

    // filling response headers
    snprintf(response_header->Content_Type,
             sizeof(response_header->Content_Type), "%s",
             response_header->Content_Type);
    response_header->Content_Length = content_len;
};
