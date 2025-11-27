#include <stdlib.h>
#include "evaluation.h"
#include "board.h"
#include "transposition.h"
#include "hashing.h"

// Check if column is available
int isValidLocation(int col)
{
    if (col < 0 || col >= cols)
        return 0;
    return board[0][col] == '.';
}
// Get a list of valid columns and return count
int getValidLocations(int valid_cols[])
{
    int count = 0;
    for (int c = 0; c < cols; c++)
        if (isValidLocation(c))
            valid_cols[count++] = c;
    return count;
}
// Count immediate threats (positions where player can win in one move)
int countThreats(char piece)
{
    int threats = 0;
    for (int col = 0; col < cols; col++)
    {
        int row = addValue(board, col, piece);
        if (row != -1)
        {
            if (checkWin(board, piece))
                threats++;
            removeValue(row, col, piece);
        }
    }
    return threats;
}

//Count potential 3-in-a-rows that can be completed into a winning 4
int countThreeInRows(char piece) {
    int count = 0;

    char opp = (piece == 'B') ? 'A' : 'B';

    //horizonatl
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols - 3; c++) {
            int piece_count = 0, empty_count = 0;
            for (int i = 0; i < 4; i++) {
                if (board[r][c+i] == piece) piece_count++;
                else if (board[r][c+i] == '.') empty_count++;
                else if (board[r][c+i] == opp) { empty_count = -1; break; } 
            }
            if (piece_count == 3 && empty_count == 1) count++;
        }
    }

    //vertical
    for (int c = 0; c < cols; c++) {
        for (int r = 0; r < rows - 3; r++) {
            int piece_count = 0, empty_count = 0;
            for (int i = 0; i < 4; i++) {
                if (board[r+i][c] == piece) piece_count++;
                else if (board[r+i][c] == '.') empty_count++;
                else if (board[r+i][c] == opp) { empty_count = -1; break; }
            }
            if (piece_count == 3 && empty_count == 1) count++;
        }
    }

    //Positive diagonal
    for (int r = 0; r < rows - 3; r++) {
        for (int c = 0; c < cols - 3; c++) {
            int piece_count = 0, empty_count = 0;
            for (int i = 0; i < 4; i++) {
                if (board[r+i][c+i] == piece) piece_count++;
                else if (board[r+i][c+i] == '.') empty_count++;
                else if (board[r+i][c+i] == opp) { empty_count = -1; break; }
            }
            if (piece_count == 3 && empty_count == 1) count++;
        }
    }

    //Negative diagonal
    for (int r = 3; r < rows; r++) {
        for (int c = 0; c < cols - 3; c++) {
            int piece_count = 0, empty_count = 0;
            for (int i = 0; i < 4; i++) {
                if (board[r-i][c+i] == piece) piece_count++;
                else if (board[r-i][c+i] == '.') empty_count++;
                else if (board[r-i][c+i] == opp) { empty_count = -1; break; }
            }
            if (piece_count == 3 && empty_count == 1) count++;
        }
    }

    return count;
}

// Score a sliding window of 4 
int score_window(char w[WINDOW_LENGTH], char piece)
{
    int score = 0;
    char opp = (piece == 'B') ? 'A' : 'B';
    int piece_count = 0, empty_count = 0, opp_count = 0;
    for (int i = 0; i < WINDOW_LENGTH; i++)
    {
        if (w[i] == piece)
            piece_count++;
        else if (w[i] == '.')
            empty_count++;
        else if (w[i] == opp)
            opp_count++;
    }
    if (piece_count == 4)
        score += 100;
    else if (piece_count == 3 && empty_count == 1)
        score += 10;
    else if (piece_count == 2 && empty_count == 2)
        score += 3;
    if (opp_count == 3 && empty_count == 1)
        score -= 8;
    else if (opp_count == 2 && empty_count == 2)
        score -= 2;
    return score;
}
// Maximum strength board evaluation
int score_position(char piece)
{
    int score = 0;
    char opp = (piece == 'B') ? 'A' : 'B';
    // Strong center column preference
    int center_col = cols / 2;
    int center_count = 0;
    for (int r = 0; r < rows; r++)
        if (board[r][center_col] == piece)
            center_count++;
    score += center_count * 4;
    // Horizontal windows
    for (int r = 0; r < rows; r++)
    {
        for (int c = 0; c < cols - (WINDOW_LENGTH - 1); c++)
        {
            char window[WINDOW_LENGTH];
            for (int i = 0; i < WINDOW_LENGTH; i++)
                window[i] = board[r][c + i];
            score += score_window(window, piece);
        }
    }
    // Vertical windows
    for (int c = 0; c < cols; c++)
    {
        for (int r = 0; r < rows - (WINDOW_LENGTH - 1); r++)
        {
            char window[WINDOW_LENGTH];
            for (int i = 0; i < WINDOW_LENGTH; i++)
                window[i] = board[r + i][c];
            score += score_window(window, piece);
        }
    }
    // Positive diagonal
    for (int r = 0; r < rows - (WINDOW_LENGTH - 1); r++)
    {
        for (int c = 0; c < cols - (WINDOW_LENGTH - 1); c++)
        {
            char window[WINDOW_LENGTH];
            for (int i = 0; i < WINDOW_LENGTH; i++)
                window[i] = board[r + i][c + i];
            score += score_window(window, piece);
        }
    }
    // Negative diagonal
    for (int r = WINDOW_LENGTH - 1; r < rows; r++)
    {
        for (int c = 0; c < cols - (WINDOW_LENGTH - 1); c++)
        {
            char window[WINDOW_LENGTH];
            for (int i = 0; i < WINDOW_LENGTH; i++)
                window[i] = board[r - i][c + i];
            score += score_window(window, piece);
        }
    }
    // Threat detection
    int my_threats = countThreats(piece);
    int opp_threats = countThreats(opp);
    score += my_threats * 100;
    score -= opp_threats * 120;
    // Three-in-a-rows
    int my_threes = countThreeInRows(piece);
    int opp_threes = countThreeInRows(opp);
    score += my_threes * 15;
    score -= opp_threes * 18;
    return score;
}
// Structure for move ordering
typedef struct
{
    int col;
    int score;
} MoveScore;
int compareMoves(const void *a, const void *b)
{
    MoveScore *moveA = (MoveScore *)a;
    MoveScore *moveB = (MoveScore *)b;
    return moveB->score - moveA->score;
}
// Move ordering with TT move prioritization
int orderMoves(int valid_cols[], int count, char piece, int tt_move)
{
    MoveScore moves[cols];

    for (int i = 0; i < count; i++)
    {
        int col = valid_cols[i];
        moves[i].col = col;
        // Prioritize TT move
        if (col == tt_move)
        {
            moves[i].score = 20000000; // Highest priority
        }
        else
        {
            int row = addValue(board, col, piece);
            if (row != -1)
            {
                if (checkWin(board, piece))
                {
                    moves[i].score = 10000000; // Winning move
                }
                else
                {
                    moves[i].score = score_position(piece);
                    // Center column bonus
                    int dist_from_center = abs(col - cols / 2);
                    moves[i].score += (cols - dist_from_center) * 20;
                }
                removeValue(row, col, piece);
            }
            else
            {
                moves[i].score = -10000000;
            }
        }
    }
    qsort(moves, count, sizeof(MoveScore), compareMoves);
    for (int i = 0; i < count; i++)
    {
        valid_cols[i] = moves[i].col;
    }

    return count;
}