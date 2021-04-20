#include  "csapp.h"

void echo(int connfd);

int main(int argc, char **argv){
	int listenfd, connfd; // listen, connect file description은 단순 번호
	socklen_t clientlen;
	struct sockaddr_storage clientaddr; // client 주소를 담는 구조체(모든 길이)
	char client_hostname[MAXLINE], client_port[MAXLINE];

	if (argc != 2){
		fprintf(stderr, "usage: %s <port>\n",argv[0]);
		exit(0);
	}

	listenfd = Open_listenfd(argv[1]); // port 번호에 따른 file descriptor 달아줌
	while(1){
		clientlen= sizeof(struct sockaddr_storage);
		connfd = Accept(listenfd, (SA*)&clientaddr, &clientlen); // SA = socket adress
		Getnameinfo((SA*) &clientaddr, clientlen, client_hostname, MAXLINE, client_port, MAXLINE, 0);
		printf("Connectd to (%s, %s)\n", client_hostname, client_port);
		echo(connfd); // echo 함수 실행
		Close(connfd); 
		printf("Connect Closed (%s, %s)\n", client_hostname, client_port);
	}
	exit(0);
}

void echo(int connfd){
	size_t n;
	char buf[MAXLINE];
	rio_t rio;

	Rio_readinitb(&rio, connfd); // connect file discription 을 rio in/out으로 할당
	while((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0){  // 입력받은 바이트가 0이 아니라면 (근데 종료시킬수가 없음)
		printf("server received %d bytes\n", (int)n); // 몇 바이트인지 출력
		Rio_writen(connfd,buf,n); // in 한 buf 내용을 다시 out
		if(buf[0]=='0'){
			printf("received 0\n");
			break;
		}
	}
}



