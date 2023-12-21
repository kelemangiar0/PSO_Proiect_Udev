#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define SIZE 1024

char currentServerPath[1000];

void removeSubstring(char *s, const char *toremove) {
    char *p = strstr(s, toremove);
    if (p) {
        int len = strlen(toremove);
        while (*(p + len)) {
            *p = *(p + len);
            p++;
        }
        *p = '\0';
    }

}

void receive_file(int sockfd, const char *completeFilePathToServer) {
    FILE *fileptr = fopen(completeFilePathToServer, "wb");
    if (fileptr == NULL) {
        perror("Error in creating file: ");
        exit(-1);
    }
    printf("File created: %s\n", completeFilePathToServer);

    char buffer[SIZE];
    size_t total_received = 0;
    ssize_t n;

    while ((n = recv(sockfd, buffer, SIZE, 0)) > 0) {
        if(strcmp(buffer, "END_FILE")==0)
            break;
        fwrite(buffer, 1, n, fileptr);
        total_received += n;
        bzero(buffer, SIZE);
    }
    
    fclose(fileptr);
    printf("[FILE RECEIVED]%s\n", completeFilePathToServer);
}

void recieve_info(int sockfd, char *destination_path, char *initial_path) {
    int n;
    char buffer[SIZE];

    while (1) {
        n = recv(sockfd, buffer, SIZE, 0);
        sleep(1);
        if (n <= 0) {
            break;
        }

        if (strncmp(buffer, "[FILE]", 6) == 0) {
            removeSubstring(buffer, "[FILE]");

            char completeFilePathToServer[1000];
            snprintf(completeFilePathToServer, sizeof(completeFilePathToServer), "%s/%s", currentServerPath, buffer);

            receive_file(sockfd, completeFilePathToServer);
        }

        if (strncmp(buffer, "[DICT]", 6) == 0) {
            removeSubstring(buffer, "[DICT]");
            removeSubstring(buffer, initial_path);

            char new_path[1000];
            snprintf(new_path, sizeof(new_path), "%s%s", destination_path, buffer);
            mkdir(new_path, 0777);

            strcpy(currentServerPath, new_path);
            printf("[FOLDER CREATED]%s\n", currentServerPath);
        }

        bzero(buffer, SIZE);
    }
    return;
}

int setup_connection(char *ip, int port) {
    int e;

    int sockfd, new_sock;
    struct sockaddr_in server_addr, new_addr;
    socklen_t addr_size;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("[-]Error in socket");
        exit(1);
    }
    printf("[+]Server socket created. \n");

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = port;
    server_addr.sin_addr.s_addr = inet_addr(ip);

    e = bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (e < 0) {
        perror("[-]Error in Binding");
        exit(1);
    }
    printf("[+]Binding Successful.\n");

    e = listen(sockfd, 10);
    if (e == 0) {
        printf("[+]Listening...\n");
    } else {
        perror("[-]Error in Binding");
        exit(1);
    }
    addr_size = sizeof(new_addr);
    new_sock = accept(sockfd, (struct sockaddr *)&new_addr, &addr_size);
    return new_sock;
}

int main() {
    char ip[] = "192.168.211.12";
    int port = 8084;
    int socket = setup_connection(ip, port);

    char *server_folder = "/home/student/pso_2023/destination"; // folderul in care clientul trimite fisierele catre server

    char client_folder_to_copy[SIZE];
    int n = recv(socket, client_folder_to_copy, SIZE, 0);
    sleep(1);
    if (n <= 0) {
        exit(-1);
    }
    printf("[CLIENT'S FOLDER TO COPY]%s\n", client_folder_to_copy);

    recieve_info(socket, server_folder, client_folder_to_copy);

    printf("[+]Data written in the text file ");
    close(socket);

    return 0;
}