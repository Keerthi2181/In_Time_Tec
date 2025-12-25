#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
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

typedef struct HashNode{
    int key;
    Node* value;
    struct HashNode* next;
}HashNode;
HashNode* hashtable[MAX];

int hash(int key){
    return key%MAX;
}
void clearInputBuffer() {
    int c;
    while((c = getchar()) != '\n' && c != EOF);
}
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
void insertHash(int key,Node* value){
    int index=hash(key);
    HashNode* temp=hashtable[index];
    while(temp!=NULL){
        if(temp->key==key){
            temp->value=value;
            return;
        }
        temp=temp->next;
    }
    HashNode* newNode=malloc(sizeof(HashNode));
    newNode->key=key;
    newNode->value=value;
    newNode->next=hashtable[index];
    hashtable[index]=newNode;
}
Node* getHash(int key) {
    int index=hash(key);
    HashNode* temp=hashtable[index];

    while(temp!=NULL) {
        if(temp->key==key){
            return temp->value;
        }    
        temp=temp->next;
    }
    return NULL;
}
void deleteHash(int key) {
    int index=hash(key);
    HashNode* temp=hashtable[index];
    HashNode* prev=NULL;

    while(temp!=NULL){
        if(temp->key==key) {
            if(prev==NULL){
                hashtable[index]=temp->next;
            }   
            else{
                prev->next=temp->next;
            }
            free(temp);
            return;
        }
        prev=temp;
        temp=temp->next;
    }
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
    Node* node1=getHash(key);
    if(node1 != NULL){
        strncpy(node1->data,data,sizeof(node1->data)-1);
        node1->data[sizeof(node1->data)-1]='\0';
        removeNode(cacheQueue,node1);
        addNode(cacheQueue,node1);
        return;
    }
    if(cacheCapacity==cacheSize){
        Node* old=removeLast(cacheQueue);
        deleteHash(old->key);
        free(old);
        cacheSize--;
    }
    Node* node=createNode(key,data);
    addNode(cacheQueue,node);
    insertHash(key,node);
    cacheSize++;
}
char* get(int key){
    Node* node=getHash(key);
    if(node==NULL)
        return "NULL";
    removeNode(cacheQueue,node);
    addNode(cacheQueue,node);
    return node->data;
}
int readCapacity(){
    char input[100];
    while(1){
        scanf(" %s",input);
        clearInputBuffer(); 
        int valid=1;
        for(int i=0;input[i]!='\0';i++){
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
        if(value<0 || value>MAX) {
            printf("Out of range.Enter again(0-999):");
            continue;
        }
        return value;
    }
}
int readKey(){
    char input[100];
    while(1){
        scanf(" %s",input);
        clearInputBuffer(); 
        int valid=1;
        for(int i=0;input[i]!='\0';i++){
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
        if(value<=0) {
            printf("Invalid.Enter number greater than 0:");
            continue;
        }
        return value;
    }
}
void readLine(char *buffer,int size) {
    fgets(buffer,size,stdin);
    buffer[strcspn(buffer,"\n")]='\0';
}
int checkValidCommand(const char *str) {
    if(str==NULL || strlen(str)==0){
        return 0;
    }
    for(int i=0;str[i]!='\0';i++) {
        if (!isalpha(str[i])){
            return 0;
        }    
    }
    return 1;
}
int main(){
    int key;
    char data[50];
    char option[50];
    while(1){
        printf("Enter option (createCache):");
        readLine(option, sizeof(option));

        if(!checkValidCommand(option)){
            printf("Invalid.Create Cache first using \"createCache\"\n");
            continue;
        }
        if(strcmp(option,"createCache")==0){
            int capacity;
            printf("Enter capacity:");
            capacity=readCapacity();
            createCache(capacity);
            break;
        }    
        printf("\nInvalid.Create Cache first using \"createCache\"\n");
    }    
    while(1){
        printf("Enter option (put/get/exit): ");
        readLine(option, sizeof(option));
        if(!checkValidCommand(option)){
            printf("Invalid\n");
            continue;
        }
        if(strcmp(option,"put")==0){
            printf("\nEnter key:");
            key=readKey();
            printf("Enter data:");
            scanf(" %s",data);
            clearInputBuffer();
            put(key,data);
        }
        else if(strcmp(option,"get")==0){
            printf("\nEnter key:");
            key=readKey();
            char* value=get(key);
            printf("%s\n",value);
        }  
        else if(strcmp(option,"exit")==0){
            break;
        }
        else {
            printf("Invalid\n");
        }
  
    }
    return 0;
    
}
