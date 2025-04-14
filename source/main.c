// Connect 4 game ported from PC to run on Wii U.
#include <stdio.h>				// For sprintf.

#include <coreinit/screen.h>	// For OSScreen.
#include <coreinit/thread.h>	// For Sleep.
#include <vpad/input.h>			// For the game pad inputs.
#include <whb/proc.h>			// For the loop and to do home button correctly.
#include <whb/log.h>			// ** Using the console logging features seems to help set up the screen output.
#include <whb/log_console.h>	// ** Found neeeded to keep these in the build for the program to display properly.

#include "Connect4AI.h"			// Header for the game processing.
#include "Draw.h"				// For graphics.
#include "Images/Images.h"		// Headers for the images to be displayed via OSScreen.
#include "Sounds.h"				// For game sound.

// These are global so that they can be accessed by the displayBoard function.
char winner = ' ';				// Winner string D draw Y yellow computer and R red for player. Space is game in progress.
int Ywin = 0;					// Counts of game wins and draws.
int Rwin = 0;
int Draw = 0;

void drawBorder()
{
	// Put a border round the screen to make a neat edge.
	drawLine(XOFFSET, YOFFSET, XOFFSET + XDISPMAX, YOFFSET, 0x01010100);
	drawLine(XOFFSET, YOFFSET - 1, XOFFSET + XDISPMAX, YOFFSET - 1, 0x01010100);

	drawLine(XOFFSET, YOFFSET + YDISPMAX, XOFFSET + XDISPMAX, YOFFSET + YDISPMAX, 0x01010100);
	drawLine(XOFFSET, YOFFSET + YDISPMAX + 1, XOFFSET + XDISPMAX, YOFFSET + YDISPMAX + 1, 0x01010100);

	drawLine(XOFFSET, YOFFSET, XOFFSET, YOFFSET + YDISPMAX, 0x01010100);
	drawLine(XOFFSET - 1, YOFFSET, XOFFSET - 1, YOFFSET + YDISPMAX, 0x01010100);

	drawLine(XOFFSET + XDISPMAX, YOFFSET, XOFFSET + XDISPMAX, YOFFSET + YDISPMAX, 0x01010100);
	drawLine(XOFFSET + XDISPMAX + 1, YOFFSET, XOFFSET + XDISPMAX + 1, YOFFSET + YDISPMAX, 0x01010100);

	// Add a separator between the headings and the game board.
	drawLine(XOFFSET, YOFFSET + (SQY * 2) - 1, XOFFSET + XDISPMAX, YOFFSET + (SQY * 2) - 1, 0x01010100);
	drawLine(XOFFSET, YOFFSET + (SQY * 2) - 2, XOFFSET + XDISPMAX, YOFFSET + (SQY * 2) - 2, 0x01010100);


}

void displayBoard()     // Function to display the board.
{
	// Strings to assembe detials about games won, as display does not work like printf.
	char sred[100]		= "\0";
	char syellow[100]	= "\0";
	char sdraw[100]		= "\0";

	// Assemble display strings as API does not work like printf.
	sprintf(sred,     "Red:    %i ", Rwin);
	sprintf(syellow,  "Yellow: %i ", Ywin);
	sprintf(sdraw,    "Draw:   %i ", Draw);
	
	// Clear the screens to have a grey background.
	OSScreenClearBufferEx(SCREEN_TV,  0x80808000u);
	OSScreenClearBufferEx(SCREEN_DRC, 0x80808000u);

	// If the game is over show the result above the board.
	if (winner == 'Y') { drawText("Yellow Win\0", 0xFEFE0000, 4, XOFFSET + 8, YOFFSET + 24, SCREEN_TV); }
	if (winner == 'R') { drawText("Red Win\0",    0xFE000000, 4, XOFFSET + 8, YOFFSET + 24, SCREEN_TV); }
	if (winner == 'D') { drawText("Draw\0",       0xFEFEFE00, 4, XOFFSET + 8, YOFFSET + 24, SCREEN_TV); }

	// Put the control names above the columns allowing space for the result above.
	drawText("ZL\0", 0xFEFEFE00, 4, XOFFSET + 8,              YOFFSET + SQY + 24, SCREEN_TV);
	drawText("ZR\0", 0xFEFEFE00, 4, XOFFSET + (1 * SQX) + 8,  YOFFSET + SQY + 24, SCREEN_TV);
	drawText("L\0",  0xFEFEFE00, 4, XOFFSET + (2 * SQX) + 24, YOFFSET + SQY + 24, SCREEN_TV);
	drawText("R\0",  0xFEFEFE00, 4, XOFFSET + (3 * SQX) + 24, YOFFSET + SQY + 24, SCREEN_TV);
	drawText("X\0",  0xFEFEFE00, 4, XOFFSET + (4 * SQX) + 24, YOFFSET + SQY + 24, SCREEN_TV);
	drawText("Y\0",  0xFEFEFE00, 4, XOFFSET + (5 * SQX) + 24, YOFFSET + SQY + 24, SCREEN_TV);
	drawText("B\0",  0xFEFEFE00, 4, XOFFSET + (6 * SQX) + 24, YOFFSET + SQY + 24, SCREEN_TV);

	// Draw the graphics for the game table.
	for (int y = 0; y <= 5; y++)    
	{
		for (int x = 0; x <= 6; x++)
		{
			// The coordinates are now the centre of the icon, need to adjust position for this and leave space for the column header.
			// Allow the top row to show who won.
			// Allow the next row to show the controls to use.
			if (getGameTable(x, 5-y) == ' ') { drawImage(SQX, SQY, BlankImage,	(x * SQX) + (SQX / 2), (y * SQY) + (SQY * 5 / 2)); }
			if (getGameTable(x, 5-y) == 'R') { drawImage(SQX, SQY, RedImage,	(x * SQX) + (SQX / 2), (y * SQY) + (SQY * 5 / 2)); }
			if (getGameTable(x, 5-y) == 'Y') { drawImage(SQX, SQY, YellowImage,	(x * SQX) + (SQX / 2), (y * SQY) + (SQY * 5 / 2)); }
		}
	}

	drawBorder();


	// Put the text elements on the gamepad screen, showing which controller buttons to use and current scores.
	drawText("Scores\0",  0xFEFEFE00, 3, 10,  60, SCREEN_DRC);
	drawText(sred,		  0xFE000000, 3, 10, 100, SCREEN_DRC);
	drawText(syellow,	  0xFEFE0000, 3, 10, 140, SCREEN_DRC);
	drawText(sdraw,		  0xFEFEFE00, 3, 10, 180, SCREEN_DRC);
	drawText("To play press the button for the column.\0", 0xFEFEFE00, 2, 10, 250, SCREEN_DRC);
	drawText("You play red, the Wii U plays yellow.\0",    0xFEFEFE00, 2, 10, 300, SCREEN_DRC);

	// Flip the screen buffer to show the new display.
    OSScreenFlipBuffersEx(SCREEN_TV);
	OSScreenFlipBuffersEx(SCREEN_DRC);
	return;
}

int main(int argc, char **argv) {
	VPADStatus status;			// Status returned for the gamepad button.
	int move = 0;				// The player move to pass into the AI.
	int plays = 0;				// Count to alternate who starts.
	bool retMove = false;		// Return value to show if player move is valid.

	// This is the main process and must be in the program at the start for the home button to operate correctly.
    WHBProcInit();
    WHBLogConsoleInit();	// Console Init seem to get the display to operate correctly so keep in the build.

	setupSound();

	// Set up the game board and display it.
	clearGameTable();
	displayBoard();

	// There must be a main loop on WHBProc running, for the program to correctly operate with the home button.
	// Home pauses this loop and continues it if resume is selected. There must therefore be one main loop of processing in the main program.
    while (WHBProcIsRunning()) {
		// Get the VPAD button last pressed.
        VPADRead(VPAD_CHAN_0, &status, 1, NULL);

		// Select the move value depending on the control.
		move=0;
		if (status.trigger & VPAD_BUTTON_ZL) { move=1; }
		if (status.trigger & VPAD_BUTTON_ZR) { move=2; }
		if (status.trigger & VPAD_BUTTON_L)  { move=3; }
		if (status.trigger & VPAD_BUTTON_R)  { move=4; }
		if (status.trigger & VPAD_BUTTON_X)  { move=5; }
		if (status.trigger & VPAD_BUTTON_Y)  { move=6; }
		if (status.trigger & VPAD_BUTTON_B)  { move=7; }

		// If a move has been selected, then attempt the move.
		if (move != 0)
		{
			retMove = putMove(move);
			if (retMove == true)
			{
				putsoundSel(MOVE);
				displayBoard();	// Update the display with the end of the game.
				OSSleepTicks(OSMillisecondsToTicks(500));		// Allow some time to see and hear the move.
				// If the move was a valid move check if the game has ended.
				winner = gameEnded();
				if (winner == ' ')
				{
					// If the game has not ended do the computer move and check for a winner again.
					calculateMove();
					putsoundSel(MOVE);
					displayBoard();	// Update the display with the end of the game.
					OSSleepTicks(OSMillisecondsToTicks(500));		// Allow some time to see and hear the move.
					winner = gameEnded();
				}
			}

			// If there has been a winner update the scores, show the board, clear ready for a new game.
			if (winner != ' ')
			{
				if (winner == 'Y') { Ywin++; putsoundSel(LOSE); }
				if (winner == 'R') { Rwin++; putsoundSel(WIN); }
				if (winner == 'D') { Draw++; putsoundSel(DRAW); }
				displayBoard();	// Update the display with the end of the game.
				clearGameTable();
				plays++;
				if (plays%2 == 1) { calculateMove(); }		// On alternate goes, let the computer go first.
				OSSleepTicks(OSMillisecondsToTicks(3000));	// Delay so the player can see the last game.
			}
		}
		displayBoard();	// Update the display.
		OSSleepTicks(OSMillisecondsToTicks(50));		// Allow some time back for the OS (TV update is 60Hz (16.67ms), 50ms is 3 cycles).
    }

	QuitSound();

	// If we get out of the program clean up and exit.
    WHBLogConsoleFree();
    WHBProcShutdown();
    return 0;
}