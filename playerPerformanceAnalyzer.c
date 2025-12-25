#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "Players_data.h"

typedef struct PlayerNode {
    int playerId;
    char name[51];
    int teamId;
    Role role;
    int totalRuns;
    float battingAverage;
    float strikeRate;
    int wickets;
    float economyRate;
    float performanceIndex;
    struct PlayerNode *next;
}PlayerNode;

typedef struct TeamNode {
    int teamId;
    char name[51];
    int totalPlayers;
    float avgBattingStrikeRate;
    PlayerNode *playersHead;
}TeamNode;
typedef enum{
    BATSMAN=1, BOWLER, ALLROUNDER
}Role;
TeamNode teamsList[10];

Role getRoleFromStr(const char* str) {
    if(strcmp(str,"Batsman")==0) 
        return BATSMAN;
    else if(strcmp(str,"Bowler")==0) 
        return BOWLER;
    else 
        return ALLROUNDER;
}
void displayTeams() {
    printf("ID   Team Name\n");
    for (int i=0; i<teamCount; i++) {
        printf("%2d   %s\n", teamsList[i].teamId, teamsList[i].name);
    }
}
int getValidPlayerId(PlayerNode *head) {
    int id;
    while (1){
        int dup=0;
        printf("Enter Player ID: ");
        if(scanf("%d",&id)!=1){
            printf("Invalid! Enter numbers only.\n");
            while(getchar()!='\n');
            continue;
        }
        if(id<=0||id>1000){
            printf("ID must be between 1 and 1000.\n");
            continue;
        }
        PlayerNode *p=head;
        while(p){
            if(p->playerId==id)
            { 
                dup=1;
                break; 
            }
            p=p->next;
        }
        if(dup){
            printf("Player ID already exists! Enter a different one.\n");
            continue;
        }
        return id;
    }
}



int checkValidName(char *name){
    if(!name || strlen(name)==0 || strlen(name)>= 50){
        return 0;
    }    
    for(int i=0;name[i]!='\0'; i++){
        if(!isalpha(name[i]) && name[i]!=' '){
            return 0;
        }    
    }
    return 1;
}


float calculatePerformanceIndex(PlayerNode *p){
    if(p->role==BATSMAN){
        return (p->battingAverage*p->strikeRate)/100.0;
    }    
    else if(p->role==BOWLER){
        return (p->wickets*2)+(100-p->economyRate);
    }    
    else{
        return ((p->battingAverage*p->strikeRate)/100.0)+(p->wickets*2);
    }    
}

void initializeData(){
    for(int i=0;i<teamCount;i++){
        teamsList[i].teamId=i+1;
        strcpy(teamsList[i].name,teams[i]);
        teamsList[i].totalPlayers=0;
        teamsList[i].avgBattingStrikeRate=0;
        teamsList[i].playersHead=NULL;
    }
    for(int i=0;i<playerCount;i++){
        PlayerNode *newNode=(PlayerNode*)malloc(sizeof(PlayerNode));
        newNode->playerId=players[i].id;
        strcpy(newNode->name,players[i].name);
        newNode->teamId=0;
        for(int t=0;t<teamCount;t++){
            if(strcmp(players[i].team,teams[t])==0){
                newNode->teamId=teamsList[t].teamId;
                break;
            }
        }
        newNode->role=getRoleFromStr(players[i].role);
        newNode->totalRuns=players[i].totalRuns;
        newNode->battingAverage=players[i].battingAverage;
        newNode->strikeRate=players[i].strikeRate;
        newNode->wickets=players[i].wickets;
        newNode->economyRate=players[i].economyRate;
        newNode->performanceIndex=calculatePerformanceIndex(newNode);

        newNode->next=teamsList[newNode->teamId-1].playersHead;
        teamsList[newNode->teamId-1].playersHead=newNode;
        teamsList[newNode->teamId-1].totalPlayers++;
    }
    
    for(int i=0;i<teamCount;i++){
        int count=0;
        float sum=0.0;
        PlayerNode *ptr=teamsList[i].playersHead;
        while(ptr){
            if(ptr->role==BATSMAN || ptr->role==ALLROUNDER){
                sum+=ptr->strikeRate;
                count++;
            }
            ptr=ptr->next;
        }
        if(count>0){
            teamsList[i].avgBattingStrikeRate=sum/count;
        }
        else{
            teamsList[i].avgBattingStrikeRate=0;
        }
    }
}


void addPlayer() {
    int teamid;
    displayTeams();
    printf("Enter Team ID to add player (1-10): ");
    while (scanf("%d", &teamid) != 1 || teamid < 1 || teamid > 10){
        printf("Invalid input! Enter a valid Team ID (1-10):");
        while (getchar()!='\n');
    }
    if(teamsList[teamid-1].totalPlayers>=50){
        printf("Team has reached the maximum limit of 50 players.\n");
        return;
    }
    PlayerNode *newNode=(PlayerNode*)malloc(sizeof(PlayerNode));
    newNode->teamId=teamid;
    newNode->playerId = getValidPlayerId(teamsList[teamid-1].playersHead);
    printf("Enter Name: ");
    scanf(" %[^\n]", newNode->name);
    while (!checkValidName(newNode->name)) {
        printf("Invalid name! Enter again: ");
        scanf(" %[^\n]", newNode->name);
    }
    int role;
    printf("Enter Role (1-Batsman, 2-Bowler, 3-All-rounder): ");
    while (scanf("%d", &role)!= 1 || role<1 || role>3) {
        printf("Invalid input! Enter 1 (Batsman), 2 (Bowler), or 3 (All-rounder): ");
        while (getchar() != '\n');
    }
    newNode->role=role;
    printf("Total Runs:");
    while (scanf("%d",&newNode->totalRuns)!=1 || newNode->totalRuns<0){
        printf("Invalid! Enter valid number: ");
        while(getchar()!='\n');
    }
    printf("Batting Average:");
    while (scanf("%f",&newNode->battingAverage)!=1 || newNode->battingAverage<0){
        printf("Invalid! Enter valid number: ");
        while(getchar()!='\n');
    }
    printf("Strike Rate:");
    while (scanf("%f",&newNode->strikeRate)!=1 || newNode->strikeRate<0){
        printf("Invalid! Enter valid number: ");
        while(getchar()!='\n');
    }
    printf("Wickets:");
    while (scanf("%d",&newNode->wickets)!=1 || newNode->wickets<0){
        printf("Invalid! Enter valid number: ");
        while(getchar()!='\n');
    }
    printf("Economy Rate:");
    while (scanf("%f",&newNode->economyRate)!=1 || newNode->economyRate<0){
        printf("Invalid! Enter valid number: ");
        while(getchar()!='\n');
    }
    newNode->performanceIndex = calculatePerformanceIndex(newNode);
    newNode->next = teamsList[teamid-1].playersHead;
    teamsList[teamid-1].playersHead = newNode;
    teamsList[teamid-1].totalPlayers++;
    int count=0;
    float sum=0;
    PlayerNode *ptr=teamsList[teamid-1].playersHead;
    while(ptr){
        if(ptr->role==BATSMAN || ptr->role==ALLROUNDER){
            sum+=ptr->strikeRate;
            count++;
        }
        ptr=ptr->next;
    }
    if(count>0){
        teamsList[teamid-1].avgBattingStrikeRate=sum/count;
    }
    else{
        teamsList[teamid-1].avgBattingStrikeRate=0;
    }
    printf("Player added successfully to Team %s.\n",teamsList[teamid-1].name);
}


void displayTeamPlayers() {
    int teamid;
    displayTeams();
    printf("Enter Team ID: ");
    while (scanf("%d", &teamid) != 1 || teamid < 1 || teamid > 10){
        printf("Invalid input! Enter a valid Team ID (1-10):");
        while (getchar()!='\n');
    }
    TeamNode team=teamsList[teamid-1];
    printf("Players of Team %s:\n", team.name);
    printf("====================================================================================\n");
    printf("ID Name Role Runs Avg SR Wkts ER Perf.Index\n");
    printf("====================================================================================\n");
    PlayerNode *ptr=team.playersHead;
    while(ptr){
        printf("%d %s %s %d %.1f %.1f %d %.1f %.2f\n",ptr->playerId,ptr->name,(ptr->role==BATSMAN)?"Batsman":(ptr->role==BOWLER)?"Bowler":"All-Rounder",ptr->totalRuns,ptr->battingAverage,ptr->strikeRate,ptr->wickets,ptr->economyRate,ptr->performanceIndex);
        ptr=ptr->next;
    }
    printf("====================================================================================\n");
    printf("Total Players:%d\n", team.totalPlayers);
    printf("Average Batting Strike Rate:%.2f\n", team.avgBattingStrikeRate);
}


int compareTeams(const void *a, const void *b){
    TeamNode *t1=(TeamNode*)a;
    TeamNode *t2=(TeamNode*)b;
    if(t2->avgBattingStrikeRate > t1->avgBattingStrikeRate){
        return 1;
    }    
    else if(t2->avgBattingStrikeRate < t1->avgBattingStrikeRate){
        return -1;
    }
    else{ 
        return 0;
    }    
}
void displayTeamsByStrikeRate(){
    TeamNode temp[10];
    memcpy(temp, teamsList, sizeof(teamsList));
    qsort(temp, teamCount, sizeof(TeamNode), compareTeams);
    printf("Teams Sorted by Average Batting Strike Rate\n=========================================================\n");
    printf("ID Team Name Avg Bat SR Total Players\n=========================================================\n");
    for(int i=0;i<teamCount;i++){
        printf("%d %s %.1f %d\n", temp[i].teamId, temp[i].name, temp[i].avgBattingStrikeRate, temp[i].totalPlayers);
    }
    printf("=========================================================\n");
}


void displayTopKTeamPlayers() {
    int teamid, k, role;
    displayTeams();
    printf("Enter Team ID: ");
    while (scanf("%d",&teamid)!=1 || teamid<1 || teamid>10){
        printf("Invalid input! Enter a valid Team ID (1-10):");
        while (getchar()!='\n');
    }
    printf("Enter Role (1-Batsman, 2-Bowler, 3-All-rounder): ");
    while (scanf("%d",&role)!=1 || role<1 || role>3) {
        printf("Invalid input! Enter 1 (Batsman), 2 (Bowler), or 3 (All-rounder): ");
        while (getchar()!='\n');
    }
    printf("Enter number of players (1-50): ");
    while (scanf("%d",&k)!=1 || k<=0 || k>50) {
        printf("Invalid input! Enter a number between 1 and 50: ");
        while (getchar()!='\n');
    }
    PlayerNode *arr[50];
    int count=0;
    PlayerNode *ptr=teamsList[teamid-1].playersHead;
    while(ptr){
        if(ptr->role==role){
            arr[count++]=ptr;
        }    
        ptr=ptr->next;
    }
    for(int i=0;i<count-1;i++)
        for(int j=i+1;j<count;j++)
            if(arr[i]->performanceIndex<arr[j]->performanceIndex){
                PlayerNode *t=arr[i];
                arr[i]=arr[j];
                arr[j]=t;
            }
    printf("Top %d %s of Team %s:\n", k, (role==BATSMAN)?"Batsmen":(role==BOWLER)?"Bowlers":"All-rounders", teamsList[teamid-1].name);
    printf("====================================================================================\n");
    printf("ID Name Role Runs Avg SR Wkts ER Perf.Index\n");
    printf("====================================================================================\n");
    for(int i=0;i<k && i<count;i++){
        ptr=arr[i];
        printf("%d %s %s %d %.1f %.1f %d %.1f %.2f\n",ptr->playerId, ptr->name,(ptr->role==BATSMAN)?"Batsman":(ptr->role==BOWLER)?"Bowler":"All-Rounder",ptr->totalRuns, ptr->battingAverage, ptr->strikeRate,ptr->wickets, ptr->economyRate, ptr->performanceIndex);
    }
}

void displayAllPlayersByRole() {
    int role;
    printf("Enter Role (1-Batsman, 2-Bowler, 3-All-rounder): ");
    while (scanf("%d", &role)!=1 || role<1 || role>3) {
        printf("Invalid input! Enter 1 (Batsman), 2 (Bowler), or 3 (All-rounder): ");
        while (getchar()!='\n');
    }
    PlayerNode *arr[playerCount];
    int count=0;
    for(int i=0;i<teamCount;i++){
        PlayerNode *ptr=teamsList[i].playersHead;
        while(ptr){
            if(ptr->role==role){
                arr[count++]=ptr;
            }    
            ptr=ptr->next;
        }
    }

    for(int i=0;i<count-1;i++)
        for(int j=i+1;j<count;j++)
            if(arr[i]->performanceIndex<arr[j]->performanceIndex){
                PlayerNode *t=arr[i];
                arr[i]=arr[j];
                arr[j]=t;
            }
    printf("All %s of all teams:\n",(role==BATSMAN)?"Batsmen":(role==BOWLER)?"Bowlers":"All-rounders");
    printf("======================================================================================\n");
    printf("ID Name Team Role Runs Avg SR Wkts ER Perf.Index\n");
    printf("======================================================================================\n");
    for(int i=0;i<count;i++){
        PlayerNode *p = arr[i];
        printf("%d %s %s %s %d %.1f %.1f %d %.1f %.2f\n",p->playerId, p->name, teamsList[p->teamId-1].name,(p->role==BATSMAN)?"Batsman":(p->role==BOWLER)?"Bowler":"All-Rounder",p->totalRuns, p->battingAverage, p->strikeRate,p->wickets, p->economyRate, p->performanceIndex);
    }
}

int main(){
    initializeData();
    int choice;
    while(1){
        printf("\n==============================================================================\n");
        printf("ICC ODI Player Performance Analyzer\n");
        printf("==============================================================================\n");
        printf("1. Add Player to Team\n");
        printf("2. Display Players of a Specific Team\n");
        printf("3. Display Teams by Average Batting Strike Rate\n");
        printf("4. Display Top K Players of a Specific Team by Role\n");
        printf("5. Display all Players of specific role Across All Teams by performance index\n");
        printf("6. Exit\n");
        printf("==============================================================================\n");
        printf("Enter your choice: ");
        while (scanf("%d", &choice) != 1 || choice<1 || choice>6){
            printf("Invalid input!Enter number between 1 and 6:");
            while (getchar()!='\n');
        }
        switch(choice){
            case 1: addPlayer(); 
                    break;
            case 2: displayTeamPlayers(); 
                    break;
            case 3: displayTeamsByStrikeRate(); 
                    break;
            case 4: displayTopKTeamPlayers(); 
                    break;
            case 5: displayAllPlayersByRole(); 
                    break;
            case 6: exit(0);
            default: printf("Invalid choice\n");
        }
    }
    return 0;
}
