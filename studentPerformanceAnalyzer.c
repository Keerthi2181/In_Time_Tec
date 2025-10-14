#include <stdio.h>
#include <ctype.h>
#include <string.h>

struct Student{
    int rollno;
    char name[20];
    int m1;
    int m2;
    int m3;
};
int checkValidName(char *name) {
    if (!name || strlen(name) == 0) return 0;
    for (int i = 0; name[i]; i++) {
        if (!isalpha(name[i]) && name[i] != ' ')
            return 0;
    }
    return 1;
}
int calculatetotal(int m1,int m2,int m3){
    int total=m1+m2+m3;
    return total;
}
float calculateavg(int total){
    float avg=total/3.0;
    return avg;
}
char calculategrade(float avg){
    if(avg>=85) return 'A';
    else if(avg>=70) return 'B';
    else if(avg>=50) return 'C';
    else if(avg>=35) return 'D';
    else return 'F';
}
void showperformance(char grade){
    int stars=0;
    switch(grade){
        case 'A':stars=5;break;
        case 'B':stars=4;break;
        case 'C':stars=3;break;
        case 'D':stars=2;break;
    }
    printf("Performance:");
    for(int j=0;j<stars;j++){
        printf("*");
    }
    printf("\n");
    
}
void printrollno(struct Student s[],int i,int numofstudents) {
    if(i==numofstudents) return;
    printf("%d ", s[i].rollno);
    printrollno(s,i+1,numofstudents);
}

int main()
{
    int numofstudents;
    
    printf("Enter number of Students:");
    scanf("%d",&numofstudents);
    while (numofstudents <= 0) {
        printf("Invalid number! Enter again: ");
        scanf("%d", &numofstudents);
    }

    struct Student s[numofstudents];
    for(int i=0;i<numofstudents;i++){
        printf("Enter details of student%d:\n",i+1);
        printf("Enter rollno: ");
        scanf("%d", &s[i].rollno);
        while (s[i].rollno <= 0) {
            printf("Invalid!Roll number can't be negative.Enter again: ");
            scanf("%d", &s[i].rollno);
        }
        while (getchar() != '\n');
        printf("Enter name: ");
        scanf(" %[^\n]", s[i].name);
        while (!checkValidName(s[i].name)) {
            printf("Invalid! Only alphabets and spaces allowed. Enter again: ");
            scanf(" %[^\n]", s[i].name);
        }

        printf("Enter marks1: ");
        scanf("%d", &s[i].m1);
        while (s[i].m1 < 0 || s[i].m1 > 100) {
            printf("Invalid! Marks should be between 0 and 100. Enter again: ");
            scanf("%d", &s[i].m1);  
        }
        
        printf("Enter marks2: ");
        scanf("%d", &s[i].m2);
        while (s[i].m2 < 0 || s[i].m2 > 100) {
            printf("Invalid! Marks should be between 0 and 100. Enter again: ");
            scanf("%d", &s[i].m2);
        }
        
        printf("Enter marks3: ");
        scanf("%d", &s[i].m3);
        while (s[i].m3 < 0 || s[i].m3 > 100) {
            printf("Invalid! Marks should be between 0 and 100. Enter again: ");
            scanf("%d", &s[i].m3);
        }

        printf("\n\n");
    }
    
    for(int i=0;i<numofstudents;i++){
        int total=calculatetotal(s[i].m1,s[i].m2,s[i].m3);
        float avg=calculateavg(total);
        char grade=calculategrade(avg);
        printf("Student%d\n",i+1);
        printf("RollNo:%d\n",s[i].rollno);
        printf("Name:%s\n",s[i].name);
        printf("Total:%d\n",total);
        printf("Average:%.2f\n",avg);
        printf("Grade:%c\n",grade);
        if(avg<35){
            printf("\n");
            continue;
        } 
        showperformance(grade);
        printf("\n");
        
    }
    printf("\nList of Rollno:");
    printrollno(s,0,numofstudents);
    printf("\n");
    


    return 0;
}
