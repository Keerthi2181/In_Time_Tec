#include <stdio.h>
#include <ctype.h>
#include <string.h>

struct Student {
    int rollNumber;
    char name[20];
    int marks1, marks2, marks3;
};

int checkDuplicateRollNumber(struct Student s[], int currentIndex, int rollNumber) {
    for (int i=0; i<currentIndex; i++) {
        if (s[i].rollNumber==rollNumber)
            return 1;
    }
    return 0;
}

int checkValidName(char *name) {
    if (!name || strlen(name) == 0) return 0;
    for (int i = 0; name[i]; i++) {
        if (!isalpha(name[i]) && name[i] != ' ')
            return 0;
    }
    return 1;
}

int calculateTotal(int marks1, int marks2, int marks3) {
    return marks1+marks2+marks3;
}

float calculateAverage(int total) {
    return total/(3.0);
}

char calculateGrade(float average) {
    if (average >= 85) return 'A';
    else if (average >= 70) return 'B';
    else if (average >= 50) return 'C';
    else if (average >= 35) return 'D';
    else return 'F';
}

void showPerformance(char grade) {
    int stars = 0;
    switch (grade) {
        case 'A': stars = 5; break;
        case 'B': stars = 4; break;
        case 'C': stars = 3; break;
        case 'D': stars = 2; break;
         
    }
    printf("Performance: ");
    for (int j = 0; j < stars; j++)
        printf("*");
    printf("\n");
}

void recursiveListingRollNumbers(struct Student s[], int i, int numofstudents) {
    if (i == numofstudents) return;
    printf("%d ", s[i].rollNumber);
    recursiveListingRollNumbers(s, i + 1, numofstudents);
}
void printRollNumbers(struct Student s[], int numofstudents) {
    printf("\nList of Roll Numbers: ");
    recursiveListingRollNumbers(s, 0, numofstudents);
    printf("\n");
}

void inputStudents(struct Student s[], int numofstudents) {
    for (int i = 0; i < numofstudents; i++) {
        printf("Enter details of student %d:\n", i + 1);

        printf("Enter rollno: ");
        while (1) {
            if (scanf("%d", &s[i].rollNumber) != 1 || s[i].rollNumber <= 0) {
                printf("Invalid! Roll number must be a positive integer. Enter again: ");
                while (getchar() != '\n');
                continue;
            }
            if (checkDuplicateRollNumber(s, i, s[i].rollNumber)) {
                printf("Duplicate roll number! Please enter a unique roll number: ");
                while (getchar() != '\n');
                continue;
            }
            break;
        }

        while (getchar() != '\n');

        printf("Enter name: ");
        scanf(" %[^\n]", s[i].name);
        while (!checkValidName(s[i].name)) {
            printf("Invalid! Only alphabets and spaces allowed. Enter again: ");
            scanf(" %[^\n]", s[i].name);
        }

        printf("Enter marks1: ");
        while (scanf("%d", &s[i].marks1) != 1 || s[i].marks1 < 0 || s[i].marks1 > 100) {
            printf("Invalid! Marks should be between 0 and 100. Enter again: ");
            while (getchar() != '\n');
        }

        printf("Enter marks2: ");
        while (scanf("%d", &s[i].marks2) != 1 || s[i].marks2 < 0 || s[i].marks2 > 100) {
            printf("Invalid! Marks should be between 0 and 100. Enter again: ");
            while (getchar() != '\n');
        }

        printf("Enter marks3: ");
        while (scanf("%d", &s[i].marks3) != 1 || s[i].marks3 < 0 || s[i].marks3 > 100) {
            printf("Invalid! Marks should be between 0 and 100. Enter again: ");
            while (getchar() != '\n');
        }

        printf("\n");
    }
}

void displayStudents(struct Student s[], int numofstudents) {
    for (int i = 0; i < numofstudents; i++) {
        int total = calculateTotal(s[i].marks1, s[i].marks2, s[i].marks3);
        float average = calculateAverage(total);
        char grade = calculateGrade(average);

        printf("Student %d\n", i + 1);
        printf("RollNo: %d\n", s[i].rollNumber);
        printf("Name: %s\n", s[i].name);
        printf("Total: %d\n", total);
        printf("Average: %.2f\n", average);
        printf("Grade: %c\n", grade);

        if (average >= 35) 
            showPerformance(grade);

        printf("\n");
    }
}


int main() {
    int numofstudents;

    printf("Enter number of Students: ");
    while (scanf("%d", &numofstudents) != 1 || numofstudents <= 0) {
        printf("Invalid input! Please enter a positive integer: ");
        while (getchar() != '\n');
    }

    struct Student s[numofstudents];

    inputStudents(s, numofstudents);
    displayStudents(s, numofstudents);
    printRollNumbers(s, numofstudents);

    return 0;
}
