#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>

#define BUFFER_SIZE 65536

bool is_foreground() {
    return getpgrp() == tcgetpgrp(STDOUT_FILENO);
}

int main(void) {
    char buffer[BUFFER_SIZE];
    char ch;

    size_t start = 0;
    size_t end = 0;

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

