// cr: compile and run
//
// Creates a named pipe and waits for a ping. Once pinged, it runs
// the command given.
//
// If no command is supplied, it pings the running instance.
// Handy for quick compile + run keybindings, or anything repetetive.
//
// Not sure if there's already an existing program for this purpose, but hey
// it's only 50 lines.

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    char* fifo_path = "/tmp/crfifo";
    char buf[1];
    int fd;

    // If arguments given, start a new process.
    if (argc == 2) {
        printf("Command: %s\n", argv[1]);

        /* puts("Creating cr pipe"); */
        mkfifo(fifo_path, 0666);

        while(1) {
            fd = open(fifo_path, O_RDONLY);

            if (fd == -1) {
                printf("%s\n", strerror(errno));
                exit(EXIT_FAILURE);
            }

            /* puts("Reading cr pipe"); */
            read(fd, buf, 1);
            system(argv[1]);
        }

        close(fd);

    } else { // No arguments given, just ping process.
        fd = open(fifo_path, O_WRONLY);

        if (fd == -1) {
            printf("%s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        /* puts("Writing to cr pipe"); */
        buf[0] = 'b';
        write(fd, buf, 1);
        close(fd);
    }

    return 0;
}
