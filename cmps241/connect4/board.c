#include <stdio.h>
#include "board.h"
#include "hashing.h"
#include "transposition.h"

char board[rows][cols];

// Initialize the board with empty values
void initializeBoard(char board[rows][cols])
{
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            board[i][j] = '.';
    // Initialize transposition table
    initTranspositionTable();
    // Initialize Zobrist hashing
    initZobrist();
}
// Print the board
void printBoard(char board[rows][cols])
{
    printf("\n");
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            if (board[i][j] == 'A')
                printf("\033[31m%c \033[0m", board[i][j]); // Red
            else if (board[i][j] == 'B')
                printf("\033[34m%c \033[0m", board[i][j]); // Blue
            else
                printf("%c ", board[i][j]); // Empty
        }
        printf("\n");
    }
    for (int j = 1; j <= cols; j++)
        printf("%d ", j);
    printf("\n\n");
}
// Add a value to a column, return row index or -1 if invalid
int addValue(char board[rows][cols], int col, char player)
{
    if (col < 0 || col >= cols)
        return -1;
    for (int i = rows - 1; i >= 0; i--)
    {
        if (board[i][col] == '.')
        {
            updateHash(i, col, '.', player);
            board[i][col] = player;
            return i;
        }
    }
    return -1;
}
// Remove a value (for undo in minimax)
void removeValue(int row, int col, char player)
{
    updateHash(row, col, player, '.');
    board[row][col] = '.';
}
// Check 4 in a row in any direction from a cell
int checkDirection(char board[rows][cols], char player, int startRow, int startCol, int dRow, int dCol)
{
    for (int k = 0; k < 4; k++)
    {
        int r = startRow + k * dRow;
        int c = startCol + k * dCol;
        if (r < 0 || r >= rows || c < 0 || c >= cols || board[r][c] != player)
            return 0;
    }
    return 1;
}
// Check if player has won
int checkWin(char board[rows][cols], char player)
{
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            if (checkDirection(board, player, i, j, 0, 1))
                return 1;
            if (checkDirection(board, player, i, j, 1, 0))
                return 1;
            if (checkDirection(board, player, i, j, 1, 1))
                return 1;
            if (checkDirection(board, player, i, j, 1, -1))
                return 1;
        }
    }
    return 0;
}
// Check if board is full
int checkDraw(char board[rows][cols])
{
    for (int j = 0; j < cols; j++)
        if (board[0][j] == '.')
            return 0;
    return 1;
}