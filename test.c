#include <stdio.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <limits.h>

int main(void)
{
    int tube[2];
    char c = 'c';
    int i;

    fprintf(stdout, "Tube Creation\n");
    fprintf(stdout, "Theoretical max size: %d\n", PIPE_BUF);
    if( pipe(tube) != 0)
    {
        perror("pipe");
        _exit(1);
    }
    int fd = open("outfile", O_WRONLY | O_CREAT | O_TRUNC, 0777);
    fprintf(stdout, "starting child proc\n");
    if (fork() == 0)
    {
        char r;
        int bytes = read(tube[0], &r, 1);
        while (bytes > 0)
        {
            write(fd, &r, 1);
            bytes = read(tube[0], &r, 1);
        }
        if (bytes == 0)
            fprintf(stdout, "eof reached\n");
        else
            perror("Read");
        close(fd);
        _exit(1);
    }
    else 
    {
        fprintf(stdout, "Writing in pipe\n");
        for(i=0; i < 1000000; i++)
        {
            fprintf(stdout, "%d bytes written\n", i+1);
            if( write(tube[1], &c, 1) != 1)
            {
                perror("Write");
                _exit(1);
            }
        }
        close(tube[1]);
    }
    close(fd);
    return 0;
}
