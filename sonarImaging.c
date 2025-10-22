#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

void printMatrix(int *matrix, int n) {
    for (int i=0;i<n;i++) {
        for (int j=0;j<n;j++) {
            printf("%4d",*(matrix+i*n+j));
        }
        printf("\n");
    }
}

void rotate90Clockwise(int *matrix, int n) {
    for (int layer=0;layer<n/2;layer++) {
        int first= layer;
        int last=n-1-layer;

        for (int i=first;i<last;i++) {
            int offset=i-first;

            int *top=matrix+first*n+i;
            int *left=matrix+(last-offset)*n+first;
            int *bottom=matrix+last*n+(last-offset);
            int *right=matrix+i*n+last;

            int temp =*left;
            *left=*bottom;
            *bottom=*right;
            *right=*top;
            *top=temp;
        }
    }
}


void applySmoothing(int *matrix,int n) {
    int *tempRow=(int *)malloc(n*sizeof(int));

    int *original = (int *)malloc(n*n*sizeof(int));
    for (int i=0;i<n*n;i++) {
        *(original+i)=*(matrix+i);
    }

    for (int i=0;i<n;i++) {
        for (int j=0;j<n;j++) {
            int sum=0; 
            int count = 0;

            for (int r=i-1;r<=i+1;r++) {
                for (int c=j-1;c<=j+1;c++) {
                    if (r>=0 && r<n && c>=0 && c<n) {
                        sum+=*(original+r*n+c);
                        count++;
                    }
                }
            }
            *(tempRow+j)=sum/count;
        }

        for (int j=0;j<n;j++) {
            *(matrix+i*n+j)=*(tempRow+j);
        }
    }
    free(tempRow);
    free(original);
}

int readMatrixSize() {
    int n;
    int result;

    while (1) {
        printf("Enter matrix size (2-10): ");

        result = scanf("%d", &n);

        if (result != 1) {
            printf("Invalid input. Only numbers between 2-10 allowed.\n");
            while (getchar() != '\n'); 
            continue;
        }

        if (n >= 2 && n <= 10) {
            return n;
        } else {
            printf("Invalid input. Only numbers between 2-10 allowed.\n");
        }
    }
}


int main() {
    int n = readMatrixSize();
    int *matrix = (int *)malloc(n * n * sizeof(int));
    if (!matrix) {
        printf("Memory allocation failed\n");
        return 1;
    }
    srand(time(0));
    for (int i = 0; i < n * n; i++) {
        *(matrix + i) = rand() % 256;
    }

    printf("\nOriginal Matrix:\n");
    printMatrix(matrix, n);

    rotate90Clockwise(matrix, n);
    printf("\nRotated Matrix:\n");
    printMatrix(matrix, n);

    applySmoothing(matrix, n);
    printf("\nSmoothing Filter:\n");
    printMatrix(matrix, n);

    free(matrix);
    return 0;
}
