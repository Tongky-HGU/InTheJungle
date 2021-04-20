#include "csapp.h"

int main(int argc, char **argv){
	int clientfd;
	char *host, *port, buf[MAXLINE];
	rio_t rio;

	if (argc != 3){ // 호스트와 포트를 제대로 입력 안했을 시 예외처리
		fprintf(stderr, "usage: %s <hos> <port>\n", argv[0]);
		exit(0);
	}
	host = argv[1]; // 호스트 주소
	port = argv[2]; // 포트 주소

	clientfd = Open_clientfd(host,port); // client file descriptor를 만든다.
	Rio_readinitb(&rio, clientfd); // 할당된 file descriptor를 파일 i/out 목적으로 설정

	while (Fgets(buf, MAXLINE, stdin) != NULL) {  // 키보드 입력을 받아서 버퍼에 저장
		Rio_writen(clientfd,buf,strlen(buf)); // 입력받은 버퍼를 rio에 입력
		Rio_readlineb(&rio, buf, MAXLINE);     // 버퍼에 rio 출력받은 문자열 저장
		Fputs(buf,stdout);   		       // 버퍼 문자열을 화면에 출력
	}
	Close(clientfd); // client file descriptor 종료
	exit(0);
}

