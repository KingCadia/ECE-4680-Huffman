#include "huffmanEncode.h"

void printTable(table *codes, int size);

int main(int argc, char **argv)
{
    node *list;
    FILE *file = fopen(argv[1], "rb");
    unsigned char *data;
    int fileSize, listSize;
    list = initDecodeTree(file, &fileSize, &data, &listSize);
    sortList(&list, listSize);
    node *tree = makeTree(list);
    table *codes = makeTable(listSize, tree);
    printTable(codes, listSize);
}

void printTable(table *codes, int size)
{
    int i, j;
    for(i=0;i<size;i++)
    {   
        printf("%c\t", codes[i].byte);

        // prints the byte code
        for(j=0;j<codes[i].size;j++)
        {
            printf("%d", (codes[i].code >> j) & 1);
        }
        printf("\n");
    }
}