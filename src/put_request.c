#include "headers/put_request.h"
#include "headers/get_request.h"

// Create new file_path_buffer and serve it

void put_request(const Irequest_line *restrict request_line,
                 const request_headers *restrict request_headers,
                 char *file_path_buffer, char *response_header_buffer,
                 int response_header_buffer_size, int file_path_buffer_size,
                 Istatus_line *status_line, response_headers *response_header,
                 const char *body) {

    int req_target_word_count = 0;
    int buffer_word_count = 0;
    char target_buffer[1024];

    char to_write[2048];
    /* const char *new_resource_file_path =
        "/home/yash-jadhav/http_scratch/src/static_folder/new_resource.html"; */

    // Vulnerable to injection if injection user data in this using snprintf or
    // anything else
    char html_template[] =
        "<!DOCTYPE html>\n"
        "<html lang=\"en\">\n"
        "<head>\n"
        "  <meta charset=\"UTF-8\">\n"
        "  <meta name=\"viewport\" content=\"width=device-width, "
        "initial-scale=1.0\">\n"
        "  <meta http-equiv=\"X-UA-Compatible\" content=\"ie=edge\">\n"
        "  <title>Document</title>\n"
        "</head>\n"
        "<body>\n"
        "  <p>User Added Resource</p>\n"
        "  <p>%s</p>\n"
        "\n"
        "  <button onclick=\"sendPost()\">Send PUT</button>\n"
        "\n"
        "</body>\n"
        "</html>\n";

    if (request_line->request_target[0] != '/') {
        status_line->status_code = 400;
        error_page_path(status_line->status_code, file_path_buffer);
        respond(file_path_buffer, response_header_buffer,
                response_header_buffer_size, status_line, response_header);
        LOG_ERROR("Request header error");
        return;
    }

    if ((*request_headers).Content_Length <= 0) {

        status_line->status_code = 411;
        error_page_path(status_line->status_code, file_path_buffer);
        respond(file_path_buffer, response_header_buffer,
                response_header_buffer_size, status_line, response_header);
        LOG_WARN("No content found for PUT");
        return;
    }

    // Capturing target resource, maybe redundant
    for (req_target_word_count = 0;
         request_line->request_target[req_target_word_count] != '?' &&
         request_line->request_target[req_target_word_count] != '\0';
         req_target_word_count++) {

        target_buffer[buffer_word_count++] =
            request_line->request_target[req_target_word_count];
    }
    target_buffer[buffer_word_count] = '\0';

    // Check if file exists & method is correct
    int route_status =
        router(target_buffer, request_line->method, file_path_buffer,
               file_path_buffer_size, response_header);

    if (route_status != -1) {
        status_line->status_code = 200;

        int new_resource_fd =
            open(file_path_buffer, O_CREAT | O_WRONLY | O_TRUNC, 0644);

        if (new_resource_fd < 0) {
            status_line->status_code = 500;
            error_page_path(status_line->status_code, file_path_buffer);
            respond(file_path_buffer, response_header_buffer,
                    response_header_buffer_size, status_line, response_header);
            LOG_ERROR("PUT Socket Error");
            return;
        }

        int len = snprintf(to_write, sizeof(to_write), html_template, body);
        if (len < 0 || len >= sizeof(to_write)) {
            status_line->status_code = 413;
            error_page_path(status_line->status_code, file_path_buffer);
            respond(file_path_buffer, response_header_buffer,
                    response_header_buffer_size, status_line, response_header);
            LOG_WARN("PUT Content To Send Too Large");
            return;
        }

        int new_resource_len = strlen(to_write);
        int bytes_written = 0;

        while (bytes_written < new_resource_len) {
            ssize_t n = write(new_resource_fd, to_write + bytes_written,
                              new_resource_len - bytes_written);
            if (n <= 0) {
                close(new_resource_fd);
                status_line->status_code = 500;
                LOG_ERROR("PUT Internal Server Error");
                return;
            }
            bytes_written += n;
        }

        if (bytes_written == new_resource_len) {
            close(new_resource_fd);

            respond(file_path_buffer, response_header_buffer,
                    response_header_buffer_size, status_line, response_header);
            LOG_INFO("PUT Success");

            return;
        }
    } else {
        status_line->status_code = 404;
        error_page_path(status_line->status_code, file_path_buffer);
        respond(file_path_buffer, response_header_buffer,
                response_header_buffer_size, status_line, response_header);
        LOG_WARN("PUT Not Found");
        return;
    }
}
