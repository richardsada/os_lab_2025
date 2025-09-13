#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <getopt.h>

#include "find_min_max.h"
#include "utils.h"

int main(int argc, char **argv) {
    pid_t pid = fork();

    if (pid == 0) {
        // Дочерний процесс
        char* args[] = {
            "parallel_min_max",    
            "--seed", "1",         
            "--array_size", "10",
            "--pnum", "3", 
            argv[1],          
            NULL
        };
        execv("./parallel_min_max", args);
        // Если exec вернул управление - ошибка!
        perror("exec failed");
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        // Родительский процесс
        wait(NULL); // Ждем завершения ребенка
    } else {
        // Ошибка fork
        perror("fork failed");
    }







}