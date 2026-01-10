#include "headers/router.h"
#include <stdio.h>
#include <string.h>

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

char extra_resource_path[2048] =
    "/home/yash-jadhav/http_scratch/src/static_folder";

route routes[] = {{"GET", "/", home_path},
                  {"GET", "/about", about_path},
                  {"GET", "/big", big_path},
                  {"GET", "/new_resource", new_resource_file_path},
                  {"PUT", "/add_resource", new_resource_file_path}};

int router(const char *target_resource, const char *method,
           char *file_path_buffer, int file_path_buffer_size,
           response_headers *headers) {

    char extra_resource_buffer[2048];

    const int MAX_ENDPOINTS = sizeof(routes) / sizeof(routes[0]);
    for (int i = 0; i < MAX_ENDPOINTS; i++) {
        if (strcmp(routes[i].name, target_resource) == 0 &&
            strncasecmp(method, routes[i].method, strlen(routes[i].method)) ==
                0) {
            strncpy(headers->Content_Type, "text/html",
                    sizeof(headers->Content_Type));
            snprintf(file_path_buffer, file_path_buffer_size, "%s",
                     routes[i].file_path);
            return 0;
        }
    }

    struct stat st;

    // strcat(extra_resource_buffer, target_resource);
    snprintf(extra_resource_buffer, sizeof(extra_resource_buffer), "%s%s",
             extra_resource_path, target_resource);

    if (strlen(extra_resource_buffer) > 2048) {
        LOG_INFO("403 Forbidden");
        return -1;
    }

    if (stat(extra_resource_buffer, &st) == 0) {
        char *file_ext = strrchr(target_resource, '.');
        char content_type[32];
        snprintf(content_type, sizeof(content_type), "text/%s", file_ext + 1);
        strncpy(headers->Content_Type, content_type,
                sizeof(headers->Content_Type));
        snprintf(file_path_buffer, file_path_buffer_size, "%s",
                 extra_resource_buffer);
        return 0;
    } else if (stat(extra_resource_buffer, &st) < 0) {
        return -1;
    }

    return -1;
}
