#ifndef HASHING_H
#define HASHING_H

#include "board.h"

extern unsigned long long zobristTable[rows][cols][3];  
extern unsigned long long currentHash;

void initZobrist();
void updateHash(int row, int col, char oldPiece, char newPiece);

#endif