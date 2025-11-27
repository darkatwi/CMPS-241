#ifndef TRANSPOSITION_H
#define TRANSPOSITION_H

#define TABLE_SIZE 8388593  // Large prime number for hash tables

// Transposition table entry
typedef struct {
    unsigned long long hash;
    int score;
    int depth;
    char flag;  // 0 = exact, 1 = lower bound, 2 = upper bound
    int best_move;  // Best move found at this position
} TTEntry;
extern TTEntry transpositionTable[TABLE_SIZE];
void initTranspositionTable();
int ttLookup(unsigned long long hash, int depth, int alpha, int beta, int *score, int *best_move);
void ttStore(unsigned long long hash, int depth, int score, char flag, int best_move);

#endif