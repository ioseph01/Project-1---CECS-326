/**
 * filecopy.c
 * 
 * This program copies files using a pipe.
 *
 */

#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define READ_END 0
#define WRITE_END 1

int main(int argc, char *argv[]) {
	if (argc != 3) {
        fprintf(stderr, "Command format: %s <source_file> <destination_file>\n", argv[0]);
        return 1;
    }
    const char* source_file = argv[1];
    const char* destination_file = argv[2];
    // Open source file
    int source_fd = open(source_file, O_RDONLY);
    if (source_fd < 0) {
        fprintf(stderr, "Error: Unable to open source file '%s'\n", source_file);
        return 1;
    }
    // Open destination file
    int destination_fd = open(destination_file, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (destination_fd < 0) {
        close(source_fd);
        fprintf(stderr, "Error: Unable to open destination file '%s'\n", destination_file);
        return 1;
    }
    // Create 1-way pipe of read and write, and fork for a child process
    int pipe_fd[2];
    pipe(pipe_fd);
    int cid = fork();

    if (cid == 0) {                     // Child Process
        close(pipe_fd[0]);
        dup2(source_fd, STDIN_FILENO); 
        dup2(pipe_fd[1], STDOUT_FILENO); 
        close(pipe_fd[1]);
        close(source_fd);
        execlp("cat", "cat", NULL); 
        perror("execlp failed");
        return 1;
    }
    else {                              // Parent Process
        close(pipe_fd[1]);              
        dup2(pipe_fd[0], STDIN_FILENO); 
        dup2(destination_fd, STDOUT_FILENO); 
        close(pipe_fd[0]);
        close(destination_fd);
        wait(NULL);
        execlp("cat", "cat", NULL);  
        perror("execlp failed");
        return 1;

    }
    return 0;
}
