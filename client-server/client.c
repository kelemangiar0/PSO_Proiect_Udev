#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

#define SIZE 1024

int setup_connection(char*ip, int port)
{
     int e;

    int sockfd;
    struct sockaddr_in server_addr;
    
    char *filename = "file.txt";
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd<0)
    {
        perror("[-]Error in socket");
        exit(1);
    }
     printf("[+]Server socket created. \n");

     server_addr.sin_family = AF_INET;
     server_addr.sin_port = port;
     server_addr.sin_addr.s_addr = inet_addr(ip);

     e = connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
     if(e == -1)
     {
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

void list_directory(const char *base_path,int sockfd) {
    struct dirent *dp;
    DIR *dir = opendir(base_path);
    if (!dir) 
        return; 

    while ((dp = readdir(dir)) != NULL) 
    {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0) 
        {
            //printf("%s/%s\n", base_path, dp->d_name);
            char new_path[1000];
            snprintf(new_path, sizeof(new_path), "%s/%s", base_path, dp->d_name);          
            struct stat statbuf;
            if (stat(new_path, &statbuf) != -1)
            {
                // Check if it's a directory 
                if (S_ISDIR(statbuf.st_mode))
                {  
                    //trimit numele directoarelor
                    char to_send_file[1000];
                    snprintf(to_send_file, sizeof(to_send_file), "[DICT]/%s", new_path+1);
                    char data[SIZE] = {0};
                    strcpy(data,to_send_file);
                    if(send(sockfd, data, sizeof(data), 0)== -1)
                    {
                        perror("[-] Error in sendung data");
                        exit(1);
                    }
                    sleep(1);
                    bzero(data, SIZE);

                    list_directory(new_path,sockfd);
                }
                else
                {
                    //trimit numele fisierelor
                    removeSubstring(new_path, base_path);

                    char to_send_file[1000];
                    snprintf(to_send_file, sizeof(to_send_file), "[FILE]%s", new_path+1);
                    char data[SIZE] = {0};
                    strcpy(data,to_send_file);
                    
                    char completeFilePathFromClient[1000];
                    snprintf(completeFilePathFromClient, sizeof(completeFilePathFromClient), "%s%s", base_path,new_path);


                    if(send(sockfd, data, sizeof(data), 0)== -1)
                    {
                        perror("[-] Error in sendung data");
                        exit(1);
                    }
                    sleep(1);

                    bzero(data, SIZE);

                    //aici trebuie trimis continutul fisierului completeFilePathFromClient
                    //cand trimiti trimiti ca fisier binar
                    //send_file(socketfd, completeFilePathFromClient)

                    //test

                }
            }
            
        }
    }

    closedir(dir);
}
int main()
{
    char *ip = "10.0.2.15"; //aceeasi adr ip ca la server
    int port = 8084;
    int socket=setup_connection(ip,port);
    char *source_folder="/home/kele/PSO-P/backup-folder"; //aici folderul pe care il vrei sa il trimiti catre server (de ka client)

    char data[SIZE] = {0};
    strcpy(data,source_folder);
    printf("%s\n",data);
    if(send(socket, data, sizeof(data), 0)== -1)
    {
        perror("[-] Error in sendung data");
        exit(1);
    }
    sleep(1);
    bzero(data, SIZE);

    list_directory(source_folder,socket);

   printf("[+] File data send successfully. \n");


    close(socket);
    printf("[+]Disconnected from the server. \n");
    return 0;

}
