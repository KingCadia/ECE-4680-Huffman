#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CHAR 256

typedef struct Node
{
    unsigned char c;
    unsigned int freq;
    struct Node *left, *right;
} Node;

Node* newNode(unsigned char c, unsigned int freq, Node* left, Node* right)
{
    Node* node = (Node*)malloc(sizeof(Node));

    node->c = c;
    node->freq = freq;
    node->left = left;
    node->right = right;

    return node;
}

// Compare two tree nodes
int compare(const void *a, const void *b)
{
    Node *node1 = *(Node**)a;
    Node *node2 = *(Node**)b;
    return node1->freq - node2->freq;
}

// Build the Huffman tree
Node* buildHuffmanTree(unsigned short frequencies[MAX_CHAR])
{
    Node* nodes[MAX_CHAR];
    int count = 0;

    // Create leaf nodes for each character
    for (int i = 0; i < MAX_CHAR; i++)
    {
        if (frequencies[i])
        {
            nodes[count++] = newNode(i, frequencies[i], NULL, NULL);
        }
    }

    // Build the tree
    while (count > 1)
    {
        // sort nodes
        qsort(nodes, count, sizeof(Node*), compare);

        Node *left = nodes[0];
        Node *right = nodes[1];

        Node *new = newNode(0, left->freq + right->freq, left, right);

        nodes[0] = new;
        nodes[1] = nodes[count - 1];

        count--;
    }

    return nodes[0];
}

// Function to free the memory of the Huffman tree
void freeTree(Node* root)
{
    if (root == NULL) return;
    freeTree(root->left);
    freeTree(root->right);
    free(root);
}

// Function to create a table with all the codes
void buildCodeTable(Node* node, char** codeTable, char* code, int depth)
{
    if (!node) return;

    if (!node->left && !node->right)
    {
        code[depth] = '\0';
        codeTable[node->c] = strdup(code);
    } else {
        if (node->left)
        {
            code[depth] = '0';
            buildCodeTable(node->left, codeTable, code, depth + 1);
        }
        if (node->right)
        {
            code[depth] = '1';
            buildCodeTable(node->right, codeTable, code, depth + 1);
        }
    }
}

void decode(FILE* inputFile, char** codeTable, FILE* outputFile)
{
    char tempCode[MAX_CHAR + 1] = {0};
    int codeLen = 0;
    unsigned char byte;
    int bit;

    // Read bytes from the file
    while (fread(&byte, 1, 1, inputFile))
    {
        //fwrite(&byte, 1, 1, outputFile);
        for (int i = 7; i >= 0; i--)
        {
            bit = (byte >> i) & 1; // Get the current bit
            tempCode[codeLen++] = bit ? '1' : '0';
            tempCode[codeLen] = '\0';

            // Check if tempCode matches any code in the codeTable
            for (int j = 0; j < MAX_CHAR; ++j)
            {
                //if(codeTable[j]) {printf("codeTable:%s tempCode:%s\n\n", codeTable[j], tempCode);}
                if (codeTable[j] && strcmp(tempCode, codeTable[j]) == 0)
                {
                    fwrite(&j, 1, 1, outputFile);
                    codeLen = 0;
                    break;
                }
            }
        }
    }
}

int main(int argc, char *argv[])
{
    FILE *inputFile = fopen(argv[1], "rb");
    if (!inputFile)
    {
        perror("Unable to open input file");
        return 1;
    }

    FILE *outputFile = fopen(argv[2], "wb");
    if (!outputFile)
    {
        perror("Unable to open output file");
        fclose(inputFile);
        return 1;
    }

    unsigned short frequencies[MAX_CHAR] = {0};
    if (fread(frequencies, sizeof(unsigned short), MAX_CHAR, inputFile) != MAX_CHAR)
    {
        perror("Error reading frequencies from file");
        fclose(inputFile);
        fclose(outputFile);
        return 1;
    }

    Node* root = buildHuffmanTree(frequencies);

    fseek(inputFile, 512, SEEK_SET);

    char* codeTable[MAX_CHAR];
    for (int i = 0; i < MAX_CHAR; ++i)
    {
        codeTable[i] = NULL;
    }

    char code[MAX_CHAR];

    buildCodeTable(root, codeTable, code, 0);
    
    for (int i = 0; i < MAX_CHAR; ++i)
    {
        if (codeTable[i] != NULL)
        {
            printf("Character %c has code %s\n", i, codeTable[i]);
        }
    }
    //printf("\n");

    decode(inputFile, codeTable, outputFile);

    freeTree(root);

    for (int i = 0; i < MAX_CHAR; ++i)
    {
        if (codeTable[i] != NULL)
        {
            free(codeTable[i]);
        }
    }

    fclose(inputFile);
    fclose(outputFile);

    return 0;
}
