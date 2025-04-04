#include "types.h"

//global var
extern struct NODE* root;
extern struct NODE* cwd;

//helper funct: searches for child node with given name and fileType 'D'
struct NODE* findChildDir(struct NODE* parent, const char* name) {
    struct NODE* temp = parent->childPtr;
    while(temp != NULL) {
        if(strcmp(temp->name, name) == 0 && temp->fileType == 'D') {
            return temp;
        }
        temp = temp->siblingPtr;
    }
    return NULL;
}

//splits the pathName into dirName and baseName; also ret a ptr to 
//directory node where new node is going to be created (parent dir)
//on err (if dir in path doesn't exist) it prints an err and ret NULL
struct NODE* splitPath(char* pathName, char* baseName, char* dirName) {
    //copy original pathName 
    char pathCopy[128];
    strcpy(pathCopy, pathName);

    //if pathName is just "/" (root) -> treat as err for mkdir
    //should be handled in mkdir as "no path provided"
    if(strcmp(pathCopy, "/") == 0) {
        strcpy(dirName, "/");
        baseName[0] = '\0';
        return root;
    }

    //find last occurrence of '/' -> separate the baseName from the dirName
    char* lastSlash = strrchr(pathCopy, '/');
    if(lastSlash != NULL) {
        //if '/' is at beg -> dirName is "/" -> baseName is the rest
        if(lastSlash == pathCopy) {
            strcpy(dirName, "/");
            strcpy(baseName, lastSlash + 1);
        } else {
            //else copy everything before last '/' into dirName
            int dirLen = lastSlash - pathCopy;  // length of dir part
            strncpy(dirName, pathCopy, dirLen);
            dirName[dirLen] = '\0';
            strcpy(baseName, lastSlash + 1);
        }
    } else {
        //no '/' found -> relative file/directory name in curr dir
        strcpy(dirName, "");
        strcpy(baseName, pathCopy);
    }

    //determine starting point for traversal:
    //if original pathName starts with '/' -> absolute -> start at root
    //else start at cwd.
    struct NODE* current;
    if(pathName[0] == '/')
        current = root;
    else
        current = cwd;

    //if dirName is empty -> parent is current dir
    if(strcmp(dirName, "") == 0) {
        return current;
    }

    //make copy of dirName for tokenization -> strtok modifies input
    char tempDir[128];
    strcpy(tempDir, dirName);

    //tokenize dir path using "/" as delimiter
    char* token = strtok(tempDir, "/");
    char currentPath[128] = ""; // to keep track of traversal path for error output

    while(token != NULL) {
        if(strlen(currentPath) > 0) {
            strcat(currentPath, "/");
        }
        strcat(currentPath, token);

        //look for dir with this name in curr dir
        struct NODE* nextDir = findChildDir(current, token);
        if(nextDir == NULL) {
            //if dir doesn't exist -> print err -> ret NULL.
            printf("ERROR: directory %s does not exist\n", currentPath);
            return NULL;
        }
        current = nextDir;
        token = strtok(NULL, "/");
    }
    return current;
}

//creates new dir node based on given pathName.
void mkdir(char pathName[]) {
    //if pathName is "/" (no new dir specified) -> print err
    if(strcmp(pathName, "/") == 0) {
        printf("MKDIR ERROR: no path provided\n");
        return;
    }

    char baseName[64];
    char dirName[128];  //allocate enough space to store dir portion.
    struct NODE* parentDir = splitPath(pathName, baseName, dirName);
    if(parentDir == NULL) {
        //err during path traversal.
        return;
    }

    //check if a node with same name already exists in parent dir
    struct NODE* temp = parentDir->childPtr;
    while(temp != NULL) {
        if(strcmp(temp->name, baseName) == 0) {
            //use full pathName in err msg
            printf("MKDIR ERROR: directory %s already exists\n", pathName);
            return;
        }
        temp = temp->siblingPtr;
    }

    //create/initialize the new dir node.
    struct NODE* newNode = (struct NODE*)malloc(sizeof(struct NODE));
    strcpy(newNode->name, baseName);
    newNode->fileType = 'D';
    newNode->childPtr = NULL;
    newNode->siblingPtr = NULL;
    newNode->parentPtr = parentDir;

    //insert new node as last child of parent dir
    if(parentDir->childPtr == NULL) {
        parentDir->childPtr = newNode;
    } else {
        struct NODE* sibling = parentDir->childPtr;
        while(sibling->siblingPtr != NULL)
            sibling = sibling->siblingPtr;
        sibling->siblingPtr = newNode;
    }

    //success msg
    printf("MKDIR SUCCESS: node %s successfully created\n", pathName);
    return;
}
