#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "board.h"
#include "bot_easy.h"
#include "bot_medium.h"
#include "bot_hard.h"
#include "utils.h"

// Main logic
int main()
{
    srand(time(NULL));
    printf("\nWelcome to Connect Four!\n\n");
    int mode = 0, level = 0;
    do
    {
        printf("Choose game mode:\n1. Player vs Player\n2. Player vs Bot\nEnter choice: ");
        if (scanf("%d", &mode) != 1)
        {
            while (getchar() != '\n')
                ;
            mode = 0;
        }
    } while (mode != 1 && mode != 2);
    if (mode == 2)
    {
        printf("\n");
        do
        {
            printf("Choose Bot difficulty:\n1. Easy\n2. Medium\n3. Hard\nEnter choice: ");
            if (scanf("%d", &level) != 1)
            {
                while (getchar() != '\n')
                    ;
                level = 0;
            }
        } while (level != 1 && level != 2 && level != 3);
    }
    printf("\nPlayer A: Red \n");
    if (mode == 1)
        printf("Player B: Blue \n");
    else
    {
        printf("Bot: Blue \n",
               (level == 1) ? "Easy" : (level == 2) ? "Medium" : "Hard");
    }
    initializeBoard(board);
    char currPlayer = 'A';
    if (mode == 2 && level == 3)
    {
        int first = 0;
        do
        {
            printf("\nWho goes first?\n");
            printf("1. Player first\n2. Bot first\n");
            if (scanf("%d", &first) != 1)
            {
                while (getchar() != '\n')
                    ;
                first = 0;
            }
        } while (first != 1 && first != 2);
        currPlayer = (first == 1) ? 'A' : 'B';
    }
    while (getchar() != '\n');
    printBoard(board);
    int col;
    while (1)
    {
        if (mode == 2 && currPlayer == 'B')
        {
            if (level == 1)
                EasyBotMove();
            else if (level == 2)
                MediumBotMove();
            else
                HardBotMove();
        }
        else
        {
            int valid = 0;
            while (!valid)
            {
                col = getColumnFromUser(currPlayer);
                int row = addValue(board, col, currPlayer);
                if (row != -1)
                {
                    valid = 1;
                    printBoard(board);
                }
                else
                    printf("Column full. Choose another column.\n");
            }
        }
        if (checkWin(board, currPlayer))
        {
            printf("Player %c WINS!\n\n", currPlayer);
            break;
        }
        if (checkDraw(board))
        {
            printf("It's a DRAW!\n\n");
            break;
        }

        alternatePlayers(&currPlayer);
    }
    return 0;
}