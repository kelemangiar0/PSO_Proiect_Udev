#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#define SIZE 1024

char currentServerPath[2048];

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

    char buffer[SIZE];
    size_t total_received = 0;
    ssize_t n;

    while ((n = recv(sockfd, buffer, SIZE, 0)) > 0) {
        sleep(1);
        if(strcmp(buffer, "END_FILE")==0)
            break;
        fwrite(buffer, 1, n, fileptr);
        total_received += n;
        bzero(buffer, SIZE);
        
    }
    
    fclose(fileptr);
    printf("[FILE CREATED]%s\n", completeFilePathToServer);
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

            char completeFilePathToServer[2048];
            snprintf(completeFilePathToServer, sizeof(completeFilePathToServer), "%s/%s", currentServerPath, buffer);
        
            receive_file(sockfd, completeFilePathToServer);
        }

        if (strncmp(buffer, "[DICT]", 6) == 0) {
            removeSubstring(buffer, "[DICT]");
            removeSubstring(buffer, initial_path);
            
            char new_path[2048];
            snprintf(new_path, sizeof(new_path), "%s%s", destination_path, buffer);
            mkdir(new_path, 0777);

            
            strcpy(currentServerPath, new_path);
            printf("[FOLDER CREATED]%s\n", currentServerPath);
        }

        bzero(buffer, SIZE);
    }
    return;
}



void handleClient(int socket, const char* baseDir) {
    char server_folder[256];
    strcpy(server_folder, baseDir);

    char client_user[SIZE];
    int n1 = recv(socket, client_user, SIZE, 0);
    if (n1 <= 0) {
        perror("Failed to receive client user");
        exit(-1);
    }
    printf("[CLIENT] %s\n", client_user);

    strcat(server_folder, "/");
    strcat(server_folder, client_user);
    mkdir(server_folder, 0777);

    char client_folder_to_copy[SIZE];
    int n = recv(socket, client_folder_to_copy, SIZE, 0);
    if (n <= 0) {
        perror("Failed to receive client folder to copy");
        exit(-1);
    }
    printf("[CLIENT'S FOLDER TO COPY] %s\n", client_folder_to_copy);
    recieve_info(socket, server_folder, client_folder_to_copy);
}

void *handle_client_thread(void *socket) {
    int client_socket = *((int *)socket);
    free(socket);

    handleClient(client_socket, "/home/kele/server-folder");

    close(client_socket);
    return NULL;
}

int main() {
    char ip[] = "192.168.1.9";
    int port = 8084;
    int e;

    int server_fd, new_socket;
    struct sockaddr_in server_addr, new_addr;
    socklen_t addr_size;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("[-]Error in socket");
        exit(1);
    }
    printf("[+]Server socket created.\n");

    int optval = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) {
        perror("[-]Error setting SO_REUSEADDR");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = port;
    server_addr.sin_addr.s_addr = inet_addr(ip);

    e = bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (e < 0) {
        perror("[-]Error in Binding");
        exit(1);
    }
    printf("[+]Binding Successful.\n");

    e = listen(server_fd, 10);
    if (e < 0) {
        perror("[-]Error in listening");
        exit(1);
    }
    printf("[+]Listening...\n");

    while (1) {
        addr_size = sizeof(new_addr);
        new_socket = accept(server_fd, (struct sockaddr *)&new_addr, &addr_size);
        if (new_socket < 0) {
            perror("[-]Error in accepting connection");
            continue;
        }

        pthread_t thread_id;
        int *client_sock = malloc(sizeof(int));
        *client_sock = new_socket;

        if (pthread_create(&thread_id, NULL, handle_client_thread, (void *)client_sock) != 0) {
            perror("pthread_create failed");
            close(new_socket);
            free(client_sock);
        }
    }

    close(server_fd);
    return 0;
}
