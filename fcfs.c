#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_NAME 100
#define HASH_BUCKETS 11

typedef enum State{
    READY,
    RUNNING,
    WAITING,
    FINISHED,
    TERMINATED_BY_USER
}State;

typedef struct PCB{
    char name[MAX_NAME];
    int burst;
    int remainingBurst;
    int ioStart;
    int ioLength;
    int ioLeft;
    int executedCPU;
    State state;
    int turnaround;
    int waiting;
    bool ioStartedTick;
}PCB;

typedef struct HashEntry{
    int pid;
    PCB* process;
    struct HashEntry* next;
}HashEntry;

typedef struct HashTable{
    HashEntry* table[HASH_BUCKETS];
}HashTable;

typedef struct QNode{
    int pid;
    struct QNode* next;
} QNode;

typedef struct Queue{
    QNode* head;
    QNode* tail;
}Queue;

typedef struct KillEvent{
    int pid;
    int tick;
}KillEvent;

HashTable* createHashTable(){
    HashTable* ht=malloc(sizeof(HashTable));
    if(!ht){
        printf("Memory Error!\n");
        exit(1);
    }
    for(int i=0;i<HASH_BUCKETS;i++){
        ht->table[i]=NULL;
    }
    return ht;
}

int hashFunc(int pid){
    return (pid%HASH_BUCKETS+HASH_BUCKETS)%HASH_BUCKETS;
}

void insertHash(HashTable* ht,int pid,PCB* pcb){
    int idx=hashFunc(pid);
    HashEntry* node=malloc(sizeof(HashEntry));
    node->pid=pid;
    node->process=pcb;
    node->next=ht->table[idx];
    ht->table[idx]=node;
}

PCB* getHash(HashTable* ht,int pid){
    int idx=hashFunc(pid);
    HashEntry* curr=ht->table[idx];
    while(curr){
        if(curr->pid == pid){
            return curr->process;
        }
        curr=curr->next;
    }
    return NULL;
}

void freeHashTable(HashTable* ht){
    for(int i=0;i<HASH_BUCKETS;i++) {
        HashEntry* curr=ht->table[i];
        while(curr){
            HashEntry* tmp=curr;
            curr=curr->next;
            free(tmp->process);
            free(tmp);
        }
    }
    free(ht);
}

Queue* initQueue(){
    Queue* q=malloc(sizeof(Queue));
    q->head=NULL;
    q->tail=NULL;
    return q;
}

QNode* createQNode(int pid){
    QNode* node=malloc(sizeof(QNode));
    node->pid=pid;
    node->next=NULL;
    return node;
}

void enqueue(Queue* q, int pid){
    QNode* node=createQNode(pid);
    if(!q->head){
        q->head=node;
        q->tail=node;
    }else{
        q->tail->next=node;
        q->tail=node;
    }
}

int dequeue(Queue* q){
    if(!q->head){
        return -1;
    }
    QNode* tmp=q->head;
    int pid=tmp->pid;
    q->head=q->head->next;
    if(!q->head){
        q->tail=NULL;
    }
    free(tmp);
    return pid;
}

bool removeFromQueue(Queue* q,int pid){
    QNode* curr=q->head;
    QNode* prev=NULL;
    while(curr){
        if(curr->pid==pid){
            if(!prev){
                q->head=curr->next;
            }else{
                prev->next=curr->next;
            }
            if(q->tail==curr){
                q->tail=prev;
            }
            free(curr);
            return true;
        }
        prev=curr;
        curr=curr->next;
    }
    return false;
}

bool emptyQueue(Queue* q){
    if(!q || !q->head){
        return true;
    }
    return false;
}

void freeQueue(Queue* q){
    while(!emptyQueue(q)){
        dequeue(q);
    }
    free(q);
}

PCB* createPCB(){
    PCB* pcb=malloc(sizeof(PCB));
    pcb->executedCPU=0;
    pcb->turnaround=0;
    pcb->waiting=0;
    pcb->ioLeft=0;
    pcb->ioStartedTick=false;
    pcb->state=READY;
    return pcb;
}

KillEvent* createKillList(int count) {
    KillEvent* kills=malloc(sizeof(KillEvent)*count);
    for (int i=0;i<count;i++) {
        kills[i].pid=-1;
        kills[i].tick=-1;
    }
    return kills;
}

void moveToWaiting(PCB* pcb,int pid,Queue* waitQ){
    pcb->state=WAITING;
    pcb->ioLeft=pcb->ioLength;
    pcb->ioStartedTick=true;
    enqueue(waitQ,pid);
}

void moveToFinished(PCB* pcb,int pid,int tick,Queue* finishedQ){
    pcb->turnaround=tick;
    pcb->waiting=pcb->turnaround-pcb->burst;
    pcb->state=FINISHED;
    enqueue(finishedQ, pid);
}

void terminateProcess(PCB* pcb,int pid,Queue* finishedQ){
    pcb->turnaround=-1;
    pcb->waiting=-1;
    pcb->state=TERMINATED_BY_USER;
    enqueue(finishedQ, pid);
}

void updateWaitingQueue(Queue* waitQ,Queue* readyQ,HashTable* ht){
    QNode* curr=waitQ->head;
    QNode* prev=NULL;
    while(curr){
        PCB* pcb=getHash(ht,curr->pid);
        if(!pcb){
            prev=curr;
            curr=curr->next;
        }else{
            if(pcb->ioStartedTick){
                pcb->ioStartedTick=false;
                prev=curr;
                curr=curr->next;
            }else{
                pcb->ioLeft--;
                if(pcb->ioLeft<=0){
                    QNode* tmp=curr;
                    if(!prev){
                        waitQ->head=curr->next;
                    }else{
                        prev->next=curr->next;
                    }
                    if(waitQ->tail==curr){
                        waitQ->tail=prev;
                    }
                    curr=curr->next;
                    pcb->state=READY;
                    enqueue(readyQ, tmp->pid);
                    free(tmp);
                }else{
                    prev=curr;
                    curr=curr->next;
                }
            }
        }
    }
}

void handleKillEvents(KillEvent* kills,int count,int tick,HashTable* ht,Queue* readyQ,Queue* waitQ, Queue* finishedQ,int* runningPID,int* finishedCount){
    for (int i=0;i<count;i++){
        if(kills[i].tick==tick){
            PCB* pcb=getHash(ht,kills[i].pid);
            if(pcb && pcb->state!=FINISHED && pcb->state!=TERMINATED_BY_USER){
                if(*runningPID==kills[i].pid) {
                    terminateProcess(pcb,kills[i].pid,finishedQ);
                    (*finishedCount)++;
                    *runningPID=-1;
                }else{
                    if(removeFromQueue(readyQ,kills[i].pid) || removeFromQueue(waitQ,kills[i].pid)){
                        terminateProcess(pcb,kills[i].pid,finishedQ);
                        (*finishedCount)++;
                    }
                }
            }
        }
    }
}

void fcfs(HashTable* ht,Queue* readyQ,int total,KillEvent* kills,int killCount){
    Queue* waitQ=initQueue();
    Queue* finishedQ=initQueue();
    int tick=0;
    int runningPID=-1;
    int finished=0;

    while(finished<total){
        handleKillEvents(kills,killCount,tick,ht,readyQ,waitQ,finishedQ,&runningPID,&finished);
        if (runningPID==-1) {
            if (!emptyQueue(readyQ)){
                runningPID=dequeue(readyQ);
                PCB* pcb=getHash(ht, runningPID);
                if(pcb){
                    pcb->state=RUNNING;
                }
            }
        }

        if(runningPID!=-1){
            PCB* pcb=getHash(ht,runningPID);
            if(pcb){
                pcb->executedCPU++;
                pcb->remainingBurst--;
                if(pcb->ioStart >= 0){
                    if(pcb->executedCPU==pcb->ioStart){
                        if(pcb->remainingBurst>0) {
                            moveToWaiting(pcb, runningPID, waitQ);
                            runningPID=-1;
                        }
                    }
                }
                if(pcb->remainingBurst<=0){
                    moveToFinished(pcb,runningPID,tick+1,finishedQ);
                    finished++;
                    runningPID=-1;
                }
            }
        }
        updateWaitingQueue(waitQ,readyQ,ht);
        tick++;
    }

    printf("\n%-8s%-15s%-10s%-10s%-15s%-12s%-10s\n", "PID", "Name", "CPU", "IO", "Turnaround", "Waiting", "Status");
    // printf("-------------------------------------------------------------\n");

    for (int i=0;i<HASH_BUCKETS;i++) {
        HashEntry* node=ht->table[i];
        while(node){
            PCB* pcb=node->process;
            const char* status;
            if(pcb->state==TERMINATED_BY_USER) {
                status="TERMINATED";
            }else{
                status="OK";
            }
            printf("%-8d%-15s%-10d%-10d%-15d%-12d%-10s\n", node->pid, pcb->name, pcb->burst, pcb->ioLength, pcb->turnaround, pcb->waiting, status);
            node=node->next;
        }
    }
    freeQueue(waitQ);
    freeQueue(finishedQ);
}

void readProcesses(HashTable* ht,Queue* readyQ,int* total){
    printf("Number of processes: ");
    scanf("%d", total);

    for(int i=0;i<*total;i++){
        printf("\nProcess %d details:\n",i+1);
        PCB* pcb=createPCB();
        printf("Name: ");
        while(getchar()!='\n');
        fgets(pcb->name, sizeof(pcb->name), stdin);
        pcb->name[strcspn(pcb->name,"\n")]=0;
        int pid;
        printf("PID:");
        scanf("%d",&pid);
        printf("Burst:");
        scanf("%d",&pcb->burst);
        pcb->remainingBurst=pcb->burst;
        printf("IO Start(-1 for none):");
        scanf("%d", &pcb->ioStart);
        printf("IO Duration:");
        scanf("%d",&pcb->ioLength);

        if(pcb->ioStart<0 || pcb->ioLength<=0){
            pcb->ioStart=-1;
            pcb->ioLength=0;
        }

        insertHash(ht,pid,pcb);
        enqueue(readyQ,pid);
    }
}

void readKillEventsInput(KillEvent** kills,int* count){
    printf("Number of kill events:");
    scanf("%d",count);

    if(*count>0){
        *kills=createKillList(*count);
        for(int i = 0;i<*count;i++){
            printf("Kill event %d PID: ",i+1);
            scanf("%d",&(*kills)[i].pid);
            printf("Time:");
            scanf("%d",&(*kills)[i].tick);
        }
    }
}

int main(){
    HashTable* ht=createHashTable();
    Queue* readyQ=initQueue();
    int totalProcesses=0;
    readProcesses(ht,readyQ,&totalProcesses);
    KillEvent* kills=NULL;
    int killCount=0;
    readKillEventsInput(&kills,&killCount);
    fcfs(ht,readyQ,totalProcesses,kills,killCount);
    if(kills){
        free(kills);
    }
    freeQueue(readyQ);
    freeHashTable(ht);
    return 0;
}
