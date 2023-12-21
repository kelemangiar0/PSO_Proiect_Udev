#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <libgen.h>

#define SIZE 1024
#define TARGET_DIR "/home/kele/server-folder"

int setup(char *ip, int port) {
    int sockfd, new_sock;
    struct sockaddr_in server_addr, new_addr;
    socklen_t addr_size;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("[-]Error in socket");
        exit(1);
    }
    printf("[+]Server socket created.\n");

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);

    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("[-]Error in binding");
        exit(1);
    }
    printf("[+]Binding successful.\n");

    if (listen(sockfd, 10) == 0) {
        printf("[+]Listening...\n");
    } else {
        perror("[-]Error in binding");
        exit(1);
    }
    addr_size = sizeof(new_addr);
    new_sock = accept(sockfd, (struct sockaddr *)&new_addr, &addr_size);
    return new_sock;
}

void create_directory_structure(const char *file_path) {
    char temp_path[SIZE];
    char *p;
    
    // Copy the file path and tokenize it
    snprintf(temp_path, sizeof(temp_path), "%s", file_path);
    p = strtok(temp_path, "/");

    // Create each directory in the path
    char dir_path[SIZE] = {0};
    while (p != NULL) {
        strcat(dir_path, "/");
        strcat(dir_path, p);
        mkdir(dir_path, 0777); // Create directory, ignore if it exists
        p = strtok(NULL, "/");
    }
}

void write_file(int sockfd) {
    int n;
    FILE *fp;
    char buffer[SIZE];
    char target_path[SIZE];
    char file_path[SIZE];

    while (1) {
        n = recv(sockfd, buffer, SIZE, 0);
        if (n <= 0) {
            break;
            return;
        }
        buffer[n] = '\0';

        if (strncmp(buffer, "FILE:", 5) == 0) {
            strcpy(file_path, buffer + 5);
            file_path[strcspn(file_path, "\n")] = 0; // Remove newline

            // Construct the full path
            snprintf(target_path, sizeof(target_path), "%s/%s", TARGET_DIR, file_path);

            // Create directory structure
            create_directory_structure(dirname(strdup(target_path)));

            fp = fopen(target_path, "w");
            if (fp == NULL) {
                perror("[-]Error in creating file");
                exit(1);
            }
        } else if (strcmp(buffer, "END_OF_FILE") == 0) {
            fclose(fp);
        } else if (strcmp(buffer, "END_OF_TRANSFER") == 0) {
            break; // All files received
        } else {
            fprintf(fp, "%s", buffer);
        }
    }
}

int main() {
    char *ip = "192.168.173.216";
    int port = 12346;
    int sockfd = setup(ip, port);

    write_file(sockfd);
    printf("[+]Data written to files in %s\n", TARGET_DIR);

    close(sockfd);
    printf("[+]Connection closed.\n");
    return 0;
}
