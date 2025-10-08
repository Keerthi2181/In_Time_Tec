#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define FILE_NAME "users.txt"
#define NAME_LEN 50
#define LINE_LEN 100

typedef struct {
    int id;
    char name[NAME_LEN];
    int age;
} User;

int checkValidInt(char *str) {
    if (!str || *str == '\0') return 0;
    for (int i = 0; str[i]; i++) {
        if (i == 0 && (str[i] == '-' || str[i] == '+')) continue;
        if (!isdigit(str[i])) return 0;
    }
    return 1;
}

int checkValidName(char *name) {
    if (!name || strlen(name) == 0) return 0;
    for (int i = 0; name[i]; i++) {
        if (!isalpha(name[i]) && name[i] != ' ')
            return 0;
    }
    return 1;
}

int checkidExists(int id) {
    FILE *file = fopen(FILE_NAME, "r");
    if (!file) return 0;

    char line[LINE_LEN];
    User u;
    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "%d|%49[^|]|%d", &u.id, u.name, &u.age) == 3) {
            if (u.id == id) { fclose(file); return 1; }
        }
    }
    fclose(file);
    return 0;
}

void addUser() {
    char input[LINE_LEN];
    User u;

    printf("Enter ID: ");
    scanf("%s", input); getchar();
    if (!checkValidInt(input)) { printf("Error: ID must be a number.\n"); return; }
    u.id = atoi(input);
    if (checkidExists(u.id)) { printf("Error: ID already exists.\n"); return; }

    printf("Enter Name: ");
    fgets(u.name, NAME_LEN, stdin);
    u.name[strcspn(u.name, "\n")] = 0;
    if (!checkValidName(u.name)) { 
        printf("Error: Name must contain only letters and spaces.\n"); 
        return; 
    }

    printf("Enter Age: ");
    scanf("%s", input); getchar();
    if (!checkValidInt(input)) { printf("Error: Age must be a number.\n"); return; }
    u.age = atoi(input);

    FILE *file = fopen(FILE_NAME, "a");
    if (!file) { printf("Error: Cannot open file.\n"); return; }
    fprintf(file, "%d|%s|%d\n", u.id, u.name, u.age);
    fclose(file);

    printf("User added successfully.\n");
}

void showUsers() {
    FILE *file = fopen(FILE_NAME, "r");
    if (!file) { printf("No records found.\n"); return; }

    char line[LINE_LEN];
    User u;
    printf("\n%-5s %-20s %-5s\n", "ID", "NAME", "AGE");
    printf("-------------------------------\n");

    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "%d|%49[^|]|%d", &u.id, u.name, &u.age) == 3) {
            printf("%-5d %-20s %-5d\n", u.id, u.name, u.age);
        }
    }

    fclose(file);
}

void editUser() {
    char input[LINE_LEN];
    int id, found = 0;

    printf("Enter ID to edit: ");
    scanf("%s", input); getchar();
    if (!checkValidInt(input)) { printf("Error: ID must be a number.\n"); return; }
    id = atoi(input);

    FILE *file = fopen(FILE_NAME, "r");
    FILE *temp = fopen("temp.txt", "w");
    if (!file || !temp) { printf("Error: Cannot open files.\n"); return; }

    char line[LINE_LEN];
    User u;
    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "%d|%49[^|]|%d", &u.id, u.name, &u.age) != 3) continue;

        if (u.id == id) {
            found = 1;
            printf("Enter new Name: ");
            fgets(u.name, NAME_LEN, stdin);
            u.name[strcspn(u.name, "\n")] = 0;
            if (!checkValidName(u.name)) { 
                printf("Invalid name. Name not edited.\n"); 
            }

            printf("Enter new Age: ");
            scanf("%s", input); getchar();
            if (checkValidInt(input)) u.age = atoi(input);
            else printf("Invalid age. Age not edited.\n");
        }

        fprintf(temp, "%d|%s|%d\n", u.id, u.name, u.age);
    }

    fclose(file);
    fclose(temp);
    remove(FILE_NAME);
    rename("temp.txt", FILE_NAME);

    printf(found ? "User updated successfully.\n" : "Error: ID not found.\n");
}

void deleteUser() {
    char input[LINE_LEN];
    int id, found = 0;

    printf("Enter ID to delete: ");
    scanf("%s", input); getchar();
    if (!checkValidInt(input)) { printf("Error: ID must be a number.\n"); return; }
    id = atoi(input);

    FILE *file = fopen(FILE_NAME, "r");
    FILE *temp = fopen("temp.txt", "w");
    if (!file || !temp) { printf("Error: Cannot open files.\n"); return; }

    char line[LINE_LEN];
    User u;
    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "%d|%49[^|]|%d", &u.id, u.name, &u.age) != 3) continue;

        if (u.id == id) { found = 1; continue; }

        fprintf(temp, "%d|%s|%d\n", u.id, u.name, u.age);
    }

    fclose(file);
    fclose(temp);
    remove(FILE_NAME);
    rename("temp.txt", FILE_NAME);

    printf(found ? "User deleted successfully.\n" : "Error: ID not found.\n");
}

int main() {
    char input[LINE_LEN];
    int choice;

    while (1) {
        printf("\n1.Add User 2.Show Users 3.Edit User 4.Delete User 5.Exit\n> ");
        scanf("%s", input); getchar();
        if (!checkValidInt(input)) { printf("Invalid choice.\n"); continue; }

        choice = atoi(input);
        switch (choice) {
            case 1: addUser(); break;
            case 2: showUsers(); break;
            case 3: editUser(); break;
            case 4: deleteUser(); break;
            case 5: exit(0);
            default: printf("Invalid choice.\n");
        }
    }
    return 0;
}
