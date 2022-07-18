#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#define BUFFSIZE 160
#define NAMESIZE 20

int StoHEX(char fi, char sc);
void error_handling(char *msg);
void* send_message(void* arg);
void* recv_message(void* arg);

char message[BUFFSIZE];

void* rcv(void* arg)
{
  printf("recv thread created\n");
  int sock = (int)arg;
  char buff[500];
  int len;

  while (1) {
    len = read(sock, buff, sizeof(buff));

    if (len == -1) {
      printf("sock close");
      break;
    }

    printf("%s\n", buff);
  }

  pthread_exit(0);
  return 0;
}

int main(int argc, char** argv) {
  int sock;
  struct sockaddr_in serv_addr;
  pthread_t snd_thread, rcv_thread;
  void* thread_result;

  char id[100];
  
  if (argc <= 1) {
    printf("You have to include ID\n");
    return 0;
  }

  strcpy(id, argv[1]);
  printf("id: %s\n", id);

  sock = socket(PF_INET, SOCK_STREAM, 0);
  if (sock == -1) {
    // error_handling("socket() error");
    printf("socket() error");
  }

  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  serv_addr.sin_port = htons(7989);

  if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
    // error_handling("connect() error");
    printf("connect() error");
  }

  char chat[1000];
  char msg[1000];

  pthread_create(&rcv_thread, NULL, rcv, (void *)sock);

  while (1) {
    printf("채팅 입력: ");
    gets(chat);

    sprintf(msg, "[%s]: %s", id, chat);

    printf("send: %s\n", msg);

    write(sock, msg, strlen(msg)+1); // +1은 \0을 보내기 위함
  }

  close(sock);
  return 0;
}