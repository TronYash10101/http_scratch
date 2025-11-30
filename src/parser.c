#include "headers/parser.h"
#include "utils/headers/leading_whitespace.h"

void request_line_parser(const char *restrict request_line_ptr,
                         Irequest_line *request_line) {

  sscanf(request_line_ptr, "%9s %1023s %49s", request_line->method,
         request_line->request_target, request_line->http_version);
  return;
}

void response_line_parser(const char *restrict http_version, int status_code,
                          const char *restrict reason_pharse,
                          Istatus_line *status_line) {
  if (strlen(http_version) > 50 || strlen(reason_pharse) > 1024) {
    perror("Size mismatch");
  }
  memcpy(status_line->http_version, http_version, strlen(http_version));
  status_line->status_code = status_code;
  memcpy(status_line->reason_phrase, reason_pharse, strlen(reason_pharse));
  return;
}

int hash_key(char *field_name) {
  char word_buffer[1024];
  int word_count;

  for (word_count = 0; field_name[word_count] != '\0'; word_count++) {
    char lowered_char = tolower(field_name[word_count]);
    word_buffer[word_count] = lowered_char;
  }
  word_buffer[word_count] = '\0';

  if (strcmp(word_buffer, "host") == 0) {
    return 0;
  } else if (strcmp(word_buffer, "content-length") == 0) {
    return 1;
  } else if (strcmp(word_buffer, "content-type") == 0) {
    return 2;
  } else if (strcmp(word_buffer, "transfer-encoding") == 0) {
    return 3;
  } else {
    return -1;
  }
}

void request_headers_parser(const char *restrict header,
                            field_values *headers) {

  int word_count = 0;

  while (1) {
    char field_name_buffer[500];
    char field_value_buffer[500];
    bool recording_value = false;

    if (header[word_count] == '\0' || header[word_count + 1] == '\0') {
      printf("Malformed header: missing CRLFCRLF\n");
      abort();
      return;
    }

    if (header[word_count] == '\r' && header[word_count + 1] == '\n') {
      // This function is used to only parse headers nothing else so it should
      // end with \r\n\r\n, NULL terminator is after request body
      break;
    }

    int name_word_count;
    for (name_word_count = 0; header[word_count] != ':'; name_word_count++) {
      field_name_buffer[name_word_count] = header[word_count];
      word_count++;
    }

    if (header[word_count] == ':') {
      field_name_buffer[name_word_count] = '\0';
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
    int index = hash_key(field_name_buffer);
    strcpy(headers->field_value, leading_whitespace(field_value_buffer));
    memset(field_name_buffer, 0, sizeof(field_name_buffer));
    memset(field_value_buffer, 0, sizeof(field_value_buffer));
  }
}

void request_parser(const char *restrict req_get, Irequest_line *request_line,
                    Istatus_line *status_line, field_values *headers) {

  int crlf = 0;
  char buffer[2048];
  int buffer_count = 0;
  int word_count = 0;

  for (word_count = 0;
       !(req_get[word_count - 3] == '\r' && req_get[word_count - 2] == '\n' &&
         req_get[word_count - 1] == '\r' && req_get[word_count] == '\n');
       word_count++) {

    int condition =
        (req_get[word_count - 3] == '\r' && req_get[word_count - 2] == '\n' &&
         req_get[word_count - 1] == '\r' && req_get[word_count] == '\n');

    if (crlf == 0 &&
        (req_get[word_count] == '\r' && req_get[word_count + 1] == '\n')) {
      buffer[buffer_count] = '\0';
      request_line_parser(buffer, request_line);
      buffer_count = 0;
      memset(buffer, 0, sizeof(buffer));
      crlf++;
      word_count++;
      continue;
    }

    buffer[buffer_count] = req_get[word_count];
    buffer_count++;
  }
  buffer[buffer_count] = '\n';
  request_headers_parser(buffer, headers);
}
