#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX 1000

typedef struct Node{
    int key;
    char data[100];
    struct Node* prev;
    struct Node* next;
}Node;

typedef struct Queue{
    Node* head;
    Node* last;
    int size;
}Queue;

Queue* createList(){
    Queue* queue=(Queue*)malloc(sizeof(Queue));
    if(!queue){
        printf("Memory allocation failed!");
        exit(1);
    }
    queue->head=NULL;
    queue->last=NULL;
    queue->size=0;
    return queue;
}
Queue* cacheQueue=NULL;
Node* hashtable[MAX];
int cacheCapacity=0;
int cacheSize=0;

Node* createNode(int key,const char* data){
    Node* node=(Node*)malloc(sizeof(Node));
    if(!node){
        printf("Memory allocation failed!");
        exit(1);
    }
    node->key=key;
    strncpy(node->data,data,sizeof(node->data)-1);
    node->data[sizeof(node->data)-1]='\0';
    node->prev=NULL;
    node->next=NULL;
    return node;
}

void createCache(int capacity){
    cacheQueue=createList();
    cacheCapacity=capacity;
    cacheSize=0;
    for(int i=0;i<MAX;i++){
        hashtable[i]=NULL;
    }
}
void addNode(Queue* queue,Node* node){
    node->prev=NULL;
    node->next=queue->head;
    if(queue->head!=NULL){
        queue->head->prev=node;
    }
    queue->head=node;
    if(queue->last==NULL){
        queue->last=node;
    }
    queue->size++;
}
void removeNode(Queue* queue,Node* node){
    if(node->prev!=NULL){
        node->prev->next=node->next;
    }
    else{
        queue->head=node->next;
    }    
    if(node->next!=NULL){
        node->next->prev=node->prev;
    }
    else{
        queue->last=node->prev;
    }
    node->next=NULL;
    node->prev=NULL;
    queue->size--;
}
Node* removeLast(Queue* queue){
    if(queue->last==NULL){
        return NULL;
    }
    Node* node=queue->last;
    queue->last=node->prev;
    if(queue->last!=NULL){
        queue->last->next=NULL;
    }    
    else{
        queue->head=NULL;
    }    
    node->prev=NULL;
    node->next=NULL;
    queue->size--;
    return node;
}
void put(int key,const char* data){
    if(hashtable[key]!=NULL){
        Node* node=hashtable[key];
        strncpy(node->data,data,sizeof(node->data)-1);
        node->data[sizeof(node->data)-1]='\0';

        removeNode(cacheQueue,node);
        addNode(cacheQueue,node);
        return;
    }
    else if(hashtable[key]==NULL){
        if(cacheCapacity==cacheSize){
            Node* node1=removeLast(cacheQueue);
            hashtable[node1->key]=NULL;
            free(node1);
            cacheSize--;
        }
        Node* node=createNode(key, data);
        addNode(cacheQueue,node);
        hashtable[key]=node;
        cacheSize++;
    }
}

char* get(int key){
    if(hashtable[key]==NULL){
        return "NULL";
    }
    Node* node=hashtable[key];
    removeNode(cacheQueue,node);
    addNode(cacheQueue,node);
    return node->data;
}
int readIntOnly() {
    char input[100];
    while(1){
        scanf("%s",input);
        int valid=1;
        for (int i=0;input[i]!='\0';i++){
            if(input[i]<'0' || input[i]>'9'){
                valid=0;
                break;
            }
        }
        if(valid==0){
            printf("Invalid.Enter only numbers:");
            continue;
        }
        int value=atoi(input);
        if (value<0 || value>MAX) {
            printf("Out of range.Enter again(0-999):");
            continue;
        }
        return value;
    }
}

int main(){
    int key;
    char data[50];
    while(1){
        printf("Enter option(\"createCache\"):");
        char option[20];
        scanf("%s",option);
        if(strcmp(option,"createCache")==0){
            int capacity;
            printf("Enter capacity:");
            capacity=readIntOnly();
            createCache(capacity);
            break;
        }    
        printf("\nInvalid.Create Cache first using \"createCache\"\n");
    }    
    while(1){
        printf("\nEnter option(put,get,exit):");
        char option[11];
        scanf("%s",option);
        if(strcmp(option,"put")==0){
            printf("\nEnter key:");
            key=readIntOnly();
            printf("Enter data:");
            scanf("%s",data);
            put(key,data);
        }
        else if(strcmp(option,"get")==0){
            printf("\nEnter key:");
            key=readIntOnly();
            char* value=get(key);
            printf("%s\n",value);
        }  
        else if(strcmp(option,"exit")==0){
            break;
        }
        else{
            printf("Invalid.Enter valid option");
        }  
    }
    return 0;
    
}
