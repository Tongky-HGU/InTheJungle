/*
2020 SW JG
web server study
searching host information
*/
#include "csapp.h"

int main(int argc, char** argv)
{
	struct addrinfo* p, * listp, hints;
	char buf[MAXLINE];
	int rc, flags;

	if (argc != 2)														// 입력 주소가 한 개 이상이면 에러
	{
		fprintf(stderr, "usage: %s <domain name>\n", argv[0]);
		exit(0);
	}

	memset(&hints, 0, sizeof(struct addrinfo));							// 힌트들을 담기 위한 
	hints.ai_family = AF_INET;											// IPv4 only
	hints.ai_socktype = SOCK_STREAM;									// Connections only
	if ((rc = getaddrinfo(argv[1], NULL, &hints, &listp)) != 0)         // 주소 방문해보고 에러 예외처리
	{
		fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(rc));
		exit(1);
	}

	/* Walk the list and display each IP address */
	flags = NI_NUMERICHOST;												 // 주소값으로 표기
	for (p = listp; p; p = p->ai_next)                                   // 모든 주소 연결리스트를 다 방문
	{
		Getnameinfo(p->ai_addr, p->ai_addrlen, buf, MAXLINE, NULL, 0, flags); 
		printf("%s\n", buf);
	}

	/* Clean up */
	Freeaddrinfo(listp);												 // 메모리 정리

	exit(0);
}
