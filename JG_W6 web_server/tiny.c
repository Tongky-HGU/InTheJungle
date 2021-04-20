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

	if (argc != 2) {                                      // 입력 인자 부족
		fprintf(stderr, "usage: %s <port>\n", argv[0]); // 포트번호를 잘 입력하라
		exit(1);
	}

	printf("Server opened in %s\n", argv[1]);


	listenfd = Open_listenfd(argv[1]); // 포트번호로 listen fd 할당
	while (1) {
		clientlen = sizeof(clientaddr);
		connfd = Accept(listenfd, (SA*)& clientaddr, &clientlen); // 반복적으로 request를 받는다.
		Getnameinfo((SA*)& clientaddr, clientlen, hostname, MAXLINE, port, MAXLINE, 0); //client의 hostname과 port번호를 받는다.
		printf("Aceepted connection from (%s, %s)\n", hostname, port);
		//echo_request(connfd); // echo 함수 실행
		doit(connfd); // connfd 교화처리과정
		Close(connfd); // connfd 종료
	}
}

void doit(int fd) {  // 받은 requst 처리하는 함수
	int is_static;
	struct stat sbuf;
	char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
	char filename[MAXLINE], cgiargs[MAXLINE];
	rio_t rio;

	Rio_readinitb(&rio, fd); // fd를 in/out으로 init
	Rio_readlineb(&rio, buf, MAXLINE); // buffer에 읽어오기
	printf("Request headers:\n");
	printf("%s", buf);
	sscanf(buf, "%s %s %s", method, uri, version); // http의 method, uri, versio 정보 분리시켜 읽어오기

	if (strcasecmp(method, "GET") != 0 && strcasecmp(method, "HEAD") != 0){ // Get이나 HEAD 명령만 받음, 아니면 에러 메시지
		clienterror(fd, method, "501", "Not implemented", "Tiny does not implement this method");
		return;
	}
	read_requesthdrs(&rio); // 헤더를 읽어옵니다.

	is_static = parse_uri(uri, filename, cgiargs); // uri 뜯어보고 static인지 확인, cgiargs도 받아옴
	if (stat(filename, &sbuf) < 0) { // 파일 이름으로 찾아보고 sbuf에 저장
		clienterror(fd, filename, "404", "Not found", "Tiny could not find this file");
		return;
	}
	if (is_static) {  // static일 때 처리
		if (!(S_ISREG(sbuf.st_mode)) || !(S_IRUSR & sbuf.st_mode)) {  // 파일이 정상인지 확인, 읽을 수 있는 지 확인
			clienterror(fd, filename, "403", "Forbidden", "Tiny could not read this file");
			return;
		}
		serve_static(fd, filename, sbuf.st_size, method); // 파일명 보내고 실행
	}
	else { // dynamic 일 때 처리
		if (!(S_ISREG(sbuf.st_mode)) || !(S_IXUSR & sbuf.st_mode)) {  // 파일이 정상인지 확인, 실행할 수 있는 지 확인
			clienterror(fd, filename, "403", "Forbidden", "Tiny could not run the cgi file");
			return;
		}
		serve_dynamic(fd, filename, cgiargs, method); // 파일명 보내고 실행
	}

}

void clienterror(int fd, char* cause, char* errnum, char* shortmsg, char* longmsg) { //에러 메시지 http로 처리하는 함수
	char buf[MAXLINE], body[MAXBUF];

	// http body 만들기
	sprintf(body, "<html><title>Tiny Error</title>");
	sprintf(body, "%s<body bgcolor=""ffffff"">\r\n", body);
	sprintf(body, "%s%s: %s\r\n", body, errnum, shortmsg);
	sprintf(body, "%s<p>%s: %s\r\n", body, longmsg, cause);
	sprintf(body, "%s<hr><em>The Tiny Web Server</em>\r\n", body);

	// response 보내기
	sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg); // http 버젼, 에러 넘버, short 메시지
	Rio_writen(fd, buf, strlen(buf));
	sprintf(buf, "Content_type: text/html\r\n"); // content type
	Rio_writen(fd, buf, strlen(buf));
	sprintf(buf, "Content-length: %d\r\n\r\n", (int)strlen(body)); // content length
	Rio_writen(fd, buf, strlen(buf));
	Rio_writen(fd, body, strlen(body)); // body
}

void read_requesthdrs(rio_t* rp) {  // tiny는 get method만 받기 때문에 헤더들이 필요가 없습니다~
	char buf[MAXLINE];
	Rio_readlineb(rp, buf, MAXLINE); // 한줄읽어옴
	while (strcmp(buf, "\r\n")) {    // 엔터일 때까지
		Rio_readlineb(rp, buf, MAXLINE);
		printf("%s", buf);
	}
	return;
}

int parse_uri(char* uri, char* filename, char* cgiargs) {
	char* ptr;

	if (!strstr(uri, "cgi-bin")) { // "cgi-bin"이 없다면 static 
		strcpy(cgiargs, ""); //
		strcpy(filename, "."); //.
		strcat(filename, uri); //.(uri)
		if (uri[strlen(uri) - 1] == '/') // 마지막 uri가 / 이라면
			strcat(filename, "home.html"); // home반환
		return 1;
	}
	else { //dynamic 
		ptr = index(uri, '?');  //? index 부터
		if (ptr) {
			strcpy(cgiargs, ptr + 1); // ? cgiargs에 저장
			*ptr = '\0'; // ? 뒤에 더이상 안 읽도록
		}
		else
			strcpy(cgiargs, "");

		strcpy(filename, "."); //파일이름앞에 .
		strcat(filename, uri); //파일이름
		return 0;
	}
}

void serve_static(int fd, char* filename, int filesize, char*method) {  // static response 에 대한 처리 하는 함수
	int srcfd;
	char* srcp, filetype[MAXLINE], buf[MAXBUF];

	get_filetype(filename, filetype); // filetype 받아오기
	sprintf(buf, "HTTP/1.0 200 OK\r\n");  // response header
	sprintf(buf, "%sServer: Tiny Web Server\r\n", buf);
	sprintf(buf, "%sConnection: close\r\n", buf);
	sprintf(buf, "%sContent-length: %d\r\n", buf, filesize);
	sprintf(buf, "%sContent-type: %s\r\n\r\n", buf, filetype);
	Rio_writen(fd, buf, strlen(buf)); // 받은 content에 대해서 response head를 보냄
	printf("Response headers:\n");
	printf("%s", buf);

	if (strcasecmp(method, "GET") == 0) { // get 요청시에만 response
		srcfd = Open(filename, O_RDONLY, 0); // 파일을 열겠다(read only)
		srcp = malloc(filesize);
		Rio_readn(srcfd, srcp, filesize); // 파일을 읽어와서 버퍼에 임시 저장
		Rio_writen(fd, srcp, filesize); // 파일 response
		free(srcp);

		//srcfd = Open(filename, O_RDONLY, 0); // 파일을 열겠다(read only)
		//srcp = Mmap(0, filesize, PROT_READ, MAP_PRIVATE, srcfd, 0); // 파일 카피                       
		//Close(srcfd);
		//Rio_writen(fd, srcp, filesize); // 파일 response
		//Munmap(srcp, filesize); // 복사 한 파일 삭제
	}
}

void get_filetype(char* filename, char* filetype) { // filetype MIME type으로 반환해주는 함수 
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

void serve_dynamic(int fd, char* filename, char* cgiargs, char* method) { // dynamic response 에 대한 처리 하는 함수
	char buf[MAXLINE], * emptylist[] = { NULL };

	sprintf(buf, "HTTP/1.0 200 OK \r\n"); // response header
	Rio_writen(fd, buf, strlen(buf));
	sprintf(buf, "Server: Tiny Web Server\r\n");
	Rio_writen(fd, buf, strlen(buf));

	if (strcasecmp(method, "GET") == 0) { // Get 요청시에만 response
		if (Fork() == 0) { // child 원래실행되던 프로세스의 복사본을만든다.
			 // 실제 서버는 여기서 모든 cgi vars를 셋팅한다.
			setenv("QUERY_STRING", cgiargs, 1); //file
			Dup2(fd, STDOUT_FILENO); // 파일경로 맞추기
			Execve(filename, emptylist, environ); // cgi 실행
		}
		Wait(NULL); // child의 응답을 기다린다
	}
}

void echo_request(int connfd) { // 받은 requeset를 echo해 화면에 표시 시키는함수
	size_t n;
	char buf[MAXLINE];
	rio_t rio;

	Rio_readinitb(&rio, connfd); // connect file discription 을 rio in/out으로 할당
	printf("Received Request & Header----------------\n");
	while ((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0) {  // 입력받은 바이트가 0이 아니라면 (근데 종료시킬수가 없음)
		Rio_writen(connfd, buf, n); // in 한 buf 내용을 다시 out
		printf("%s",buf);
		if (buf[0] == '0') {
			printf("received 0\n");
			break;
		}
	}
	printf("-----------------------------------------\n");

}