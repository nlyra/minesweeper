//Clara Poffenberger and Nathaniel Lyra
//11/27/18
//Final Intro To C project
//Simulation of a MineSweeper game

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

//this is the struct cube we use to represent each "cube," or square/coordinate on the board.
//showval refers to the values and characters that the user can see on his screen and gets printed out.
//realval refers to the values and characters that we have used to equate showval to.
typedef struct cube {
  char showval;
  int realval;
 } cube;

//our symbols and what they represent:
#define unchecked   '#'
#define bomb        '@'     // bomb is equal to -1 on realval board
#define blankSpace  '_'     // blankSpace is equal to -2 when it is revealed, 0 before discovered
#define flag        '+'

//these are our arrays with different difficulty levels in which user will choose from
const int maxrowArray[3] = {9,16, 16};
const int maxcolArray[3] = {9, 16, 30};
const int numMinesArray[3] = {10, 40, 99};

//coordinates to all the points to that need to be checked e.g. (1,-1),(-1,-1)...
const int DX[8] = {1,-1,-1,-1, 0,0,1,1};
const int DY[8] = {-1,-1, 0, 1,-1,1,0,1};

void menu ();
int MoveOptions();
void checkpnt(int maxrow, int maxcol, struct cube array[maxrow][maxcol], int row, int col);
void flagpnt(int maxrow, int maxcol, struct cube array[maxrow][maxcol], int row, int col);
void printBoard(int maxrow, int maxcol, struct cube array[maxrow][maxcol], int printboard);
void initBoard(int maxrow, int maxcol, struct cube array[maxrow][maxcol]);
void PutInBombs(int maxrow, int maxcol, struct cube array[maxrow][maxcol], int numMines);
void initNum(int maxrow, int maxcol, struct cube array[maxrow][maxcol]);
int inbounds(int x, int y, int rows, int cols);
void revealBlank(int maxrow, int maxcol, struct cube array[maxrow][maxcol], int x, int y);
void fullBoard(FILE *ofp, int maxrow, int maxcol, struct cube array[maxrow][maxcol],int startTime, int endTime, int numturns, int *bestmin, int *bestsec, int numMines, int exists, int win, int overallWins);
int WinCase(int maxrow, int maxcol,struct cube array[maxrow][maxcol],int numMines);


int main (void){
    //seeds the random number generator for the bomb placements
    srand(time(0));

    //initializes variables to be used and explained throughout code
    int i, j, numMines, maxrow, maxcol, bestmin , bestsec , choice = -4, move, row = 100, col = 100, numturns = 0, endTime, startTime, exist = 0;
    int overallWins = 0, totalGames = 0;
    //assigns the file pointer and creates a text file
    FILE *ofp;
    char current_string[100], trash;

    //open file for reading in the last best time results
    ofp = fopen("GameSummary.txt", "r");

    //while true, scan current word (string)
    while (1){
        fscanf(ofp, "%s", current_string);

        //if current word (string) is equal to end of file, break
        if (fscanf(ofp, "%s", current_string) == EOF){
            break;
        }

            //if current string is equal to "Time:", enter the if
            if (strcmp(current_string, "Time:") == 0)  {
                exist = 1;
                //scan in the integers after "Best Time:"
                fscanf(ofp, "%d %c %d", &bestmin, &trash, &bestsec);
                break;
            }
    }
    fclose(ofp);

    //open file for writing and clears the past text
    ofp = fopen("GameSummary.txt", "w");

    //prints a welcome message when you first start the game
    printf("Welcome to Minesweeper!\n\n");

    //loops through the initial menu until a valid number is chosen
    while(1){
        menu ();
        scanf("%d", &choice);
        printf("\n");

        if (choice <= 4 && choice >= 1)
            break;
    }

    // loops through the game until they quit playing
    while (choice != 4) {
        int win = 0;
        //assigns the correct level values of the row and column size as well as the number of bombs
        maxrow = maxrowArray[choice-1];
        maxcol = maxcolArray[choice-1];
        numMines = numMinesArray[choice-1];

        //initializes the array and prints the board out
        cube array[maxrow][maxcol];
        initBoard(maxrow, maxcol,array);
        printBoard( maxrow, maxcol,array,1);
        printf("\n");

        //prints out the options for the first move
        move = MoveOptions();
        //if they choose to select a coordinate
        if (move == 1){
           printf("Please enter the row and column numbers.\n");
           scanf("%d %d", &row, &col);
           printf("\n");
           //checks that the number entered is within the bounds, if not, it prompts again
            if (!inbounds(row,col,maxrow,maxcol)){
                printf("That is not a valid move.\n\n");
                continue;
            }
            //changes the point chosen to a blankspace for when PutInBombs is called so it does not put a bomb where you chose
            array[row][col].showval = blankSpace;
            //puts in the bombs
            PutInBombs(maxrow, maxcol, array, numMines);
            //puts numbers near all the bombs
            initNum(maxrow,maxcol,array);
            //changes back the point to an unchecked spot for it triggers the revealBlank function, this then turns it back into
            //a blankSpace
            array[row][col].showval = unchecked;
            revealBlank(maxrow, maxcol, array, row, col);
            //prints out the board
            printBoard(maxrow, maxcol,array,1);
            numturns++;
            startTime = time(0);
          }
        // makes sure that the user inputs a coordinate in the first move
        else if (move == 2){
           printf("Please enter coordinates before flagging a point.\n\n");
           continue;
          }
        // makes sure that the first move is a coordinate
        else{
            printf("That was not the correct input.\n\n");
            continue;
        }

        //loops till user loses current game
        while (1) {

            //prints out the move options
            printf("\n");
            move = MoveOptions();

            //if the user wants to check a coordinate
            if (move == 1){
                printf("Please enter the row and column numbers.\n");
                scanf("%d %d", &row, &col);
                //makes sure that the coordinate is in the bounds and is one of two valid selections
                if ((inbounds(row,col,maxrow,maxcol) && array[row][col].showval == unchecked) || (array[row][col].showval == flag && inbounds(row,col,maxrow,maxcol))){
                    checkpnt(maxrow, maxcol,array, row, col);
                    printBoard(maxrow, maxcol,array,1);
                    numturns++;
                }
                //if it is not either one of the above if conditions, it is not a valid input
                else{
                    printf("\nThat is not a valid input.\n");
                }
                printf("\n");

            }
            // if the user wants to flag a point
            else if (move == 2){
                printf("Please enter the row and column numbers.\n");
                scanf("%d %d", &row, &col);
                //checks that the coordinates are in bounds and that you have not already selected the point
                if (array[row][col].showval != unchecked || !inbounds(row,col,maxrow,maxcol)){
                    printf("\nThat is not a valid input.\n");
                }

                //flags the point and prints out the board
                else {
                    flagpnt(maxrow, maxcol,array, row, col);
                    printBoard(maxrow, maxcol,array,1);
                    numturns++;
                }
                printf("\n");

          }
            //notifies the user that they did not enter a valid move
            else {
                printf("\nThat is not a valid input.\n");
                numturns--;
                continue;
            }

            //if the point selected is a bomb, it breaks out of the loop and brings them back to the opening menu
            if (array[row][col].showval == bomb){
                endTime = time(0);
                //opens up the file and prints out the game information
                fprintf(ofp,"This is the answer board:\n");
                fullBoard(ofp, maxrow,maxcol,array, startTime,endTime, numturns, &bestmin, &bestsec, numMines, exist, win, overallWins);
                fprintf(ofp, "\n");
                totalGames++;
                break;
            }
            //if they clear all of the non-bomb coordinates, prints out the win message
            if (WinCase(maxrow,maxcol,array,numMines)){
                endTime = time(0);
                //writes to the file and prints out the answer board and statistics
                fprintf(ofp,"Congratulations, you won!\n");
                win = 1;
                overallWins++;
                totalGames++;
                fullBoard(ofp, maxrow,maxcol,array, startTime,endTime, numturns, &bestmin, &bestsec, numMines, exist, win, overallWins);
                fprintf(ofp, "\n");
                break;
            }
        }

        //prints out a message if you won or lost
        if (win == 0)
            printf("\nAwe, You lost ;(\n");
        else if (win == 1)
            printf("\nCongrats, you won!!!:D\n");
        else {
            continue;
        }

        //scans in if they want to keep playing and at what difficulty level
        menu();
        scanf("%d", &choice);
        printf("\n");
        numturns = 0;
        //restarts the timer
        startTime = time(0);
  }
    printf("Your game summary is now available!\n\n");
    //closes the game summary file
    fclose(ofp);

  return 0;
}

//takes in the board and number of mines and calculates if the number of unchecked spots is equal to the number of bombs
//if the player won, function returns 1, if not it returns 0
int WinCase(int maxrow, int maxcol,struct cube array[maxrow][maxcol],int numMines){
    int placesleft = 0, bombsflaged = 0;

    //goes through the board adding up the unchecked and flagged spots
    for (int i = 0; i < maxrow; i++){
        for (int j = 0; j < maxcol; j++){
            if (array[i][j].showval == unchecked || array[i][j].showval == flag ){
                placesleft ++;
            }
            if (array[i][j].showval == flag)
                if( array[i][j].realval == -1)
                    bombsflaged++;
        }
    }
    
    //if the flag all the bombs they win
    if (bombsflaged == numMines)
        return 1;
    //if they win, returns 1
    if (placesleft == numMines)
        return 1;

    return 0;
}

//converts the realval board to the showval board then print out the answers to a file
void fullBoard(FILE *ofp, int maxrow, int maxcol, struct cube array[maxrow][maxcol],int startTime, int endTime, int numturns, int* bestmin, int* bestsec, int numMines, int exist, int win, int overallWins){

    //goes through the array and converts all the realvalues to showvalues
    for(int i = 0; i < maxrow; i++)
        for(int j = 0; j < maxcol; j++)
            if (array[i][j].showval != blankSpace)
                checkpnt(maxrow,maxcol,array,i, j);

    //prints out the numbers at the top
    fprintf(ofp,"  ");
    for(int j = 0; j < maxcol; j++){
        //formats the spacing
        if (j<10)
            fprintf(ofp,"%d ",j);
        else
            fprintf(ofp,"%d",j);
    }
    
    fprintf(ofp,"\n");

    for (int i = 0; i < maxrow; i++){
        //prints out the numbers on the side and formats the spacing
        if (i<10)
            fprintf(ofp,"%d ",i);
        else
            fprintf(ofp,"%d",i);

        //prints out the showvalues
        for(int k = 0; k < maxcol; k++) {
            fprintf(ofp,"%c ", array[i][k].showval);
        }
        fprintf(ofp, "\n");
    }

    //our minutes and seconds
    int currmin = (endTime-startTime)/60;
    int currsec = (endTime-startTime)%60;

    //prints out how many turns and how long it took to complete the game to the text file.
    fprintf(ofp, "\nNumber of turns = %d\n", numturns);
    
    //prints the game duration time
    if (currmin == 0){
        fprintf(ofp, "This game took 0 : %.2d secs\n", currsec);
    }
    else{
        fprintf(ofp, "This game took %d : %.2d mins\n", currmin, currsec);
    }

    //if you win...
    if (win == 1){
        //if you win and "Best Time:"" does not exist yet (meaning it's the first time you've won) print the current best time
        if (exist == 0){
            fprintf(ofp, "Best Time: %d : %.2d", currmin, currsec);
            *bestmin = currmin;
            *bestsec = currsec;
        }
        //if you win and "Best Time:" does exist already, check it against the current best time.
        else {
            if (currmin<*bestmin){
                *bestmin = currmin;
                *bestsec = currsec;
            }
            if (currmin==*bestmin){
                if (currsec < *bestsec){
                    *bestsec = currsec;
                }
            }
            fprintf(ofp, "Best Time: %d : %.2d", *bestmin, *bestsec);
            }
        }
    //if did not win and "Best Time:" does not exist, print out the following:
    else {
        //if previous file does not exist and you have not won a game
        if (exist == 0 && overallWins == 0)
            fprintf(ofp, "Best Time: N / A");
        else
            fprintf(ofp, "Best Time: %d : %.2d", *bestmin, *bestsec);
    }

}


// Fills the showval board with all of the blankspaces given a coordinate point and the numbers around that border it.
void revealBlank(int maxrow, int maxcol, struct cube array[maxrow][maxcol], int x, int y) {

    int i;

    // Not a valid square to go to and returns to the recursion stack for the next coordinate to be checked
    if (!inbounds(x,y,maxrow,maxcol) || array[x][y].realval != 0 || array[x][y].showval == blankSpace)
        return;

    // Fills the current spot.
    array[x][y].showval = blankSpace;

    //fills in the surrounding border of numbers around the blanks spaces
    for (i=0; i < 8 ; i++){
        if (inbounds(x+ DX[i],y+DY[i],maxrow,maxcol))
            checkpnt(maxrow, maxcol, array , x+ DX[i], y+DY[i]);
        }

    // Recursively fill all the ones blankspaces around the original point .
    for (i=0; i < 8 ; i++)
        revealBlank(maxrow, maxcol, array , x+ DX[i], y+DY[i]);

}

//checks if a given point is inbounds by comparing it to the max row and column lengths
int inbounds(int x, int y, int rows, int cols) {
    return x >= 0 && x < rows && y >= 0 && y < cols;
}

//takes in the board and puts in the bombs randomly
void PutInBombs(int maxrow, int maxcol, struct cube array[maxrow][maxcol], int numMines){
    int i = 0, row, col, k;

    //makes sure a bomb is not put right next to where you click
    //goes through the array
     for (int w = 0; w<maxrow;w++){
        for(int c = 0; c<maxrow; c++){
                
            //if it has been clicked on and is a blank space, converts surrounding squares to -4
             if (array[w][c].showval == blankSpace){
                 
                //goes through the coordinates
                for ( k = 0; k< 8;k++){
                    
                    //makes sure it is in bounds
                    if (inbounds(w + DX[k],c + DY[k], maxrow,maxcol))
                            array[w + DX[k]][c+DY[k]].realval = -4;
                }
            }
        }
    }
    //randomly places mines
    while (i < numMines){
        row = rand()%maxrow;
        col = rand()%maxcol;

        //makes sure that it does not overwrite a previous bomb or is near the first click
       if ((array[row][col].realval != -1) && (array[row][col].showval != blankSpace) && (array[row][col].realval != -4)){
            array[row][col].realval = -1;
            i++;
        }
    }
    //make all the -4s zeros again
    for (int w = 0; w<maxrow;w++){
            for(int c = 0; c<maxrow; c++){
                 if (array[w][c].realval == -4)
                        array[w][c].realval = 0;
        }
    }
}

//given a coordinate, changes the showval symbol to reflect the realval number
void checkpnt(int maxrow, int maxcol, struct cube array[maxrow][maxcol], int row, int col){

    //if it is -1, it prints out a bomb
    if (array[row][col].realval == -1)
        array[row][col].showval = bomb;

    //if it is 0, it prints out a blank space as well as surrounding blank spaces
    else if (array[row][col].realval == 0)
        //calls function to print surrounding blank spaces
        revealBlank(maxrow, maxcol, array, row, col);

    //prints out the numbers that correspond to nearby bombs
    else
        array[row][col].showval = array[row][col].realval +'0';
}

//flags a point by changing the showval to the flag symbol
void flagpnt(int maxrow, int maxcol, struct cube array[maxrow][maxcol], int row, int col){
    //makes sure that player is not trying to flag a point that has already been revealed
    if (array [row][col].showval == unchecked)
        array[row][col].showval = flag;
    else
        printf("That is not a valid move");
}

//prints out the player's options
int MoveOptions(){
    int move;
    printf("Would you like to:\n\t [1] Enter coordinates. \n\t [2] Flag a point.");
    printf("\n");
    scanf("%d", &move);

    return move;
}

//prints out the main menu
void menu (){
    printf("\n");
    printf("Please select your desired difficulty level:\n");
    printf("For beginner, press 1.\n");
    printf("For intermediate, press 2.\n");
    printf("For hard, press 3.\n");
    printf("To quit, press 4.\n");
}

//takes in an cube with 2 empty 2D array and initializes it with '#' for showval and 0's for realval
void initBoard(int maxrow, int maxcol, struct cube array[maxrow][maxcol]){
    for (int i = 0; i < maxrow; i++)
        for(int j = 0; j < maxcol; j++)
            //showval represents the board that payers see; realval represents the behind the scenes calculations and debugging component
            (array[i][j].showval = unchecked) && (array[i][j].realval = 0);
}

//takes in array and the size parameters and prints it out
void printBoard(int maxrow, int maxcol, struct cube array[maxrow][maxcol],int printboard){

    //prints out the top numbers
    printf("  ");
    for(int j = 0; j < maxcol; j++){
        //formats the spacing
        if (printboard==2){
            if (j<10)
            printf(" %d ",j);
            else
            printf(" %d",j);
        }
        else{
            if (j<10)
                printf("%d ",j);
            else
                printf("%d",j);
        }

    }
    printf("\n");

    for (int i = 0; i < maxrow; i++){
        //prints out the side numbers and formats the spacing
        if (i<10)
            printf("%d ",i);
        else
            printf("%d",i);

        //prints out the showvalues
        for(int k = 0; k < maxcol; k++) {

            //prints out the realval board for debugging
            if (printboard == 2){
                //turns blank space char into a integer, -2
                if (array[i][k].showval == blankSpace)
                    array[i][k].realval = -2;
                //prints out all numbers, zero represents a blank space in showval
                if (array[i][k].realval >= 0)
                    printf(" %d ", array[i][k].realval);
                else
                    printf("%d ", array[i][k].realval);
            }
            //prints out the showval board to the terminal
            else
                printf("%c ", array[i][k].showval);
        }
        printf("\n");
    }
}

//takes in a array initialized with bombs and adds numbers next to the bombs to detail proximity
void initNum(int maxrow, int maxcol, struct cube array[maxrow][maxcol]){

    int k,w,i,j;
    //runs through the whole array to find bombs
    for( i = 0; i < maxrow; i++){
        for ( j = 0; j < maxcol; j++){
            //if it is equal to a bomb, add nums to the surrounding squares
            if (array[i][j].realval == -1){
                //runs through the DX DY array to check all the coordinates
                for ( k = 0; k< 8;k++){
                    //checks if it is inbounds
                    if (inbounds(i+DX[k],j+DY[k], maxrow,maxcol)) {
                        //makes sure that it does not change a bomb or a spot clicked into a numbers
                        if (array[i + DX[k]][j+DY[k]].realval != -1 && array[i + DX[k]][j+DY[k]].realval != -2){
                            array[i + DX[k]][j+DY[k]].realval += 1;
                        }
                    }
                }
            }
        }
    }
}
