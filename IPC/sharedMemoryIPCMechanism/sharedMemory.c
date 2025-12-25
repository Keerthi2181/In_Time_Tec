#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>

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
    int shmid;
    int *shared_mem;
    int n, i;
    pid_t pid;

    key = ftok("shmfile", 65);
    if (key == -1) {
        perror("ftok");
        return 1;
    }

    printf("Enter number of elements: ");
    scanf("%d", &n);
    if (n <= 0) {
        printf("Invalid size.\n");
        return 0;
    }

    shmid = shmget(key, (n + 1) * sizeof(int), 0666 | IPC_CREAT);
    if (shmid < 0) {
        perror("shmget");
        return 1;
    }

    shared_mem = (int *)shmat(shmid, NULL, 0);
    if (shared_mem == (void *)-1) {
        perror("shmat");
        return 1;
    }

    shared_mem[0] = n;
    printf("Enter %d integers: ", n);
    for (i = 0; i < n; i++) {
        scanf("%d", &shared_mem[i + 1]);
    }

    printf("Array before sorting (Process 1): ");
    for (i = 0; i < n; i++) {
        printf("%d ", shared_mem[i + 1]);
    }
    printf("\n");

    pid = fork();
    if (pid < 0) {
        perror("fork");
        shmdt(shared_mem);
        shmctl(shmid, IPC_RMID, NULL);
        return 1;
    } else if (pid == 0) {
        int *child_mem = (int *)shmat(shmid, NULL, 0);
        if (child_mem == (void *)-1) {
            perror("shmat child");
            exit(1);
        }

        int count = child_mem[0];
        sort_int_array(&child_mem[1], count);

        shmdt(child_mem);
        exit(0);
    } else {
        wait(NULL);

        printf("Array after sorting (Process 1): ");
        for (i = 0; i < n; i++) {
            printf("%d ", shared_mem[i + 1]);
        }
        printf("\n");

        shmdt(shared_mem);
        shmctl(shmid, IPC_RMID, NULL);
    }

    return 0;
}
