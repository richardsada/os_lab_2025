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


int main(int argc, char **argv) {
    int pnum = 2;


  pid_t *child_pids = malloc(sizeof(pid_t) * pnum);


  
  

  for (int i = 0; i < pnum; i++) {
    pid_t child_pid = fork();
    if (child_pid >= 0) {
      // successful fork
      child_pids[i] = child_pid;

      if (child_pid == 0) {
        return 0;
      }

    } else {
      printf("Fork failed!\n");
      return 1;
    }
  }


    // Ожидаем завершения процессов
    int completed = 0;
    for (int i = 0; i < pnum; i++) {
        //pid_t result = waitpid(child_pids[i], &status, 0);
        
        
    }

    sleep(5);
    return 0;
}
