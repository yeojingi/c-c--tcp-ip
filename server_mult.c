#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
// socket에 필요한 헤더파일
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#define CLNT_MAX 10 // max amount of client sockets
#define BUFFSIZE 200

int g_clnt_socks[CLNT_MAX];
int g_clnt_count = 0;
pthread_mutex_t g_mutex;

void send_all_clnt(char* msg, int my_sock) {
  pthread_mutex_lock(&g_mutex);
  for (int i = 0; i < g_clnt_count; i++) {
    if (g_clnt_socks[i] != my_sock) {
      printf("write: %s\n", msg);
      write(g_clnt_socks[i], msg, strlen(msg)+1);
    }
  }
  pthread_mutex_unlock(&g_mutex);
}

void* clnt_connection (void* arg) {
  int clnt_sock = (int)arg;
  int str_len = 0;

  char msg[BUFFSIZE];
  int i;

  while (1) {
    str_len = read(clnt_sock, msg, sizeof(msg));

    if (str_len == -1) {
      printf("clnt[%d] closed\n", clnt_sock);
      break;
    }
    send_all_clnt(msg, clnt_sock);

    printf("%s\n", msg);
  }

  pthread_mutex_lock(&g_mutex);
  close(clnt_sock);

  for (int i = 0; i < g_clnt_count; i++) {
    if (clnt_sock == g_clnt_socks[i]) {
      g_clnt_socks[i] = g_clnt_socks[g_clnt_count-1];
      g_clnt_count--;
      break;
    }
  }
  pthread_mutex_unlock(&g_mutex);

  pthread_exit(0);

  return NULL;
}

int main(int argc, char** argv)
{
  //server
  int serv_sock; // socket of server
  struct sockaddr_in serv_addr;

  //client
  int clnt_sock;
  socklen_t clnt_addr_size;
  struct sockaddr_in clnt_addr;

  pthread_t t_thread;

  char buff[200];
  int recv_len = 0;

  pthread_mutex_init(&g_mutex, NULL);

  serv_sock = socket(PF_INET, SOCK_STREAM, 0); // make an empty socket
    
  serv_addr.sin_family = AF_INET; // AF_INET - address 정의할 때, PF_INET - protocol 정의할 때
  serv_addr.sin_addr. s_addr = htonl(INADDR_ANY);
  serv_addr.sin_port = htons(7989);
    //  htons: short(16bit)

  // bind
  if (bind(serv_sock, (const struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
    printf("bind error\n");
  };

  if (listen(serv_sock, 5) == -1) {
    printf("listen error");
  }

  while (1) {
    clnt_addr_size = sizeof(clnt_addr);
    clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_size);

    // 클라이언트 저장
    // critical 영역
    pthread_mutex_lock(&g_mutex);
    g_clnt_socks[g_clnt_count++] = clnt_sock;
    pthread_mutex_unlock(&g_mutex);

    pthread_create(&t_thread, NULL, clnt_connection, clnt_sock);
  }
}
