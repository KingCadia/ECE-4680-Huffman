#include "huffmanEncode.h"
int nodesDone;
// private function defs
node *makeNode(node *first, node *second, unsigned char byte, int frequnecy);
void searchTree(node *curr, unsigned char code, int level, table *codes);

// public functions

// returns a linked this with the unsorted byte occurences
node *initEncodeTree(FILE *file, int *fileSize, unsigned char **data, int *listSize)
{
    // finds the size of the file 
    int i;
    unsigned char *holderData;
    fseek(file, 0L, SEEK_END);
    *fileSize = ftell(file);
    // seeks back to the start of the file
    fseek(file, 0L, SEEK_SET);
    holderData = (unsigned char *)malloc(sizeof(unsigned char) * *fileSize);
    fread(holderData, sizeof(char), *fileSize, file);
    unsigned char holder;
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

// function that takes the table and the data and encodes it to a output file
void encodeData(FILE *file, unsigned char *data, int dataSize, table *codex, node *list)
{
    unsigned char writeData = 0;
    unsigned char holder;
    int i, j, shiftAmount;
    node *seeker = list;
    int bitsWritten = 0;

    // writes the frequencies for each byte
    short int sizes[256];
    for(i=0;i<256;i++)
    {
        sizes[i] = 0;
    }

    while(seeker != NULL)
    {
        sizes[seeker->byte] = (short int)seeker->frequency;
        seeker = seeker->next;
    }
    fwrite(&sizes, sizeof(short int), 256, file);

    // writes the file data out to the file
    for(i=0;i<dataSize;i++)
    {
        holder = data[i];
        // finds holder in the codex
        j = 0;
        while(1)
        {
            if(codex[j].byte == holder)
            {
                break;
            }
            j++;
        }

        // finds out if the code can fit in the current write data
        if((bitsWritten + codex[j].size) <= 8)
        {
            writeData = writeData | (codex[j].code << ((8 - bitsWritten) - codex[j].size));
            bitsWritten = bitsWritten + codex[j].size;
        }
        else
        {
            // the code does not fit in the current byte
            shiftAmount = codex[j].size - (8 - bitsWritten);
            writeData = writeData | (codex[j].code >> shiftAmount);

            // writes the data to the file
            fwrite(&writeData, sizeof(unsigned char), 1, file);

            // puts the remaining bytes in the new writedata
            writeData = 0;
            writeData = codex[j].code << ((8 - codex[j].size) + (codex[j].size - shiftAmount));
            bitsWritten = shiftAmount;
        }
        if(bitsWritten == 8)
        {
            // writes the current writedata to the file
            fwrite(&writeData, sizeof(unsigned char), 1, file);
            writeData = 0;
            bitsWritten = 0;
        }
    }  
    fclose(file);
}
// private functions

/*
functions to make new node that can be added to a list or a tree 
if first and last are both null the function will return an single byte not 
else it will return a node with combined first and second
*/ 
node *makeNode(node *first, node *second, unsigned char byte, int frequnecy)
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
void searchTree(node *curr, unsigned char code, int level, table *codes)
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
