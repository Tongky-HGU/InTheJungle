#include "../csapp.h"

int main(void){
	char *buf, *p;
	char arg1[MAXLINE], arg2[MAXLINE], content[MAXLINE];
	int n1=0, n2=0;

	if((buf = getenv("QUERY_STRING")) != NULL){ //getenv로 입력인자가 있다면
		p = strchr(buf, '&'); // &을 찾아서 포인터로 만든뒤
		*p = '\0'; // 거기에 EOF
		strcpy(arg1, buf+2); // 첫번째 인자 \0 이라 막히겠지? (+2의 이유는 앞에 키 때문에)
		strcpy(arg2, p+1+2); // 두번째 인자
		n1 = atoi(arg1); //int로
		n2 = atoi(arg2);
	}
	// http response body 만들어 놓기
	sprintf(content, "QUERY_STRING=%s",buf);
	sprintf(content, "Welcome to add.com: ");
	sprintf(content, "%sThe Internet addition portal.\r\n<p>", content);
	sprintf(content, "%sThe answer is: %d + %d = %d\r\n<p>", content,n1,n2,n1+n2);
	sprintf(content, "%sThanks for visiting!\r\n",content);
	// http response
	printf("Connection: close\r\n");
	printf("Content-length: %d\r\n", (int)strlen(content));
	printf("Content-type: text/html\r\n\r\n");
	printf("%s",content);
	fflush(stdout);

	exit(0);
}

	
