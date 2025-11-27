#include <stdio.h>
#include <stdlib.h>
#include "bot_easy.h"
#include "board.h"

// Easy bot implementation
void EasyBotMove()
{
    int col, row, success = 0;
    while (!success)
    {
        col = rand() % cols; // Make a completely random move
        row = addValue(board, col, 'B'); //returns row if valid & -1 if cols are full
        success = (row != -1); //save in success (true or false -- valid or invalid)
    }
    printf("Bot placed its checker in column %d (easy move).\n", col + 1); //col+1 since col is 0 based
    printBoard(board);
}