#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <pthread.h>
#include<fcntl.h>

#define NUMBER_OF_LAN_ADDRESSES 255
int totalIP = 0;

struct parameters {
    char id[16];
    char subnet[16];
    char network[16];
};

typedef struct parameters parameters_t;

// Function declarations
void *thread_routine(void *params);
void get_network_info(const char* iface, char* ip, char* subnet);
void calculate_network_address(const char* ip, const char* subnet, char* network);
void ping_addresses(const char* network, const char* subnet, const char* myIP);

int main(int argc, char** argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <network_interface>\n", argv[0]);
        return 1;
    }

    char log_file[256];
    sprintf(log_file,"/home/%s/PSO-P/log.txt",argv[1]);

    int log_fd = open(log_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (log_fd < 0) {
        perror("Failed to open log file");
        exit(EXIT_FAILURE);
    }

    // Redirect stdout to log file
    if (dup2(log_fd, STDOUT_FILENO) < 0) {
        perror("Failed to redirect stdout to log file");
        close(log_fd);
        exit(EXIT_FAILURE);
    }
    char myIP[16], subnet[16], network[16];
    get_network_info(argv[2], myIP, subnet);
    calculate_network_address(myIP, subnet, network);
    ping_addresses(network, subnet, myIP);

    printf("======\nSUMMARY: %d IP ADDRESSES IN NETWORK.\n======\n", totalIP);
    return 0;
}

void get_network_info(const char* iface, char* ip, char* subnet) {
    int fd;
    struct ifreq ifr;
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1) {
        perror("socket");
        exit(1);
    }

    ifr.ifr_addr.sa_family = AF_INET;
    strncpy(ifr.ifr_name, iface, IFNAMSIZ - 1);

    // Get IP address
    if (ioctl(fd, SIOCGIFADDR, &ifr) == -1) {
        perror("ioctl IP");
        close(fd);
        exit(1);
    }
    strcpy(ip, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));

    // Get subnet mask
    if (ioctl(fd, SIOCGIFNETMASK, &ifr) == -1) {
        perror("ioctl Netmask");
        close(fd);
        exit(1);
    }
    strcpy(subnet, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_netmask)->sin_addr));

    close(fd);
}

void calculate_network_address(const char* ip, const char* subnet, char* network) {
    struct in_addr ip_addr, subnet_mask, network_addr;
    inet_aton(ip, &ip_addr);
    inet_aton(subnet, &subnet_mask);
    network_addr.s_addr = ip_addr.s_addr & subnet_mask.s_addr;
    strcpy(network, inet_ntoa(network_addr));
    int len = strlen(network);
    if (len > 0 && network[len - 1] == '0') {
        network[len - 1] = '\0';
    }
}

void ping_addresses(const char* network, const char* subnet, const char* myIP) {
    pthread_t tid[NUMBER_OF_LAN_ADDRESSES];
    parameters_t params[NUMBER_OF_LAN_ADDRESSES];

    for (int i = 0; i < NUMBER_OF_LAN_ADDRESSES; i++) {
        sprintf(params[i].id, "%d", i);
        strcpy(params[i].subnet, subnet);
        strcpy(params[i].network, network);

        char potentialIP[16];
        snprintf(potentialIP, sizeof(potentialIP), "%s%d", network, i);
        if (strcmp(potentialIP, myIP) == 0) {
            continue;
        }
        pthread_create(&tid[i], NULL, thread_routine, &params[i]);
    }

    for (int i = 0; i < NUMBER_OF_LAN_ADDRESSES; i++) {
        pthread_join(tid[i], NULL);
    }
}

void *thread_routine(void *params) {
    parameters_t *args = (parameters_t*)params;
    char ping_command[100];
    snprintf(ping_command, sizeof(ping_command), "ping -c 1 %s%s > /dev/null", args->network, args->id);
    int result = system(ping_command);
    if (result == 0) {
        printf("%s%s: Ping success\n", args->network, args->id);
        __sync_fetch_and_add(&totalIP, 1);
    }
    return NULL;
}
