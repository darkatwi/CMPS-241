#include <stdio.h>
#include <stdlib.h>   
#include <time.h>     

#define rows 6
#define cols 7

char board[rows][cols];

//initialize the board with empty values 
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

//put a value (A or B)
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


//check 4 in a row in any direction
int checkDirection(char board[rows][cols], char player, int startRow, int startCol, int dRow, int dCol){
    int inARow= 4; 
    for (int k= 0; k< inARow; k++){
        int r= startRow + k * dRow;
        int c= startCol + k * dCol;
        if (r<0 || r>=rows || c<0 ||c>=cols) return 0; // out of bounds
        if (board[r][c]!=player) return 0; // not same player
    }
    return 1;
}

int checkWin(char board[rows][cols], char player) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (checkDirection(board, player, i, j, 0, 1)) return 1;    // horizontal right
            if (checkDirection(board, player, i, j, 1, 0)) return 1;    // vertical down
            if (checkDirection(board, player, i, j, 1, 1)) return 1;    // diagonal down-right
            if (checkDirection(board, player, i, j, 1, -1)) return 1;   // diagonal down-left
        }
    }
    return 0; // no win found
}

// check for draw 
int checkDraw(char board[rows][cols]) {
    for (int j = 0; j < cols; j++) {
        if (board[0][j] == '.') return 0; // still space left
    }
    return 1; // board is full
}

//easy bot logic 
void botMove(){
    int col;
    int success= 0;
    while (!success){
        col= rand() % cols; // pick random col
        success= addValue(board, col, 'B');
    }
    printf("Bot placed its checker in column %d.\n", col + 1);
}

//testing all logic (person 1 & person 2) 
int main() {
    srand(time(NULL));
    printf("Welcome to Connect Four!\n");
  
    //player OR bot?
    int mode= 0;
    do{
        printf("Choose game mode:\n1. Player vs Player\n2. Player vs Bot\nEnter choice: ");
        if (scanf("%d", &mode)!=1){ //scanf should return 1 if input is valid
            printf("Invalid input! Enter 1 or 2.\n");
            while (getchar()!='\n'); //remove invalid input
            mode= 0;
        }
    } while (mode!=1 && mode!=2);

    
    //choose bot difficulty
    int level= 0;
    if (mode==2){
        do{
            printf("Choose Bot difficulty:\n1. Easy-level bot.\n"); //add the rest of the options later on
            if (scanf("%d", &level)!=1){
                printf("Invalid input! Enter a number from the available options \n"); //fix later when we add other options
                while (getchar()!='\n');
                level= 0;
            }
        } while (level!=1); //fix later when we add other options
    }


    //declare players
    printf("Player A: A\n");
    if (mode==1)
      printf("Player B: B\n");
    else{
      if (level==1) //add other options later
        printf("Player B: Bot (easy) \n");
    } 
    
    //initialize
    initializeBoard(board);
    printBoard(board);
    
    char currPlayer= 'A';
    int col;
  
  
    //playing
        while (1){
        if (mode==2 && currPlayer=='B'){
            botMove();
        }
        else{
          int valid= 0;
          while (!valid) {
           printf("Player %c, choose a column (1-7): ", currPlayer);
           if (scanf("%d", &col)!=1){
                    printf("Invalid input! Enter a number between 1-7.\n");
           while (getchar()!='\n');
             continue;
           }
             col--;
             if (!addValue(board, col, currPlayer)){
                    printf("Invalid move. Column full or out of range.\n");
               continue;
             }
             valid= 1;
           }
        }
        
  
        //declaring winners & ending game
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
    
    return 0;
}
