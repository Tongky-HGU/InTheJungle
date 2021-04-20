#include "csapp.h"

void doit(int fd);
void read_requesthdrs(rio_t* rp);
int parse_uri(char* uri, char* filename, char* cgiargs);
void serve_static(int fd, char* filename, int filesize, char* method);
void get_filetype(char* filename, char* filetype);
void serve_dynamic(int fd, char* filename, char* cgiargs, char* method);
void clienterror(int fd, char* cause, char* errnum, char* shortmsg, char* longmsg);
void echo_request(int connfd);


int main(int argc, char** argv)
{
	int listenfd, connfd;
	char hostname[MAXLINE], port[MAXLINE];
	socklen_t clientlen;
	struct sockaddr_storage clientaddr;

	if (argc != 2) {                                      // �Է� ���� ����
		fprintf(stderr, "usage: %s <port>\n", argv[0]); // ��Ʈ��ȣ�� �� �Է��϶�
		exit(1);
	}

	printf("Server opened in %s\n", argv[1]);


	listenfd = Open_listenfd(argv[1]); // ��Ʈ��ȣ�� listen fd �Ҵ�
	while (1) {
		clientlen = sizeof(clientaddr);
		connfd = Accept(listenfd, (SA*)& clientaddr, &clientlen); // �ݺ������� request�� �޴´�.
		Getnameinfo((SA*)& clientaddr, clientlen, hostname, MAXLINE, port, MAXLINE, 0); //client�� hostname�� port��ȣ�� �޴´�.
		printf("Aceepted connection from (%s, %s)\n", hostname, port);
		//echo_request(connfd); // echo �Լ� ����
		doit(connfd); // connfd ��ȭó������
		Close(connfd); // connfd ����
	}
}

void doit(int fd) {  // ���� requst ó���ϴ� �Լ�
	int is_static;
	struct stat sbuf;
	char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
	char filename[MAXLINE], cgiargs[MAXLINE];
	rio_t rio;

	Rio_readinitb(&rio, fd); // fd�� in/out���� init
	Rio_readlineb(&rio, buf, MAXLINE); // buffer�� �о����
	printf("Request headers:\n");
	printf("%s", buf);
	sscanf(buf, "%s %s %s", method, uri, version); // http�� method, uri, versio ���� �и����� �о����

	if (strcasecmp(method, "GET") != 0 && strcasecmp(method, "HEAD") != 0){ // Get�̳� HEAD ��ɸ� ����, �ƴϸ� ���� �޽���
		clienterror(fd, method, "501", "Not implemented", "Tiny does not implement this method");
		return;
	}
	read_requesthdrs(&rio); // ����� �о�ɴϴ�.

	is_static = parse_uri(uri, filename, cgiargs); // uri ���� static���� Ȯ��, cgiargs�� �޾ƿ�
	if (stat(filename, &sbuf) < 0) { // ���� �̸����� ã�ƺ��� sbuf�� ����
		clienterror(fd, filename, "404", "Not found", "Tiny could not find this file");
		return;
	}
	if (is_static) {  // static�� �� ó��
		if (!(S_ISREG(sbuf.st_mode)) || !(S_IRUSR & sbuf.st_mode)) {  // ������ �������� Ȯ��, ���� �� �ִ� �� Ȯ��
			clienterror(fd, filename, "403", "Forbidden", "Tiny could not read this file");
			return;
		}
		serve_static(fd, filename, sbuf.st_size, method); // ���ϸ� ������ ����
	}
	else { // dynamic �� �� ó��
		if (!(S_ISREG(sbuf.st_mode)) || !(S_IXUSR & sbuf.st_mode)) {  // ������ �������� Ȯ��, ������ �� �ִ� �� Ȯ��
			clienterror(fd, filename, "403", "Forbidden", "Tiny could not run the cgi file");
			return;
		}
		serve_dynamic(fd, filename, cgiargs, method); // ���ϸ� ������ ����
	}

}

void clienterror(int fd, char* cause, char* errnum, char* shortmsg, char* longmsg) { //���� �޽��� http�� ó���ϴ� �Լ�
	char buf[MAXLINE], body[MAXBUF];

	// http body �����
	sprintf(body, "<html><title>Tiny Error</title>");
	sprintf(body, "%s<body bgcolor=""ffffff"">\r\n", body);
	sprintf(body, "%s%s: %s\r\n", body, errnum, shortmsg);
	sprintf(body, "%s<p>%s: %s\r\n", body, longmsg, cause);
	sprintf(body, "%s<hr><em>The Tiny Web Server</em>\r\n", body);

	// response ������
	sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg); // http ����, ���� �ѹ�, short �޽���
	Rio_writen(fd, buf, strlen(buf));
	sprintf(buf, "Content_type: text/html\r\n"); // content type
	Rio_writen(fd, buf, strlen(buf));
	sprintf(buf, "Content-length: %d\r\n\r\n", (int)strlen(body)); // content length
	Rio_writen(fd, buf, strlen(buf));
	Rio_writen(fd, body, strlen(body)); // body
}

void read_requesthdrs(rio_t* rp) {  // tiny�� get method�� �ޱ� ������ ������� �ʿ䰡 �����ϴ�~
	char buf[MAXLINE];
	Rio_readlineb(rp, buf, MAXLINE); // �����о��
	while (strcmp(buf, "\r\n")) {    // ������ ������
		Rio_readlineb(rp, buf, MAXLINE);
		printf("%s", buf);
	}
	return;
}

int parse_uri(char* uri, char* filename, char* cgiargs) {
	char* ptr;

	if (!strstr(uri, "cgi-bin")) { // "cgi-bin"�� ���ٸ� static 
		strcpy(cgiargs, ""); //
		strcpy(filename, "."); //.
		strcat(filename, uri); //.(uri)
		if (uri[strlen(uri) - 1] == '/') // ������ uri�� / �̶��
			strcat(filename, "home.html"); // home��ȯ
		return 1;
	}
	else { //dynamic 
		ptr = index(uri, '?');  //? index ����
		if (ptr) {
			strcpy(cgiargs, ptr + 1); // ? cgiargs�� ����
			*ptr = '\0'; // ? �ڿ� ���̻� �� �е���
		}
		else
			strcpy(cgiargs, "");

		strcpy(filename, "."); //�����̸��տ� .
		strcat(filename, uri); //�����̸�
		return 0;
	}
}

void serve_static(int fd, char* filename, int filesize, char*method) {  // static response �� ���� ó�� �ϴ� �Լ�
	int srcfd;
	char* srcp, filetype[MAXLINE], buf[MAXBUF];

	get_filetype(filename, filetype); // filetype �޾ƿ���
	sprintf(buf, "HTTP/1.0 200 OK\r\n");  // response header
	sprintf(buf, "%sServer: Tiny Web Server\r\n", buf);
	sprintf(buf, "%sConnection: close\r\n", buf);
	sprintf(buf, "%sContent-length: %d\r\n", buf, filesize);
	sprintf(buf, "%sContent-type: %s\r\n\r\n", buf, filetype);
	Rio_writen(fd, buf, strlen(buf)); // ���� content�� ���ؼ� response head�� ����
	printf("Response headers:\n");
	printf("%s", buf);

	if (strcasecmp(method, "GET") == 0) { // get ��û�ÿ��� response
		srcfd = Open(filename, O_RDONLY, 0); // ������ ���ڴ�(read only)
		srcp = malloc(filesize);
		Rio_readn(srcfd, srcp, filesize); // ������ �о�ͼ� ���ۿ� �ӽ� ����
		Rio_writen(fd, srcp, filesize); // ���� response
		free(srcp);

		//srcfd = Open(filename, O_RDONLY, 0); // ������ ���ڴ�(read only)
		//srcp = Mmap(0, filesize, PROT_READ, MAP_PRIVATE, srcfd, 0); // ���� ī��                       
		//Close(srcfd);
		//Rio_writen(fd, srcp, filesize); // ���� response
		//Munmap(srcp, filesize); // ���� �� ���� ����
	}
}

void get_filetype(char* filename, char* filetype) { // filetype MIME type���� ��ȯ���ִ� �Լ� 
	if (strstr(filename, ".html"))
		strcpy(filetype, "text/html");
	else if (strstr(filename, ".gif"))
		strcpy(filetype, "image/gif");
	else if (strstr(filename, ".png"))
		strcpy(filetype, "image/png");
	else if (strstr(filename, ".jpg"))
		strcpy(filetype, "image/jpeg");
	else if (strstr(filename, ".mp4"))
		strcpy(filetype, "video/mp4");
	else if (strstr(filename, ".mpeg"))
		strcpy(filetype, "video/mpeg");
	else
		strcpy(filetype, "text.plain");
}

void serve_dynamic(int fd, char* filename, char* cgiargs, char* method) { // dynamic response �� ���� ó�� �ϴ� �Լ�
	char buf[MAXLINE], * emptylist[] = { NULL };

	sprintf(buf, "HTTP/1.0 200 OK \r\n"); // response header
	Rio_writen(fd, buf, strlen(buf));
	sprintf(buf, "Server: Tiny Web Server\r\n");
	Rio_writen(fd, buf, strlen(buf));

	if (strcasecmp(method, "GET") == 0) { // Get ��û�ÿ��� response
		if (Fork() == 0) { // child ��������Ǵ� ���μ����� ���纻�������.
			 // ���� ������ ���⼭ ��� cgi vars�� �����Ѵ�.
			setenv("QUERY_STRING", cgiargs, 1); //file
			Dup2(fd, STDOUT_FILENO); // ���ϰ�� ���߱�
			Execve(filename, emptylist, environ); // cgi ����
		}
		Wait(NULL); // child�� ������ ��ٸ���
	}
}

void echo_request(int connfd) { // ���� requeset�� echo�� ȭ�鿡 ǥ�� ��Ű���Լ�
	size_t n;
	char buf[MAXLINE];
	rio_t rio;

	Rio_readinitb(&rio, connfd); // connect file discription �� rio in/out���� �Ҵ�
	printf("Received Request & Header----------------\n");
	while ((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0) {  // �Է¹��� ����Ʈ�� 0�� �ƴ϶�� (�ٵ� �����ų���� ����)
		Rio_writen(connfd, buf, n); // in �� buf ������ �ٽ� out
		printf("%s",buf);
		if (buf[0] == '0') {
			printf("received 0\n");
			break;
		}
	}
	printf("-----------------------------------------\n");

}