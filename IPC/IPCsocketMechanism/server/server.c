#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#define PORT 8080
#define ACCOUNT_FILE "../resource/accountDB.txt"

typedef struct {
    int operation;  
    int amount;     
} Request;

typedef struct {
    int success;    
    int balance;
    char message[100];
} Response;

pthread_mutex_t file_mutex = PTHREAD_MUTEX_INITIALIZER;

int read_balance_from_file() {
    FILE *fp = fopen(ACCOUNT_FILE, "r");
    int bal = 0;
    if (!fp) {
        return 0;
    }
    if (fscanf(fp, "%d", &bal) != 1) {
        bal = 0;
    }
    fclose(fp);
    return bal;
}

void write_balance_to_file(int balance) {
    FILE *fp = fopen(ACCOUNT_FILE, "w");
    if (!fp) {
        perror("fopen write_balance_to_file");
        return;
    }
    fprintf(fp, "%d\n", balance);
    fclose(fp);
}

void process_request(const Request *req, Response *res) {
    int balance;

    pthread_mutex_lock(&file_mutex);
    balance = read_balance_from_file();

    if (req->operation == 1) {
        if (req->amount <= 0) {
            res->success = 0;
            snprintf(res->message, sizeof(res->message),
                     "Invalid withdrawal amount.");
        } else if (req->amount > balance) {
            res->success = 0;
            snprintf(res->message, sizeof(res->message),
                     "Withdrawal failed: insufficient balance.");
        } else {
            balance -= req->amount;
            write_balance_to_file(balance);
            res->success = 1;
            snprintf(res->message, sizeof(res->message),
                     "Withdrawal successful.");
        }
    } else if (req->operation == 2) {
        if (req->amount <= 0) {
            res->success = 0;
            snprintf(res->message, sizeof(res->message),
                     "Invalid deposit amount.");
        } else {
            balance += req->amount;
            write_balance_to_file(balance);
            res->success = 1;
            snprintf(res->message, sizeof(res->message),
                     "Deposit successful.");
        }
    } else if (req->operation == 3) {
        res->success = 1;
        snprintf(res->message, sizeof(res->message),
                 "Balance fetched successfully.");
    } else {
        res->success = 0;
        snprintf(res->message, sizeof(res->message),
                 "Unknown operation.");
    }

    res->balance = balance;
    pthread_mutex_unlock(&file_mutex);
}

void *client_handler(void *arg) {
    int client_sock = *(int *)arg;
    free(arg);

    Request req;
    Response res;
    ssize_t bytes_read;

    while (1) {
        bytes_read = recv(client_sock, &req, sizeof(Request), 0);
        if (bytes_read <= 0) {
            break; 
        }

        if (req.operation == 4) {
            break;
        }

        process_request(&req, &res);

        if (send(client_sock, &res, sizeof(Response), 0) <= 0) {
            break;
        }
    }

    close(client_sock);
    pthread_exit(NULL);
}

int main(void) {
    int server_fd, *new_sock;
    struct sockaddr_in address;
    socklen_t addr_len = sizeof(address);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket");
        return 1;
    }

    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR,
                   &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        close(server_fd);
        return 1;
    }

    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address,
             sizeof(address)) < 0) {
        perror("bind");
        close(server_fd);
        return 1;
    }

    if (listen(server_fd, 5) < 0) {
        perror("listen");
        close(server_fd);
        return 1;
    }

    printf("ATM Server listening on port %d...\n", PORT);

    while (1) {
        int client_sock = accept(server_fd,
                                 (struct sockaddr *)&address,
                                 &addr_len);
        if (client_sock < 0) {
            perror("accept");
            continue;
        }

        new_sock = (int *)malloc(sizeof(int));
        if (!new_sock) {
            perror("malloc new_sock");
            close(client_sock);
            continue;
        }
        *new_sock = client_sock;

        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL,
                           client_handler, (void *)new_sock) != 0) {
            perror("pthread_create");
            close(client_sock);
            free(new_sock);
            continue;
        }

        pthread_detach(thread_id);
    }

    close(server_fd);
    return 0;
}
