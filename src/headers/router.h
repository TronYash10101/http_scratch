#ifndef ROUTER
#define ROUTER
#include <stdio.h>
#include <string.h>

#include "errors.h"
#include "message.h"
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <unistd.h> // for POSIX systems

typedef struct {
  const char *method;
  const char *name;
  const char *file_path;
} route;

extern const char home_path[];
extern const char about_path[];
extern const char not_found_path[];
extern const char malformed_path[];
extern const char big_path[];

/* Returns 0 if path is found and -1 if not found */
int router(const char *target_resource, const char *method,
           char *file_path_buffer, int file_path_buffer_size,
           response_headers *headers);

#endif
