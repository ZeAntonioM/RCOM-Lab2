#define _POSIX_SOURCE 1
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <memory.h>
#include <netdb.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include "../include/download.h"

int parse_url(char * url, char * username, char * password, char * host, char * path){
    char *aux = NULL ;
    sscanf(url,AT,aux);
    if( sscanf(url, "%*[^@]@%c", &aux) == 1)sscanf(url,USER_REGEX,username);
    if(username[0]!='\0')sscanf(url,PASS_REGEX,password);
    if(username[0]!='\0')
        sscanf(url, HOST_AT_REGEX, host);
    else 
        sscanf(url, HOST_REGEX, host);
    sscanf(url, PATH_REGEX, path);

   
    return 0;
}

int create_socket(char * host, char * port){
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(host, port, &hints, &servinfo)) < 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return -1;
    }

    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connect");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return -1;
    }

    inet_ntop(p->ai_family, (void *)&((struct sockaddr_in *)p->ai_addr)->sin_addr, s, sizeof s);
    printf("client: connecting to %s\n", s);

    freeaddrinfo(servinfo); // all done with this structure
    return sockfd;
}

int login(int socket, char * username, char * password ){
    char request[512];
    int resquest_len, response_code;
    response_code = read_response_code(socket);
    if(!response_code){
        return -1;
    }else if(response_code == LOGIN_SUCCESSFULL_CODE){
        printf("Already Logged in \n");
        return 0;
    }
    if(username[0]=='\0'){
        snprintf(request, BUFFER_SIZE, "USER %s\n%n", DEAFAULT_USERNAME, &resquest_len);
    }
    else
        snprintf(request, BUFFER_SIZE, "USER %s\n%n", username, &resquest_len);

    printf("Sending request: %s", request);
    if (send(socket, request, resquest_len, 0512) < 0) {
        printf("Error while sending user command: %s\n", strerror(errno));
        return -1;
    }
    sleep(1);
    response_code = read_response_code(socket);
    printf("%d\n", response_code);
    if (response_code == LOGIN_SUCCESSFULL_CODE) {
        printf("Login successful \n");
        return 0;
    } else if (response_code != PASSWORD_CODE) {
        printf("%s is invalid for login\n", username);
        return -1;
    }
    
    if(password[0]=='\0')
        snprintf(request, BUFFER_SIZE, "PASS  \n%n", &resquest_len);
    else 
        snprintf(request, BUFFER_SIZE, "PASS %s\n%n", password, &resquest_len);

    printf("Sending request: %s", request);
    if (send(socket, request, resquest_len, 0) < 0) {
        printf("Error while sending pass command: %s\n", strerror(errno));
        return -1;
    }

    response_code = read_response_code(socket);
    if (response_code != LOGIN_SUCCESSFULL_CODE) {
        printf("Credentials are invalid\n");
        return -1;
    }

    printf("Logged in successfully\n");
    return 0;

}

int setPassiveMode(const int socket, char *ip, char *port) {

    char response[BUFFER_SIZE];
    char request[6] = "pasv\n";
    int port_,ip1, ip2, ip3, ip4, port1, port2;

    printf("Sending request: %s", request);

    if(send(socket, request,5,0)<0){
        printf("Error while sending the pasv command\n");
    }
    
    if (read_response(socket,response,BUFFER_SIZE)){ 
        printf("Failed to set passive mode \n");
        return 1;
    }

    sscanf(response, PASSIVE_REGEX, &ip1, &ip2, &ip3, &ip4, &port1, &port2);
    port_ = port1 * 256 + port2;
    sprintf(port, "%d",port_);
    sprintf(ip, "%d.%d.%d.%d", ip1, ip2, ip3, ip4);
    printf("Setting Passive\n");

    return 0;
}

int transfer_request(int socket, char * path) {
    char request[13 + strlen(path)];
    int request_len;
    printf("Request file to transfer\n");
    snprintf(request,13 + strlen(path), "retr %s\n%n", path, &request_len);
    printf("Sending request: %s", request);
    if(send(socket, request,request_len,0)<0){
        printf("Error while sending request for file\n");
        return 1;
    }
   
    int response_code = read_response_code(socket);
    if ((response_code != TRANSFER_COMPLETED_CODE) && (response_code != START_TRANSFER_CODE)){
        printf("Error Could not transfer file\n");
        return 1;
    }
   
    return 0;
    
}

int get_file(int socket, char * filename){

    FILE *fd = fopen(filename, "wb");
    if (fd == NULL) {
        printf("Error opening or creating file '%s'\n", filename);
        exit(-1);
    }

    char buffer[BUFFER_SIZE];
    int bytes_read;
    printf("Start download \n");
    do {
        bytes_read = recv(socket, buffer, BUFFER_SIZE, 0);
        if (fwrite(buffer, bytes_read, 1, fd) < 0) return 1;
    } while (bytes_read);

    fclose(fd);
    printf("Finishing download\n");

    return 0;
}

int read_response_code(int socket){
    char response [BUFFER_SIZE];
    int response_code =0;
    read_response(socket, response, BUFFER_SIZE);
    sscanf(response, RESPCODE_REGEX, &response_code);
    return response_code;
}

int read_response (int socket, char * response, int response_len){
    int total_bytes_read = 0, bytes_read = 0;
    int n_tries = N_TRIES;
    usleep(100000);
    while (n_tries) {
        bytes_read = recv(socket, response + total_bytes_read,response_len - total_bytes_read, MSG_DONTWAIT);
        if(bytes_read <=0){
            n_tries--;
            usleep(100000);
        }
        else{
            n_tries = N_TRIES;
        }
        total_bytes_read += bytes_read;
    }


    response[total_bytes_read] = '\0';
    if(total_bytes_read>0) printf("Server Response: %s\n", response);
    else printf("Empty response\n");
    return 0;
}



int main(int argc, char *argv[]) {

    // Validate arguments
    if (argc < 2) {
        printf("Error: Missing URL as argument!\n");
        exit(1);
    }

    // Initialize variables
    char username[256]="\0", password[256] = "", host[256] = "",
         port[6] = "21", path[256] = "", passive_host[INET_ADDRSTRLEN],
        passive_port[13];

    
    // Parse url
    if (parse_url(argv[1], username, password, host, path) < 0) {
        printf("Invalid url: %s!\n", argv[1]);
        return -1;
    }
    
    // Extract file name
    char *file_name = strrchr(path, '/');

    if (file_name != NULL) {
        file_name++; 
    } else {
        file_name = path; 
    }
    char *new_path = malloc(strlen(file_name) + strlen(DOWNLOAD_DIR) + 1);
    strcpy(new_path, DOWNLOAD_DIR);
    strcat(new_path, file_name);

    //Print Values used
    printf("----------------------------------------------------\n");
    printf("Info: \n");
    printf("----------------------------------------------------\n");
    printf("username: %s\n", username);
    printf("password: %s\n", password);
    printf("host: %s\n", host);
    printf("port: %s\n", port);
    printf("path: %s\n", path);
    printf("file name: %s\n", file_name);
    printf("----------------------------------------------------\n");

    // Create socket which is going to connect to the server
    int socket_A = create_socket(host, port);
    if (socket_A < 0)
        return -1;

    // Login into the server
    if (login(socket_A, username, password) < 0)
        return -1;

    // Set passive
    if (setPassiveMode(socket_A, passive_host, passive_port) < 0)
        return -1;

    // Create a new socket for passive mode data transfer
    int socket_B = create_socket(passive_host, passive_port);
    if (socket_B < 0)
        return -1;

    // Request file
    if (transfer_request(socket_A, path) )
        return -1;

    // download file
    get_file(socket_B, new_path);

    // terminate connections
    close(socket_A);
    close(socket_B);

    return 0;
}