#define _CRT_SECURE_NO_WARNINGS
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <errno.h>
#include <getopt.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <bits/waitflags.h>

struct Server {
  char ip[255];
  int port;
};


uint64_t MultModulo(uint64_t a, uint64_t b, uint64_t mod) {
  uint64_t result = 0;
  a = a % mod;
  while (b > 0) {
    if (b % 2 == 1)
      result = (result + a) % mod;
    a = (a * 2) % mod;
    b /= 2;
  }

  return result % mod;
}

bool ConvertStringToUI64(const char *str, uint64_t *val) {
  char *end = NULL;
  unsigned long long i = strtoull(str, &end, 10);
  if (errno == ERANGE) {
    fprintf(stderr, "Out of uint64_t range: %s\n", str);
    return false;
  }

  if (errno != 0)
    return false;

  *val = i;
  return true;
}


int main(int argc, char **argv) {
  uint64_t k = -1;
  uint64_t mod = -1;
  char servers[255] = {'\0'}; // TODO: explain why 255

  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"k", required_argument, 0, 0},
                                      {"mod", required_argument, 0, 0},
                                      {"servers", required_argument, 0, 0},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "", options, &option_index);

    if (c == -1)
      break;

    switch (c) {
    case 0: {
      switch (option_index) {
      case 0:
        if (ConvertStringToUI64(optarg, &k)  <= 0) {
          printf("Введите положительный k\n");
           return 1;
        }
        break;
      case 1:
        if (!ConvertStringToUI64(optarg, &mod)) {
          printf("Введите правильный mod\n");
          return 1;
        }
        break;
      case 2:
        memcpy(servers, optarg, strlen(optarg));
        servers[sizeof(servers) - 1] = '\0'; // Гарантируем завершающий нуль.
        break;
      default:
        printf("Index %d is out of options\n", option_index);
      }
    } break;

    case '?':
      printf("Arguments error\n");
      break;
    default:
      fprintf(stderr, "getopt returned character code 0%o?\n", c);
    }
  }

  if (k == -1 || mod == -1 || !strlen(servers)) {
    fprintf(stderr, "Using: %s --k 1000 --mod 5 --servers /path/to/file\n",
            argv[0]);
    return 1;
  }

  

  
  
  FILE *file = fopen(servers, "r");
  unsigned int servers_num = 0;
  char temp[100];
  while (fgets(temp, sizeof(temp), file)) {
        servers_num++;
  }
  struct Server *to = malloc(sizeof(struct Server) * servers_num);
  rewind(file);

  printf("servers:\n");

  char *token; // штука для разделения строки
  for (int i = 0; i < servers_num; i++){
    fgets(temp, sizeof(temp), file);


    token = strtok(temp, ":"); // Первый вызов для получения первого токена

    strcpy(to[i].ip, token);
    token = strtok(NULL, ":"); // Последующие вызовы для получения следующих токенов
    to[i].port = atoi(token);
    printf("Сервер %d: IP=%s, Port=%d\n", i + 1, to[i].ip, to[i].port);


  }

  fclose(file);






// TODO: work continiously, rewrite to make parallel
  pid_t *pids = malloc(sizeof(pid_t) * servers_num);
  int *pipes = malloc(sizeof(int) * 2 * servers_num); // pipe для каждого процесса

  // Создаем pipe для каждого будущего процесса
  for (int i = 0; i < servers_num; i++) {
      if (pipe(pipes + i * 2) < 0) {
          fprintf(stderr, "Pipe creation failed!\n");
          exit(1);
      }
  }

  for (int i = 0; i < servers_num; i++) {
      pids[i] = fork();
      
      if (pids[i] < 0) {
          fprintf(stderr, "Fork failed!\n");
          exit(1);
      }
      
      if (pids[i] == 0) {
          // Дочерний процесс
          close(pipes[i * 2]); // Закрываем read end
          
          struct hostent *hostname = gethostbyname(to[i].ip);
          if (hostname == NULL) {
              fprintf(stderr, "gethostbyname failed with %s\n", to[i].ip);
              exit(1);
          }

          struct sockaddr_in server;
          server.sin_family = AF_INET;
          server.sin_port = htons(to[i].port);
          server.sin_addr.s_addr = *((unsigned long *)hostname->h_addr_list[0]);

          int sck = socket(AF_INET, SOCK_STREAM, 0);
          if (sck < 0) {
              fprintf(stderr, "Socket creation failed!\n");
              exit(1);
          }

          if (connect(sck, (struct sockaddr *)&server, sizeof(server)) < 0) {
              fprintf(stderr, "Connection failed\n");
              exit(1);
          }

          // Распределяем работу между серверами
          uint64_t part_size = k / servers_num;
          uint64_t begin = i * part_size + 1;
          uint64_t end = (i == servers_num - 1) ? k : (i + 1) * part_size;

          char task[sizeof(uint64_t) * 3];
          memcpy(task, &begin, sizeof(uint64_t));
          memcpy(task + sizeof(uint64_t), &end, sizeof(uint64_t));
          memcpy(task + 2 * sizeof(uint64_t), &mod, sizeof(uint64_t));

          if (send(sck, task, sizeof(task), 0) < 0) {
              fprintf(stderr, "Send failed\n");
              exit(1);
          }

          char response[sizeof(uint64_t)];
          if (recv(sck, response, sizeof(response), 0) < 0) {
              fprintf(stderr, "Recieve failed\n");
              exit(1);
          }

          // Отправляем результат через pipe родителю
          if (write(pipes[i * 2 + 1], response, sizeof(response)) < 0) {
              fprintf(stderr, "Write to pipe failed\n");
              exit(1);
          }

          close(pipes[i * 2 + 1]);
          close(sck);
          exit(0);
      } else {
          // Родительский процесс
          close(pipes[i * 2 + 1]); // Закрываем write end
      }
  }

  uint64_t total_result = 1;
  int completed_processes = 0;
  
  printf("\nWaiting for results from all servers...\n");
  
  // Ожидаем завершения ВСЕХ процессов и собираем результаты
  while (completed_processes < servers_num) {
      // Используем waitpid с WNOHANG чтобы не блокироваться на одном процессе
      int status;
      pid_t finished_pid = waitpid(-1, &status, WNOHANG);
      
      if (finished_pid > 0) {
          // Находим какой процесс завершился
          for (int i = 0; i < servers_num; i++) {
              if (pids[i] == finished_pid) {
                  if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
                      char response[sizeof(uint64_t)];
                      if (read(pipes[i * 2], response, sizeof(response)) > 0) {
                          uint64_t partial_result;
                          memcpy(&partial_result, response, sizeof(uint64_t));
                          total_result = MultModulo(total_result, partial_result, mod);
                          printf("✓ Received result from server %d (pid %d): %lu\n", 
                                 i + 1, finished_pid, partial_result);
                      }
                  } else {
                      fprintf(stderr, "✗ Child process %d finished with error\n", finished_pid);
                  }
                  close(pipes[i * 2]);
                  completed_processes++;
                  break;
              }
          }
      } else if (finished_pid == 0) {
          // Нет завершенных процессов в данный момент - небольшая пауза
          usleep(10000); // 10ms
      } else {
          // Ошибка waitpid
          perror("waitpid");
          break;
      }
  }

  printf("\n=== FINAL RESULT: %lu ===\n", total_result);

  free(pids);
  free(pipes);
  free(to);

  return 0;
}
