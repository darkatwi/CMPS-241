#ifndef EVALUATION_H
#define EVALUATION_H

#define WINDOW_LENGTH 4

int isValidLocation(int col);
int getValidLocations(int valid_cols[]);
int countThreats(char piece);
int countThreeInRows(char piece);
int score_window(char w[WINDOW_LENGTH], char piece);
int score_position(char piece);
int orderMoves(int valid_cols[], int count, char piece, int tt_move);

#endif