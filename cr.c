// cr: compile and run
//
// Creates a named pipe and waits for a ping. Once pinged, it runs
// the command given.
//
// If no command is supplied, it pings the running instance.
// Handy for quick compile + run keybindings, or anything repetitive.
//
// Notes: Handles at most 26 processes. One for each letter id.
//
// Examples:
// cr r 'make && ./run'
// cr t 'make tests'

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    char buf[1];
    int fd;

    if (argc < 2 || argc > 3) {
        fprintf(stderr, "Usage: cr <id> <cmd>\n");
        exit(EXIT_FAILURE);
    }

    char id = tolower(argv[1][0]);
    if (!isalpha(id)) {
        fprintf(stderr, "Bad id %c given: id must be alphabetical.\n", id);
        exit(EXIT_FAILURE);
    }

    char* fifo_path;
    sprintf(fifo_path, "/tmp/crfifo_%c", id);

    // If a command argument is given, start a new process.
    if (argc == 3) {
        char* cmd = argv[2];

        printf("Running command (on %c): %s\n", id, cmd);
        mkfifo(fifo_path, 0666);

        while(1) {
            fd = open(fifo_path, O_RDONLY);

            if (fd == -1) {
                fprintf(stderr, "%s\n", strerror(errno));
                exit(EXIT_FAILURE);
            }

            read(fd, buf, 1);
            system(cmd);
        }

        close(fd);

    } else { // No command argument given, just ping process <id>.
        fd = open(fifo_path, O_WRONLY);

        if (fd == -1) {
            fprintf(stderr, "%s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        buf[0] = id;
        write(fd, buf, 1);
        close(fd);
    }

    return 0;
}
