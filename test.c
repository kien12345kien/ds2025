
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void list_local_files(const char *path) {
    DIR *dir;
    struct dirent *entry;
    dir = opendir(path);
    if (dir == NULL) {
        perror("Failed to open directory");
        return;
    }
    printf("Local files:\n");
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) { // Only regular files
            printf("- %s\n", entry->d_name);
        }
    }
    closedir(dir);
}

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Socket creation error\n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("Invalid address/ Address not supported\n");
        return -1;
    }

    // Connect to the server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("Connection failed\n");
        return -1;
    }

    printf("Connected to server.\n");

    // Main menu
    while (1) {
        printf("\n1. List local files\n2. List server files\n3. Exit\nChoose an option: ");
        int choice;
        scanf("%d", &choice);

        if (choice == 1) {
            list_local_files("./client_files");
        } else if (choice == 2) {
            send(sock, "LIST", strlen("LIST"), 0);
            memset(buffer, 0, BUFFER_SIZE);
            read(sock, buffer, BUFFER_SIZE);
            printf("Files on server:\n%s", buffer);
        } else if (choice == 3) {
            send(sock, "EXIT", strlen("EXIT"), 0);
            break;
        } else {
            printf("Invalid choice.\n");
        }
    }

    close(sock);
    return 0;
}

