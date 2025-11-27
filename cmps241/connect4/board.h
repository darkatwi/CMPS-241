#ifndef BOARD_H
#define BOARD_H

#define rows 6
#define cols 7

extern char board[rows][cols];

void initializeBoard(char board[rows][cols]);
void printBoard(char board[rows][cols]);
int addValue(char board[rows][cols], int col, char player);
void removeValue(int row, int col, char player);
int checkDirection(char board[rows][cols], char player, int startRow, int startCol, int dRow, int dCol);
int checkWin(char board[rows][cols], char player);
int checkDraw(char board[rows][cols]);

#endif