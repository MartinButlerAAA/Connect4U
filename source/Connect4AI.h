// Header file for functions that can be accessed outside of the C file.
#pragma once

void clearGameTable(void);		// Clears the game table must called before each game to clear out any old pieces.

bool putMove(int);				// Put the player's move (1 to 7) into the game board, returns true if it worked or false if the move is not valid.

char gameEnded(void);			// Returns a char for who has won Y-Yellow(Computer), R-Red(Player), D for Draw or Space if the game has not ended.

void calculateMove(void);		// Call to calculate the computer move which is added to the game table.

char getGameTable(int, int);	// Standard Connect 4 game board with 7 rows and 6 columns [7][6]. API to get items for display.
								// Note as per C rows and columns start from 0 and the columns count from the bottom.
								// [2][0] is the bottom of column 3 on the board.


