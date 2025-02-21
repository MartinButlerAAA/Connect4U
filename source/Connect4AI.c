// Connect4AI.c : Java Connect 4 program converted to C/C++ to build under Visual Studio.

#include <stdlib.h>
#include <stdbool.h>
#include "Connect4AI.h"

	   char     gameTable[7][6]; 	                    // Standard Connect 4 game board with 7 rows and 6 columns.
static int      winningColumns[7][3][2];	            // Array of possible vertical winning groups of 4, with counts of 'R' red and 'Y' pieces.
static int      winningRows[4][6][2];		            // Array of possible horizontal winning groups of 4, with counts of 'R' red and 'Y' pieces.
static int      winningDiagonalsUp[4][3][2];	        // Array of possible diagonal up/right winning groups of 4, with counts of 'R' red and 'Y' pieces.
static int      winningDiagonalsDown[4][3][2];	        // Array of possible diagonal down/right winning groups of 4, with counts of 'R' red and 'Y' pieces.
static double   combinedScoresR[7][6];		            // Array used to combine vertical, horizontal and diagonal scores for each board position for red.
static double   combinedScoresY[7][6];		            // Array used to combine vertical, horizontal and diagonal scores for each board position for yellow.

// The default values below have been selected after optimisation.
static double	PIECESDEFAULT = 5.0;	 			    // (x from documentation) tweaked from 4	
static int		RANDOMDEFAULT = 4;				        // (r from documentation) tweaked from 2
static double	HORIZONTALDEFAULT = 1.02;		        // (h from documentation) 
static double	VERTICALDEFAULT = 1.64;			        // (v from documentation)
static double	DIAGONALDEFAULT = 1.70;			        // (d from documentation)
static double	NEXTMOVEDEFAULT = 0.7;			        // (n from documentation)

// Quick approximation of a pow function as the math.h does not seem to be available on Wii U.
float myPow(float power, float num)
{
	int intPower = (int)power;
	float retVal = 0;

	// If the power value passed in is outside a sensible range to process, exit with an error code.
	if (intPower < 0) { return -99.0; }
	if (intPower >= 100) { return -99.0; }

	if (intPower == 0) { return 0.0; }	// Handle power of 0.
	if (intPower == 1) { return num; }	// Handle power of 1, i.e. no change.

	// Loop to multiply by the number of times set by power.
	retVal = num;
	for (int a = 1; a < intPower; a++)
	{
		retVal = retVal * num;
	}
	return retVal;
}

char getGameTable(int x, int y)	// Standard Connect 4 game board with 7 rows and 6 columns, API to get current board pieces.
{
	// If the pass parameters are in range of the gameTable, return the corresponding character
	if ((x >= 0) && (x <= 6) && (y >= 0) && (y <= 5))
	{
		return (gameTable[x][y]);
	}
	return (' ');
}

// This is a small function to clear all places in the game table ready for a new game.
void clearGameTable()
{
	for (int x = 0; x < 7; x++)
	{
		for (int y = 0; y < 6; y++)
		{
			gameTable[x][y] = ' ';
		}
	}
	return;
}

// This function checks the move entered and puts it into gameTable if it is a valid move.
bool putMove(int move) 
{
	// Only if the move is inside the game table and the column has space is the move used and the function exited.
	if ((move >= 1) && (move <= 7))
	{
		// If there is space available in the column selected make the move and return pass.
		for (int y = 0; y < 6; y++)
		{
			if (gameTable[move - 1][y] == ' ')
			{
				gameTable[move - 1][y] = 'R';
				return true;
			}
		}
	}
	// If we got to here the move was not valid, so return false.
	return false;
}

// This function looks through all of the possible winning lines of 4 to see if there is a winner. If there is a winner R is returned for red or Y for yellow.
// If the top row of the board is full the game is over and it was a draw, otherwise a space is returned.
char gameEnded() 
{
	// Check for vertical winning lines.
	for (int x = 0; x < 7; x++) 
	{
		for (int y = 0; y < 3; y++) 
		{
			if ((gameTable[x][y] == 'R') && (gameTable[x][y + 1] == 'R') && (gameTable[x][y + 2] == 'R') && (gameTable[x][y + 3] == 'R')) 
			{
				return 'R';
			}
			if ((gameTable[x][y] == 'Y') && (gameTable[x][y + 1] == 'Y') && (gameTable[x][y + 2] == 'Y') && (gameTable[x][y + 3] == 'Y')) 
			{
				return 'Y';
			}
		}
	}
	// Check for horizontal winning lines.
	for (int x = 0; x < 4; x++) 
	{
		for (int y = 0; y < 6; y++) 
		{
			if ((gameTable[x][y] == 'R') && (gameTable[x + 1][y] == 'R') && (gameTable[x + 2][y] == 'R') && (gameTable[x + 3][y] == 'R')) 
			{
				return 'R';
			}
			if ((gameTable[x][y] == 'Y') && (gameTable[x + 1][y] == 'Y') && (gameTable[x + 2][y] == 'Y') && (gameTable[x + 3][y] == 'Y')) 
			{
				return 'Y';
			}
		}
	}
	// Check for diagonal up winning lines.
	for (int x = 0; x < 4; x++) 
	{
		for (int y = 0; y < 3; y++) 
		{
			if ((gameTable[x][y] == 'R') && (gameTable[x + 1][y + 1] == 'R') && (gameTable[x + 2][y + 2] == 'R') && (gameTable[x + 3][y + 3] == 'R')) 
			{
				return 'R';
			}
			if ((gameTable[x][y] == 'Y') && (gameTable[x + 1][y + 1] == 'Y') && (gameTable[x + 2][y + 2] == 'Y') && (gameTable[x + 3][y + 3] == 'Y')) 
			{
				return 'Y';
			}
		}
	}
	// Check for diagonal down winning lines.
	for (int x = 0; x < 4; x++) 
	{
		for (int y = 3; y < 6; y++) 
		{
			if ((gameTable[x][y] == 'R') && (gameTable[x + 1][y - 1] == 'R') && (gameTable[x + 2][y - 2] == 'R') && (gameTable[x + 3][y - 3] == 'R')) 
			{
				return 'R';
			}
			if ((gameTable[x][y] == 'Y') && (gameTable[x + 1][y - 1] == 'Y') && (gameTable[x + 2][y - 2] == 'Y') && (gameTable[x + 3][y - 3] == 'Y')) 
			{
				return 'Y';
			}
		}
	}
	// Check if the top row is full, showing the board is full and therefore the end of the game.
	if ((gameTable[0][5] != ' ') && (gameTable[1][5] != ' ') && (gameTable[2][5] != ' ') && (gameTable[3][5] != ' ') && (gameTable[4][5] != ' ') && (gameTable[5][5] != ' ') && (gameTable[6][5] != ' ')) 
	{
		return 'D';
	}

	// If not over, return a blank.
	return ' ';
}

// This function populates the array of piece counts for each players columns.
// 21 neurons
void doWinningColumns() 
{
	for (int x = 0; x < 7; x++) 
	{
		for (int y = 0; y < 3; y++) 
		{
			winningColumns[x][y][0] = 0;
			if (gameTable[x][y] == 'R') { winningColumns[x][y][0] = winningColumns[x][y][0] + 1; }
			if (gameTable[x][y + 1] == 'R') { winningColumns[x][y][0] = winningColumns[x][y][0] + 1; }
			if (gameTable[x][y + 2] == 'R') { winningColumns[x][y][0] = winningColumns[x][y][0] + 1; }
			if (gameTable[x][y + 3] == 'R') { winningColumns[x][y][0] = winningColumns[x][y][0] + 1; }

			winningColumns[x][y][1] = 0;
			if (gameTable[x][y] == 'Y') { winningColumns[x][y][1] = winningColumns[x][y][1] + 1; }
			if (gameTable[x][y + 1] == 'Y') { winningColumns[x][y][1] = winningColumns[x][y][1] + 1; }
			if (gameTable[x][y + 2] == 'Y') { winningColumns[x][y][1] = winningColumns[x][y][1] + 1; }
			if (gameTable[x][y + 3] == 'Y') { winningColumns[x][y][1] = winningColumns[x][y][1] + 1; }

			// If both players have pieces, the line is blocked and no longer part of the game, so set the scores to zero.
			if ((winningColumns[x][y][0] != 0) && (winningColumns[x][y][1] != 0)) 
			{
				winningColumns[x][y][0] = 0;
				winningColumns[x][y][1] = 0;
			}
		}
	}
	return;
}

// This function populates the array of piece counts for each players rows.
// 24 neurons
static void doWinningRows() 
{
	for (int x = 0; x < 4; x++) 
	{
		for (int y = 0; y < 6; y++) 
		{
			winningRows[x][y][0] = 0;
			if (gameTable[x][y] == 'R') { winningRows[x][y][0] = winningRows[x][y][0] + 1; }
			if (gameTable[x + 1][y] == 'R') { winningRows[x][y][0] = winningRows[x][y][0] + 1; }
			if (gameTable[x + 2][y] == 'R') { winningRows[x][y][0] = winningRows[x][y][0] + 1; }
			if (gameTable[x + 3][y] == 'R') { winningRows[x][y][0] = winningRows[x][y][0] + 1; }

			winningRows[x][y][1] = 0;
			if (gameTable[x][y] == 'Y') { winningRows[x][y][1] = winningRows[x][y][1] + 1; }
			if (gameTable[x + 1][y] == 'Y') { winningRows[x][y][1] = winningRows[x][y][1] + 1; }
			if (gameTable[x + 2][y] == 'Y') { winningRows[x][y][1] = winningRows[x][y][1] + 1; }
			if (gameTable[x + 3][y] == 'Y') { winningRows[x][y][1] = winningRows[x][y][1] + 1; }

			// If both players have pieces, the line is blocked and no longer part of the game, so set the scores to zero.
			if ((winningRows[x][y][0] != 0) && (winningRows[x][y][1] != 0)) 
			{
				winningRows[x][y][0] = 0;
				winningRows[x][y][1] = 0;
			}
		}
	}
	return;
}

// This function populates the array of piece counts for each players diagonals going up/right.
// 12 neurons
void doWinningDiagonalsUp() 
{
	for (int x = 0; x < 4; x++) 
	{
		for (int y = 0; y < 3; y++) 
		{
			winningDiagonalsUp[x][y][0] = 0;
			if (gameTable[x][y] == 'R') { winningDiagonalsUp[x][y][0] = winningDiagonalsUp[x][y][0] + 1; }
			if (gameTable[x + 1][y + 1] == 'R') { winningDiagonalsUp[x][y][0] = winningDiagonalsUp[x][y][0] + 1; }
			if (gameTable[x + 2][y + 2] == 'R') { winningDiagonalsUp[x][y][0] = winningDiagonalsUp[x][y][0] + 1; }
			if (gameTable[x + 3][y + 3] == 'R') { winningDiagonalsUp[x][y][0] = winningDiagonalsUp[x][y][0] + 1; }

			winningDiagonalsUp[x][y][1] = 0;
			if (gameTable[x][y] == 'Y') { winningDiagonalsUp[x][y][1] = winningDiagonalsUp[x][y][1] + 1; }
			if (gameTable[x + 1][y + 1] == 'Y') { winningDiagonalsUp[x][y][1] = winningDiagonalsUp[x][y][1] + 1; }
			if (gameTable[x + 2][y + 2] == 'Y') { winningDiagonalsUp[x][y][1] = winningDiagonalsUp[x][y][1] + 1; }
			if (gameTable[x + 3][y + 3] == 'Y') { winningDiagonalsUp[x][y][1] = winningDiagonalsUp[x][y][1] + 1; }

			// If both players have pieces, the line is blocked and no longer part of the game, so set the scores to zero.
			if ((winningDiagonalsUp[x][y][0] != 0) && (winningDiagonalsUp[x][y][1] != 0)) 
			{
				winningDiagonalsUp[x][y][0] = 0;
				winningDiagonalsUp[x][y][1] = 0;
			}
		}
	}
	return;
}

// This function populates the array of piece counts for each players diagonals going down/right.
// 12 neurons
void doWinningDiagonalsDown() 
{
	for (int x = 0; x < 4; x++) 
	{
		for (int y = 3; y < 6; y++) 
		{
			winningDiagonalsDown[x][y - 3][0] = 0;
			if (gameTable[x][y] == 'R') { winningDiagonalsDown[x][y - 3][0] = winningDiagonalsDown[x][y - 3][0] + 1; }
			if (gameTable[x + 1][y - 1] == 'R') { winningDiagonalsDown[x][y - 3][0] = winningDiagonalsDown[x][y - 3][0] + 1; }
			if (gameTable[x + 2][y - 2] == 'R') { winningDiagonalsDown[x][y - 3][0] = winningDiagonalsDown[x][y - 3][0] + 1; }
			if (gameTable[x + 3][y - 3] == 'R') { winningDiagonalsDown[x][y - 3][0] = winningDiagonalsDown[x][y - 3][0] + 1; }

			winningDiagonalsDown[x][y - 3][1] = 0;
			if (gameTable[x][y] == 'Y') { winningDiagonalsDown[x][y - 3][1] = winningDiagonalsDown[x][y - 3][1] + 1; }
			if (gameTable[x + 1][y - 1] == 'Y') { winningDiagonalsDown[x][y - 3][1] = winningDiagonalsDown[x][y - 3][1] + 1; }
			if (gameTable[x + 2][y - 2] == 'Y') { winningDiagonalsDown[x][y - 3][1] = winningDiagonalsDown[x][y - 3][1] + 1; }
			if (gameTable[x + 3][y - 3] == 'Y') { winningDiagonalsDown[x][y - 3][1] = winningDiagonalsDown[x][y - 3][1] + 1; }

			// If both players have pieces, the line is blocked and no longer part of the game, so set the scores to zero.
			if ((winningDiagonalsDown[x][y - 3][0] != 0) && (winningDiagonalsDown[x][y - 3][1] != 0)) 
			{
				winningDiagonalsDown[x][y - 3][0] = 0;
				winningDiagonalsDown[x][y - 3][1] = 0;
			}
		}
	}
	return;
}

// This function populates the combined score array to determine moves.
// 42 neurons
void doCombinedScores(void) 
{	// The weights is 1 default, 2 current or 3 new
	// To simplify neuron processing for people to understand the arrays are re-populated to 7x6 to match the game board so that they can be added.
	double combinedColumnsR[7][6];
	double combinedColumnsY[7][6];
	double combinedRowsR[7][6];
	double combinedRowsY[7][6];
	double combinedDiagonalsUpR[7][6];
	double combinedDiagonalsUpY[7][6];
	double combinedDiagonalsDownR[7][6];
	double combinedDiagonalsDownY[7][6];

	// Local copies of weightings so that code can be used with each different type of weighting.
	double 	piecesLocal = PIECESDEFAULT;	// Local copy taken so it can be optimised.
	double 	horizontalLocal = HORIZONTALDEFAULT;
	double 	verticalLocal = VERTICALDEFAULT;
	double 	diagonalLocal = DIAGONALDEFAULT;
	int		randomLocal = RANDOMDEFAULT;	// Local copy taken so it can be optimised.			

	// Do arrays of 7 by 6 of the scores for vertical.
	// This can't all be simplified with a loop, positions in the middle of the table are in multiple lines of 4.
	for (int x = 0; x < 7; x++) 
	{
		combinedColumnsR[x][0] = myPow(piecesLocal, winningColumns[x][0][0]);
		combinedColumnsR[x][1] = myPow(piecesLocal, winningColumns[x][0][0]) + myPow(piecesLocal, winningColumns[x][1][0]);
		combinedColumnsR[x][2] = myPow(piecesLocal, winningColumns[x][0][0]) + myPow(piecesLocal, winningColumns[x][1][0]) + myPow(piecesLocal, winningColumns[x][2][0]);
		combinedColumnsR[x][3] = myPow(piecesLocal, winningColumns[x][0][0]) + myPow(piecesLocal, winningColumns[x][1][0]) + myPow(piecesLocal, winningColumns[x][2][0]);
		combinedColumnsR[x][4] = myPow(piecesLocal, winningColumns[x][1][0]) + myPow(piecesLocal, winningColumns[x][2][0]);
		combinedColumnsR[x][5] = myPow(piecesLocal, winningColumns[x][2][0]);

		combinedColumnsY[x][0] = myPow(piecesLocal, winningColumns[x][0][1]);
		combinedColumnsY[x][1] = myPow(piecesLocal, winningColumns[x][0][1]) + myPow(piecesLocal, winningColumns[x][1][1]);
		combinedColumnsY[x][2] = myPow(piecesLocal, winningColumns[x][0][1]) + myPow(piecesLocal, winningColumns[x][1][1]) + myPow(piecesLocal, winningColumns[x][2][1]);
		combinedColumnsY[x][3] = myPow(piecesLocal, winningColumns[x][0][1]) + myPow(piecesLocal, winningColumns[x][1][1]) + myPow(piecesLocal, winningColumns[x][2][1]);
		combinedColumnsY[x][4] = myPow(piecesLocal, winningColumns[x][1][1]) + myPow(piecesLocal, winningColumns[x][2][1]);
		combinedColumnsY[x][5] = myPow(piecesLocal, winningColumns[x][2][1]);
	}

	// Do arrays of 7 by 6 for the scores for horizontal.
	// This can't all be simplified with a loop, positions in the middle of the table are in multiple lines of 4.
	for (int y = 0; y < 6; y++) 
	{
		combinedRowsR[0][y] = myPow(piecesLocal, winningRows[0][y][0]);
		combinedRowsR[1][y] = myPow(piecesLocal, winningRows[0][y][0]) + myPow(piecesLocal, winningRows[1][y][0]);
		combinedRowsR[2][y] = myPow(piecesLocal, winningRows[0][y][0]) + myPow(piecesLocal, winningRows[1][y][0]) + myPow(piecesLocal, winningRows[2][y][0]);
		combinedRowsR[3][y] = myPow(piecesLocal, winningRows[0][y][0]) + myPow(piecesLocal, winningRows[1][y][0]) + myPow(piecesLocal, winningRows[2][y][0]) + myPow(piecesLocal, winningRows[3][y][0]);
		combinedRowsR[4][y] = myPow(piecesLocal, winningRows[1][y][0]) + myPow(piecesLocal, winningRows[2][y][0]) + myPow(piecesLocal, winningRows[3][y][0]);
		combinedRowsR[5][y] = myPow(piecesLocal, winningRows[2][y][0]) + myPow(piecesLocal, winningRows[3][y][0]);
		combinedRowsR[6][y] = myPow(piecesLocal, winningRows[3][y][0]);

		combinedRowsY[0][y] = myPow(piecesLocal, winningRows[0][y][1]);
		combinedRowsY[1][y] = myPow(piecesLocal, winningRows[0][y][1]) + myPow(piecesLocal, winningRows[1][y][1]);
		combinedRowsY[2][y] = myPow(piecesLocal, winningRows[0][y][1]) + myPow(piecesLocal, winningRows[1][y][1]) + myPow(piecesLocal, winningRows[2][y][1]);
		combinedRowsY[3][y] = myPow(piecesLocal, winningRows[0][y][1]) + myPow(piecesLocal, winningRows[1][y][1]) + myPow(piecesLocal, winningRows[2][y][1]) + myPow(piecesLocal, winningRows[3][y][1]);
		combinedRowsY[4][y] = myPow(piecesLocal, winningRows[1][y][1]) + myPow(piecesLocal, winningRows[2][y][1]) + myPow(piecesLocal, winningRows[3][y][1]);
		combinedRowsY[5][y] = myPow(piecesLocal, winningRows[2][y][1]) + myPow(piecesLocal, winningRows[3][y][1]);
		combinedRowsY[6][y] = myPow(piecesLocal, winningRows[3][y][1]);
	}

	// Do an arrays of 7 by 6 for diagonals.
	// Diagonals need to be processed with a specific line of code for each position. Some positions are not part of diagonals at all, some are in one and some are more than one diagonal of 4.
	combinedDiagonalsUpR[0][0] = myPow(piecesLocal, winningDiagonalsUp[0][0][0]);
	combinedDiagonalsUpR[1][0] = myPow(piecesLocal, winningDiagonalsUp[1][0][0]);
	combinedDiagonalsUpR[2][0] = myPow(piecesLocal, winningDiagonalsUp[2][0][0]);
	combinedDiagonalsUpR[3][0] = myPow(piecesLocal, winningDiagonalsUp[3][0][0]);
	combinedDiagonalsUpR[4][0] = 0;
	combinedDiagonalsUpR[5][0] = 0;
	combinedDiagonalsUpR[6][0] = 0;
	combinedDiagonalsUpR[0][1] = myPow(piecesLocal, winningDiagonalsUp[0][1][0]);
	combinedDiagonalsUpR[1][1] = myPow(piecesLocal, winningDiagonalsUp[0][0][0]) + myPow(piecesLocal, winningDiagonalsUp[1][1][0]);
	combinedDiagonalsUpR[2][1] = myPow(piecesLocal, winningDiagonalsUp[1][0][0]) + myPow(piecesLocal, winningDiagonalsUp[2][1][0]);
	combinedDiagonalsUpR[3][1] = myPow(piecesLocal, winningDiagonalsUp[2][0][0]) + myPow(piecesLocal, winningDiagonalsUp[3][1][0]);
	combinedDiagonalsUpR[4][1] = myPow(piecesLocal, winningDiagonalsUp[3][0][0]);
	combinedDiagonalsUpR[5][1] = 0;
	combinedDiagonalsUpR[6][1] = 0;
	combinedDiagonalsUpR[0][2] = myPow(piecesLocal, winningDiagonalsUp[0][2][0]);
	combinedDiagonalsUpR[1][2] = myPow(piecesLocal, winningDiagonalsUp[0][1][0]) + myPow(piecesLocal, winningDiagonalsUp[1][2][0]);
	combinedDiagonalsUpR[2][2] = myPow(piecesLocal, winningDiagonalsUp[0][0][0]) + myPow(piecesLocal, winningDiagonalsUp[1][1][0]) + myPow(piecesLocal, winningDiagonalsUp[2][2][0]);
	combinedDiagonalsUpR[3][2] = myPow(piecesLocal, winningDiagonalsUp[1][0][0]) + myPow(piecesLocal, winningDiagonalsUp[2][1][0]) + myPow(piecesLocal, winningDiagonalsUp[3][2][0]);
	combinedDiagonalsUpR[4][2] = myPow(piecesLocal, winningDiagonalsUp[2][0][0]) + myPow(piecesLocal, winningDiagonalsUp[3][1][0]);
	combinedDiagonalsUpR[5][2] = myPow(piecesLocal, winningDiagonalsUp[3][0][0]);
	combinedDiagonalsUpR[6][2] = 0;
	combinedDiagonalsUpR[0][3] = 0;
	combinedDiagonalsUpR[1][3] = myPow(piecesLocal, winningDiagonalsUp[0][2][0]);
	combinedDiagonalsUpR[2][3] = myPow(piecesLocal, winningDiagonalsUp[0][1][0]) + myPow(piecesLocal, winningDiagonalsUp[1][2][0]);
	combinedDiagonalsUpR[3][3] = myPow(piecesLocal, winningDiagonalsUp[0][0][0]) + myPow(piecesLocal, winningDiagonalsUp[1][1][0]) + myPow(piecesLocal, winningDiagonalsUp[2][2][0]);
	combinedDiagonalsUpR[4][3] = myPow(piecesLocal, winningDiagonalsUp[1][0][0]) + myPow(piecesLocal, winningDiagonalsUp[2][1][0]) + myPow(piecesLocal, winningDiagonalsUp[3][2][0]);
	combinedDiagonalsUpR[5][3] = myPow(piecesLocal, winningDiagonalsUp[2][0][0]) + myPow(piecesLocal, winningDiagonalsUp[3][1][0]);
	combinedDiagonalsUpR[6][3] = myPow(piecesLocal, winningDiagonalsUp[3][0][0]);
	combinedDiagonalsUpR[0][4] = 0;
	combinedDiagonalsUpR[1][4] = 0;
	combinedDiagonalsUpR[2][4] = myPow(piecesLocal, winningDiagonalsUp[0][2][0]);
	combinedDiagonalsUpR[3][4] = myPow(piecesLocal, winningDiagonalsUp[0][1][0]) + myPow(piecesLocal, winningDiagonalsUp[1][2][0]);
	combinedDiagonalsUpR[4][4] = myPow(piecesLocal, winningDiagonalsUp[1][1][0]) + myPow(piecesLocal, winningDiagonalsUp[2][2][0]);
	combinedDiagonalsUpR[5][4] = myPow(piecesLocal, winningDiagonalsUp[2][1][0]) + myPow(piecesLocal, winningDiagonalsUp[3][2][0]);
	combinedDiagonalsUpR[6][4] = myPow(piecesLocal, winningDiagonalsUp[3][1][0]);
	combinedDiagonalsUpR[0][5] = 0;
	combinedDiagonalsUpR[1][5] = 0;
	combinedDiagonalsUpR[2][5] = 0;
	combinedDiagonalsUpR[3][5] = myPow(piecesLocal, winningDiagonalsUp[0][2][0]);
	combinedDiagonalsUpR[4][5] = myPow(piecesLocal, winningDiagonalsUp[1][2][0]);
	combinedDiagonalsUpR[5][5] = myPow(piecesLocal, winningDiagonalsUp[2][2][0]);
	combinedDiagonalsUpR[6][5] = myPow(piecesLocal, winningDiagonalsUp[3][2][0]);

	combinedDiagonalsUpY[0][0] = myPow(piecesLocal, winningDiagonalsUp[0][0][1]);
	combinedDiagonalsUpY[1][0] = myPow(piecesLocal, winningDiagonalsUp[1][0][1]);
	combinedDiagonalsUpY[2][0] = myPow(piecesLocal, winningDiagonalsUp[2][0][1]);
	combinedDiagonalsUpY[3][0] = myPow(piecesLocal, winningDiagonalsUp[3][0][1]);
	combinedDiagonalsUpY[4][0] = 0;
	combinedDiagonalsUpY[5][0] = 0;
	combinedDiagonalsUpY[6][0] = 0;
	combinedDiagonalsUpY[0][1] = myPow(piecesLocal, winningDiagonalsUp[0][1][1]);
	combinedDiagonalsUpY[1][1] = myPow(piecesLocal, winningDiagonalsUp[0][0][1]) + myPow(piecesLocal, winningDiagonalsUp[1][1][1]);
	combinedDiagonalsUpY[2][1] = myPow(piecesLocal, winningDiagonalsUp[1][0][1]) + myPow(piecesLocal, winningDiagonalsUp[2][1][1]);
	combinedDiagonalsUpY[3][1] = myPow(piecesLocal, winningDiagonalsUp[2][0][1]) + myPow(piecesLocal, winningDiagonalsUp[3][1][1]);
	combinedDiagonalsUpY[4][1] = myPow(piecesLocal, winningDiagonalsUp[3][0][1]);
	combinedDiagonalsUpY[5][1] = 0;
	combinedDiagonalsUpY[6][1] = 0;
	combinedDiagonalsUpY[0][2] = myPow(piecesLocal, winningDiagonalsUp[0][2][1]);
	combinedDiagonalsUpY[1][2] = myPow(piecesLocal, winningDiagonalsUp[0][1][1]) + myPow(piecesLocal, winningDiagonalsUp[1][2][1]);
	combinedDiagonalsUpY[2][2] = myPow(piecesLocal, winningDiagonalsUp[0][0][1]) + myPow(piecesLocal, winningDiagonalsUp[1][1][1]) + myPow(piecesLocal, winningDiagonalsUp[2][2][1]);
	combinedDiagonalsUpY[3][2] = myPow(piecesLocal, winningDiagonalsUp[1][0][1]) + myPow(piecesLocal, winningDiagonalsUp[2][1][1]) + myPow(piecesLocal, winningDiagonalsUp[3][2][1]);
	combinedDiagonalsUpY[4][2] = myPow(piecesLocal, winningDiagonalsUp[2][0][1]) + myPow(piecesLocal, winningDiagonalsUp[3][1][1]);
	combinedDiagonalsUpY[5][2] = myPow(piecesLocal, winningDiagonalsUp[3][0][1]);
	combinedDiagonalsUpY[6][2] = 0;
	combinedDiagonalsUpY[0][3] = 0;
	combinedDiagonalsUpY[1][3] = myPow(piecesLocal, winningDiagonalsUp[0][2][1]);
	combinedDiagonalsUpY[2][3] = myPow(piecesLocal, winningDiagonalsUp[0][1][1]) + myPow(piecesLocal, winningDiagonalsUp[1][2][1]);
	combinedDiagonalsUpY[3][3] = myPow(piecesLocal, winningDiagonalsUp[0][0][1]) + myPow(piecesLocal, winningDiagonalsUp[1][1][1]) + myPow(piecesLocal, winningDiagonalsUp[2][2][1]);
	combinedDiagonalsUpY[4][3] = myPow(piecesLocal, winningDiagonalsUp[1][0][1]) + myPow(piecesLocal, winningDiagonalsUp[2][1][1]) + myPow(piecesLocal, winningDiagonalsUp[3][2][1]);
	combinedDiagonalsUpY[5][3] = myPow(piecesLocal, winningDiagonalsUp[2][0][1]) + myPow(piecesLocal, winningDiagonalsUp[3][1][1]);
	combinedDiagonalsUpY[6][3] = myPow(piecesLocal, winningDiagonalsUp[3][0][1]);
	combinedDiagonalsUpY[0][4] = 0;
	combinedDiagonalsUpY[1][4] = 0;
	combinedDiagonalsUpY[2][4] = myPow(piecesLocal, winningDiagonalsUp[0][2][1]);
	combinedDiagonalsUpY[3][4] = myPow(piecesLocal, winningDiagonalsUp[0][1][1]) + myPow(piecesLocal, winningDiagonalsUp[1][2][1]);
	combinedDiagonalsUpY[4][4] = myPow(piecesLocal, winningDiagonalsUp[1][1][1]) + myPow(piecesLocal, winningDiagonalsUp[2][2][1]);
	combinedDiagonalsUpY[5][4] = myPow(piecesLocal, winningDiagonalsUp[2][1][1]) + myPow(piecesLocal, winningDiagonalsUp[3][2][1]);
	combinedDiagonalsUpY[6][4] = myPow(piecesLocal, winningDiagonalsUp[3][1][1]);
	combinedDiagonalsUpY[0][5] = 0;
	combinedDiagonalsUpY[1][5] = 0;
	combinedDiagonalsUpY[2][5] = 0;
	combinedDiagonalsUpY[3][5] = myPow(piecesLocal, winningDiagonalsUp[0][2][1]);
	combinedDiagonalsUpY[4][5] = myPow(piecesLocal, winningDiagonalsUp[1][2][1]);
	combinedDiagonalsUpY[5][5] = myPow(piecesLocal, winningDiagonalsUp[2][2][1]);
	combinedDiagonalsUpY[6][5] = myPow(piecesLocal, winningDiagonalsUp[3][2][1]);

	combinedDiagonalsDownR[0][0] = 0;
	combinedDiagonalsDownR[1][0] = 0;
	combinedDiagonalsDownR[2][0] = 0;
	combinedDiagonalsDownR[3][0] = myPow(piecesLocal, winningDiagonalsDown[0][0][0]);
	combinedDiagonalsDownR[4][0] = myPow(piecesLocal, winningDiagonalsDown[1][0][0]);
	combinedDiagonalsDownR[5][0] = myPow(piecesLocal, winningDiagonalsDown[2][0][0]);
	combinedDiagonalsDownR[6][0] = myPow(piecesLocal, winningDiagonalsDown[3][0][0]);
	combinedDiagonalsDownR[0][1] = 0;
	combinedDiagonalsDownR[1][1] = 0;
	combinedDiagonalsDownR[2][1] = myPow(piecesLocal, winningDiagonalsDown[0][0][0]);
	combinedDiagonalsDownR[3][1] = myPow(piecesLocal, winningDiagonalsDown[0][1][0]) + myPow(piecesLocal, winningDiagonalsDown[1][0][0]);
	combinedDiagonalsDownR[4][1] = myPow(piecesLocal, winningDiagonalsDown[1][1][0]) + myPow(piecesLocal, winningDiagonalsDown[2][0][0]);
	combinedDiagonalsDownR[5][1] = myPow(piecesLocal, winningDiagonalsDown[2][1][0]) + myPow(piecesLocal, winningDiagonalsDown[3][0][0]);
	combinedDiagonalsDownR[6][1] = myPow(piecesLocal, winningDiagonalsDown[3][1][0]);
	combinedDiagonalsDownR[0][2] = 0;
	combinedDiagonalsDownR[1][2] = myPow(piecesLocal, winningDiagonalsDown[0][0][0]);
	combinedDiagonalsDownR[2][2] = myPow(piecesLocal, winningDiagonalsDown[0][1][0]) + myPow(piecesLocal, winningDiagonalsDown[1][0][0]);
	combinedDiagonalsDownR[3][2] = myPow(piecesLocal, winningDiagonalsDown[0][2][0]) + myPow(piecesLocal, winningDiagonalsDown[1][1][0]) + myPow(piecesLocal, winningDiagonalsDown[2][0][0]);
	combinedDiagonalsDownR[4][2] = myPow(piecesLocal, winningDiagonalsDown[1][2][0]) + myPow(piecesLocal, winningDiagonalsDown[2][1][0]) + myPow(piecesLocal, winningDiagonalsDown[3][0][0]);
	combinedDiagonalsDownR[5][2] = myPow(piecesLocal, winningDiagonalsDown[2][2][0]) + myPow(piecesLocal, winningDiagonalsDown[3][1][0]);
	combinedDiagonalsDownR[6][2] = myPow(piecesLocal, winningDiagonalsDown[3][2][0]);
	combinedDiagonalsDownR[0][3] = myPow(piecesLocal, winningDiagonalsDown[0][0][0]);
	combinedDiagonalsDownR[1][3] = myPow(piecesLocal, winningDiagonalsDown[0][1][0]) + myPow(piecesLocal, winningDiagonalsDown[1][0][0]);
	combinedDiagonalsDownR[2][3] = myPow(piecesLocal, winningDiagonalsDown[0][2][0]) + myPow(piecesLocal, winningDiagonalsDown[1][1][0]) + myPow(piecesLocal, winningDiagonalsDown[2][0][0]);
	combinedDiagonalsDownR[3][3] = myPow(piecesLocal, winningDiagonalsDown[1][2][0]) + myPow(piecesLocal, winningDiagonalsDown[2][1][0]) + myPow(piecesLocal, winningDiagonalsDown[3][0][0]);
	combinedDiagonalsDownR[4][3] = myPow(piecesLocal, winningDiagonalsDown[2][2][0]) + myPow(piecesLocal, winningDiagonalsDown[3][1][0]);
	combinedDiagonalsDownR[5][3] = myPow(piecesLocal, winningDiagonalsDown[3][2][0]);
	combinedDiagonalsDownR[6][3] = 0;
	combinedDiagonalsDownR[0][4] = myPow(piecesLocal, winningDiagonalsDown[0][1][0]);
	combinedDiagonalsDownR[1][4] = myPow(piecesLocal, winningDiagonalsDown[0][2][0]) + myPow(piecesLocal, winningDiagonalsDown[1][1][0]);
	combinedDiagonalsDownR[2][4] = myPow(piecesLocal, winningDiagonalsDown[1][2][0]) + myPow(piecesLocal, winningDiagonalsDown[2][1][0]);
	combinedDiagonalsDownR[3][4] = myPow(piecesLocal, winningDiagonalsDown[3][2][0]) + myPow(piecesLocal, winningDiagonalsDown[3][1][0]);
	combinedDiagonalsDownR[4][4] = myPow(piecesLocal, winningDiagonalsDown[3][2][0]);
	combinedDiagonalsDownR[5][4] = 0;
	combinedDiagonalsDownR[6][4] = 0;
	combinedDiagonalsDownR[0][5] = myPow(piecesLocal, winningDiagonalsDown[0][2][0]);
	combinedDiagonalsDownR[1][5] = myPow(piecesLocal, winningDiagonalsDown[1][2][0]);
	combinedDiagonalsDownR[2][5] = myPow(piecesLocal, winningDiagonalsDown[2][2][0]);
	combinedDiagonalsDownR[3][5] = myPow(piecesLocal, winningDiagonalsDown[3][2][0]);
	combinedDiagonalsDownR[4][5] = 0;
	combinedDiagonalsDownR[5][5] = 0;
	combinedDiagonalsDownR[6][5] = 0;

	combinedDiagonalsDownY[0][0] = 0;
	combinedDiagonalsDownY[1][0] = 0;
	combinedDiagonalsDownY[2][0] = 0;
	combinedDiagonalsDownY[3][0] = myPow(piecesLocal, winningDiagonalsDown[0][0][1]);
	combinedDiagonalsDownY[4][0] = myPow(piecesLocal, winningDiagonalsDown[1][0][1]);
	combinedDiagonalsDownY[5][0] = myPow(piecesLocal, winningDiagonalsDown[2][0][1]);
	combinedDiagonalsDownY[6][0] = myPow(piecesLocal, winningDiagonalsDown[3][0][1]);
	combinedDiagonalsDownY[0][1] = 0;
	combinedDiagonalsDownY[1][1] = 0;
	combinedDiagonalsDownY[2][1] = myPow(piecesLocal, winningDiagonalsDown[0][0][1]);
	combinedDiagonalsDownY[3][1] = myPow(piecesLocal, winningDiagonalsDown[0][1][1]) + myPow(piecesLocal, winningDiagonalsDown[1][0][1]);
	combinedDiagonalsDownY[4][1] = myPow(piecesLocal, winningDiagonalsDown[1][1][1]) + myPow(piecesLocal, winningDiagonalsDown[2][0][1]);
	combinedDiagonalsDownY[5][1] = myPow(piecesLocal, winningDiagonalsDown[2][1][1]) + myPow(piecesLocal, winningDiagonalsDown[3][0][1]);
	combinedDiagonalsDownY[6][1] = myPow(piecesLocal, winningDiagonalsDown[3][1][1]);
	combinedDiagonalsDownY[0][2] = 0;
	combinedDiagonalsDownY[1][2] = myPow(piecesLocal, winningDiagonalsDown[0][0][1]);
	combinedDiagonalsDownY[2][2] = myPow(piecesLocal, winningDiagonalsDown[0][1][1]) + myPow(piecesLocal, winningDiagonalsDown[1][0][1]);
	combinedDiagonalsDownY[3][2] = myPow(piecesLocal, winningDiagonalsDown[0][2][1]) + myPow(piecesLocal, winningDiagonalsDown[1][1][1]) + myPow(piecesLocal, winningDiagonalsDown[2][0][1]);
	combinedDiagonalsDownY[4][2] = myPow(piecesLocal, winningDiagonalsDown[1][2][1]) + myPow(piecesLocal, winningDiagonalsDown[2][1][1]) + myPow(piecesLocal, winningDiagonalsDown[3][0][1]);
	combinedDiagonalsDownY[5][2] = myPow(piecesLocal, winningDiagonalsDown[2][2][1]) + myPow(piecesLocal, winningDiagonalsDown[3][1][1]);
	combinedDiagonalsDownY[6][2] = myPow(piecesLocal, winningDiagonalsDown[3][2][1]);
	combinedDiagonalsDownY[0][3] = myPow(piecesLocal, winningDiagonalsDown[0][0][1]);
	combinedDiagonalsDownY[1][3] = myPow(piecesLocal, winningDiagonalsDown[0][1][1]) + myPow(piecesLocal, winningDiagonalsDown[1][0][1]);
	combinedDiagonalsDownY[2][3] = myPow(piecesLocal, winningDiagonalsDown[0][2][1]) + myPow(piecesLocal, winningDiagonalsDown[1][1][1]) + myPow(piecesLocal, winningDiagonalsDown[2][0][1]);
	combinedDiagonalsDownY[3][3] = myPow(piecesLocal, winningDiagonalsDown[1][2][1]) + myPow(piecesLocal, winningDiagonalsDown[2][1][1]) + myPow(piecesLocal, winningDiagonalsDown[3][0][1]);
	combinedDiagonalsDownY[4][3] = myPow(piecesLocal, winningDiagonalsDown[2][2][1]) + myPow(piecesLocal, winningDiagonalsDown[3][1][1]);
	combinedDiagonalsDownY[5][3] = myPow(piecesLocal, winningDiagonalsDown[3][2][1]);
	combinedDiagonalsDownY[6][3] = 0;
	combinedDiagonalsDownY[0][4] = myPow(piecesLocal, winningDiagonalsDown[0][1][1]);
	combinedDiagonalsDownY[1][4] = myPow(piecesLocal, winningDiagonalsDown[0][2][1]) + myPow(piecesLocal, winningDiagonalsDown[1][1][1]);
	combinedDiagonalsDownY[2][4] = myPow(piecesLocal, winningDiagonalsDown[1][2][1]) + myPow(piecesLocal, winningDiagonalsDown[2][1][1]);
	combinedDiagonalsDownY[3][4] = myPow(piecesLocal, winningDiagonalsDown[3][2][1]) + myPow(piecesLocal, winningDiagonalsDown[3][1][1]);
	combinedDiagonalsDownY[4][4] = myPow(piecesLocal, winningDiagonalsDown[3][2][1]);
	combinedDiagonalsDownY[5][4] = 0;
	combinedDiagonalsDownY[6][4] = 0;
	combinedDiagonalsDownY[0][5] = myPow(piecesLocal, winningDiagonalsDown[0][2][1]);
	combinedDiagonalsDownY[1][5] = myPow(piecesLocal, winningDiagonalsDown[1][2][1]);
	combinedDiagonalsDownY[2][5] = myPow(piecesLocal, winningDiagonalsDown[2][2][1]);
	combinedDiagonalsDownY[3][5] = myPow(piecesLocal, winningDiagonalsDown[3][2][1]);
	combinedDiagonalsDownY[4][5] = 0;
	combinedDiagonalsDownY[5][5] = 0;
	combinedDiagonalsDownY[6][5] = 0;

	// Go through the board and combine the different scores for each position. However if the position has already been played, set it to 0. 
	for (int x = 0; x < 7; x++) 
	{
		for (int y = 0; y < 6; y++) 
		{
			if (gameTable[x][y] == ' ') 
			{
				// For each position a random number is selected then added to the corresponding vertical, horizontal and diagonals scores.
				// Each of the scores is multiplied by a weighting, before the addition.
				combinedScoresR[x][y] = (double)(rand() % randomLocal) + (combinedColumnsR[x][y] * verticalLocal) + (combinedRowsR[x][y] * horizontalLocal) + (combinedDiagonalsUpR[x][y] * diagonalLocal) + (combinedDiagonalsDownR[x][y] * diagonalLocal);
				combinedScoresY[x][y] = (double)(rand() % randomLocal) + (combinedColumnsY[x][y] * verticalLocal) + (combinedRowsY[x][y] * horizontalLocal) + (combinedDiagonalsUpY[x][y] * diagonalLocal) + (combinedDiagonalsDownY[x][y] * diagonalLocal);
			}
			else 
			{
				// Clearing the array elements that have already been played isn't strictly necessary, but it makes the code easier to debug.
				combinedScoresR[x][y] = 0;
				combinedScoresY[x][y] = 0;
			}
		}
	}
	return;
}

// function to look at the combined scores for each column and select the highest as the computer move.
// 8 neurons
int selectMove(void) 
{	
	double 	nextMoveLocal = NEXTMOVEDEFAULT;
	double possibles[7]; 						// array of scores for each column to select move.
	double highest = -1.0; 						// variable to select the highest to identify the column.
	int move = 3; 								// The move is set to default to the middle of the table.

	// Go through all columns to calculate the score for each one to select the move.
	for (int x = 0; x < 7; x++) 
	{
		// The possibles array isn't strictly needed, but helps with debugging and development.
		possibles[x] = 0.0;
		// Go through the column to find the next free place.
		for (int y = 0; y < 6; y++) 
		{
			if (gameTable[x][y] == ' ') 
			{
				// Calculate the score for the column by adding the red and yellow scores.
				// It is a good idea to block a position, if it is a good move for the opponent.
				possibles[x] = combinedScoresY[x][y] + (combinedScoresR[x][y] * 0.8);		// A simple change so that the opponents move is less important in the decision.
				// check if column at top before looking at next move.
				if (y < 5) 
				{
					possibles[x] = possibles[x] - (combinedScoresR[x][y + 1] * nextMoveLocal);
				}
				// check each possible value to see if it is the highest, then capture it and the corresponding move. 
				if (possibles[x] > highest) 
				{
					highest = possibles[x];
					move = x;
				}
				y = 6; // exit the y loop, once the column has been processed.
			}
		}
	}
	// Just in case something has gone wrong, the move is checked to see that there is space for it to fit. 
	// If there isn't, the table is searched for any valid move.
	// Coding errors in the neural network meant this happened in the past.
	if (gameTable[move][5] != ' ') 
	{
		for (int x = 0; x < 7; x++) 
		{
			if (gameTable[x][5] == ' ') 
			{
				move = x;
				break;
			}
		}
	}
	return (move);
}

// function to work out the computer move. This calls different functions to calculate the neural network. 
// Then puts the computer move into the next available space in the chosen column.
void calculateMove(void) 
{	// The player selects R for red or Y for Yellow. The weights is 1 default, 2 current or 3 new
	int move = 0;
	doWinningColumns();					// 21 neurons
	doWinningRows();					// 24 neurons
	doWinningDiagonalsUp();				// 12 neurons
	doWinningDiagonalsDown();			// 12 neurons
	doCombinedScores();			// The selection of which weightings to use must be passed to the function.
	move = selectMove(); // The move selected depends on the player viewpoint as well as weightings.
	// put the player move into the game table.
	for (int y = 0; y < 6; y++) 
	{
		if (gameTable[move][y] == ' ') 
		{
			gameTable[move][y] = 'Y';
			// The move is returned to support the command line option.
			break;
		}
	}
	return;
}
