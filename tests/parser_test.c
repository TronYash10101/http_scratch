#include "../src/headers/message.h"
#include "../src/headers/parser.h"
#include "colors.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

void test_request_line() {
  Irequest_line request_line = {0};

  const char *good_request_line = "GET / HTTP/1.1\r\n";

  request_line_parser(good_request_line, &request_line);

  assert(strcmp(request_line.method, "GET") == 0);
  assert(strcmp(request_line.request_target, "/") == 0);
  assert(strcmp(request_line.http_version, "HTTP/1.1") == 0);

  PASS("REQUEST LINE");
}

void test_fill_headers() {

  const char *field_name = "Content-Length";
  const char *field_value = "900";

  request_headers header_struct = {0};

  fill_headers(&header_struct, field_name, field_value);

  assert(strcmp(header_struct.Content_Length, "900") == 0);
  PASS("FILL HEADER");
}
void test_is_complete_request_complete() {
  const char *req = "GET / HTTP/1.1\r\n"
                    "Host: localhost\r\n"
                    "\r\n";

  int len = 0;
  int ret = is_complete_request(req, &len);

  assert(ret == 0);
  assert(len == (int)strlen(req));

  PASS("IS COMPLETE REQUEST (COMPLETE)");
}

void test_is_complete_request_incomplete() {
  const char *req = "GET / HTTP/1.1\r\n"
                    "Host: localhost\r\n";

  int len = 0;
  int ret = is_complete_request(req, &len);

  assert(ret == -1);

  PASS("IS COMPLETE REQUEST (INCOMPLETE)");
}

void test_is_complete_request_short_buffer() {
  const char *req = "\r\n";

  int len = 0;
  int ret = is_complete_request(req, &len);

  assert(ret == -1);

  PASS("IS COMPLETE REQUEST (SHORT BUFFER)");
}
void test_request_headers_parser_single() {
  request_headers headers = {0};

  const char *hdrs = "Host: localhost\r\n"
                     "\r\n";

  request_headers_parser(hdrs, &headers);

  assert(strcmp(headers.Host, " localhost") == 0);

  PASS("REQUEST HEADERS PARSER (SINGLE)");
}

void test_request_headers_parser_multiple() {
  request_headers headers = {0};

  const char *hdrs = "Host: localhost\r\n"
                     "Content-Length: 123\r\n"
                     "User-Agent: test\r\n"
                     "\r\n";

  request_headers_parser(hdrs, &headers);

  assert(strcmp(headers.Host, " localhost") == 0);
  assert(strcmp(headers.Content_Length, " 123") == 0);
  assert(strcmp(headers.User_Agent, " test") == 0);

  PASS("REQUEST HEADERS PARSER (MULTIPLE)");
}
void test_request_parser_get() {
  const char *req = "GET / HTTP/1.1\r\n"
                    "Host: localhost\r\n"
                    "User-Agent: test\r\n"
                    "\r\n";

  Irequest_line request_line = {0};
  request_headers headers = {0};
  char body[2048] = {0};

  request_parser(req, &request_line, &headers, body);

  assert(strcmp(request_line.method, "GET") == 0);
  assert(strcmp(request_line.request_target, "/") == 0);
  assert(strcmp(request_line.http_version, "HTTP/1.1") == 0);

  assert(strcmp(headers.Host, " localhost") == 0);
  assert(strcmp(headers.User_Agent, " test") == 0);

  assert(body[0] == '\0');

  PASS("REQUEST PARSER (GET)");
}

int main() {
  test_request_line();
  test_fill_headers();

  test_is_complete_request_complete();
  test_is_complete_request_incomplete();
  test_is_complete_request_short_buffer();

  test_request_headers_parser_single();
  test_request_headers_parser_multiple();

  test_request_parser_get();
  return 0;
}
