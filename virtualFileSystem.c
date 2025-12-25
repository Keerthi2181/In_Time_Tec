#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define BLOCK_SIZE 512
#define NUM_BLOCKS 1024
#define MAX_NAME_LEN 50
#define MAX_BLOCKS_PER_FILE NUM_BLOCKS
#define INPUT_BUFFER_SIZE 2048
#define MAX_DEPTH 1024

static unsigned char virtualDisk[NUM_BLOCKS][BLOCK_SIZE];

typedef struct FreeBlock 
{
    int blockIndex;
    struct FreeBlock *prev;
    struct FreeBlock *next;
}FreeBlock;

static FreeBlock *freeBlockHead=NULL;
static FreeBlock *freeBlockTail=NULL;
static int totalFreeBlocks=0;

static void initializeFreeBlocks() 
{
    for(int index=0;index<NUM_BLOCKS;++index) 
    {
        FreeBlock *newFreeBlock=(FreeBlock *)malloc(sizeof(FreeBlock));
        newFreeBlock->blockIndex=index;
        newFreeBlock->prev=freeBlockTail;
        newFreeBlock->next=NULL;
        if(freeBlockTail) 
            freeBlockTail->next=newFreeBlock;
        freeBlockTail=newFreeBlock;
        if(!freeBlockHead) 
            freeBlockHead=newFreeBlock;
        totalFreeBlocks++;
    }
}

static int allocateFreeBlockFromHead() 
{
    if(!freeBlockHead) 
        return -1;
    FreeBlock *takenBlock=freeBlockHead;
    int index=takenBlock->blockIndex;
    freeBlockHead=takenBlock->next;
    if (freeBlockHead) 
        freeBlockHead->prev=NULL;
    else 
        freeBlockTail=NULL;
    free(takenBlock);
    totalFreeBlocks--;
    return index;
}

static void appendFreeBlockToTail(int blockIndex) 
{
    FreeBlock *newFreeBlock=(FreeBlock *)malloc(sizeof(FreeBlock));
    newFreeBlock->blockIndex=blockIndex;
    newFreeBlock->next=NULL;
    newFreeBlock->prev=freeBlockTail;
    if(freeBlockTail) 
        freeBlockTail->next=newFreeBlock;
    freeBlockTail = newFreeBlock;
    if(!freeBlockHead) 
        freeBlockHead=newFreeBlock;
    totalFreeBlocks++;
}

static void freeFreeBlockList() 
{
    FreeBlock *walker=freeBlockHead;
    while(walker) {
        FreeBlock *nextBlock=walker->next;
        free(walker);
        walker=nextBlock;
    }
    freeBlockHead=freeBlockTail=NULL;
    totalFreeBlocks=0;
}

static int allocateNBlocksTemp(int n, int destIndices[]) 
{
    if(n<=0) 
        return 1;
    if(n>totalFreeBlocks) 
        return 0;
    int allocatedCount = 0;
    for(int index=0; index<n; ++index) 
    {
        int blockIndex = allocateFreeBlockFromHead();
        if(blockIndex<0) 
        {
            for(int indexj=0; indexj<allocatedCount; ++indexj) 
                appendFreeBlockToTail(destIndices[indexj]);
            return 0;
        }
        destIndices[allocatedCount++]=blockIndex;
    }
    return 1;
}
 
typedef struct FileNode 
{
    char name[MAX_NAME_LEN + 1];
    int isDirectory;
    struct FileNode *parent;
    struct FileNode *childHead;
    struct FileNode *siblingNext;
    struct FileNode *siblingPrev;
    int fileSizeBytes;
    int blocksAllocated;
    int blockPointers[MAX_BLOCKS_PER_FILE];
} FileNode;

static FileNode *rootDirectory=NULL;
static FileNode *currentWorkingDirectory=NULL;

static FileNode* createFilerNode(const char *name, int isDirectoryFlag, FileNode *parentDirectory) 
{
    FileNode *newNode=(FileNode *)malloc(sizeof(FileNode));
    strncpy(newNode->name, name, MAX_NAME_LEN);
    newNode->name[MAX_NAME_LEN]='\0';
    newNode->isDirectory = isDirectoryFlag;
    newNode->parent = parentDirectory;
    newNode->childHead = NULL;
    newNode->siblingNext = newNode->siblingPrev = NULL;
    newNode->fileSizeBytes = 0;
    newNode->blocksAllocated = 0;
    for (int index = 0; index < MAX_BLOCKS_PER_FILE; ++index) 
        newNode->blockPointers[index] = -1;
    return newNode;
}
 
static void addChildToDirectory(FileNode *directoryNode, FileNode *childNode) 
{
    if(!directoryNode || !directoryNode->isDirectory) 
        return;
    if(!directoryNode->childHead) 
    {
        directoryNode->childHead = childNode;
        childNode->siblingNext = childNode->siblingPrev = childNode;
    } 
    else 
    {
        FileNode *headChild = directoryNode->childHead;
        FileNode *tailChild = headChild->siblingPrev;
        tailChild->siblingNext = childNode;
        childNode->siblingPrev = tailChild;
        childNode->siblingNext = headChild;
        headChild->siblingPrev = childNode;
    }
}

static int removeChildFromDirectory(FileNode *directoryNode, FileNode *childNode) 
{
    if(!directoryNode || !directoryNode->childHead || !childNode) 
        return 0;
    FileNode *headChild = directoryNode->childHead;
    FileNode *walker = headChild;
    do 
    {
        if(walker == childNode) 
        {
            if (walker->siblingNext == walker) 
            {
                directoryNode->childHead = NULL;
            } 
            else 
            {
                walker->siblingPrev->siblingNext = walker->siblingNext;
                walker->siblingNext->siblingPrev = walker->siblingPrev;
                if(directoryNode->childHead == walker) 
                    directoryNode->childHead = walker->siblingNext;
            }
            walker->siblingNext = walker->siblingPrev = NULL;
            return 1;
        }
        walker = walker->siblingNext;
    }while(walker != headChild);
    return 0;
}

static FileNode* findChildInDirectory(FileNode *directoryNode, const char *name) 
{
    if(!directoryNode || !directoryNode->childHead) 
        return NULL;
    FileNode *headChild = directoryNode->childHead;
    FileNode *walker = headChild;
    do 
    {
        if (strcmp(walker->name, name) == 0) 
            return walker;
        walker = walker->siblingNext;
    } while (walker != headChild);
    return NULL;
}

static void freeFilerNodeMemory(FileNode *node) 
{
    if(!node) 
        return;
    free(node);
}

static int computeBlocksNeeded(int byteCount) 
{
    if(byteCount <= 0) 
        return 0;
    return(byteCount + BLOCK_SIZE - 1) / BLOCK_SIZE;
}

static void freeBlocksOfFile(FileNode *fileNode) 
{
    if(!fileNode || fileNode->isDirectory) 
        return;
    for(int blockIndex = 0; blockIndex < fileNode->blocksAllocated; ++blockIndex) 
    {
        int freedBlockIndex = fileNode->blockPointers[blockIndex];
        if(freedBlockIndex >= 0) 
        {
            appendFreeBlockToTail(freedBlockIndex);
            fileNode->blockPointers[blockIndex] = -1;
        }
    }
    fileNode->blocksAllocated = 0;
    fileNode->fileSizeBytes = 0;
}

static void readFileAndPrint(FileNode *fileNode) 
{
    if(!fileNode || fileNode->isDirectory) 
    {
        printf("File not found.\n");
        return;
    }
    if(fileNode->fileSizeBytes == 0 || fileNode->blocksAllocated == 0) 
    {
        printf("(empty)\n");
        return;
    }

    int bytesRemaining = fileNode->fileSizeBytes;
    for(int index = 0; index < fileNode->blocksAllocated && bytesRemaining > 0; ++index) 
    {
        int blockIndex = fileNode->blockPointers[index];
        int bytesToRead = (bytesRemaining < BLOCK_SIZE) ? bytesRemaining : BLOCK_SIZE;
        fwrite(virtualDisk[blockIndex], 1, bytesToRead, stdout);
        bytesRemaining -= bytesToRead;
    }
    printf("\n");
}

static void showHelp()
{
    printf("VFS COMMAND HELP\n");
    printf("help                          -Show help menu\n");
    printf("mkdir <dirname>               -Create a new directory\n");
    printf("rmdir <dirname>               -Remove an empty directory\n");
    printf("create <filename>             -Create a new empty file\n");
    printf("delete <filename>             -Delete an existing file\n");
    printf("write <filename> <data>       -Write data to a file\n");
    printf("read <filename>               -Display contents of a file\n");
    printf("ls                            -List items in the current directory\n");
    printf("cd <dirname>                  -Change directory\n");
    printf("cd ..                         -Move to previous directory\n");
    printf("pwd                           -Show current directory path\n");
    printf("df                            -Display disk usage information\n");
    printf("exit                          -Exit the virtual file system\n");
}

static void cmdMkdir(const char *directoryName) 
{
    if(!directoryName || strlen(directoryName) == 0) 
    {
        printf("ERROR:Invalid directory name.\n");
        return;
    }
    if(findChildInDirectory(currentWorkingDirectory, directoryName)) 
    {
        printf("ERROR:Name already exists in current directory.\n");
        return;
    }
    FileNode *newDirectory = createFilerNode(directoryName, 1, currentWorkingDirectory);
    addChildToDirectory(currentWorkingDirectory, newDirectory);
    printf("LOG:Directory '%s' created successfully.\n", directoryName);
}


static void cmdRmdir(const char *directoryName) 
{
    if(!directoryName || strlen(directoryName) == 0) 
    {
        printf("ERROR:Invalid directory name.\n");
        return;
    }
    FileNode *targetDirectory = findChildInDirectory(currentWorkingDirectory, directoryName);
    if(!targetDirectory) 
    {
        printf("ERROR:Directory not found.\n");
        return;
    }
    if(!targetDirectory->isDirectory) 
    {
        printf("ERROR:Not a directory.\n");
        return;
    }
    if(targetDirectory->childHead != NULL) 
    {
        printf("WARN:Directory not empty. Remove files first.\n");
        return;
    }
    removeChildFromDirectory(currentWorkingDirectory, targetDirectory);
    freeFilerNodeMemory(targetDirectory);
    printf("LOG:Directory removed successfully.\n");
}


static void cmdCreate(const char *fileName) 
{
    if(!fileName || strlen(fileName) == 0) 
    {
        printf("ERROR:Invalid file name.\n");
        return;
    }
    if(findChildInDirectory(currentWorkingDirectory, fileName)) 
    {
        printf("ERROR:Name already exists in current directory.\n");
        return;
    }
    FileNode *newFile = createFilerNode(fileName, 0, currentWorkingDirectory);
    addChildToDirectory(currentWorkingDirectory, newFile);
    printf("LOG:File '%s' created successfully.\n", fileName);
}


static void cmdDelete(const char *fileName) 
{
    if(!fileName || strlen(fileName) == 0) 
    {
        printf("ERROR:Invalid file name.\n");
        return;
    }
    FileNode *targetFile = findChildInDirectory(currentWorkingDirectory, fileName);
    if(!targetFile) 
    {
        printf("ERROR:File not found.\n");
        return;
    }
    if(targetFile->isDirectory) 
    {
        printf("WARN:Is a directory. Use rmdir to remove directories.\n");
        return;
    }
    freeBlocksOfFile(targetFile);
    removeChildFromDirectory(currentWorkingDirectory, targetFile);
    freeFilerNodeMemory(targetFile);
    printf("ERROR:File deleted successfully.\n");
}

static void cmdWrite(const char *fileName, const char *contentString) 
{
    if(!fileName) 
    { 
        printf("ERROR:Invalid file name.\n"); 
        return; 
    }
    FileNode *targetFile = findChildInDirectory(currentWorkingDirectory, fileName);
    if(!targetFile) 
    { 
        printf("ERRR:File not found.\n"); 
        return; 
    }
    if(targetFile->isDirectory) 
    { 
        printf("ERROR:Cannot write to directory.\n"); 
        return; 
    }

    int contentLength = (contentString ? (int)strlen(contentString) : 0);
    if(contentLength == 0) 
    {
        printf("LOG:Data written successfully (size=0 bytes).\n");
        return;
    }

    int existingSize = targetFile->fileSizeBytes;
    int usedInLastBlock = (existingSize == 0) ? 0 : (existingSize % BLOCK_SIZE);
    int freeInLastBlock = (usedInLastBlock == 0) ? 0 : (BLOCK_SIZE - usedInLastBlock);

    int bytesRemaining = contentLength;
    int contentOffset = 0;

    int remainingAfterFill = bytesRemaining;
    if(freeInLastBlock > 0) 
    {
        int willFill = (bytesRemaining < freeInLastBlock) ? bytesRemaining : freeInLastBlock;
        remainingAfterFill -= willFill;
    }
    int blocksNeeded = computeBlocksNeeded(remainingAfterFill);

    int *newBlockIndices = NULL;
    if(blocksNeeded > 0) 
    {
        newBlockIndices = (int *)malloc(sizeof(int) * blocksNeeded);
        if (!allocateNBlocksTemp(blocksNeeded, newBlockIndices)) 
        {
            free(newBlockIndices);
            printf("ERROR:Disk full. Write failed.\n");
            return;
        }
    }


    if (freeInLastBlock > 0 && bytesRemaining > 0 && targetFile->blocksAllocated > 0) 
    {
        int lastBlockIndex = targetFile->blockPointers[targetFile->blocksAllocated - 1];
        int toWrite = (bytesRemaining < freeInLastBlock) ? bytesRemaining : freeInLastBlock;
        memcpy(virtualDisk[lastBlockIndex] + usedInLastBlock, contentString + contentOffset, toWrite);
        contentOffset += toWrite;
        bytesRemaining -= toWrite;
    }


    for (int blockIndex = 0; blockIndex < blocksNeeded && bytesRemaining > 0; ++blockIndex) 
    {
        int assignedBlockIndex = newBlockIndices[blockIndex];
        int toWrite = (bytesRemaining < BLOCK_SIZE) ? bytesRemaining : BLOCK_SIZE;
        memcpy(virtualDisk[assignedBlockIndex], contentString + contentOffset, toWrite);
        if (toWrite < BLOCK_SIZE) 
            memset(virtualDisk[assignedBlockIndex] + toWrite, 0, BLOCK_SIZE - toWrite);
        targetFile->blockPointers[targetFile->blocksAllocated + blockIndex] = assignedBlockIndex;
        contentOffset += toWrite;
        bytesRemaining -= toWrite;
    }

    targetFile->blocksAllocated += blocksNeeded;
    targetFile->fileSizeBytes += contentLength;

    if (newBlockIndices) free(newBlockIndices);

    printf("LOG:Data written successfully (size=%d bytes).\n", contentLength);
}


static void cmdRead(const char *fileName) 
{
    if (!fileName) 
    { 
        printf("ERROR:Invalid file name.\n"); 
        return; 
    }
    FileNode *targetFile = findChildInDirectory(currentWorkingDirectory, fileName);
    if (!targetFile) 
    { 
        printf("ERROR:File not found.\n"); 
        return; 
    }
    if (targetFile->isDirectory) 
    { 
        printf("ERROR:Cannot read a directory.\n"); 
        return; 
    }
    readFileAndPrint(targetFile);
}


static void cmdLs() 
{
    if (!currentWorkingDirectory->childHead) 
    {
        printf("(empty)\n");
        return;
    }
    FileNode *headChild = currentWorkingDirectory->childHead;
    FileNode *walker = headChild;
    do 
    {
        if (walker->isDirectory) 
            printf("%s/\n", walker->name);
        else 
            printf("%s\n", walker->name);
        walker = walker->siblingNext;
    } while (walker != headChild);
}

static void cmdCd(const char *targetDirectoryName) 
{
    if (!targetDirectoryName) 
    { 
        printf("ERROR:Invalid target.\n"); 
        return; 
    }

    if (strcmp(targetDirectoryName, "..") == 0) 
    {
        if (currentWorkingDirectory->parent) 
        {
            currentWorkingDirectory = currentWorkingDirectory->parent;
            FileNode *currentDirectory = currentWorkingDirectory;
            if (currentDirectory == rootDirectory) 
            {
                printf("LOG:Moved to /\n");
            } 
            else 
            {
                const FileNode *directoryStack[MAX_DEPTH];
                int directoryDepth = 0;
                while (currentDirectory && currentDirectory != rootDirectory && directoryDepth < MAX_DEPTH) 
                {
                    directoryStack[directoryDepth++] = currentDirectory;
                    currentDirectory = currentDirectory->parent;
                }
                printf("LOG:Moved to /");
                for (int index = directoryDepth - 1; index >= 0; --index) 
                {
                    printf("%s", directoryStack[index]->name);
                    if (index > 0) printf("/");
                }
                printf("\n");
            }
        } else {
            printf("LOG:Moved to /\n");
        }
        return;
    }

    if (strcmp(targetDirectoryName, "/") == 0) {
        currentWorkingDirectory = rootDirectory;
        printf("LOG:Moved to /\n");
        return;
    }

    FileNode *targetDirectory = findChildInDirectory(currentWorkingDirectory, targetDirectoryName);
    if (!targetDirectory) { printf("ERROR:Directory not found.\n"); return; }
    if (!targetDirectory->isDirectory) { printf("ERROR:Not a directory.\n"); return; }

    currentWorkingDirectory = targetDirectory;

    FileNode *directoryWalker = currentWorkingDirectory;
    if (directoryWalker == rootDirectory) { printf("LOG:Moved to /\n"); return; }

    const FileNode *directoryStack[MAX_DEPTH];
    int directoryDepth = 0;
    while (directoryWalker && directoryWalker != rootDirectory && directoryDepth < MAX_DEPTH) {
        directoryStack[directoryDepth++] = directoryWalker;
        directoryWalker = directoryWalker->parent;
    }

    printf("LOG:Moved to /");
    for (int idx = directoryDepth - 1; idx >= 0; --idx) {
        printf("%s", directoryStack[idx]->name);
        if (idx > 0) printf("/");
    }
    printf("\n");
}



static void cmdPwd() 
{
    FileNode *directoryWalker = currentWorkingDirectory;
    if (directoryWalker == rootDirectory) 
    { 
        printf("/\n"); 
        return; 
    }

    const FileNode *directoryStack[MAX_DEPTH];
    int directoryDepth = 0;
    while (directoryWalker && directoryWalker != rootDirectory && directoryDepth < MAX_DEPTH) 
    {
        directoryStack[directoryDepth++] = directoryWalker;
        directoryWalker = directoryWalker->parent;
    }

    printf("/");
    for (int index = directoryDepth - 1; index >= 0; --index) 
    {
        printf("%s", directoryStack[index]->name);
        if (index > 0) 
            printf("/");
    }
    printf("\n");
}



static void cmdDf() 
{
    int usedBlocks = NUM_BLOCKS - totalFreeBlocks;
    double diskUsagePercent = ((double)usedBlocks / (double)NUM_BLOCKS) * 100.0;
    printf("Total Blocks: %d\n", NUM_BLOCKS);
    printf("Used Blocks: %d\n", usedBlocks);
    printf("Free Blocks: %d\n", totalFreeBlocks);
    printf("Disk Usage: %.2f%%\n", diskUsagePercent);
    printf("\n");

}


static void freeAllFilerNodesRecursive(FileNode *directoryNode) 
{
    if (!directoryNode) 
        return;
    if (directoryNode->isDirectory && directoryNode->childHead) 
    {
        FileNode *headChild = directoryNode->childHead;
        FileNode *walker = headChild;
        FileNode *childrenList[4096];
        int childCount = 0;
        do 
        {
            childrenList[childCount++] = walker;
            walker = walker->siblingNext;
            if (childCount >= 4096) 
                break;
        } while (walker != headChild);
        for (int index = 0; index < childCount; ++index) 
        {
            FileNode *childNode = childrenList[index];
            if (childNode->isDirectory) 
                freeAllFilerNodesRecursive(childNode);
            else 
            {
                freeBlocksOfFile(childNode);
                freeFilerNodeMemory(childNode);
            }
        }
        directoryNode->childHead = NULL;
    }
    freeFilerNodeMemory(directoryNode);
}

static void cleanupAndExit() 
{
    if (rootDirectory) 
    {
        if (rootDirectory->childHead) 
        {
            FileNode *headChild = rootDirectory->childHead;
            FileNode *walker = headChild;
            FileNode *childrenList[4096];
            int childCount = 0;
            do 
            {
                childrenList[childCount++] = walker;
                walker = walker->siblingNext;
                if (childCount >= 4096) 
                    break;
            } while (walker != headChild);
            for (int index = 0; index < childCount; ++index) 
            {
                FileNode *childNode = childrenList[index];
                if (childNode->isDirectory) 
                    freeAllFilerNodesRecursive(childNode);
                else 
                {
                    freeBlocksOfFile(childNode);
                    freeFilerNodeMemory(childNode);
                }
            }
            rootDirectory->childHead = NULL;
        }
        freeFilerNodeMemory(rootDirectory);
        rootDirectory = NULL;
        currentWorkingDirectory = NULL;
    }
    freeFreeBlockList();
}

static void trimWhitespace(char *s) 
{
    char *walker = s;
    while (isspace((unsigned char)*walker)) 
        walker++;
    if (walker != s) 
        memmove(s, walker, strlen(walker) + 1);
    size_t length = strlen(s);
    while (length > 0 && isspace((unsigned char)s[length - 1])) 
        s[--length] = '\0';
}


static int tokenizeCommand(char *inputLine, char *tokenArray[], int maxTokens) 
{
    int tokenCount = 0;
    char *walker = inputLine;
    while (*walker && tokenCount < maxTokens) 
    {
        while (isspace((unsigned char)*walker)) 
            walker++;
        if (*walker == '\0') 
            break;
        if (*walker == '"') 
        {
            walker++;
            char *start = walker;
            while (*walker && *walker != '"') 
                walker++;
            if (*walker == '\0') 
            {
                tokenArray[tokenCount++] = start;
                break;
            } 
            else 
            {
                *walker = '\0';
                tokenArray[tokenCount++] = start;
                walker++;
            }
        } 
        else 
        {
            char *start = walker;
            while (*walker && !isspace((unsigned char)*walker)) 
                walker++;
            if (*walker) 
            { 
                *walker = '\0'; 
                walker++; 
            }
            tokenArray[tokenCount++] = start;
        }
    }
    return tokenCount;
}


static void initializeVirtualFileSystem() 
{
    memset(virtualDisk, 0, sizeof(virtualDisk));
    initializeFreeBlocks();
    rootDirectory = createFilerNode("/", 1, NULL);
    rootDirectory->childHead = NULL;
    currentWorkingDirectory = rootDirectory;
}

int main() 
{
    initializeVirtualFileSystem();
    printf("Compact VFS - ready. Type 'exit' to quit.\n");
    printf("Type 'help' for command list.\n\n");
    char inputLine[INPUT_BUFFER_SIZE];
    while (1) 
    {
        if (currentWorkingDirectory == rootDirectory) 
            printf("/ > ");
        else 
            printf("%s > ", currentWorkingDirectory->name);

        if (!fgets(inputLine, sizeof(inputLine), stdin)) 
        {
            printf("\n");
            cleanupAndExit();
            printf("Memory released. Exiting program...\n");
            return 0;
        }

        size_t inputLength = strlen(inputLine);
        if (inputLength > 0 && inputLine[inputLength - 1] == '\n') inputLine[inputLength - 1] = '\0';
        trimWhitespace(inputLine);
        if (strlen(inputLine) == 0) continue;

        char *tokens[6];
        int tokenCount = tokenizeCommand(inputLine, tokens, 6);
        if (tokenCount == 0) continue;

        char *command = tokens[0];

        if (strcmp(command, "exit") == 0) 
        {
            cleanupAndExit();
            printf("Memory released. Exiting program...\n");
            return 0;
        }
        else if(strcmp(command, "help") == 0) 
        {
            showHelp();
        } 
        else if (strcmp(command, "mkdir") == 0) 
        {
            if (tokenCount < 2) 
            { 
                printf("Usage: mkdir <dirname>\n"); 
                continue; 
            }
            cmdMkdir(tokens[1]);
        } 
        else if (strcmp(command, "rmdir") == 0) 
        {
            if (tokenCount < 2) 
            { 
                printf("Usage: rmdir <dirname>\n"); 
                continue; 
            }
            cmdRmdir(tokens[1]);
        } 
        else if (strcmp(command, "create") == 0) 
        {
            if (tokenCount < 2) 
            { 
                printf("Usage: create <filename>\n"); 
                continue; 
            }
            cmdCreate(tokens[1]);
        } 
        else if (strcmp(command, "delete") == 0) 
        {
            if (tokenCount < 2) 
            { 
                printf("Usage: delete <filename>\n"); 
                continue; 
            }
            cmdDelete(tokens[1]);
        } 
        else if (strcmp(command, "write") == 0) 
        {
            if (tokenCount < 3) 
            { 
                printf("Usage: write <filename> \"<content>\"\n"); continue; 
            }
            cmdWrite(tokens[1], tokens[2]);
        } 
        else if (strcmp(command, "read") == 0) 
        {
            if (tokenCount < 2) 
            { 
                printf("Usage: read <filename>\n"); 
                continue; 
            }
            cmdRead(tokens[1]);
        } 
        else if (strcmp(command, "ls") == 0) 
        {
            cmdLs();
        } 
        else if (strcmp(command, "cd") == 0) 
        {
            if (tokenCount < 2) 
            { 
                printf("Usage: cd <dirname>\n"); 
                continue; 
            }
            cmdCd(tokens[1]);
        } 
        else if (strcmp(command, "pwd") == 0) 
        {
            cmdPwd();
        } 
        else if (strcmp(command, "df") == 0) 
        {
            cmdDf();
        } 
        else 
        {
            printf("Unknown command: %s\n", command);
        }
    }

    return 0;
}
