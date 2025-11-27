#include <stdio.h>
#include "utils.h"

// Switch players
void alternatePlayers(char *curr)
{
    *curr = (*curr == 'A') ? 'B' : 'A';
}
// Get a valid column from human player
int getColumnFromUser(char player)
{
    char input[100];
    int col;
    while (1)
    {
        printf("Player %c, choose a column (1-7): ", player);
        if (!fgets(input, sizeof(input), stdin))
            continue; // read line
        if (sscanf(input, "%d", &col) != 1)
        {
            printf("Invalid input. Please enter a number 1-7.\n");
            continue;
        }
        if (col < 1 || col > 7)
        {
            printf("Out of range. Please enter a number 1-7.\n");
            continue;
        }
        return col - 1; 
    }
}