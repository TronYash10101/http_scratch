#include "sys_includes.h"
#include <stdbool.h>
#include <time.h>

#ifndef MESSAGE
#define MESSAGE

#define CLR_RESET "\033[0m"
#define CLR_RED "\033[31m"
#define CLR_GREEN "\033[32m"
#define CLR_YELLOW "\033[33m"
#define CLR_BLUE "\033[34m"
#define CLR_CYAN "\033[36m"
#define CLR_GRAY "\033[90m"

// Log levels
#define LOG_ERROR(fmt, ...)                                                    \
  fprintf(stderr, CLR_RED "[ERROR] " fmt CLR_RESET "\n", ##__VA_ARGS__)

#define LOG_WARN(fmt, ...)                                                     \
  fprintf(stderr, CLR_YELLOW "[WARN ] " fmt CLR_RESET "\n", ##__VA_ARGS__)

#define LOG_INFO(fmt, ...)                                                     \
  fprintf(stdout, CLR_BLUE "[INFO ] " fmt CLR_RESET "\n", ##__VA_ARGS__)

#define LOG_DEBUG(fmt, ...)                                                    \
  fprintf(stdout, CLR_GRAY "[DEBUG] " fmt CLR_RESET "\n", ##__VA_ARGS__)

#define MAX_FIELD_LINES 4

typedef struct {
  char Host[256];
  char Content_Length[32];
  char Content_Type[128];
  char User_Agent[512];
  char Accept[512];
  char Connection[32];
  char Accept_Encoding[128];
  char Accept_Language[128];
  char Upgrade[512];
  char Sec_WebSocket_Key[512];
  char Sec_WebSocket_Extensions[512];
} request_headers;

typedef struct {
  int Content_Length;
  char Content_Type[1024];
} response_headers;

typedef struct {
  char method[10];
  char request_target[1024];
  char http_version[500];
} Irequest_line;

typedef struct {
  char http_version[500];
  int status_code;
} Istatus_line;

typedef struct {
  char field_value[1024];
} field_values;

/* Struct for keep-alive

   @ handled_request max 10
   @ timeout 20s
*/
typedef enum {
  CONN_HTTP,
  CONN_WEBSOCKET,
} ConnectionState;

struct alive_struct {
  int fd;
  bool keep_alive;
  int handled_requests;
  time_t last_active;
  ConnectionState conn_state;
  char per_connection_buffer[8049];
  size_t per_connection_buffer_len;
};
#endif
