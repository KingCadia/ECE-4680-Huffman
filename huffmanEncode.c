#include "huffmanEncode.h"
int nodesDone;
// private function defs
node *makeNode(node *first, node *second, char byte, int frequnecy);
void searchTree(node *curr, char code, int level, table *codes);

// public functions

// returns a linked this with the unsorted byte occurences
node *initEncodeTree(FILE *file, int *fileSize, char **data, int *listSize)
{
    // finds the size of the file 
    int i;
    char *holderData;
    fseek(file, 0L, SEEK_END);
    *fileSize = ftell(file);
    // seeks back to the start of the file
    fseek(file, 0L, SEEK_SET);
    holderData = (char *)malloc(sizeof(char) * *fileSize);
    fread(holderData, sizeof(char), *fileSize, file);
    char holder;
    int table[256];
    for(i=0;i<256;i++)
    {
        table[i] = 0;
    }

    // looks through the data to count the frequencies
    for(i=0;i<*fileSize;i++)
    {
        holder = holderData[i];
        table[(int)holder]++;
    }
    node *head = NULL;
    node *seeker = NULL;
    
    // makes the init list  
    for(i=0;i<256;i++)
    {
        if(table[i] > 0)
        {
            if(head == NULL)
            {   
                head = makeNode(NULL, NULL, (char)i, table[i]);
                seeker = head;
            }
            else
            {
                seeker->next = makeNode(NULL, NULL, (char)i, table[i]);
                seeker = seeker->next;
            }
        }
    }
    
    // counts the number of items in the list
    seeker = head;
    int holderListSize = 0;
    while(seeker != NULL)
    {
        holderListSize++;
        seeker = seeker->next;
    }
    *listSize = holderListSize;
    fclose(file);
    *data = holderData;
    return head;
}

// function to sort the linked list from smallest to largest 
void sortList(node *list)
{
    int tempFre;
    char tempByte;

    node *seeker = list;
    node *temp = NULL;
    while(seeker != NULL)
    {
        temp = seeker;
        while(temp->next != NULL)
        {
            if(temp->frequency < temp->next->frequency)
            {
                // swap the data
                tempFre = temp->frequency;
                tempByte = temp->byte;

                temp->frequency = temp->next->frequency;
                temp->byte = temp->next->byte;

                temp->next->frequency = tempFre;
                temp->next->byte = tempByte;
            }
            temp = temp->next;
        }
        seeker = seeker->next;
    }   
}

// takes the sorted list and makes the huffman tree
node *makeTree(node *list)
{
    node *seeker = list;
    node *holder;
    node *next, *prev;
    while(seeker->next != NULL)
    {
        holder = makeNode(seeker, seeker->next, 0, 0);
        seeker = seeker->next->next;
        if(seeker == NULL)
        {
            break;
        }
        next = seeker->next;
        prev = seeker;
        // puts new node in the right spot in the list
        while(1)
        {
            // case for holder being placed at the head of the list
            if(holder->frequency <= prev->frequency)
            {
                holder->next = seeker;
                seeker = holder;
                break;
            }

            // case for holder needing to be at the end of the list
            if(next == NULL)
            {
                prev->next = holder;
                holder->next = NULL;
                break;
            }
            
            // general case
            if(holder->frequency >= prev->frequency && holder->frequency <= next->frequency)
            {
                    holder->next = next;
                    prev->next = holder;
                    break;
            }
            prev = prev->next;
            next = next->next;
        }
    }
    return holder;
}

// function that make the table of codes from the tree
table *makeTable(int size, node *tree)
{
    nodesDone = 0; 
    // sets up recurrsion
    table *codes = (table *)malloc(sizeof(table) * size);
    searchTree(tree, 0, 0, codes);
    return codes;

}

// private functions

/*
functions to make new node that can be added to a list or a tree 
if first and last are both null the function will return an single byte not 
else it will return a node with combined first and second
*/ 
node *makeNode(node *first, node *second, char byte, int frequnecy)
{
    node *newNode = (node *)malloc(sizeof(node));
    if(first == NULL && second == NULL)
    {
        newNode->byte = byte;
        newNode->frequency = frequnecy;
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
recurrsive function that sets up makes the table and the codes for huffman encoding 
*/
void searchTree(node *curr, char code, int level, table *codes)
{
    // checks if the fuction needs to go deeper
    if(curr->left != NULL)
    {
        // goes deeper to the left of the tree
        searchTree(curr->left, (code | (0 << (level))), (level + 1), codes);
    }
    if(curr->right != NULL)
    {
        // goes deeper to the right of the tree
        searchTree(curr->right, (code | (1 << (level))), (level + 1), codes);
    }

    // makes a new entry to the table if needed
    if(curr->right == NULL && curr->left == NULL)
    {
        codes[nodesDone].byte = curr->byte;
        codes[nodesDone].code = code;
        codes[nodesDone].size = level;
        nodesDone++;
    }
    return;
}
