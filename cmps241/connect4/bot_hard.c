#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "bot_hard.h"
#include "board.h"
#include "bot_medium.h"
#include "evaluation.h"
#include "minimax.h"

// Hard bot implementation with iterative deepening
void HardBotMove()
{
    int best_col = -1;
    clock_t start = clock();
    // Count moves for opening book
    int move_count = 0;
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            if (board[i][j] != '.')
                move_count++;
    // Opening book - first move
    if (move_count == 0)
    {
        addValue(board, 3, 'B'); // Center column (column 4 in 1-indexed)
        printf("Bot placed its checker in column 4 (opening book - center).\n");
        printBoard(board);
        return;
    }
    // Opening book - second move response
    if (move_count == 1)
    {
        // If opponent played center, play next to it
        if (board[rows - 1][3] == 'A')
        {
            int response = (rand() % 2 == 0) ? 2 : 4; // Column 3 or 5
            addValue(board, response, 'B');
            printf("Bot placed its checker in column %d (opening book response).\n", response + 1);
            printBoard(board);
            return;
        }
        // Otherwise play center
        if (board[rows - 1][3] == '.')
        {
            addValue(board, 3, 'B');
            printf("Bot placed its checker in column 4 (opening book - center).\n");
            printBoard(board);
            return;
        }
    }
    // 1. Quick win check
    for (int col = 0; col < cols; col++)
    {
        int row = addValue(board, col, 'B');
        if (row != -1)
        {
            if (checkWin(board, 'B'))
            {
                printf("Bot placed its checker in column %d (winning move).\n", col + 1);
                printBoard(board);
                return;
            }
            removeValue(row, col, 'B');
        }
    }
    // 2. Block opponent win
    int blockCol = checkOpponentWin(board);
    if (blockCol != -1)
    {
        addValue(board, blockCol, 'B');
        printf("Bot placed its checker in column %d (blocking opponent's winning move).\n", blockCol + 1);
        printBoard(board);
        return;
    }
    // 3. Iterative deepening with aspiration windows
    int prev_score = 0;
    int aspiration_window = 50;
    for (int depth = 1; depth <= AI_DEPTH; depth++)
    {
        int col = -1;
        int score;

        if (depth <= 2)
        {
            // Full window for shallow depths
            score = minimax(depth, -10000000, 10000000, 1, &col);
        }
        else
        {
            // Try aspiration window
            score = minimax(depth, prev_score - aspiration_window, prev_score + aspiration_window, 1, &col);
            // If we failed high or low, re-search with full window
            if (score <= prev_score - aspiration_window ||
                score >= prev_score + aspiration_window)
            {
                score = minimax(depth, -10000000, 10000000, 1, &col);
            }
        }
        if (col != -1)
        {
            best_col = col;
            prev_score = score;
        }
        // Time limit check (20 seconds max)
        clock_t current = clock();
        double time_spent = (double)(current - start) / CLOCKS_PER_SEC;
        if (time_spent > 20.0)
        {
            printf("(Search stopped at depth %d due to time limit)\n", depth);
            break;
        }
    }
    clock_t end = clock();
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
    if (best_col != -1 && isValidLocation(best_col))
    {
        addValue(board, best_col, 'B');
        printf("Bot placed its checker in column %d (iterative deepening minimax, computed in %.2f sec).\n",
               best_col + 1, time_spent);
        printBoard(board);
        return;
    }
    // 4. Fallback (center-first strategy)
    int centerOrder[] = {3, 2, 4, 1, 5, 0, 6};
    for (int i = 0; i < cols; i++)
    {
        int col = centerOrder[i];
        int row = addValue(board, col, 'B');
        if (row != -1)
        {
            printf("Bot placed its checker in column %d (fallback center-oriented move).\n", col + 1);
            printBoard(board);
            return;
        }
    }
    // 5. Random fallback
    int success = 0;
    while (!success)
    {
        int col = rand() % cols;
        int row = addValue(board, col, 'B');
        success = (row != -1);
        if (success)
        {
            printf("Bot placed its checker in column %d (random fallback move).\n", col + 1);
            printBoard(board);
        }
    }
}