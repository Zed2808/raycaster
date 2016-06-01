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

#define fullscreen 0
#define screenWidth 854
#define screenHeight 480
#define texWidth 128
#define texHeight 128
#define mapWidth 24
#define mapHeight 24

#define texCount 4    // number of textures to load
#define weaponCount 3 // number of weapon sprites
#define numSprites 3  // number of sprite instances in the map

struct Sprite {
	double x;
	double y;
	int texture;
};

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

Sprite sprite[numSprites] = {
	// some Doom imps
	{2.5,  18.5,  3},
	{2.5,  21.5,  3},
	{21.5, 15.5,  3}
};

Uint32 buffer[screenHeight][screenWidth]; // y-coordinate first because it works per scanline

// weapon vectors
std::vector<Uint32> weaponLoader[weaponCount];
std::vector<Uint32> weapon[weaponCount];

// 1D z-buffer
double ZBuffer[screenWidth];

// arrays used to sort the sprites
int spriteOrder[numSprites];
double spriteDistance[numSprites];

// function used to sort the sprites
void combSort(int* order, double* dist, int amount) {
	int gap = amount;
	bool swapped = false;
	while(gap > 1 || swapped) {
		// shrink factor 1.3
		gap = (gap * 10) / 13;
		if(gap == 9 || gap == 10) gap = 11;
		if(gap < 1) gap = 1;

		swapped = false;

		for(int i = 0; i < amount - gap; i++) {
			int j = i + gap;
			if(dist[i] < dist[j]) {
				std::swap(dist[i], dist[j]);
				std::swap(order[i], order[j]);
				swapped = true;
			}
		}
	}
}

void drawWeapon(int id) {
	int drawStartY = h - 256;
	int drawEndY = drawStartY + 256;
	int drawStartX = (w / 2) - 128;
	int drawEndX = drawStartX + 256;
	for(int y = drawStartY; y < drawEndY; y++) {
		for(int x = drawStartX; x < drawEndX; x++) {
			Uint32 color = weapon[id][256 * (y - drawStartY) + (x - drawStartX)];
			if(INTtoRGB(color) != ColorRGB(0, 255, 0)) {
				buffer[y][x] = color;
			}
		}
	}
}

int main() {
	double posX = 22, posY = 22;   // x and y start position
	double dirX = -1, dirY = 0;    // initial direction vector
	double planeX = 0, planeY = 1; // the 2d raycaster version of camera plane

	double time = 0;    // time of current frame
	double oldTime = 0; // time of previous frame

	// weapon animation
	bool animateWeapon = false;
	double lastWeaponFrameTime = 0;
	int weaponFrame = 0;

	// texture vector
	std::vector<Uint32> texture[texCount];
	for(int i = 0; i < texCount; i++) {
		texture[i].resize(texWidth * texHeight);
	}

	// load textures & sprites
	unsigned long tw, th;
	loadImage(texture[0], tw, th, "data/textures/bricks.png");
	loadImage(texture[1], tw, th, "data/textures/tiles.png");
	loadImage(texture[2], tw, th, "data/textures/ceiling.png");
	loadImage(texture[3], tw, th, "data/sprites/imp_standing.png");

	// load weapons
	for(int i = 0; i < weaponCount; i++) {
		weaponLoader[i].resize(128 * 128);
	}

	loadImage(weaponLoader[0], tw, th, "data/weapons/shotgun.png");
	loadImage(weaponLoader[1], tw, th, "data/weapons/shotgun1.png");
	loadImage(weaponLoader[2], tw, th, "data/weapons/shotgun2.png");

	// scale & store weapon image
	for(int i = 0; i < weaponCount; i++) {
		weapon[i].resize(256 * 256);
		for(int y = 0; y < 128; y++) {
			for(int x = 0; x < 128; x++) {
				weapon[i][512 * y + 2 * x] = weaponLoader[i][128 * y + x];
				weapon[i][512 * y + 2 * x + 1] = weaponLoader[i][128 * y + x];
				weapon[i][512 * y + 2 * x + 256] = weaponLoader[i][128 * y + x];
				weapon[i][512 * y + 2 * x + 257] = weaponLoader[i][128 * y + x];
			}
		}
	}

	// swap x/y since they're used as vertical stripes
	for(size_t i = 0; i < texCount; i++) {
		for(size_t x = 0; x < texHeight; x++) {
			for(size_t y = 0; y < x; y++) {
				std::swap(texture[i][texHeight * y + x], texture[i][texHeight * x + y]);
			}
		}
	}

	screen(screenWidth, screenHeight, fullscreen, "Raycaster");

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

			// draw walls
			for(int y = wallStart; y < wallEnd; y++) {
				int d = y * 256 - h * 128 + lineHeight * 128; // 256 and 128 factors to avoid floats
				int texY = ((d * texHeight) / lineHeight) / 256;
				Uint32 color = texture[texNum][texHeight * texX + texY];
				// make color darker for y-sides:
				// R, G, and B byte each divided by 2 with a shift and binary and
				if(side == 1) color = (color >> 1) & 8355711;
				buffer[y][x] = color;
			}

			// set z-buffer for sprite casting
			ZBuffer[x] = perpWallDist;

			// floor casting
			double floorXWall, floorYWall; // x and y positions of the floor texture pixel at the bottom of the wall

			// 4 different wall directions possible
			if(side == 0 && rayDirX > 0) {
				floorXWall = mapX;
				floorYWall = mapY + wallX;
			} else if(side == 0 && rayDirX < 0) {
				floorXWall = mapX + 1.0;
				floorYWall = mapY + wallX;
			} else if(side == 1 && rayDirY > 0) {
				floorXWall = mapX + wallX;
				floorYWall = mapY;
			} else {
				floorXWall = mapX + wallX;
				floorYWall = mapY + 1.0;
			}

			double distWall, distPlayer, currentDist;

			distWall = perpWallDist;
			distPlayer = 0.0;

			if(wallEnd < 0) wallEnd = h; // becomes 0 when the integer overflows

			// draw the floor from wallEnd to the bottom of the screen
			for(int y = wallEnd + 1; y < h; y++) {
				currentDist = h / (2.0 * y - h);

				double weight = (currentDist - distPlayer) / (distWall - distPlayer);

				double currentFloorX = weight * floorXWall + (1.0 - weight) * posX;
				double currentFloorY = weight * floorYWall + (1.0 - weight) * posY;

				int floorTexX, floorTexY;
				floorTexX = int(currentFloorX * texWidth) % texWidth;
				floorTexY = int(currentFloorY * texHeight) % texHeight;

				// floor
				buffer[y][x] = (texture[1][texWidth * floorTexY + floorTexX]);
				// ceiling (symmetrical)
				buffer[h - y][x] = texture[2][texWidth * floorTexY + floorTexX];
			}
		}

		// sprite casting
		// sort sprites from furthest to closest
		for(int i = 0; i < numSprites; i++) {
			spriteOrder[i] = i;
			spriteDistance[i] = ((posX - sprite[i].x) * (posX - sprite[i].x) + (posY - sprite[i].y) * (posY - sprite[i].y));
		}

		combSort(spriteOrder, spriteDistance, numSprites);

		// after sorting the sprites, do the projection and draw them
		for(int i = 0; i < numSprites; i++) {
			// translate sprite position to be relative to camera
			double spriteX = sprite[spriteOrder[i]].x - posX;
			double spriteY = sprite[spriteOrder[i]].y - posY;

			// transform sprite with the inverse camera matrix:
			// [ planeX  dirX ] - 1                                            [ dirY     -dirX ]
			// [              ]        = 1 / (planeX * dirY - dirX * planeY) * [                ]
			// [ planeY  dirY ]                                                [ -planeY  planeX]

			double invDet = 1.0 / (planeX * dirY - dirX * planeY); // required for correct matrix multiplication

			double transformX = invDet * (dirY * spriteX - dirX * spriteY);
			double transformY = invDet * (-planeY * spriteX + planeX * spriteY); // depth inside the screen

			int spriteScreenX = int((w / 2) * (1 + transformX / transformY));

			// calculate the height of the sprite on screen
			int spriteHeight = abs(int(h / transformY)); // using transformY instead of the real distance prevents fisheye

			// calculate lowest and highest pixel to fill in current stripe
			int drawStartY = -spriteHeight / 2 + h / 2;
			if(drawStartY < 0) drawStartY = 0;

			int drawEndY = spriteHeight / 2 + h / 2;
			if(drawEndY >= h) drawEndY = h - 1;

			// calculate the width of the sprite
			int spriteWidth = abs(int(h / transformY));

			int drawStartX = -spriteWidth / 2 + spriteScreenX;
			if(drawStartX < 0) drawStartX = 0;

			int drawEndX = spriteWidth / 2 + spriteScreenX;
			if(drawEndX >= w) drawEndX = w - 1;

			// loop through every vertical stripe of the sprite on screen
			for(int stripe = drawStartX; stripe < drawEndX; stripe++) {
				int texX = int(256 * (stripe - (-spriteWidth / 2 + spriteScreenX)) * texWidth / spriteWidth) / 256;

				// the conditions in the if are:
				// 1) it's in front of the camera plane, so you don't see things behind you
				// 2) it's on the screen (left)
				// 3) it's on the screen (right)
				// 4) ZBuffer, with perpendicular distance
				if(transformY > 0 && stripe > 0 && stripe < w && transformY < ZBuffer[stripe]) {
					for(int y = drawStartY; y < drawEndY; y++) {
						int d = y * 256 - h * 128 + spriteHeight * 128; // 256 and 128 factors to avoid floats
						int texY = ((d * texHeight) / spriteHeight) / 256;
						Uint32 color = texture[sprite[spriteOrder[i]].texture][texWidth * texX + texY]; // get current color from the texture

						// color pixel if it isn't the invisible color (black)
						if((color & 0x00FFFFFF) != 0) {
							buffer[y][stripe] = color;
						}
					}
				}
			}
		}

		// timing for input and FPS counter
		oldTime = time;
		time = getTicks();
		double frameTime = (time - oldTime) / 1000.0; // time this frame has taken (seconds)

		// speed modifiers
		double moveSpeedMultiplier = 5.0; // squares/second
		double rotSpeedMultiplier = 3.0;  // radians/second
		double moveSpeed = frameTime * moveSpeedMultiplier;
		double rotSpeed = frameTime * rotSpeedMultiplier;

		readKeys();

		double deltaPosX = 0;
		double deltaPosY = 0;

		// move forward if no wall in front of you
		if (keyDown(SDLK_w)) {
			if(worldMap[int(posX + dirX * moveSpeed)][int(posY)] == false) deltaPosX += dirX;
			if(worldMap[int(posX)][int(posY + dirY * moveSpeed)] == false) deltaPosY += dirY;
		}
		
		// move backwards if no wall behind you
		if (keyDown(SDLK_s)) {
			if(worldMap[int(posX - dirX * moveSpeed)][int(posY)] == false) deltaPosX -= dirX;
			if(worldMap[int(posX)][int(posY - dirY * moveSpeed)] == false) deltaPosY -= dirY;
		}

		// move left if no wall to the left
		if (keyDown(SDLK_a)) {
			if(worldMap[int(posX - planeX * moveSpeed)][int(posY)] == false) deltaPosX -= planeX;
			if(worldMap[int(posX)][int(posY - planeY * moveSpeed)] == false) deltaPosY -= planeY;
		}
		
		// strafe right if no wall to the right
		if (keyDown(SDLK_d)) {
			if(worldMap[int(posX + planeX * moveSpeed)][int(posY)] == false) deltaPosX += planeX;
			if(worldMap[int(posX)][int(posY + planeY * moveSpeed)] == false) deltaPosY += planeY;
		}

		// pythagorean theorem to get actual vector
		double diagonal = sqrt(pow(deltaPosX, 2) + pow(deltaPosY, 2));

		// normalize vector to not go above 1.0
		if(diagonal > 1) {
			deltaPosX /= diagonal;
			deltaPosY /= diagonal;
		}

		// change x & y position by position delta modified by move speed
		posX += deltaPosX * moveSpeed;
		posY += deltaPosY * moveSpeed;

		// rotate to the right
		if (keyDown(SDLK_PERIOD)) {
			// both camera direction and camera plane must be rotated
			double oldDirX = dirX;
			dirX = dirX * cos(-rotSpeed) - dirY * sin(-rotSpeed);
			dirY = oldDirX * sin(-rotSpeed) + dirY * cos(-rotSpeed);
			double oldPlaneX = planeX;
			planeX = planeX * cos(-rotSpeed) - planeY * sin(-rotSpeed);
			planeY = oldPlaneX * sin(-rotSpeed) + planeY * cos(-rotSpeed);
		}
		
		// rotate to the left
		if (keyDown(SDLK_COMMA)) {
			// both camera direction and camera plane must be rotated
			double oldDirX = dirX;
			dirX = dirX * cos(rotSpeed) - dirY * sin(rotSpeed);
			dirY = oldDirX * sin(rotSpeed) + dirY * cos(rotSpeed);
			double oldPlaneX = planeX;
			planeX = planeX * cos(rotSpeed) - planeY * sin(rotSpeed);
			planeY = oldPlaneX * sin(rotSpeed) + planeY * cos(rotSpeed);
		}

		if (keyDown(SDLK_SPACE)) {
			animateWeapon = true;
		}

		if(animateWeapon) {
			if(weaponFrame == 0) {
				weaponFrame++;
				lastWeaponFrameTime = time;
			} else if(weaponFrame < 2 && (time - lastWeaponFrameTime) >= 100) {
				weaponFrame++;
				lastWeaponFrameTime = time;
			} else if(time - lastWeaponFrameTime >= 100){
				weaponFrame = 0;
				animateWeapon = false;
			}
		}

		drawWeapon(weaponFrame);

		drawBuffer(buffer[0]);

		// clear the buffer
		for(int x = 0; x < w; x++) {
			for(int y = 0; y < h; y++) {
				buffer[y][x] = 0;
			}
		}

		// FPS counter
		printString(std::to_string(int(1.0/frameTime)) + " FPS", 0, 0, RGB_White, true, RGB_Black);

		redraw();
	}
}
