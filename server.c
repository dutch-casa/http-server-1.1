#include <sys/socket.h>
#include <string.h>
#include <fcntl.h>
#include <sys/uio.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>

int main() {
    int s = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(36895),
        .sin_addr.s_addr = INADDR_ANY
    };

    bind(s, (struct sockaddr *)&addr, sizeof(addr));
    listen(s, 10);

    int client_fd = accept(s, NULL, NULL);

    char buffer[256] = {0};
    recv(client_fd, buffer, 256, 0);

    // Parse GET request: "GET /file.html"
    char *f = buffer + 5;
    *strchr(f, ' ') = 0;

    int opened_fd = open(f, O_RDONLY);
    if (opened_fd < 0) {
        perror("Error opening file");
        close(client_fd);
        close(s);
        return 1; // File not found or error opening
    }

    off_t offset = 0; // Start of file
    off_t len = 256;  // Number of bytes to send

    if (sendfile(client_fd, opened_fd, offset, &len, NULL, 0) == -1) {
        perror("Error sending file");
    }

    close(opened_fd);
    close(client_fd);
    close(s);

    return 0;
}
