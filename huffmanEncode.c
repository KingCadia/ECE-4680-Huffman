#include "huffmanEncode.h"

// public functions

// returns a linked this with the sorted byte occurences
node *initEncodeTree(FILE *file, int *fileSize, char **data)
{
    // finds the size of the file 
    int i;
    fseek(file, 0L, SEEK_END);
    *fileSize = ftell(file);
    // seeks back to the start of the file
    fseek(file, 0L, SEEK_SET);
    *data = (char *)malloc(sizeof(char) * *fileSize);
    char holder;
    // looks through the data to count the frequencies
    for(i=0;i<*fileSize;i++)
    {
        holder = *data[i];
        
    }
}


// private functions

/*
functions to make new node that can be added to a list or a tree 
if first and last are both null the function will return an single byte not 
else it will return a node with combined first and second
*/ 
node *makeNode(node *first, node *second, char byte)
{
    node *newNode = (node *)malloc(sizeof(node));
    if(first == NULL && second == NULL)
    {
        newNode->byte = byte;
        newNode->frequency = 0;
        newNode->left = NULL;
        newNode->right = NULL;
        newNode->next = NULL;
        
    }
    else
    {
        newNode->right = first;
        newNode->left = second;
        newNode->frequency = first->frequency + second->frequency;
        newNode->byte = 0;
    }
    return newNode;
}

/*

*/
