#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080

typedef struct {
    int operation; 
    int amount;
} Request;

typedef struct {
    int success;
    int balance;
    char message[100];
} Response;

int main() {
    int sock;
    struct sockaddr_in server_addr;
    Request req;
    Response res;
    int choice, amount;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return 1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        perror("inet_pton");
        close(sock);
        return 1;
    }

    if (connect(sock, (struct sockaddr *)&server_addr,
                sizeof(server_addr)) < 0) {
        perror("connect");
        close(sock);
        return 1;
    }

    printf("Connected to ATM Server.\n");

    while (1) {
        printf("\n------ ATM Menu ------\n");
        printf("1. Withdraw\n");
        printf("2. Deposit\n");
        printf("3. Display Balance\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input.\n");
            break;
        }

        if (choice == 4) {
            req.operation = 4;
            req.amount = 0;
            send(sock, &req, sizeof(Request), 0);
            printf("Exiting.\n");
            break;
        } else if (choice == 1 || choice == 2) {
            printf("Enter amount: ");
            scanf("%d", &amount);
            req.operation = choice;
            req.amount = amount;
        } else if (choice == 3) {
            req.operation = 3;
            req.amount = 0;
        } else {
            printf("Invalid choice.\n");
            continue;
        }

        if (send(sock, &req, sizeof(Request), 0) <= 0) {
            printf("Failed to send request.\n");
            break;
        }

        if (recv(sock, &res, sizeof(Response), 0) <= 0) {
            printf("Failed to receive response.\n");
            break;
        }

        printf("Server: %s\n", res.message);
        printf("Current Balance: %d\n", res.balance);
    }

    close(sock);
    return 0;
}
