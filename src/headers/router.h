#ifndef ROUTER
#define ROUTER
#include <stdio.h>
#include <string.h>

#include <fcntl.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
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
           char *file_path_buffer);

#endif
