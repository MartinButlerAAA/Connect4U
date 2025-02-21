// Header for drawing functions.
#pragma once

// The background colour is defined. Pixels in images that match the background colour are not processed, in effect giving sprites a transparent background.
#define BKGNDCLR 0x80808000

#define ZMAX 36		// Number of images to support rotation.

#define SQX  80		// Sizes for the image arrays used in the graphics display.
#define SQY  80

#define XDISPMAX (SQX * 7) // Limits to game screen size.
#define YDISPMAX (SQY * 8)

#define XDISPCTR (XDISPMAX / 2) // Centre of the screen.
#define YDISPCTR (YDISPMAX / 2)

#define XOFFSET  ((1240 - XDISPMAX) / 2)	// Offset to position game screen on the TV.
#define YOFFSET  (( 720 - YDISPMAX) / 2)

// Initialise 3D array for rotating image.
bool initialiseRotatingImage(unsigned int xmax, unsigned int ymax, unsigned int ImageP[ymax][xmax], unsigned int ImagesP[ZMAX][ymax][xmax]);

// Draw a rotated image.
bool rotateImage(unsigned int xmax, unsigned int ymax, unsigned int ImageP[ZMAX][ymax][xmax], unsigned int xpos, unsigned int ypos, unsigned int angle);

// Draw the visible part of the background based on xpos and ypos.
bool drawBackground(unsigned int xmax, unsigned int ymax, unsigned int ImageP[ymax][xmax], unsigned int xpos, unsigned int ypos);

// Draw an image on the screen at xpos and ypos.
bool drawImage(unsigned int xmax, unsigned int ymax, unsigned int ImageP[ymax][xmax], unsigned int xpos, unsigned int ypos);

// Draw a scaled image on the screen at xpos and ypos. Scaled between 0% and 100%.
bool scaleImage(unsigned int xmax, unsigned int ymax, unsigned int ImageP[ymax][xmax], unsigned int xpos, unsigned int ypos, float pct);

// Draw a line of the colour specified.
bool drawLine(float x1, float y1, float x2, float y2, unsigned int colour);
