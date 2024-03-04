#include <stdio.h>
#include <stdlib.h>


// struct defs
typedef struct node node;

struct node
{
    int frequency;
    node *left, *right;
    unsigned char byte;
    node *next;
};

typedef struct table_t
{
    unsigned char byte;
    unsigned char code;
    int size;
}table;

// function macros

node *initEncodeTree(FILE *file, int *fileSize, unsigned char **data, int *listSize);
void sortList(node *list);
node *makeTree(node *list);
table *makeTable(int size, node *tree);
void encodeData(FILE *file, unsigned char *data, int dataSize, table *codex, node *list);