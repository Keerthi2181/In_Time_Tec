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
    FILE *fp;
    pid_t pid;

    printf("Enter number of elements: ");
    scanf("%d", &n);

    if (n <= 0) {
        printf("Invalid size.\n");
        return 0;
    }

    int *arr = (int *)malloc(n * sizeof(int));
    if (!arr) {
        printf("Memory allocation failed.\n");
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

    fp = fopen("data.txt", "w");
    if (!fp) {
        perror("fopen");
        free(arr);
        return 1;
    }
    fprintf(fp, "%d\n", n);
    for (i = 0; i < n; i++) {
        fprintf(fp, "%d ", arr[i]);
    }
    fprintf(fp, "\n");
    fclose(fp);

    pid = fork();
    if (pid < 0) {
        perror("fork");
        free(arr);
        return 1;
    } else if (pid == 0) {
        fp = fopen("data.txt", "r");
        if (!fp) {
            perror("fopen child");
            exit(1);
        }

        int count;
        if (fscanf(fp, "%d", &count) != 1 || count != n) {
            printf("File format error in child.\n");
            fclose(fp);
            exit(1);
        }
        for (i = 0; i < n; i++) {
            if (fscanf(fp, "%d", &arr[i]) != 1) {
                printf("File read error in child.\n");
                fclose(fp);
                exit(1);
            }
        }
        fclose(fp);

        sort_int_array(arr, n);

        fp = fopen("data.txt", "w");
        if (!fp) {
            perror("fopen child write");
            exit(1);
        }
        fprintf(fp, "%d\n", n);
        for (i = 0; i < n; i++) {
            fprintf(fp, "%d ", arr[i]);
        }
        fprintf(fp, "\n");
        fclose(fp);

        exit(0);
    } else {
        wait(NULL);

        fp = fopen("data.txt", "r");
        if (!fp) {
            perror("fopen parent read");
            free(arr);
            return 1;
        }

        int count;
        if (fscanf(fp, "%d", &count) != 1 || count != n) {
            printf("File format error in parent.\n");
            fclose(fp);
            free(arr);
            return 1;
        }
        for (i = 0; i < n; i++) {
            fscanf(fp, "%d", &arr[i]);
        }
        fclose(fp);

        printf("Array after sorting (Process 1): ");
        for (i = 0; i < n; i++) {
            printf("%d ", arr[i]);
        }
        printf("\n");
    }

    free(arr);
    return 0;
}
