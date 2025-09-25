#include <stdio.h>

#define rows 6
#define cols 7

char board[rows][cols];

//initialize the board with empty values (.)
void initializeBoard (char board[rows][cols]){
    for (int i= 0; i<rows; i++) {
        for (int j= 0; j<cols; j++){
            board[i][j]= '.'; 
        }
    }
}

//print the board
void printBoard(char board[rows][cols]){
    printf("\n");
  //printing the values (. or A or B)
    for (int i= 0; i<rows; i++){
        for (int j= 0; j<cols; j++) {
            printf("%c ", board[i][j]);
        }
        printf("\n");
    }
    //printing the index nbs
    for (int j= 1; j<= cols; j++){
        printf("%d ", j); 
    }
    printf("\n\n");
}

// put a value (A or B)
int addValue (char board[rows][cols], int col, char player){
  //invalid case
    if (col<0 || col>=cols) 
      return 0; 
  //adding the value after finding the right place
    for (int i= rows-1; i>=0; i--){
        if (board[i][col]=='.'){
            board[i][col]= player;
            return 1; 
        }
    }
    return 0; 
}

//function to switch between players
void alternatePlayers(char *currentPlayer) {
    if (*currentPlayer=='A'){
        *currentPlayer= 'B';
    } 
     else{
        *currentPlayer= 'A';
    }
}


//function to check horizontal 4 in a row
int checkHorizontal(char board[rows][cols], char player) {
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j <= cols - 4; j++) {
            if(board[i][j] == player && 
                board[i][j+1] == player && 
                board[i][j+2] == player && 
                board[i][j+3] == player)  
                return 1; //horizontal 4 in a row found
        }
    }
    return 0;
}
//function to check vertical 4 in a row
int checkVertical (char board[rows][cols], char player){
    for (int i= 0; i<=rows-4; i++) {
        for (int j= 0; j<cols; j++){
            if (board[i][j]==player &&
                board[i+1][j]==player &&
                board[i+2][j]==player &&
                board[i+3][j]==player){
                return 1; //found
            }
        }
    }
    return 0;
}
//function to check right diagonal 4 in a row
int checkDiagonalRight (char board[rows][cols], char player){
    for (int i= 0; i<=rows-4; i++){
        for (int j= 0; j<=cols-4; j++){
            if (board[i][j] == player &&
                board[i+1][j+1]==player &&
                board[i+2][j+2]==player &&
                board[i+3][j+3]==player){
                return 1; //found
            }
        }
    }
    return 0;
}
//function to check left diagonal 4 in a row
int checkDiagonalLeft (char board[rows][cols], char player){
    for (int i= 0; i<=rows-4; i++){
        for (int j= 3; j<cols; j++){
            if (board[i][j]==player &&
                board[i+1][j-1]==player &&
                board[i+2][j-2]==player &&
                board[i+3][j-3]==player){
                return 1; //found
            }
        }
    }
    return 0;
}
//function that combines all the above to check win
int checkWin(char board[rows][cols], char player) {
    return checkHorizontal(board, player) ||
           checkVertical(board, player) ||
           checkDiagonalRight(board, player) ||
           checkDiagonalLeft(board, player);
}
int checkDraw(char board[rows][cols]) {
    for (int j = 0; j < cols; j++) {
        if (board[0][j] == '.') {
            return 0; // still space left
        }
    }
    return 1; // board is full
}

//testing all logic (person 1 & person 2) 
int main() {
    printf("Welcome to Connect Four!\n");
    printf("Player A: A\n");
    printf("Player B: B\n");
    
    initializeBoard(board);
    printBoard(board);
    
    char currPlayer= 'A';
    int col;
    
    while (1){
        printf("Player %c, choose a column (1-7): ", currPlayer);
        if (scanf("%d", &col) != 1) {
    printf("Invalid input! Please enter a number between 1 and 7.\n");
    while (getchar() != '\n'); //keep reading the chars to skip them
    continue;
}

        
        if (!addValue(board, col - 1, currPlayer)) {
            printf("Invalid move. Try again.\n");
            continue;
        }
        
        printBoard(board);
        
        if (checkWin(board, currPlayer)) {
            printf ("Player %c wins! \n", currPlayer);
            break;
        }
        
        if (checkDraw(board)) {
            printf("It's a draw! \n");
            break;
        }
        
        alternatePlayers(&currPlayer);
    }
    
    return 0;
}
