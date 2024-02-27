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

typedef struct sortingTable_t
{
    char byte;
    int frequency;
}sortingTable;