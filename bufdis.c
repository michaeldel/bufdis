#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#define BUFFER_SIZE 65536

bool is_foreground() {
    return getpgrp() == tcgetpgrp(STDOUT_FILENO);
}

const char * temp_directory_path() {
    /* ISO/IEC 9945 (POSIX) spec */
    const char * path;

    if ((path = getenv("TMPDIR"))) return path;
    if ((path = getenv("TMP"))) return path;
    if ((path = getenv("TEMP"))) return path;
    if ((path = getenv("TEMPDIR"))) return path;

    return "/tmp";
}

char * fifo_path() {
    const pid_t pid = getpid();
    char path[1024];

    const int written = snprintf(
        path, 1024, "%s/bufdis-%jd", temp_directory_path(), (uintmax_t)pid
    );
    if (written >= 1024) {
        fprintf(stderr, "Path to create FIFO too long\n");
        exit(1);
    }

    return strdup(path);
}

struct fifo_s {
    int r;
    int w;
};

struct fifo_s create_fifo(const char * path) {
    const int err = mkfifo(path, 0600);

    if (err == -1) {
        perror("Cannot create FIFO");
        exit(1);
    }
    assert(err == 0);

    struct fifo_s result;

    /* TODO: make multithread */
    result.r = open(path, O_RDONLY | O_NONBLOCK);
    result.w = open(path, O_WRONLY);

    if (result.r == -1) { perror("Cannot open FIFO in read mode"); exit(1); }
    if (result.w == -1) { perror("Cannot open FIFO in write mode"); exit(1); }

    return result;
}


int main(void) {
    char * path = fifo_path();
    const struct fifo_s fifo = create_fifo(path);
    char ch;

    /* TODO: handle error on read */
    while (read(STDIN_FILENO, &ch, 1)) {
        /* TODO: handle error on write */
        write(fifo.w, &ch, 1);

        if (is_foreground()) {
            char rch;
            int written;
            while (written = read(fifo.r, &rch, 1), written > 0)
                write(STDOUT_FILENO, &rch, 1);
        }
    }

    /* TODO: cleanup on interrupt */
    const int err = remove(path);
    if (err == -1) {
        perror("Cannot remove FIFO");
        exit(1);
    }

    free(path);

    return EXIT_SUCCESS;
}

