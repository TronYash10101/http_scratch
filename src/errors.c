#include "headers/errors.h"
#include <stdio.h>
#include <string.h>

void error_page_path(const int code, char *file_path_buffer) {

  const char *error_page;

  switch (code) {
  case 400:
    error_page =
        "/home/yash-jadhav/http_scratch/src/static_folder/bad_request.html";
    break;
  case 404:
    error_page =
        "/home/yash-jadhav/http_scratch/src/static_folder/not_found.html";
    break;
  case 411:
    error_page =
        "/home/yash-jadhav/http_scratch/src/static_folder/body_not_found.html";
    break;
  case 500:
    error_page =
        "/home/yash-jadhav/http_scratch/src/static_folder/internal_error.html";
    break;
  default:
    error_page =
        "/home/yash-jadhav/http_scratch/src/static_folder/internal_error.html";
  }
  snprintf(file_path_buffer, strlen(error_page) + 1, "%s", error_page);
}
