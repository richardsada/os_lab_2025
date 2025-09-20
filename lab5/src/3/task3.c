#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
void threadFunction()
{

    for (int i = 0; i < 3; i++)
        {
            pthread_mutex_lock(&mut);
             print_smth(i);
            pthread_mutex_unlock(&mut);
        }
}
 
void print_smth(int a)
{   
    for (int i = 0; i < 3; i++)
        {
            pthread_mutex_lock(&mut);
             printf("%d %d", a, i);
            pthread_mutex_unlock(&mut);
        }
}
int main()
{
    pthread_t thread1, thread2;

  pthread_create(&thread1, NULL, (void *)threadFunction, NULL);
  pthread_create(&thread2, NULL, (void *)threadFunction, NULL);

  if (pthread_join(thread1, NULL) != 0) {
    perror("pthread_join");
    exit(1);
  }

  if (pthread_join(thread2, NULL) != 0) {
    perror("pthread_join");
    exit(1);
  
     return 0;
}
}
