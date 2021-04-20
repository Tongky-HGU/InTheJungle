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

	if (argc != 2)														// �Է� �ּҰ� �� �� �̻��̸� ����
	{
		fprintf(stderr, "usage: %s <domain name>\n", argv[0]);
		exit(0);
	}

	memset(&hints, 0, sizeof(struct addrinfo));							// ��Ʈ���� ��� ���� 
	hints.ai_family = AF_INET;											// IPv4 only
	hints.ai_socktype = SOCK_STREAM;									// Connections only
	if ((rc = getaddrinfo(argv[1], NULL, &hints, &listp)) != 0)         // �ּ� �湮�غ��� ���� ����ó��
	{
		fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(rc));
		exit(1);
	}

	/* Walk the list and display each IP address */
	flags = NI_NUMERICHOST;												 // �ּҰ����� ǥ��
	for (p = listp; p; p = p->ai_next)                                   // ��� �ּ� ���Ḯ��Ʈ�� �� �湮
	{
		Getnameinfo(p->ai_addr, p->ai_addrlen, buf, MAXLINE, NULL, 0, flags); 
		printf("%s\n", buf);
	}

	/* Clean up */
	Freeaddrinfo(listp);												 // �޸� ����

	exit(0);
}
