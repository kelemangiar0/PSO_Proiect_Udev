#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <sys/stat.h>

#define SIZE 1024

int serverConnection(char *ip, int port) {
    int sockfd;
    struct sockaddr_in server_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("[-]Error in socket");
        exit(1);
    }
    printf("[+]Server socket created.\n");

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("[-]Error in connecting to server");
        exit(1);
    }
    printf("[+]Connected to server.\n");
    return sockfd;
}

void send_file(FILE *fp, int sockfd) {
    char data[SIZE] = {0};

    while (fgets(data, SIZE, fp) != NULL) {
        if (send(sockfd, data, sizeof(data), 0) == -1) {
            perror("[-]Error in sending file");
            exit(1);
        }
        bzero(data, SIZE);
    }
}

void send_filename(int sockfd, const char *filename) {
    char buffer[SIZE];
    snprintf(buffer, SIZE, "FILE:%s\n", filename);
    if (send(sockfd, buffer, SIZE, 0) == -1) {
        perror("[-]Error in sending filename");
        exit(1);
    }
}

void send_directory(int sockfd, const char *base_path, const char *root_path) {
    DIR *dir;
    struct dirent *entry;
    char full_path[1024];

    if (!(dir = opendir(base_path)))
        return;

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        snprintf(full_path, sizeof(full_path), "%s/%s", base_path, entry->d_name);
        struct stat statbuf;

        if (!stat(full_path, &statbuf)) {
            if (S_ISDIR(statbuf.st_mode)) {
                send_directory(sockfd, full_path, root_path);
            } else {
                // Calculate the relative path
                char *relative_path = full_path + strlen(root_path);
                if (*relative_path == '/') relative_path++;  // Skip the leading '/'

                send_filename(sockfd, relative_path);
                FILE *fp = fopen(full_path, "r");
                if (fp == NULL) {
                    perror("[-]Error in reading file");
                    continue;
                }
                send_file(fp, sockfd);
                fclose(fp);
                send(sockfd, "END_OF_FILE", SIZE, 0); // Send end of file marker
            }
        }
    }
    closedir(dir);
}

int main() {
    char *ip = "192.168.173.216";
    int port = 12346;
    const char *directory_path = "/home/kele/PSO-P/backup-folder";

    int sockfd = serverConnection(ip, port);
    send_directory(sockfd, directory_path, directory_path);
    send(sockfd, "END_OF_TRANSFER", SIZE, 0); // Send end of transfer marker

    printf("[+]Directory transfer complete.\n");
    close(sockfd);
    printf("[+]Disconnected from the server.\n");
    return 0;
}
