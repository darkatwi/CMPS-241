#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define rows 6
#define cols 7

#define WINDOW_LENGTH 4
#define AI_DEPTH 11  // Increased to 11 with transposition tables
#define TABLE_SIZE 8388593  

char board[rows][cols];

typedef struct {
    unsigned long long hash;
    int score;
    int depth;
    char flag;  
} TTEntry;

TTEntry transpositionTable[TABLE_SIZE];

// Zobrist hashing - random numbers for each position/piece combination
unsigned long long zobristTable[rows][cols][3];  // [row][col][piece: empty=0, A=1, B=2]
unsigned long long currentHash = 0;

// Initialize Zobrist hash table with random values
void initZobrist() {
    srand(123456789);  // Fixed seed for reproducibility
    for(int i=0; i<rows; i++) {
        for(int j=0; j<cols; j++) {
            for(int k=0; k<3; k++) {
                zobristTable[i][j][k] = ((unsigned long long)rand() << 32) | rand();
            }
        }
    }
    currentHash = 0;
}

// Initialize the board with empty values
void initializeBoard(char board[rows][cols]){
    for(int i=0; i<rows; i++)
        for(int j=0; j<cols; j++)
            board[i][j] = '.';
    
    // Initialize transposition table
    for(int i=0; i<TABLE_SIZE; i++) {
        transpositionTable[i].hash = 0;
        transpositionTable[i].score = 0;
        transpositionTable[i].depth = -1;
        transpositionTable[i].flag = 0;
    }
    
    // Initialize Zobrist hashing
    initZobrist();
}

// Print the board
void printBoard(char board[rows][cols]){
    printf("\n");
    for(int i=0; i<rows; i++){
        for(int j=0; j<cols; j++){
            if(board[i][j] == 'A') 
                printf("\033[31m%c \033[0m", board[i][j]); // Red
            else if(board[i][j] == 'B') 
                printf("\033[34m%c \033[0m", board[i][j]); // Blue
            else 
                printf("%c ", board[i][j]); // Empty
        }
        printf("\n");
    }
    for(int j=1; j<=cols; j++)
        printf("%d ", j);
    printf("\n\n");
}

// Update hash when making/unmaking a move
void updateHash(int row, int col, char oldPiece, char newPiece) {
    int oldIdx = (oldPiece == '.') ? 0 : (oldPiece == 'A') ? 1 : 2;
    int newIdx = (newPiece == '.') ? 0 : (newPiece == 'A') ? 1 : 2;
    
    currentHash ^= zobristTable[row][col][oldIdx];
    currentHash ^= zobristTable[row][col][newIdx];
}

// Add a value to a column; return row index or -1 if invalid
int addValue(char board[rows][cols], int col, char player){
    if(col < 0 || col >= cols) return -1;
    for(int i=rows-1; i>=0; i--){
        if(board[i][col]=='.'){
            updateHash(i, col, '.', player);
            board[i][col] = player;
            return i;
        }
    }
    return -1; // column full
}

// Remove a value (for undo in minimax)
void removeValue(int row, int col, char player) {
    updateHash(row, col, player, '.');
    board[row][col] = '.';
}

//Switch players
void alternatePlayers(char *curr){
    *curr = (*curr=='A') ? 'B' : 'A';
}

//Check 4 in a row in any direction from a cell
int checkDirection(char board[rows][cols], char player, int startRow, int startCol, int dRow, int dCol){
    for(int k=0; k<4; k++){
        int r = startRow + k*dRow;
        int c = startCol + k*dCol;
        if(r<0 || r>=rows || c<0 || c>=cols || board[r][c]!=player)
            return 0;
    }
    return 1;
}

//Check if player has won
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

//Check if board is full
int checkDraw(char board[rows][cols]){
    for(int j=0;j<cols;j++)
        if(board[0][j]=='.') return 0;
    return 1;
}

// Easy bot implementation
void EasyBotMove(){
    int col, row, success=0;
    while(!success){
        col = rand()%cols;
        row = addValue(board, col, 'B');
        success = (row!=-1);
    }
    printBoard(board);
}

//Check if opponent ('A') can win next move; returns winning column or -1
int checkOpponentWin(char board[rows][cols]){
    for(int col=0; col<cols; col++){
        int row = addValue(board, col, 'A');
        if(row != -1){
            if(checkWin(board, 'A')){
                removeValue(row, col, 'A'); // undo
                return col;
            }
            removeValue(row, col, 'A'); // undo
        }
    }
    return -1;
}

// Medium bot implementation
void MediumBotMove(){
    int centerOrder[]={3,2,4,1,5,0,6};

    // 1. Try to win immediately
    for(int col=0; col<cols; col++){
        int row = addValue(board, col, 'B');
        if(row != -1){
            if(checkWin(board, 'B')){
                printBoard(board);
                return;
            }
            removeValue(row, col, 'B');
        }
    }

    // 2. Block opponent's immediate win
    int blockCol = checkOpponentWin(board);
    if(blockCol != -1){
        addValue(board, blockCol, 'B');
        printBoard(board);
        return;
    }

    // 3. Strategic move (center first)
    for(int i=0;i<cols;i++){
        int col = centerOrder[i];
        int row = addValue(board, col, 'B');
        if(row != -1){
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


//is column available
int isValidLocation(int col) {
    if(col < 0 || col >= cols) return 0;
    return board[0][col] == '.';
}

// get list of valid columns into array, return count
int getValidLocations(int valid_cols[]) {
    int count = 0;
    for(int c=0;c<cols;c++) if(isValidLocation(c)) valid_cols[count++] = c;
    return count;
}

// Count immediate threats (positions where player can win in one move)
int countThreats(char piece) {
    int threats = 0;
    for(int col=0; col<cols; col++){
        int row = addValue(board, col, piece);
        if(row != -1){
            if(checkWin(board, piece)) threats++;
            removeValue(row, col, piece);
        }
    }
    return threats;
}

// Count potential 3-in-a-rows that can be completed into a winning 4
int countThreeInRows(char piece) {
    int count = 0;

    char opp = (piece == 'B') ? 'A' : 'B';

    // HORIZONTAL 
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

    // VERTICAL
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

    //POSITIVE DIAGONAL 
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

    //NEGATIVE DIAGONAL 
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

// heuristic: score a sliding window of 4 (char-based)
int score_window(char w[WINDOW_LENGTH], char piece) {
    int score = 0;
    char opp = (piece == 'B') ? 'A' : 'B';

    int piece_count = 0, empty_count = 0, opp_count = 0;
    for(int i=0;i<WINDOW_LENGTH;i++){
        if(w[i]==piece) piece_count++;
        else if(w[i]=='.') empty_count++;
        else if(w[i]==opp) opp_count++;
    }

    // Strong positive scoring
    if(piece_count == 4) score += 100000;
    else if(piece_count == 3 && empty_count == 1) score += 1000;
    else if(piece_count == 2 && empty_count == 2) score += 50;

    // Strong negative scoring (opponent threats)
    if(opp_count == 3 && empty_count == 1) score -= 800;
    else if(opp_count == 2 && empty_count == 2) score -= 30;

    return score;
}

// Maximum strength board evaluation
int score_position(char piece) {
    int score = 0;
    char opp = (piece == 'B') ? 'A' : 'B';
    
    // Strong center column preference
    int center_col = cols/2;
    int center_count = 0;
    for(int r=0;r<rows;r++) if(board[r][center_col] == piece) center_count++;
    score += center_count * 4;

    // Horizontal windows
    for(int r=0;r<rows;r++){
        for(int c=0;c<cols - (WINDOW_LENGTH-1); c++){
            char window[WINDOW_LENGTH];
            for(int i=0;i<WINDOW_LENGTH;i++) window[i] = board[r][c+i];
            score += score_window(window, piece);
        }
    }
    
    // Vertical windows
    for(int c=0;c<cols;c++){
        for(int r=0;r<rows - (WINDOW_LENGTH-1); r++){
            char window[WINDOW_LENGTH];
            for(int i=0;i<WINDOW_LENGTH;i++) window[i] = board[r+i][c];
            score += score_window(window, piece);
        }
    }
    
    // Positive diagonal
    for(int r=0;r<rows - (WINDOW_LENGTH-1); r++){
        for(int c=0;c<cols - (WINDOW_LENGTH-1); c++){
            char window[WINDOW_LENGTH];
            for(int i=0;i<WINDOW_LENGTH;i++) window[i] = board[r+i][c+i];
            score += score_window(window, piece);
        }
    }
    
    // Negative diagonal
    for(int r=WINDOW_LENGTH-1; r<rows; r++){
        for(int c=0;c<cols - (WINDOW_LENGTH-1); c++){
            char window[WINDOW_LENGTH];
            for(int i=0;i<WINDOW_LENGTH;i++) window[i] = board[r-i][c+i];
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
typedef struct {
    int col;
    int score;
} MoveScore;

int compareMoves(const void *a, const void *b) {
    MoveScore *moveA = (MoveScore *)a;
    MoveScore *moveB = (MoveScore *)b;
    return moveB->score - moveA->score;
}

// Enhanced move ordering
int orderMoves(int valid_cols[], int count, char piece) {
    MoveScore moves[cols];
    
    for(int i=0; i<count; i++){
        int col = valid_cols[i];
        moves[i].col = col;
        
        int row = addValue(board, col, piece);
        if(row != -1){
            if(checkWin(board, piece)){
                moves[i].score = 10000000;
            } else {
                moves[i].score = score_position(piece);
                int dist_from_center = abs(col - cols/2);
                moves[i].score += (cols - dist_from_center) * 20;
            }
            removeValue(row, col, piece);
        } else {
            moves[i].score = -10000000;
        }
    }
    
    qsort(moves, count, sizeof(MoveScore), compareMoves);
    
    for(int i=0; i<count; i++){
        valid_cols[i] = moves[i].col;
    }
    
    return count;
}

// Lookup transposition table
int ttLookup(unsigned long long hash, int depth, int alpha, int beta, int *score) {
    int index = hash % TABLE_SIZE;
    TTEntry *entry = &transpositionTable[index];
    
    if(entry->hash == hash && entry->depth >= depth) {
        if(entry->flag == 0) {  // Exact score
            *score = entry->score;
            return 1;
        } else if(entry->flag == 1) {  // Lower bound
            if(entry->score >= beta) {
                *score = entry->score;
                return 1;
            }
        } else if(entry->flag == 2) {  // Upper bound
            if(entry->score <= alpha) {
                *score = entry->score;
                return 1;
            }
        }
    }
    return 0;
}

// Store in transposition table
void ttStore(unsigned long long hash, int depth, int score, char flag) {
    int index = hash % TABLE_SIZE;
    TTEntry *entry = &transpositionTable[index];
    
    // Replace if this is deeper or new
    if(entry->depth <= depth || entry->hash != hash) {
        entry->hash = hash;
        entry->score = score;
        entry->depth = depth;
        entry->flag = flag;
    }
}

// ULTIMATE Minimax with Transposition Tables - Depth 11
int minimax(int depth, int alpha, int beta, int maximizingPlayer, int *best_col_ptr) {
    // Check transposition table
    int ttScore;
    if(ttLookup(currentHash, depth, alpha, beta, &ttScore)) {
        return ttScore;
    }
    
    int valid_cols[cols], valid_count = getValidLocations(valid_cols);
    int terminal = checkWin(board, 'A') || checkWin(board, 'B') || (valid_count == 0);

    if(depth == 0 || terminal) {
        int score;
        if(terminal){
            if(checkWin(board, 'B')) score = 100000 + depth;
            else if(checkWin(board, 'A')) score = -100000 - depth;
            else score = 0;
        } else {
            score = score_position('B');
        }
        ttStore(currentHash, depth, score, 0);  // Exact score
        return score;
    }

    int originalAlpha = alpha;

    if(maximizingPlayer) {
        orderMoves(valid_cols, valid_count, 'B');
        
        int value = -10000000;
        for(int i=0;i<valid_count;i++){
            int col = valid_cols[i];
            int row = addValue(board, col, 'B');
            if(row == -1) continue;
            
            int new_score = minimax(depth-1, alpha, beta, 0, best_col_ptr);
            removeValue(row, col, 'B');
            
            if(new_score > value){
                value = new_score;
                if(depth == AI_DEPTH && best_col_ptr) *best_col_ptr = col;
            }
            if(value > alpha) alpha = value;
            if(alpha >= beta) break;
        }
        
        // Store in transposition table
        char flag = (value <= originalAlpha) ? 2 : (value >= beta) ? 1 : 0;
        ttStore(currentHash, depth, value, flag);
        
        return value;
    } else {
        orderMoves(valid_cols, valid_count, 'A');
        
        int value = 10000000;
        for(int i=0;i<valid_count;i++){
            int col = valid_cols[i];
            int row = addValue(board, col, 'A');
            if(row == -1) continue;
            
            int new_score = minimax(depth-1, alpha, beta, 1, best_col_ptr);
            removeValue(row, col, 'A');
            
            if(new_score < value) value = new_score;
            if(value < beta) beta = value;
            if(alpha >= beta) break;
        }
        
        char flag = (value <= originalAlpha) ? 2 : (value >= beta) ? 1 : 0;
        ttStore(currentHash, depth, value, flag);
        
        return value;
    }
}

// ULTIMATE Hard bot with Transposition Tables
void HardBotMove(){
    int best_col = -1;
    
    clock_t start = clock();
    
    // 1) Quick win check
    for(int col=0; col<cols; col++){
        int row = addValue(board, col, 'B');
        if(row != -1){
            if(checkWin(board, 'B')){
                printBoard(board);
                return;
            }
            removeValue(row, col, 'B');
        }
    }
    
    // 2) Block opponent win
    int blockCol = checkOpponentWin(board);
    if(blockCol != -1){
        addValue(board, blockCol, 'B');
        printBoard(board);
        return;
    }

    // 3) Use ULTIMATE minimax with transposition tables
    minimax(AI_DEPTH, -10000000, 10000000, 1, &best_col);

    clock_t end = clock();
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
    (void)time_spent; 

    if(best_col != -1 && isValidLocation(best_col)){
        addValue(board, best_col, 'B');
        printBoard(board);
        return;
    }

    // 4) Fallback
    int centerOrder[]={3,2,4,1,5,0,6};
    for(int i=0;i<cols;i++){
        int col = centerOrder[i];
        int row = addValue(board, col, 'B');
        if(row != -1){
            printBoard(board);
            return;
        }
    }

    // 5) Random fallback
    int success=0;
    while(!success){
        int col = rand()%cols;
        int row = addValue(board, col, 'B');
        success = (row != -1);
        if(success){
            printBoard(board);
        }
    }
}

//main logic
int main(){
    srand((unsigned)time(NULL));
    printf("Welcome to Connect Four!\n");

    int mode=0, level=0;
    do{
        printf("Choose game mode:\n1. Player vs Player\n2. Player vs Bot\nEnter choice: ");
        if(scanf("%d",&mode)!=1){ while(getchar()!='\n'); mode=0; }
    }while(mode!=1 && mode!=2);

    if(mode==2){
        do{
            printf("Choose Bot difficulty:\n1. Easy\n2. Medium\n3. Hard\nEnter choice: ");
            if(scanf("%d",&level)!=1){ while(getchar()!='\n'); level=0; }
        }while(level!=1 && level!=2 && level!=3);
    }

    printf("\nPlayer A: A (Red)\n");
    if(mode==1) printf("Player B: B (Blue)\n");
    else {
        printf("Player B: Bot (%s)\n",
            (level == 1) ? "Easy" : (level == 2) ? "Medium" : "Hard");
    }

    initializeBoard(board);

    char currPlayer='A';

    if(mode==2 && level==3){
        int first = 0;
        do{
            printf("\nWho goes first?\n");
            printf("1. Player first\n2. Bot first\n");
            if(scanf("%d", &first) != 1){
                while(getchar()!='\n');
                first= 0;
            }
        }
        while(first!=1 && first!=2);
        currPlayer= (first==1) ? 'A' : 'B';
    }

    while(getchar()!='\n');

    printBoard(board);
    int col;
    while(1){
        if(mode==2 && currPlayer=='B'){
            if(level==1) EasyBotMove();
            else if (level==2) MediumBotMove();
            else HardBotMove();
        }
        else{
            int valid=0;
            while(!valid){
                col = getColumnFromUser(currPlayer);
                int row = addValue(board, col, currPlayer);
                if(row != -1){
                    valid=1;
                    printBoard(board);
                }
                else printf("Column full. Choose another column.\n");
            }
        }

        if(checkWin(board, currPlayer)){
            printf("Player %c WINS!\n", currPlayer);
            break;
        }

        if(checkDraw(board)){
            printf("It's a DRAW!\n");
            break;
        }

        alternatePlayers(&currPlayer);
    }

    return 0;
}
