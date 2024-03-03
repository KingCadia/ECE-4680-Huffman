#include <stdio.h>
#include <stdlib.h>


// struct defs
typedef struct node node;

struct node
{
    int frequency;
    node *left, *right;
    char byte;
    node *next;
};

typedef struct table_t
{
    char byte;
    char code;
    int size;
}table;

// function macros

node *initEncodeTree(FILE *file, int *fileSize, char **data, int *listSize);
void sortList(node *list);
node *makeTree(node *list);
table *makeTable(int size, node *tree);