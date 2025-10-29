#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define rows 6
#define cols 7

char board[rows][cols];

// Initialize the board with empty values
void initializeBoard(char board[rows][cols]){
    for(int i=0; i<rows; i++)
        for(int j=0; j<cols; j++)
            board[i][j] = '.';
}

// Print the board
void printBoard(char board[rows][cols]){
    printf("\n");
    for(int i=0; i<rows; i++){
        for(int j=0; j<cols; j++)
            printf("%c ", board[i][j]);
        printf("\n");
    }
    for(int j=1; j<=cols; j++)
        printf("%d ", j);
    printf("\n\n");
}

// Add a value to a column; return row index or -1 if invalid
int addValue(char board[rows][cols], int col, char player){
    if(col < 0 || col >= cols) return -1;
    for(int i=rows-1; i>=0; i--){
        if(board[i][col]=='.'){
            board[i][col] = player;
            return i;
        }
    }
    return -1; // column full
}

// Switch players
void alternatePlayers(char *curr){
    *curr = (*curr=='A') ? 'B' : 'A';
}

// Check 4 in a row in any direction from a cell
int checkDirection(char board[rows][cols], char player, int startRow, int startCol, int dRow, int dCol){
    for(int k=0; k<4; k++){
        int r = startRow + k*dRow;
        int c = startCol + k*dCol;
        if(r<0 || r>=rows || c<0 || c>=cols || board[r][c]!=player)
            return 0;
    }
    return 1;
}

// Check if player has won
int checkWin(char board[rows][cols], char player){
    for(int i=0;i<rows;i++){
        for(int j=0;j<cols;j++){
            if(checkDirection(board, player, i, j, 0, 1)) return 1;
            if(checkDirection(board, player, i, j, 1, 0)) return 1;
            if(checkDirection(board, player, i, j, 1, 1)) return 1;
            if(checkDirection(board, player, i, j, 1, -1)) return 1;
        }
    }
    return 0;
}

// Check if board is full
int checkDraw(char board[rows][cols]){
    for(int j=0;j<cols;j++)
        if(board[0][j]=='.') return 0;
    return 1;
}

// Easy bot: random move
void EasyBotMove(){
    int col, row, success=0;
    while(!success){
        col = rand()%cols;
        row = addValue(board, col, 'B');
        success = (row!=-1);
    }
    printf("Bot placed its checker in column %d (easy move).\n", col+1);
    printBoard(board);
}

// Check if opponent ('A') can win next move; returns winning column or -1
int checkOpponentWin(char board[rows][cols]){
    for(int col=0; col<cols; col++){
        int row = addValue(board, col, 'A');
        if(row != -1){
            if(checkWin(board, 'A')){
                board[row][col] = '.'; // undo
                return col;
            }
            board[row][col] = '.'; // undo
        }
    }
    return -1;
}

// Medium bot
void MediumBotMove(){
    int centerOrder[]={3,2,4,1,5,0,6};
    
    // 1. Try to win immediately
    for(int col=0; col<cols; col++){
        int row = addValue(board, col, 'B');
        if(row != -1){
            if(checkWin(board, 'B')){
                printf("Bot placed its checker in column %d (winning move).\n", col+1);
                printBoard(board);
                return;
            }
            board[row][col]='.'; // undo
        }
    }
    
    // 2. Block opponent's immediate win
    int blockCol = checkOpponentWin(board);
    if(blockCol != -1){
        addValue(board, blockCol, 'B');
        printf("Bot placed its checker in column %d (blocking move).\n", blockCol+1);
        printBoard(board);
        return;
    }

    // 3. Strategic move (center first)
    for(int i=0;i<cols;i++){
        int col = centerOrder[i];
        int row = addValue(board, col, 'B');
        if(row != -1){
            printf("Bot placed its checker in column %d (strategic move).\n", col+1);
            printBoard(board);
            return;
        }
    }

    // 4. Fallback random move
    int success=0;
    while(!success){
        int col = rand()%cols;
        int row = addValue(board, col, 'B');
        success = (row != -1);
        if(success){
            printf("Bot placed its checker in column %d (random move).\n", col+1);
            printBoard(board);
        }
    }
}

// Get a valid column from human player
int getColumnFromUser(char player){
    char input[100];
    int col;
    while(1){
        printf("Player %c, choose a column (1-7): ", player);
        if(!fgets(input, sizeof(input), stdin)) continue; // read line
        if(sscanf(input, "%d", &col) != 1){
            printf("Invalid input. Please enter a number 1-7.\n");
            continue;
        }
        if(col < 1 || col > 7){
            printf("Out of range. Please enter a number 1-7.\n");
            continue;
        }
        return col-1; // convert to 0-based index
    }
}

int main(){
    srand(time(NULL));
    printf("Welcome to Connect Four!\n");

    int mode=0, level=0;
    do{
        printf("Choose game mode:\n1. Player vs Player\n2. Player vs Bot\nEnter choice: ");
        if(scanf("%d",&mode)!=1){ while(getchar()!='\n'); mode=0; }
    }while(mode!=1 && mode!=2);

    if(mode==2){
        do{
            printf("Choose Bot difficulty:\n1. Easy\n2. Medium\nEnter choice: ");
            if(scanf("%d",&level)!=1){ while(getchar()!='\n'); level=0; }
        }while(level!=1 && level!=2);
    }

    // clear leftover newline from scanf
    while(getchar()!='\n');

    printf("Player A: A\n");
    if(mode==1) printf("Player B: B\n");
    else printf("Player B: Bot (%s)\n", (level==1)?"Easy":"Medium");

    initializeBoard(board);
    printBoard(board);

    char currPlayer='A';
    int col;
    while(1){
        if(mode==2 && currPlayer=='B'){
            if(level==1) EasyBotMove();
            else MediumBotMove();
        }
        else{
            int valid=0;
            while(!valid){
                col = getColumnFromUser(currPlayer);
                int row = addValue(board, col, currPlayer);
                if(row != -1){
                    valid=1;
                    printBoard(board); // only prints after successful move
                }
                else printf("Column full. Choose another column.\n");
            }
        }

        if(checkWin(board, currPlayer)){
            printf("Player %c wins!\n", currPlayer);
            break;
        }

        if(checkDraw(board)){
            printf("It's a draw!\n");
            break;
        }

        alternatePlayers(&currPlayer);
    }

    return 0;
}
