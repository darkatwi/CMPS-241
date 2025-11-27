#include <stdlib.h>
#include "hashing.h"

unsigned long long zobristTable[rows][cols][3];
unsigned long long currentHash = 0;

// Initialize Zobrist hash table with random values
void initZobrist()
{
    srand(123456789);
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            for (int k = 0; k < 3; k++)
            {
                zobristTable[i][j][k] = ((unsigned long long)rand() << 32) | rand();
            }
        }
    }
    currentHash = 0;
}
// Update hash when making/unmaking a move
void updateHash(int row, int col, char oldPiece, char newPiece)
{
    int oldIdx = (oldPiece == '.') ? 0 : (oldPiece == 'A') ? 1
                                                           : 2;
    int newIdx = (newPiece == '.') ? 0 : (newPiece == 'A') ? 1
                                                           : 2;
    currentHash ^= zobristTable[row][col][oldIdx];
    currentHash ^= zobristTable[row][col][newIdx];
}