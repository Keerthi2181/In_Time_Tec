#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

void printMatrix(int *matrix, int size) {
    for (int i=0;i<size;i++) {
        for (int j=0;j<size;j++) {
            printf("%4d",*(matrix+i*size+j));
        }
        printf("\n");
    }
}

void rotate90Clockwise(int *matrix, int size) {
    for (int layer=0;layer<size/2;layer++) {
        int startIndex=layer;
        int endIndex=size-1-layer;

        for (int i=startIndex;i<endIndex;i++) {
            int offset=i-startIndex;
            int *top=matrix+startIndex*size+i;
            int *left=matrix+(endIndex-offset)*size+startIndex;
            int *bottom=matrix+endIndex*size+(endIndex-offset);
            int *right=matrix+i*size+endIndex;

            int temp =*left;
            *left=*bottom;
            *bottom=*right;
            *right=*top;
            *top=temp;
        }
    }
}


void applySmoothing(int *matrix,int size) {
    int *tempRow=(int *)malloc(size*sizeof(int));

    int *original=(int *)malloc(size*size*sizeof(int));
    for(int i=0;i<size*size;i++) {
        *(original+i)=*(matrix+i);
    }

    for(int i=0;i<size;i++) {
        for(int j=0;j<size;j++) {
            int sum=0; 
            int count=0;

            for(int row=i-1;row<=i+1;row++) {
                for(int column=j-1;column<=j+1;column++) {
                    if(row>=0 && row<size && column>=0 && column<size) {
                        sum+=*(original+row*size+column);
                        count++;
                    }
                }
            }
            *(tempRow+j)=sum/count;
        }

        for(int j=0;j<size;j++) {
            *(matrix+i*size+j)=*(tempRow+j);
        }
    }
    free(tempRow);
    free(original);
}

int readMatrixSize() {
    int size;
    int result;

    while(1) {
        printf("Enter matrix size (2-10): ");
        result = scanf("%d", &size);
        if (result != 1) {
            printf("Invalid input.Only numbers between 2-10 allowed.\n");
            while (getchar()!='\n');
            continue;
        }

        if (size>=2 && size<=10) {
            return size;
        } else {
            printf("Invalid input.Only numbers between 2-10 allowed.\n");
        }
    }
}


int main() {
    int size = readMatrixSize();
    int *matrix = (int *)malloc(size*size*sizeof(int));
    srand(time(0));
    for (int i = 0; i < size*size; i++) {
        *(matrix + i) = rand() % 256;
    }

    printf("\nOriginal Matrix:\n");
    printMatrix(matrix, size);

    rotate90Clockwise(matrix, size);
    printf("\nRotated Matrix:\n");
    printMatrix(matrix, size);

    applySmoothing(matrix, size);
    printf("\nSmoothing Filter:\n");
    printMatrix(matrix, size);

    free(matrix);
    return 0;
}
