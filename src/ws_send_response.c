#include "headers/ws_send_response.h"

void ws_send_response(int fd) {
  const char *web_socket_res = "Hello Peer";
  size_t payload_len = strlen(web_socket_res);

  size_t header_len;
  if (payload_len <= 125) {
    header_len = 2;
  } else if (payload_len <= 65535) {
    header_len = 4;
  } else {
    header_len = 10;
  }

  uint8_t *frame = malloc(header_len + payload_len);
  if (!frame) {
    perror("malloc");
    return;
  }
  const uint8_t opcode = 0x1;
  int total_len =
      ws_build_frame(0x1, payload_len, (const uint8_t *)web_socket_res, frame);

  int total_sent = 0;
  while (total_sent < total_len) {
    int n = send(fd, frame + total_sent, total_len - total_sent, 0);
    if (n < 0) {
      perror("send");
      break;
    }
    total_sent += n;
  }

  free(frame);
}
