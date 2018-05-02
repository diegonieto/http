#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>

int socket_connect(char *host, in_port_t port){
    struct hostent *hp;
    struct sockaddr_in addr;
    int on = 1, sock;

    if((hp = gethostbyname(host)) == NULL){
        herror("gethostbyname");
        exit(1);
    }
    bcopy(hp->h_addr, &addr.sin_addr, hp->h_length);
    addr.sin_port = htons(port);
    addr.sin_family = AF_INET;
    sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (const char *)&on, sizeof(int));

    if(sock == -1){
        perror("setsockopt");
        exit(1);
    }

    if(connect(sock, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) == -1){
        perror("connect");
        exit(1);
    }
    return sock;
}

char *generate_http_request(const char* hostname, const char* path)
{
    char *http_request;
    const char *get = "GET  HTTP/1.1\r\n";
    const char *host = "Host: \n\n";
    const char *end = "\r\n";
    int total_size = strlen(get) + strlen(host) + strlen(end) + strlen(path) + strlen(hostname);

    http_request = malloc(total_size*sizeof(char));

    strncpy(http_request, get, 4); // copy only get
    strncpy(&http_request[strlen(http_request)], path, strlen(path)); // append path
    strncpy(&http_request[strlen(http_request)], &get[4], strlen(get)-4); // append remaining data from get
    printf("GET data: %s\n", http_request);

    strncpy(&http_request[strlen(http_request)], host, 6); // copy host header
    strncpy(&http_request[strlen(http_request)], hostname, strlen(hostname)); // append host name
    strncpy(&http_request[strlen(http_request)], &host[6], strlen(host)-6); // append remaining date from host
    printf("GET+Host data: %s\n", http_request);

    return http_request;
}

int main(int argc, char *argv[]){
    int fd;
    const BUFFER_SIZE = 1024;
    char *buffer = malloc(BUFFER_SIZE*sizeof(char));

    if(argc < 4){
        fprintf(stderr, "Usage: %s <hostname> <path> <port>. Using www.google.com / 80\n", argv[0]);
    }

    fd = socket_connect(argc<4?"www.google.com":argv[1], argc<4?80:atoi(argv[3]));
    char *req = generate_http_request(argc<4?"www.google.com":argv[1], argc<4?"/":argv[2]);
    write(fd, req, strlen(req));
    free(req);
    bzero(buffer, BUFFER_SIZE);

    while(read(fd, buffer, BUFFER_SIZE - 1) != 0){
        fprintf(stdout, "%s", buffer);
        bzero(buffer, BUFFER_SIZE);
    }

    shutdown(fd, SHUT_RDWR);
    close(fd);

    return 0;
}
