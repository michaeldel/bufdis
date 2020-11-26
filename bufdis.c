#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define BUFFER_SIZE 65536
#define FIFO_MAX_NAME_LEN 32

bool is_foreground() {
    return getpgrp() == tcgetpgrp(STDOUT_FILENO);
}

void store_buffer(const char * buffer, size_t size, ptrdiff_t start) {
    assert(start < size);

    char * path = mkstemp("bufdis.XXXXXX"); /* TODO: check error */
    int err = mkfifo(path, 0600);

    if (err == -1) {
        perror("Cannot create FIFO");
        exit(1);
    }

    int fd = open(path, O_WRONLY);

    if (fd == -1) {
        perror("Cannot open FIFO");
        exit(1);
    }
    free(path);

    ssize_t written = write(fd, &buffer[start], size - start);

    if (written == -1) {
        perror("Cannot write to FIFO");
        exit(1);
    }
    assert(written == size - start);

    written = write(fd, buffer, start);

    if (written == -1) {
        perror("Cannot write to FIFO");
        exit(1);
    }
    assert(written == start);

    err = close(fd);

    if (err == -1) {
        perror("Cannot close FIFO");
        exit(1);
    }
}


int main(void) {
    char buffer[BUFFER_SIZE];
    char ch;

    ptrdiff_t start = 0;
    ptrdiff_t end = 0;

    while (read(STDIN_FILENO, &ch, 1)) {
        buffer[end] = ch;
        end = (end + 1) % BUFFER_SIZE;

        if (end == start) /* buffer full */
            exit(1); /* TODO: handle correctly */

        while (is_foreground() && start != end) {
            write(STDOUT_FILENO, &buffer[start], 1);
            start = (start + 1) % BUFFER_SIZE;
        }
    }

    return EXIT_SUCCESS;
}

