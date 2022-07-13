#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

int a = 0;
pthread_mutex_t mutex;

void* thread1(void* arg)
{
  printf("arg: %d\n", (int)arg);

  while (1)
  {
    // mutex를 통해 크리티컬 영역 문제를 해결함
    pthread_mutex_lock(&mutex);
    printf("thread%d: a[%d]\n", (int)arg, ++a);
    pthread_mutex_unlock(&mutex);
    sleep(2);
  }

  return NULL;
}

int main()
{
  pthread_t s_thread[2];
  int id1 = 77;
  int id2 = 88;

  pthread_mutex_init(&mutex, NULL); // mutex, option

  // 스레드 생성
  pthread_create(&s_thread[0], NULL, thread1, (void*)id1);
    // 인자 1 - 스레드 아이디, 운영체자가 다루는 운전대 같은 것
    // 인자 2 - 스레드 옵션
    // 인자 3 - 스레드 함수
    // 인자 4 - 아이디
  pthread_create(&s_thread[1], NULL, thread1, (void*)id2);

  // 조인
  pthread_join(s_thread[0], NULL);
  pthread_join(s_thread[1], NULL);
    // 조인 하지 않으면 main이 끝날 때 프로그램이 꺼짐.
}
