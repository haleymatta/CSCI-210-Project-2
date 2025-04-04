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

// Splits the pathName into dirName and baseName. It also returns a pointer to the 
// directory node in which the new node is to be created (the parent directory).
// On error (if a directory in the path doesn't exist) it prints an error and returns NULL.
struct NODE* splitPath(char* pathName, char* baseName, char* dirName) {
    // Preserve the original pathName by copying it.
    char pathCopy[128];
    strcpy(pathCopy, pathName);

    // If the pathName is just "/" (root), then for our purposes we treat it as an error for mkdir.
    // (This case should be handled in mkdir as "no path provided".)
    if(strcmp(pathCopy, "/") == 0) {
        strcpy(dirName, "/");
        baseName[0] = '\0';
        return root;
    }

    // Find the last occurrence of '/' to separate the baseName from the dirName.
    char* lastSlash = strrchr(pathCopy, '/');
    if(lastSlash != NULL) {
        // If the slash is at the beginning, then the dirName is "/" and the baseName is the rest.
        if(lastSlash == pathCopy) {
            strcpy(dirName, "/");
            strcpy(baseName, lastSlash + 1);
        } else {
            // Otherwise, copy everything before the last slash into dirName.
            int dirLen = lastSlash - pathCopy;  // length of directory part
            strncpy(dirName, pathCopy, dirLen);
            dirName[dirLen] = '\0';
            strcpy(baseName, lastSlash + 1);
        }
    } else {
        // No '/' found; this is a relative file/directory name in the current directory.
        strcpy(dirName, "");
        strcpy(baseName, pathCopy);
    }

    // Determine the starting point for traversal:
    // If the original pathName starts with '/', it is absolute and we start at root.
    // Otherwise, we start at cwd.
    struct NODE* current;
    if(pathName[0] == '/')
        current = root;
    else
        current = cwd;

    // If dirName is empty, then the parent is the current directory.
    if(strcmp(dirName, "") == 0) {
        return current;
    }

    // Make a copy of dirName for tokenization, as strtok modifies its input.
    char tempDir[128];
    strcpy(tempDir, dirName);

    // Tokenize the directory path using "/" as the delimiter.
    char* token = strtok(tempDir, "/");
    while(token != NULL) {
        // Look for a directory with this name in the current directory.
        struct NODE* nextDir = findChildDir(current, token);
        if(nextDir == NULL) {
            // If the directory does not exist, print the error and return NULL.
            printf("ERROR: directory %s does not exist\n", token);
            return NULL;
        }
        current = nextDir;
        token = strtok(NULL, "/");
    }
    return current;
}

// Creates a new directory node based on the given pathName.
void mkdir(char pathName[]) {
    // If the pathName is "/" (i.e., no new directory specified), then print error.
    if(strcmp(pathName, "/") == 0) {
        printf("MKDIR ERROR: no path provided\n");
        return;
    }

    char baseName[64];
    char dirName[128];  // Allocate enough space to store the directory portion.
    struct NODE* parentDir = splitPath(pathName, baseName, dirName);
    if(parentDir == NULL) {
        // An error was encountered during path traversal.
        return;
    }

    // Check if a node with the same name already exists in the parent directory.
    struct NODE* temp = parentDir->childPtr;
    while(temp != NULL) {
        if(strcmp(temp->name, baseName) == 0) {
            printf("MKDIR ERROR: directory %s already exists\n", baseName);
            return;
        }
        temp = temp->siblingPtr;
    }

    // Create and initialize the new directory node.
    struct NODE* newNode = (struct NODE*)malloc(sizeof(struct NODE));
    strcpy(newNode->name, baseName);
    newNode->fileType = 'D';
    newNode->childPtr = NULL;
    newNode->siblingPtr = NULL;
    newNode->parentPtr = parentDir;

    // Insert the new node as the last child of the parent directory.
    if(parentDir->childPtr == NULL) {
        parentDir->childPtr = newNode;
    } else {
        struct NODE* sibling = parentDir->childPtr;
        while(sibling->siblingPtr != NULL)
            sibling = sibling->siblingPtr;
        sibling->siblingPtr = newNode;
    }

    // Print success message.
    printf("MKDIR SUCCESS: node %s successfully created\n", pathName);
    return;
}
