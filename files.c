#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILE_NAME "users.txt"

typedef struct {
    int id;
    char name[50];
    int age;
} User;

int exists(int id) {
    FILE *f = fopen(FILE_NAME, "r");
    if(!f) return 0;
    User u;
    while(fscanf(f, "%d %[^\n] %d\n", &u.id, u.name, &u.age) != EOF) {
        if(u.id == id) { fclose(f); return 1; }
    }
    fclose(f);
    return 0;
}

void add() {
    FILE *f = fopen(FILE_NAME, "a");
    if(!f) { puts("Cannot open file"); return; }
    User u;
    printf("ID: ");
    scanf("%d", &u.id); getchar();
    if(exists(u.id)) { puts("ID exists"); fclose(f); return; }
    printf("Name: "); fgets(u.name,50,stdin); u.name[strcspn(u.name,"\n")]=0;
    printf("Age: "); scanf("%d", &u.age);
    fprintf(f,"%d %s %d\n",u.id,u.name,u.age);
    fclose(f);
    puts("Added");
}

void show() {
    FILE *f = fopen(FILE_NAME, "r");
    if(!f) { puts("No records"); return; }
    User u;
    printf("\n%-5s %-20s %-5s\n","ID","NAME","AGE");
    printf("-----------------------------\n");
    while(fscanf(f,"%d %[^\n] %d\n",&u.id,u.name,&u.age)!=EOF)
        printf("%-5d %-20s %-5d\n",u.id,u.name,u.age);
    fclose(f);
}

void edit() {
    int id, found=0;
    printf("ID to edit: "); scanf("%d",&id); getchar();
    FILE *f=fopen(FILE_NAME,"r"),*t=fopen("temp.txt","w");
    if(!f||!t){ puts("Error"); return; }
    User u;
    while(fscanf(f,"%d %[^\n] %d\n",&u.id,u.name,&u.age)!=EOF){
        if(u.id==id){ found=1; printf("New Name: "); fgets(u.name,50,stdin); u.name[strcspn(u.name,"\n")]=0; printf("New Age: "); scanf("%d",&u.age); getchar(); }
        fprintf(t,"%d %s %d\n",u.id,u.name,u.age);
    }
    fclose(f); fclose(t);
    remove(FILE_NAME); rename("temp.txt",FILE_NAME);
    puts(found?"Updated":"ID not found");
}

void del() {
    int id, found=0;
    printf("ID to delete: "); scanf("%d",&id); getchar();
    FILE *f=fopen(FILE_NAME,"r"),*t=fopen("temp.txt","w");
    if(!f||!t){ puts("Error"); return; }
    User u;
    while(fscanf(f,"%d %[^\n] %d\n",&u.id,u.name,&u.age)!=EOF){
        if(u.id==id){ found=1; continue; }
        fprintf(t,"%d %s %d\n",u.id,u.name,u.age);
    }
    fclose(f); fclose(t);
    remove(FILE_NAME); rename("temp.txt",FILE_NAME);
    puts(found?"Deleted":"ID not found");
}

int main(){
    int c;
    while(1){
        printf("\n1.Add 2.Show 3.Edit 4.Delete 5.Exit\n> ");
        scanf("%d",&c); getchar();
        switch(c){ case 1:add(); 
                   break;
                   case 2:show(); 
                   break; 
                   case 3:edit();
                   break;
                   case 4:del(); 
                   break; 
                   case 5: 
                   exit(0); 
                   default:puts("Invalid"); 
        }
    }
    return 0;
}
