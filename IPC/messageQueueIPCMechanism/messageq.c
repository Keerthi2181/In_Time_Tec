#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>

#define MAX_SIZE 100

struct message {
    long msg_type;        
    int size;
    int data[MAX_SIZE];
};

void sort_int_array(int *arr, int n) {
    int i, j, temp;
    for (i = 0; i < n - 1; i++) {
        for (j = 0; j < n - 1 - i; j++) {
            if (arr[j] > arr[j + 1]) {
                temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

int main() {
    key_t key;
    int msgid;
    pid_t pid;

    key = ftok("msgfile", 65);
    if (key == -1) {
        perror("ftok");
        return 1;
    }

    msgid = msgget(key, 0666 | IPC_CREAT);
    if (msgid == -1) {
        perror("msgget");
        return 1;
    }

    pid = fork();
    if (pid < 0) {
        perror("fork");
        msgctl(msgid, IPC_RMID, NULL);
        return 1;
    } else if (pid == 0) {
        struct message msg_recv, msg_send;

        if (msgrcv(msgid, &msg_recv,
                   sizeof(struct message) - sizeof(long),
                   1, 0) == -1) {
            perror("msgrcv child");
            exit(1);
        }

        sort_int_array(msg_recv.data, msg_recv.size);

        msg_send.msg_type = 2;
        msg_send.size = msg_recv.size;
        for (int i = 0; i < msg_recv.size; i++) {
            msg_send.data[i] = msg_recv.data[i];
        }

        if (msgsnd(msgid, &msg_send,
                   sizeof(struct message) - sizeof(long),
                   0) == -1) {
            perror("msgsnd child");
            exit(1);
        }

        exit(0);
    } else {
        int n, i;
        struct message msg_send, msg_recv;

        printf("Enter number of elements (<= %d): ", MAX_SIZE);
        scanf("%d", &n);
        if (n <= 0 || n > MAX_SIZE) {
            printf("Invalid size.\n");
            msgctl(msgid, IPC_RMID, NULL);
            return 0;
        }

        printf("Enter %d integers: ", n);
        for (i = 0; i < n; i++) {
            scanf("%d", &msg_send.data[i]);
        }

        printf("Array before sorting (Process 1): ");
        for (i = 0; i < n; i++) {
            printf("%d ", msg_send.data[i]);
        }
        printf("\n");

        msg_send.msg_type = 1;
        msg_send.size = n;

        if (msgsnd(msgid, &msg_send,
                   sizeof(struct message) - sizeof(long),
                   0) == -1) {
            perror("msgsnd parent");
            msgctl(msgid, IPC_RMID, NULL);
            return 1;
        }

        wait(NULL);

        if (msgrcv(msgid, &msg_recv,
                   sizeof(struct message) - sizeof(long),
                   2, 0) == -1) {
            perror("msgrcv parent");
            msgctl(msgid, IPC_RMID, NULL);
            return 1;
        }

        printf("Array after sorting (Process 1): ");
        for (i = 0; i < msg_recv.size; i++) {
            printf("%d ", msg_recv.data[i]);
        }
        printf("\n");

        msgctl(msgid, IPC_RMID, NULL);
    }

    return 0;
}
