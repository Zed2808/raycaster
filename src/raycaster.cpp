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

#define screenWidth 1024
#define screenHeight 768
#define texCount 2
#define texWidth 128
#define texHeight 128
#define mapWidth 24
#define mapHeight 24

static const ColorRGB RGB_Ground (0, 100, 0);
static const ColorRGB RGB_Sky (150, 200, 255);

int worldMap[mapWidth][mapHeight] = {
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,1,1,1,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

int main() {
	double posX = 22, posY = 22;      // x and y start position
	double dirX = -1, dirY = 0;       // initial direction vector
	double planeX = 0, planeY = 0.66; // the 2d raycaster version of camera plane

	double time = 0;    // time of current frame
	double oldTime = 0; // time of previous frame

	Uint32 buffer[screenHeight][screenWidth]; // y-coordinate first because it works per scanline

	std::vector<Uint32> texture[texCount];
	for(int i = 0; i < texCount; i++) {
		texture[i].resize(texWidth * texHeight);
	}

	// load textures
	unsigned long tw, th;
	loadImage(texture[0], tw, th, "data/textures/bricks.png");
	loadImage(texture[1], tw, th, "data/textures/tiles.png");

	// swap x/y since they're used as vertical stripes
	for(size_t i = 0; i < texCount; i++) {
		for(size_t x = 0; x < texHeight; x++) {
			for(size_t y = 0; y < x; y++) {
				std::swap(texture[i][texHeight * y + x], texture[i][texHeight * x + y]);
			}
		}
	}

	screen(screenWidth, screenHeight, 0, "Raycaster");

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
				perpWallDist = (mapX - rayPosX + (1 - stepX) / 2) / rayDirX;
			} else {
				perpWallDist = (mapY - rayPosY + (1 - stepY) / 2) / rayDirY;
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

			// texturing calculations
			int texNum = worldMap[mapX][mapY] - 1; // subtract 1 so texture 0 is used

			// calculate value of wallX: where on wall the ray hit
			double wallX;
			if(side == 0) {
				wallX = rayPosY + perpWallDist * rayDirY;
			} else {
				wallX = rayPosX + perpWallDist * rayDirX;
			}
			wallX -= floor((wallX));

			// x-coordinate on the texture
			int texX = int(wallX * double(texWidth));
			if(side == 0 && rayDirX > 0) texX = texWidth - texX - 1;
			if(side == 1 && rayDirY < 0) texX = texWidth - texX - 1;

			for(int y = wallStart; y < wallEnd; y++) {
				int d = y * 256 - h * 128 + lineHeight * 128; // 256 and 128 factors to avoid floats
				int texY = ((d * texHeight) / lineHeight) / 256;
				Uint32 color = texture[texNum][texHeight * texX + texY];
				// make color darker for y-sides:
				// R, G, and B byte each divided by 2 with a shift and binary and
				if(side == 1) color = (color >> 1) & 8355711;
				buffer[y][x] = color;
			}
		}

		drawBuffer(buffer[0]);

		// clear the buffer
		for(int x = 0; x < w; x++) {
			for(int y = 0; y < h; y++) {
				buffer[y][x] = 0;
			}
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
