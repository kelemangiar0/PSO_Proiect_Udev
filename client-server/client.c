#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

#define SIZE 1024

int setup_connection(char *ip, int port) {
    int e;

    int sockfd;
    struct sockaddr_in server_addr;


    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("[-]Error in socket");
        exit(1);
    }
    printf("[+]Server socket created. \n");

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = port;
    server_addr.sin_addr.s_addr = inet_addr(ip);

    e = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (e == -1) {
        perror("[-]Error in Connecting");
        exit(1);
    }
    printf("[+]Connected to server.\n");
    return sockfd;
}

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

void send_file(int sockfd, const char *completeFilePathFromClient) {
    FILE *fileptr = fopen(completeFilePathFromClient, "rb");
    if (fileptr == NULL) {
        perror("Error in opening file: ");
        exit(-1);
    }

    char buffer[SIZE];
    size_t bytesRead;

    while ((bytesRead = fread(buffer, 1, SIZE, fileptr)) > 0) {
        if (send(sockfd, buffer, bytesRead, 0) == -1) {
            perror("Error in sending file");
            exit(-1);
        }
        bzero(buffer, SIZE);
        sleep(1);
    }

    send(sockfd, "END_FILE", 9, 0);
    fclose(fileptr);
    printf("[FILE SENT]%s\n", completeFilePathFromClient);
}

void list_directory(const char *base_path, int sockfd) {
    struct dirent *dp;
    DIR *dir = opendir(base_path);
    if (!dir) return;

    while ((dp = readdir(dir)) != NULL) {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0) {
            char new_path[1000];
            snprintf(new_path, sizeof(new_path), "%s/%s", base_path, dp->d_name);
            struct stat statbuf;
            if (stat(new_path, &statbuf) != -1) {
                if (S_ISDIR(statbuf.st_mode)) {
                    char to_send_file[1000];
                    snprintf(to_send_file, sizeof(to_send_file), "[DICT]/%s", new_path + 1);
                    char data[SIZE] = {0};
                    strcpy(data, to_send_file);
                    if (send(sockfd, data, sizeof(data), 0) == -1) {
                        perror("[-] Error in sending data");
                        exit(1);
                    }
                    sleep(1);
                    bzero(data, SIZE);

                    list_directory(new_path, sockfd);
                } else {
                    removeSubstring(new_path, base_path);

                    char to_send_file[1000];
                    snprintf(to_send_file, sizeof(to_send_file), "[FILE]%s", new_path + 1);
                    char data[SIZE] = {0};
                    strcpy(data, to_send_file);

                    char completeFilePathFromClient[1000];
                    snprintf(completeFilePathFromClient, sizeof(completeFilePathFromClient), "%s%s", base_path, new_path);

                    if (send(sockfd, data, sizeof(data), 0) == -1) {
                        perror("[-] Error in sending data");
                        exit(1);
                    }
                    sleep(1);

                    bzero(data, SIZE);

                    send_file(sockfd, completeFilePathFromClient);
                }
            }
        }
    }

    closedir(dir);
}

int main() {
    char ip[] = "192.168.205.216"; // adresa IP a serverului
    int port = 8084;
    int socket = setup_connection(ip, port);
    char *source_folder = "/home/student/testssh"; // folderul pe care il vrei sa il trimiti catre server (de la client)

    char data[SIZE] = {0};
    strcpy(data, source_folder);
    printf("%s\n", data);
    if (send(socket, data, sizeof(data), 0) == -1) {
        perror("[-] Error in sending data");
        exit(1);
    }
    sleep(1);
    bzero(data, SIZE);

    list_directory(source_folder, socket);

    printf("[+] File data sent successfully. \n");

    close(socket);
    printf("[+] Disconnected from the server. \n");
    return 0;
}
