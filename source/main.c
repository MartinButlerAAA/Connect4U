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
	drawLine(XOFFSET, YOFFSET + (SQY * 2), XOFFSET + XDISPMAX, YOFFSET + (SQY * 2), 0x01010100);
	drawLine(XOFFSET, YOFFSET + (SQY * 2) - 1, XOFFSET + XDISPMAX, YOFFSET + (SQY * 2) - 1, 0x01010100);


}

void displayBoard()     // Function to display the board.
{
	// Strings to assembe detials about games won, as display does not work like printf.
	char swinner[100] 	= "\0";
	char sred[100]		= "\0";
	char syellow[100]	= "\0";
	char sdraw[100]		= "\0";

	// Assemble display strings as API does not work like printf.
	sprintf(swinner,  "Winner: %c ", winner);
	sprintf(sred,     "Red:    %i ", Rwin);
	sprintf(syellow,  "Yellow: %i ", Ywin);
	sprintf(sdraw,    "Draw:   %i ", Draw);
	
	// Clear the screens to have a grey background.
	OSScreenClearBufferEx(SCREEN_TV,  0x80808000u);
	OSScreenClearBufferEx(SCREEN_DRC, 0x80808000u);

	// If the game is over show the result above the board.
	if (winner == 'Y') { drawImage(300, 80, YellowWinImage, SQX * 3, SQY / 2); }
	if (winner == 'R') { drawImage(240, 80, RedWinImage, SQX * 3, SQY / 2); }
	if (winner == 'D') { drawImage(240, 80, DrawImage, SQX * 3, SQY / 2); }

	// Put the control names above the columns allowing space for the result above.
	drawImage(SQX, SQY, ZLImage,             (SQX / 2), (SQY * 3 / 2));
	drawImage(SQX, SQY, ZRImage, (1 * SQX) + (SQX / 2), (SQY * 3 / 2));
	drawImage(SQX, SQY, LImage,  (2 * SQX) + (SQX / 2), (SQY * 3 / 2));
	drawImage(SQX, SQY, RImage,  (3 * SQX) + (SQX / 2), (SQY * 3 / 2));
	drawImage(SQX, SQY, XImage,  (4 * SQX) + (SQX / 2), (SQY * 3 / 2));
	drawImage(SQX, SQY, YImage,  (5 * SQX) + (SQX / 2), (SQY * 3 / 2));
	drawImage(SQX, SQY, BImage,  (6 * SQX) + (SQX / 2), (SQY * 3 / 2));

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
	OSScreenPutFontEx(SCREEN_DRC,  3,  2, swinner);
	OSScreenPutFontEx(SCREEN_DRC,  3,  4, "Scores:");
	OSScreenPutFontEx(SCREEN_DRC,  3,  5, sred);
	OSScreenPutFontEx(SCREEN_DRC,  3,  6, syellow);
	OSScreenPutFontEx(SCREEN_DRC,  3,  7, sdraw);
	OSScreenPutFontEx(SCREEN_DRC,  3, 10, "To play press the button for the column.");
	OSScreenPutFontEx(SCREEN_DRC,  3, 12, "You play red, the Wii U plays yellow.");

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