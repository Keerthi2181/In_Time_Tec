#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
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
    int n, i;
    int pipe_parent_to_child[2];
    int pipe_child_to_parent[2];
    pid_t pid;

    if (pipe(pipe_parent_to_child) == -1) {
        perror("pipe1");
        return 1;
    }
    if (pipe(pipe_child_to_parent) == -1) {
        perror("pipe2");
        return 1;
    }

    printf("Enter number of elements: ");
    scanf("%d", &n);
    if (n <= 0) {
        printf("Invalid size.\n");
        return 0;
    }

    int *arr = (int *)malloc(n * sizeof(int));
    if (!arr) {
        perror("malloc");
        return 1;
    }

    printf("Enter %d integers: ", n);
    for (i = 0; i < n; i++) {
        scanf("%d", &arr[i]);
    }

    printf("Array before sorting (Process 1): ");
    for (i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");

    pid = fork();
    if (pid < 0) {
        perror("fork");
        free(arr);
        return 1;
    } else if (pid == 0) {
        close(pipe_parent_to_child[1]);
        close(pipe_child_to_parent[0]);

        int count;
        if (read(pipe_parent_to_child[0], &count, sizeof(int)) != sizeof(int)) {
            printf("Child read error on count.\n");
            exit(1);
        }

        int *local_arr = (int *)malloc(count * sizeof(int));
        if (!local_arr) {
            perror("malloc child");
            exit(1);
        }

        if (read(pipe_parent_to_child[0], local_arr, count * sizeof(int))
            != (ssize_t)(count * sizeof(int))) {
            printf("Child read error on array.\n");
            free(local_arr);
            exit(1);
        }
        close(pipe_parent_to_child[0]);

        sort_int_array(local_arr, count);

        if (write(pipe_child_to_parent[1], &count, sizeof(int)) != sizeof(int)) {
            printf("Child write error on count.\n");
        }
        if (write(pipe_child_to_parent[1], local_arr, count * sizeof(int))
            != (ssize_t)(count * sizeof(int))) {
            printf("Child write error on array.\n");
        }
        close(pipe_child_to_parent[1]);
        free(local_arr);
        exit(0);
    } else {
        close(pipe_parent_to_child[0]);
        close(pipe_child_to_parent[1]);

        if (write(pipe_parent_to_child[1], &n, sizeof(int)) != sizeof(int)) {
            printf("Parent write error on count.\n");
        }
        if (write(pipe_parent_to_child[1], arr, n * sizeof(int))
            != (ssize_t)(n * sizeof(int))) {
            printf("Parent write error on array.\n");
        }
        close(pipe_parent_to_child[1]);

        wait(NULL);

        int count;
        if (read(pipe_child_to_parent[0], &count, sizeof(int)) != sizeof(int)) {
            printf("Parent read error on count.\n");
            free(arr);
            return 1;
        }
        if (count != n) {
            printf("Size mismatch.\n");
            free(arr);
            return 1;
        }

        if (read(pipe_child_to_parent[0], arr, count * sizeof(int))
            != (ssize_t)(count * sizeof(int))) {
            printf("Parent read error on array.\n");
            free(arr);
            return 1;
        }
        close(pipe_child_to_parent[0]);

        printf("Array after sorting (Process 1): ");
        for (i = 0; i < n; i++) {
            printf("%d ", arr[i]);
        }
        printf("\n");
    }

    free(arr);
    return 0;
}
