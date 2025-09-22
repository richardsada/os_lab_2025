#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>
#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <getopt.h>



struct Args
{
    int begin;
    int end;
    int mod;
};

long common_res = 1;
pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;

void factorialing(const struct Args *data)
{
    int res = 1;
    for(int i = data->begin; i <= data->end; i++)
    {
        res *= i; 
    }
    pthread_mutex_lock(&mut);
    common_res = (common_res * res) % (data->mod);
    pthread_mutex_unlock(&mut);
    
}




int main(int argc, char **argv) {
 

  uint32_t pnum = 0;
  uint32_t mod = 0;
  uint32_t k = 0;



  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"mod", required_argument, 0, 0},
                                      {"k", required_argument, 0, 0},
                                      {"pnum", required_argument, 0, 0},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "f", options, &option_index);

    if (c == -1) break;

    switch (c) {
      case 0:
        switch (option_index) {
          case 0:
            mod = atoi(optarg);
            if (mod <= 0) {
                printf("mod must be a positive number\n");
                return 1;
            }
            break;
          case 1:
            k = atoi(optarg);
            if (k <= 0) {
                printf("k must be a positive number\n");
                return 1;
            }
            break;
          case 2:
            pnum = atoi(optarg);
            if (pnum <= 0) {
                printf("pnum must be a positive number\n");
                return 1;
            }
            break;
          defalut:
            printf("Index %d is out of options\n", option_index);
        }
        break;

      case '?':
        break;

      default:
        printf("getopt returned character code 0%o?\n", c);
    }
  }

  if (optind < argc) {
    printf("Has at least one no option argument\n");
    return 1;
  }

  if (mod == 0 || k == 0 || pnum == 0 ) {
    printf("Usage: %s --mod \"num\" --k \"num\" --pnum \"num\" \n",
           argv[0]);
    return 1;
  }

 
  pthread_t threads[pnum];



  struct timeval start_time;
  gettimeofday(&start_time, NULL);
  int part_size = k / pnum;

  struct Args args[pnum];


  for (uint32_t i = 0; i < pnum; i++) {
    
    args[i].begin = i * part_size + 1;
    args[i].end = ((i == pnum - 1) ? k : (i + 1) * part_size);
    args[i].mod = mod;
    
    printf("Thread %d: begin=%d, end=%d\n", i + 1, args[i].begin, args[i].end);
    
    if (pthread_create(&threads[i], NULL,(void *) factorialing, (void *)&args[i])) {
      printf("Error: pthread_create failed!\n");
      return 1;
    }
  }

  
  for (uint32_t i = 0; i < pnum; i++) {
    pthread_join(threads[i], NULL);
  }

  


  printf("Total: %ld\n", common_res);

  return 0;
}
