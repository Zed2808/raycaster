/*
Copyright (c) 2004-2007, Lode Vandevenne

All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <cmath>
#include <string>
#include <vector>
#include <iostream>

#include "quickcg.h"
using namespace QuickCG;

#define mapWidth 24
#define mapHeight 24

static const ColorRGB RGB_Ground (0, 100, 0);
static const ColorRGB RGB_Sky (150, 200, 255);

int worldMap[mapWidth][mapHeight] = {
	{8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8},
	{8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,0,0,0,0,0,0,8},
	{8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,0,0,0,0,0,0,8},
	{8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,0,0,0,0,0,0,8},
	{8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,0,0,0,0,0,0,8},
	{8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,0,0,0,0,0,0,8},
	{8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,0,0,0,0,0,0,8},
	{8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,0,0,0,0,0,0,8},
	{8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,0,0,0,0,0,0,8},
	{8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,0,8,8,8,8,0,8},
	{8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,0,8},
	{8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,0,8},
	{8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,0,8},
	{8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,0,8},
	{8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,0,8},
	{8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,0,8},
	{8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,0,8},
	{8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,0,8},
	{8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,0,8},
	{8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,8,8,0,8},
	{8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,0,0,0,8},
	{8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8},
	{8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,0,0,0,8},
	{8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8}
};

int main() {
	double posX = 22, posY = 22;      // x and y start position
	double dirX = -1, dirY = 0;       // initial direction vector
	double planeX = 0, planeY = 0.66; // the 2d raycaster version of camera plane

	double time = 0;    // time of current frame
	double oldTime = 0; // time of previous frame

	screen(1024, 768, 0, "Raycaster");

	while(!done()) {
		// for each pixel in the width of the screen
		for(int x = 0; x < w; x++) {
			// calculate ray position and direction 
			double cameraX = 2 * x / double(w) - 1; // x-coordinate in camera space
			double rayPosX = posX;
			double rayPosY = posY;
			double rayDirX = dirX + planeX * cameraX;
			double rayDirY = dirY + planeY * cameraX;

			// which box of the map we're in  
			int mapX = int(rayPosX);
			int mapY = int(rayPosY);

			// length of ray from current position to next x or y-side
			double sideDistX;
			double sideDistY;

			// length of ray from one x or y-side to next x or y-side
			double deltaDistX = sqrt(1 + (rayDirY * rayDirY) / (rayDirX * rayDirX));
			double deltaDistY = sqrt(1 + (rayDirX * rayDirX) / (rayDirY * rayDirY));
			double perpWallDist;

			// what direction to step in x or y-direction (either +1 or -1)
			int stepX;
			int stepY;

			int hit = 0; // was there a wall hit?
			int side;    // was it a NS or a EW wall hit?

			// calculate step and initial sideDist
			if (rayDirX < 0) {
				stepX = -1;
				sideDistX = (rayPosX - mapX) * deltaDistX;
			} else {
				stepX = 1;
				sideDistX = (mapX + 1.0 - rayPosX) * deltaDistX;
			}

			if (rayDirY < 0) {
				stepY = -1;
				sideDistY = (rayPosY - mapY) * deltaDistY;
			} else {
				stepY = 1;
				sideDistY = (mapY + 1.0 - rayPosY) * deltaDistY;
			}

			// perform DDA
			while (hit == 0) {
				// jump to next map square, OR in x-direction, OR in y-direction
				if (sideDistX < sideDistY) {
					sideDistX += deltaDistX;
					mapX += stepX;
					side = 0;
				} else {
					sideDistY += deltaDistY;
					mapY += stepY;
					side = 1;
				}

				// check if ray has hit a wall
				if (worldMap[mapX][mapY] > 0) {
					hit = 1;
				}
			} 

			// calculate distance projected on camera direction (oblique distance will give fisheye effect!)
			if (side == 0) {
				perpWallDist = fabs((mapX - rayPosX + (1 - stepX) / 2) / rayDirX);
			} else {
				perpWallDist = fabs((mapY - rayPosY + (1 - stepY) / 2) / rayDirY);
			}

			// calculate height of line to draw on screen
			int lineHeight = abs(int(h / perpWallDist));

			// calculate lowest and highest pixel to fill in current stripe
			int wallStart = -lineHeight / 2 + h / 2;
			if(wallStart < 0) {
				wallStart = 0;
			}

			int wallEnd = lineHeight / 2 + h / 2;
			if(wallEnd >= h) {
				wallEnd = h - 1;
			}

			// choose wall color
			ColorRGB wallColor;
			switch(worldMap[mapX][mapY]) {
				case 1:  wallColor = RGB_Black;     break;
				case 2:  wallColor = RGB_Maroon;    break;
				case 3:  wallColor = RGB_Darkgreen; break;
				case 4:  wallColor = RGB_Olive;     break;
				case 5:  wallColor = RGB_Navy;      break;
				case 6:  wallColor = RGB_Purple;    break;
				case 7:  wallColor = RGB_Teal;      break;
				case 8:  wallColor = RGB_Gray;      break;
				case 9:  wallColor = RGB_Grey;      break;
				case 10: wallColor = RGB_Red;       break;
				case 11: wallColor = RGB_Green;     break;
				case 12: wallColor = RGB_Yellow;    break;
				case 13: wallColor = RGB_Blue;      break;
				case 14: wallColor = RGB_Magenta;   break;
				case 15: wallColor = RGB_Cyan;      break;
				case 16: wallColor = RGB_White;     break;
				default: wallColor = RGB_White;     break;
			}

			// give x and y sides different brightness
			if (side == 1) {
				wallColor = 4 * wallColor / 5;
			}

			// draw the pixels of the stripes as a vertical line
			verLine(x, 0, wallStart, RGB_Sky);     // sky
			verLine(x, wallStart, wallEnd, wallColor); // wall
			verLine(x, wallEnd, h-1, RGB_Ground);  // ground
		}

		// timing for input and FPS counter
		oldTime = time;
		time = getTicks();
		double frameTime = (time - oldTime) / 1000.0; // time this frame has taken (seconds)

		printString(std::to_string(int(1.0/frameTime)) + " FPS", 3, 3, RGB_Black, true, RGB_White); // FPS counter

		redraw();
		cls();

		// speed modifiers
		double moveSpeedMultiplier = 5.0; // squares/second
		double rotSpeedMultiplier = 3.0;  // radians/second
		double moveSpeed = frameTime * moveSpeedMultiplier;
		double rotSpeed = frameTime * rotSpeedMultiplier;

		readKeys();

		// move forward if no wall in front of you
		if (keyDown(SDLK_UP)) {
			if(worldMap[int(posX + dirX * moveSpeed)][int(posY)] == false) posX += dirX * moveSpeed;
			if(worldMap[int(posX)][int(posY + dirY * moveSpeed)] == false) posY += dirY * moveSpeed;
		}
		
		// move backwards if no wall behind you
		if (keyDown(SDLK_DOWN)) {
			if(worldMap[int(posX - dirX * moveSpeed)][int(posY)] == false) posX -= dirX * moveSpeed;
			if(worldMap[int(posX)][int(posY - dirY * moveSpeed)] == false) posY -= dirY * moveSpeed;
		}
		
		// rotate to the right
		if (keyDown(SDLK_RIGHT)) {
			// both camera direction and camera plane must be rotated
			double oldDirX = dirX;
			dirX = dirX * cos(-rotSpeed) - dirY * sin(-rotSpeed);
			dirY = oldDirX * sin(-rotSpeed) + dirY * cos(-rotSpeed);
			double oldPlaneX = planeX;
			planeX = planeX * cos(-rotSpeed) - planeY * sin(-rotSpeed);
			planeY = oldPlaneX * sin(-rotSpeed) + planeY * cos(-rotSpeed);
		}
		
		// rotate to the left
		if (keyDown(SDLK_LEFT)) {
			// both camera direction and camera plane must be rotated
			double oldDirX = dirX;
			dirX = dirX * cos(rotSpeed) - dirY * sin(rotSpeed);
			dirY = oldDirX * sin(rotSpeed) + dirY * cos(rotSpeed);
			double oldPlaneX = planeX;
			planeX = planeX * cos(rotSpeed) - planeY * sin(rotSpeed);
			planeY = oldPlaneX * sin(rotSpeed) + planeY * cos(rotSpeed);
		}
	}
}
