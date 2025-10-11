#include <stdio.h>
struct Student{
    int rollno;
    char name[20];
    int m1;
    int m2;
    int m3;
};

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
void printrollno(struct Student s[],int i,int n) {
    if(i==n) return;
    printf("%d ", s[i].rollno);
    printrollno(s,i+1,n);
}

int main()
{
    int n;
    
    printf("Enter number of Students:");
    scanf("%d",&n);
    struct Student s[n];
    for(int i=0;i<n;i++){
        printf("Enter details of student%d:\n",i+1);
        printf("Enter rollno:");
        scanf("%d",&s[i].rollno);
        printf("Enter name:");
        scanf("%s",s[i].name);
        printf("Enter marks1:");
        scanf("%d",&s[i].m1);
        printf("Enter marks2:");
        scanf("%d",&s[i].m2);
        printf("Enter marks3:");
        scanf("%d",&s[i].m3);
        printf("\n\n");
    }
    
    for(int i=0;i<n;i++){
        int total=calculatetotal(s[i].m1,s[i].m2,s[i].m3);
        float avg=calculateavg(total);
        char grade=calculategrade(avg);
        printf("Student%d\n",i+1);
        printf("RollNo:%d\n",s[i].rollno);
        printf("Name:%s\n",s[i].name);
        printf("Total:%d\n",total);
        printf("Average:%.2f\n",avg);
        printf("Grade:%c\n",grade);
        if(avg<35) continue;
        showperformance(grade);
        printf("\n");
        
    }
    printf("\nList of Rollno:");
    printrollno(s,0,n);
    printf("\n");
    


    return 0;
}
