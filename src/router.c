#include "headers/router.h"
#include <string.h>
#include <strings.h>

const char home_path[] =
    "/home/yash-jadhav/http_scratch/src/static_folder/index.html";

const char about_path[] =
    "/home/yash-jadhav/http_scratch/src/static_folder/about.html";

const char big_path[] =
    "/home/yash-jadhav/http_scratch/src/static_folder/big.html";

const char new_resource_file_path[] =
    "/home/yash-jadhav/http_scratch/src/static_folder/new_resource.html";

/* char not_found_path[] =
    "/home/yash-jadhav/http_scratch/src/static_folder/not_found.html";

char malformed_path[] =
    "/home/yash-jadhav/http_scratch/src/static_folder/malformed.html";

char no_body_found_path[] =
    "/home/yash-jadhav/http_scratch/src/static_folder/no_body_found_path.html";
*/

route routes[] = {{"GET", "/", home_path},
                  {"GET", "/about", about_path},
                  {"GET", "/big", big_path},
                  {"POST", "/add_resource", new_resource_file_path}};

int router(const char *target_resource, const char *method,
           char *file_path_buffer) {

  const int MAX_ENDPOINTS = sizeof(routes) / sizeof(routes[0]);

  for (int i = 0; i < MAX_ENDPOINTS; i++) {
    if (strcmp(routes[i].name, target_resource) == 0 &&
        strncasecmp(method, routes[i].method, strlen(routes[i].method)) == 0) {
      snprintf(file_path_buffer, strlen(routes[i].file_path) + 1, "%s",
               routes[i].file_path);
      return 0;
    }
  }
  return -1;
}
