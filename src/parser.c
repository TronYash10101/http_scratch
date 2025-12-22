#include "headers/parser.h"
#include <stdio.h>

void request_line_parser(const char *restrict request_line_ptr,
                         Irequest_line *request_line) {

  sscanf(request_line_ptr, "%9s %1023s %49s", request_line->method,
         request_line->request_target, request_line->http_version);
  return;
}

void fill_headers(request_headers *headers, const char *field_name_buffer,
                  const char *field_value_buffer) {
  if (strcasecmp(field_name_buffer, "Host") == 0) {
    snprintf(headers->Host, sizeof(headers->Host), "%s", field_value_buffer);
  } else if (strcasecmp(field_name_buffer, "Content-Length") == 0) {
    snprintf(headers->Content_Length, sizeof(headers->Content_Length), "%s",
             field_value_buffer);
  } else if (strcasecmp(field_name_buffer, "Content-Type") == 0) {
    snprintf(headers->Content_Type, sizeof(headers->Content_Type), "%s",
             field_value_buffer);
  } else if (strcasecmp(field_name_buffer, "User-Agent") == 0) {
    snprintf(headers->User_Agent, sizeof(headers->User_Agent), "%s",
             field_value_buffer);
  } else if (strcasecmp(field_name_buffer, "Accept") == 0) {
    snprintf(headers->Accept, sizeof(headers->Accept), "%s",
             field_value_buffer);
  } else if (strcasecmp(field_name_buffer, "Connection") == 0) {
    snprintf(headers->Connection, sizeof(headers->Connection), "%s",
             field_value_buffer);
  } else if (strcasecmp(field_name_buffer, "Accept-Encoding") == 0) {
    snprintf(headers->Accept_Encoding, sizeof(headers->Accept_Encoding), "%s",
             field_value_buffer);
  } else if (strcasecmp(field_name_buffer, "Accept-Language") == 0) {
    snprintf(headers->Accept_Language, sizeof(headers->Accept_Language), "%s",
             field_value_buffer);
  } else {
  }
}

void request_headers_parser(const char *restrict header,
                            request_headers *headers) {

  int word_count = 0;

  while (1) {
    char field_name_buffer[500];
    char field_value_buffer[500];
    char curr_field_name[500];
    bool recording_value = false;

    if (header[word_count] == '\0' || header[word_count + 1] == '\0') {
      LOG_ERROR("Malformed header: missing CRLFCRLF\n");
      abort();
      return;
    }

    if (header[word_count] == '\r' && header[word_count + 1] == '\n') {
      // This function is used to only parse headers nothing else so it
      // should end with \r\n\r\n, NULL terminator is after request body
      break;
    }

    int name_word_count;
    for (name_word_count = 0; header[word_count] != ':'; name_word_count++) {
      field_name_buffer[name_word_count] = header[word_count];
      word_count++;
    }

    if (header[word_count] == ':') {
      field_name_buffer[name_word_count] = '\0';
      strcpy(curr_field_name, field_name_buffer);
      recording_value = true;
      word_count++;
    }

    if (recording_value == true) {
      int value_word_count;
      for (value_word_count = 0;
           !(header[word_count] == '\r' && header[word_count + 1] == '\n');
           value_word_count++) {
        field_value_buffer[value_word_count] = header[word_count];
        word_count++;
      }

      field_value_buffer[value_word_count] = '\0';
      word_count += 2;
    }
    // int index = hash_key(field_name_buffer);
    /* if (index == -1) {
      break;
    } */
    fill_headers(headers, field_name_buffer, field_value_buffer);

    memset(field_name_buffer, 0, sizeof(field_name_buffer));
    memset(field_value_buffer, 0, sizeof(field_value_buffer));
  }
}

/* Checks if request is complete, if yes returns 0 else -1 */
int is_complete_request(const char *buffer, int *one_request_len) {
  int word_count = 0;
  // printf("%p", one_request_len);
  for (word_count = 0; buffer[word_count] != '\0'; word_count++) {
    if (buffer[word_count - 3] == '\r' && buffer[word_count - 2] == '\n' &&
        buffer[word_count - 1] == '\r' && buffer[word_count] == '\n') {
      *one_request_len = word_count + 1;
      return 0;
    }
  }
  /* for (int i = 3; buffer[i] != '\0'; i++) {
    if (buffer[i - 3] == '\r' && buffer[i - 2] == '\n' &&
        buffer[i - 1] == '\r' && buffer[i] == '\n') {

      *one_request_len = i + 1;
      return 0;
    }
  } */
  return -1;
}

void request_parser(const char *restrict req_get, Irequest_line *request_line,
                    request_headers *headers, char body[2048]) {

  int crlf = 0;
  char headers_buffer[2048];
  char body_buffer[2048];
  int buffer_count = 0;
  int body_word_count = 0;
  int word_count = 0;

  for (word_count = 0;
       !(req_get[word_count - 3] == '\r' && req_get[word_count - 2] == '\n' &&
         req_get[word_count - 1] == '\r' && req_get[word_count] == '\n');
       word_count++) {

    if (crlf == 0 &&
        (req_get[word_count] == '\r' && req_get[word_count + 1] == '\n')) {
      headers_buffer[buffer_count] = '\0';
      request_line_parser(headers_buffer, request_line);
      buffer_count = 0;
      memset(headers_buffer, 0, sizeof(headers_buffer));
      crlf++;
      word_count++;
      continue;
    }

    headers_buffer[buffer_count] = req_get[word_count];
    buffer_count++;
  }

  headers_buffer[buffer_count] = '\n';
  headers_buffer[buffer_count + 1] = '\0';
  request_headers_parser(headers_buffer, headers);

  // Parse Body
  if (headers->Content_Length > 0) {
    word_count++;
    for (body_word_count = 0; req_get[word_count] != '\0'; body_word_count++) {
      body_buffer[body_word_count] = req_get[word_count];
      word_count++;
    }
    body_buffer[body_word_count] = '\0';
    stpncpy(body, body_buffer, strlen(body_buffer));
  }
}
