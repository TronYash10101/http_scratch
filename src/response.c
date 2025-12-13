#include "headers/response.h"

void format_response(char *buffer, size_t buff_size, Istatus_line *status_line,
                     int content_len, const char *content_type) {

  // support apending
  // int content_len = strlen(response);
  snprintf(buffer, buff_size,
           "%s %d\r\nContent-Length: %d\r\nContent-Type: %s\r\n\r\n",
           status_line->http_version, status_line->status_code, content_len,
           content_type);
  buffer[buff_size] = '\0';
}

void respond(const char *restrict file_path, char *file_buffer,
             char *response_buffer, int response_buffer_size,
             Istatus_line *status_line, response_headers *reponse_header) {

  int content_len = 0;
  struct stat file_stat;
  status_line->status_code = 400;
  stat(file_path, &file_stat);
  content_len = file_stat.st_size;
  format_response(response_buffer, response_buffer_size, status_line,
                  file_stat.st_size, "text");
  strcpy(file_buffer, file_path);
  snprintf(reponse_header->Content_Type, sizeof(reponse_header->Content_Type),
           "%s", "text");
  reponse_header->Content_Length = content_len;
};
