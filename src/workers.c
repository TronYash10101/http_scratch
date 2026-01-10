#include "headers/workers.h"
#include "headers/job_queue.h"
// #include <cstdio>
#include <pthread.h>

void *static_serve(void *arg) {

    job_queue_t *queue = arg;

    while (1) {
        job_t *job = job_queue_pop(queue);

        if (job == POISON_PILL) {
            break;
        }

        int file_socket = open(job->file_path_buffer, O_RDONLY);

        if (file_socket == -1) {
            job->status_line->status_code = 500;
            LOG_ERROR("Could not open file");
            return ERROR;
        }
        int bytes_read = 0;
        char response_buffer[RESPONSE_BUFFER_SIZE];
        job->socket_descriptor_properties->is_busy = true;
        while ((bytes_read = read(file_socket, response_buffer,
                                  sizeof(response_buffer))) > 0) {
            int bytes_send = 0;
            while (bytes_send < bytes_read) {
                int n = send(job->socket_descriptor_properties->fd,
                             response_buffer + bytes_send,
                             bytes_read - bytes_send, 0);

                if (n <= 0) {

                    close(file_socket);
                    return ERROR;
                }
                bytes_send += n;
            }
        }
        job->socket_descriptor_properties->is_busy = false;
        free(job);
    }

    return NULL;
}
