#include <stdio.h>
#include <pthread.h>
#include <strings.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>

void *serve(void *connection);

struct connection_info {
    int fd;
    struct sockaddr connection;
    socklen_t address_len;
};

int main(void) {
    
    int result;
    pthread_t thread;
    pthread_attr_t attrs;
    struct connection_info *info;

    struct sockaddr_in server_address;
    struct sockaddr connection;
    // server host related info on the network
    struct hostent *server;
 
    int socket_fd, port = 8081;

    /* create the socket */
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_fd < 0){
        printf("failed creating socket\n");
        return -1;
    }

    bzero(&server_address, sizeof(server_address));    
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = INADDR_ANY;
	server_address.sin_port = htons(port);
	if(bind(socket_fd, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
        printf("Failed bidning socket\n");
        close(socket_fd);
        return -3;

    }


    if (listen(socket_fd, 10)) {
        printf("cannot open socket for listening\n");
        close(socket_fd);
        return -2;
    }


    while(1) {

        info = malloc(sizeof(struct connection_info));

        if (info == NULL) {
            printf("Couldn't allocate new connection structure\n");
            return -5;
        }


        info->fd = accept(socket_fd, &(info->connection), &(info->address_len));
        if (info->fd < 0) {
            printf("Something is wrong!\n");
            free(info);
            return -6;
        }

        if(!pthread_attr_init(&attrs))
            puts("Pthread attrs initialized succesfully");
        if(!pthread_create(&thread, &attrs, &serve, (void *)info))
            puts("Pthread created successfully");
        pthread_attr_destroy(&attrs);


    } 
    close(socket_fd);
    return 0;

}

void *serve(void *connection) {

    struct connection_info *info = (struct connection_info *)connection;

    unsigned char buffer[1024];

    printf("Entering recv loop...\n");
    while(recv(info->fd, buffer, 1024, 0) > 0) {

       printf("%s\n", buffer);

    }
    printf("leaving recv loop\n");
    close(info->fd);
    free(info);

}
