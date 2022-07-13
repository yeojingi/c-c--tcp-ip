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

int g_clnt_socks[CLNT_MAX];
int g_clnt_count = 0;

int main(int argc, char** argv)
{
  int serv_sock; // socket of server
  struct sockaddr_in serv_addr;

  int clnt_sock;
  socklen_t clnt_addr_size;
  struct sockaddr_in clnt_addr;

  char buff[200];
  int recv_len = 0;

  serv_sock = socket(PF_INET, SOCK_STREAM, 0); // make an empty socket
    // 인자 1 - 네트워크 주소 체계
    // PF_INET: 2 - IPv4.
    // PF_INET6: 23 - IPv6.
    // 인자 2 - 소켓 타입
    // SOCK_STREAM: 1 - TCP protocol
    // SOCK_DGRAM: 2 - UDP protol (Datagram)
    // SOCK_RAW: 3 - RAW socket. 가공하지 않은 소켓. 거의 안 씀
    // 인자 3 - 프로토콜
    // 0을 넣어주면 자동으로 첫번쨰, 두번째 매개변수를 기준으로 인자값을 지정해줍니다.
    // IPPROTO_TCP
    // IPPROTO_UDP
  serv_addr.sin_family = AF_INET; // AF_INET - address 정의할 때, PF_INET - protocol 정의할 때
    // IPv4 쓰겠다는 얘기
  serv_addr.sin_addr. s_addr = htonl(INADDR_ANY);
    // htonl: host to network long(32bit)
    //        host의 order 방식을 network order로 바꿔라
    // socket bind 할 때 어떤 주소로 bind를 할 지 설정
    // INADDR_ANY: 내 현재 PC의 IP
  serv_addr.sin_port = htons(7989);
    //  htons: short(16bit)

  // bind
  if (bind(serv_sock, (const struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
    printf("bind error\n");
  };

  // listen을 하면 accept 하는데, queue에 5개의 공간을 만든다.
  // 기다리는 고객이 5명을 초과하면 error가 난다.
  if (listen(serv_sock, 5) == -1) {
    printf("listen error");
  }

  // 서버에서는 소켓을 2개 사용하고 있음
  // 서버 소켓: 처음에 생성되고 listen. 클라이언트는 이 소켓을 통해 접속.
  // 클라이언트 소켓: 클라이언트 1명 당 1개씩 생성되는 소켓. 실제로 패킷을 주고 받는 소켓
  while (1) {
    clnt_addr_size = sizeof(clnt_addr);
    clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_size);

    // g_clnt_socks[g_clnt_count++] = clnt_sock;

    // Thread 생성
    // 또 다른 client를 받기 위해
    // 일단 이번 시간에는 쓰레드 없이
    // 고객 한 명 받고 listen만 가능
    while (1) {
      recv_len = read(clnt_sock, buff, sizeof(buff)/sizeof(char)); // 읽어서 buffer 에 저장, 읽은 길이 반환
      
      printf("recv: ");
      for (int i = 0; i < recv_len; i++) {
        printf("%02X ", (unsigned char)buff[i]);
      }
      printf("\n");
    }
  }
}
