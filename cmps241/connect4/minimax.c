#include "minimax.h"
#include "board.h"
#include "evaluation.h"
#include "transposition.h"
#include "hashing.h"

// Minimax with transposition tables, null move pruning, and enhanced features
int minimax(int depth, int alpha, int beta, int maximizingPlayer, int *best_col_ptr)
{
    // Check transposition table
    int ttScore;
    int ttMove = -1;
    if (ttLookup(currentHash, depth, alpha, beta, &ttScore, &ttMove))
    {
        return ttScore;
    }
    int valid_cols[cols], valid_count = getValidLocations(valid_cols);
    int terminal = checkWin(board, 'A') || checkWin(board, 'B') || (valid_count == 0);
    if (depth == 0 || terminal)
    {
        int score;
        if (terminal)
        {
            if (checkWin(board, 'B'))
                score = 100000 + depth; // Prefer faster wins
            else if (checkWin(board, 'A'))
                score = -100000 - depth; // Prefer slower losses
            else
                score = 0; // Draw
        }
        else
        {
            score = score_position('B');
        }
        ttStore(currentHash, depth, score, 0, -1);
        return score;
    }
    int originalAlpha = alpha;
    int best_move = -1;
    if (maximizingPlayer)
    {
        // Null move pruning - skip our turn to see if position is still good
        if (depth >= 3 && !terminal && countThreats('A') == 0)
        {
            int null_score = minimax(depth - 3, alpha, beta, 0, best_col_ptr);
            if (null_score >= beta)
            {
                return beta; // Beta cutoff
            }
        }
        orderMoves(valid_cols, valid_count, 'B', ttMove);
        int value = -10000000;
        for (int i = 0; i < valid_count; i++)
        {
            int col = valid_cols[i];
            int row = addValue(board, col, 'B');
            if (row == -1)
                continue;
            int new_score = minimax(depth - 1, alpha, beta, 0, best_col_ptr);
            removeValue(row, col, 'B');

            if (new_score > value)
            {
                value = new_score;
                best_move = col;
                if (depth == AI_DEPTH && best_col_ptr)
                    *best_col_ptr = col;
            }
            if (value > alpha)
                alpha = value;
            if (alpha >= beta)
                break; // Beta cutoff
        }
        // Store in transposition table
        char flag = (value <= originalAlpha) ? 2 : (value >= beta) ? 1 : 0;
        ttStore(currentHash, depth, value, flag, best_move);
        return value;
    }
    else
    {
        // Null move pruning for minimizing player
        if (depth >= 3 && !terminal && countThreats('B') == 0)
        {
            int null_score = minimax(depth - 3, alpha, beta, 1, best_col_ptr);
            if (null_score <= alpha)
            {
                return alpha; // Alpha cutoff
            }
        }
        orderMoves(valid_cols, valid_count, 'A', ttMove);
        int value = 10000000;
        for (int i = 0; i < valid_count; i++)
        {
            int col = valid_cols[i];
            int row = addValue(board, col, 'A');
            if (row == -1)
                continue;
            int new_score = minimax(depth - 1, alpha, beta, 1, best_col_ptr);
            removeValue(row, col, 'A');
            
            if (new_score < value)
            {
                value = new_score;
                best_move = col;
            }
            if (value < beta)
                beta = value;
            if (alpha >= beta)
                break; // Alpha cutoff
        }
        char flag = (value <= originalAlpha) ? 2 : (value >= beta) ? 1 : 0;
        ttStore(currentHash, depth, value, flag, best_move);
        return value;
    }
}