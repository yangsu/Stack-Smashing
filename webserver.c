#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>

/*
BuggyServer v. 2.1
Copyright (c) 2008-9, Matt Jacobson [mhj32]
All Rights Reserved.
*/

typedef unsigned char byte;

int main(int, char *[]);
char *handle(char *, int *);
int check_filename_length(byte);
void sigint(int);
void sigsegv(int);

char *port_arg;
int sock = 0;
const char *prefix = "GET /";
const char *resp_ok = "HTTP/1.1 200 OK\nServer: BuggyServer/1.0\n";
const char *resp_bad = "HTTP/1.1 404 Not Found\nServer: BuggyServer/1.0\n\n";
const char *content_html = "Content-type: text/html\n\n";
const char *content_text = "Content-type: text/plain\n\n";

const char *www_path = "./www/";

int main(int argc, char *argv[]) {
	int port;

	if (argc < 2 || (port = atoi(argv[1])) < 1 || port > 65535) {
		printf("Usage: webserver port\n");
		exit(1);
	}

	port_arg = (char *)malloc(6*sizeof (char));
	memset(port_arg, '\0', 6);
	strncpy(port_arg, argv[1], 5);

	signal(SIGINT, &sigint);
	signal(SIGSEGV, &sigsegv);

	struct sockaddr_in socket_addr;
	sock = socket(PF_INET, SOCK_STREAM, 0);
        int on = 1;
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof on);

	memset(&socket_addr, 0, sizeof socket_addr);
	socket_addr.sin_family = PF_INET;
	socket_addr.sin_port = htons(port);
	socket_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(sock, (struct sockaddr *)&socket_addr, sizeof socket_addr) < 0) {
		perror("couldn't bind");
		exit(1);
	}
	listen(sock, 10);

	while (1) {
		int acceptsock = accept(sock, NULL, NULL);
		char *input = (char *)malloc(1024*sizeof (char));
		recv(acceptsock, input, 1024, 0);
		int is_html = 0;
		char *contents = handle(input,&is_html);
		free(input);

		if (contents != NULL) {
			send(acceptsock, resp_ok, strlen(resp_ok), 0);
			if (is_html)
				send(acceptsock, content_html, strlen(content_html), 0);
			else
				send(acceptsock, content_text, strlen(content_text), 0);
			send(acceptsock, contents, strlen(contents), 0);
			send(acceptsock, "\n", 1, 0);
			free(contents);
		} else
			send(acceptsock, resp_bad, strlen(resp_bad), 0);

		close(acceptsock);
	}

	return 0;
}

char *handle(char *request, int *is_html) {
	char filename[100];
        //clear filename
	memset(filename, '\0', 100*sizeof (char));
        //check to see if there's a get request in the input
	if (strstr(request, prefix) == request) {
		char *start = request + strlen(prefix);
		char *end = strstr(start, " HTTP");
		if (end == NULL) {
			printf("Unsupported command.\n");
			return NULL;
		}
		int len = (int) (end - start);
		if (check_filename_length(len))
			strncpy(filename, start, len);
		else
			return NULL;
	} else {
		printf("Unsupported command.\n");
		return NULL;
	}

	if (strstr(filename,"/") != NULL)
		return NULL;
	if (!strcmp(filename, ""))
		strcpy(filename,"index.html");
	if (strstr(filename,".html") != NULL)
		*is_html = 1;

	char *path = (char *)malloc((strlen(www_path) + strlen(filename))*sizeof (char));
	strcpy(path, www_path);
	strcpy(path + strlen(www_path), filename);

	struct stat file_status;
	if (stat(path, &file_status) != 0)
		return NULL;

	char *contents = (char *)malloc((file_status.st_size+1)*sizeof (char));
	memset(contents, '\0', file_status.st_size+1);

	FILE *readfile = fopen(path, "r");
	int i;
	for (i=0;i<file_status.st_size;i++)
		contents[i] = fgetc(readfile);
	fclose(readfile);

	return contents;
}

int check_filename_length(byte len) {
	if (len < 100)
		return 1;
	return 0;
}

void sigint(int arg) {
	if (sock)
		close(sock);
	printf("Shutting down peacefully.\n");
	exit(0);
}

void sigsegv(int arg) {
	if (sock)
		close(sock);
	printf("Segmentation fault.  Shutting down peacefully, then rebooting.\n");

	sleep(2);
	char *argv[3];
	argv[0] = "./webserver";
	argv[1] = port_arg;
	argv[2] = NULL;
	execv(argv[0], argv);
	free(port_arg);
	exit(1);
}
