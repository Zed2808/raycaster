#ifndef SMALLCG_H
#define SMALLCH_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <string>
#include <vector>

namespace SmallCG {

////////////////////////////////////////////////////////////////////////////////
// COLOR STRUCTS ///////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
struct ColorRGB {
	Uint8 r;
	Uint8 g;
	Uint8 b;

	ColorRGB(Uint8 r, Uint8 g, Uint8 b);
	ColorRGB(const ColorRGB& color);
	ColorRGB();
};

bool operator!=(const ColorRGB& color1, const ColorRGB& color2);

static const ColorRGB RGB_Black    (  0,   0,   0);
static const ColorRGB RGB_Red      (255,   0,   0);
static const ColorRGB RGB_Green    (  0, 255,   0);
static const ColorRGB RGB_Blue     (  0,   0, 255);
static const ColorRGB RGB_Cyan     (  0, 255, 255);
static const ColorRGB RGB_Magenta  (255,   0, 255);
static const ColorRGB RGB_Yellow   (255, 255,   0);
static const ColorRGB RGB_White    (255, 255, 255);
static const ColorRGB RGB_Gray     (128, 128, 128);
static const ColorRGB RGB_Grey     (192, 192, 192);
static const ColorRGB RGB_Maroon   (128,   0,   0);
static const ColorRGB RGB_Darkgreen(  0, 128,   0);
static const ColorRGB RGB_Navy     (  0,   0, 128);
static const ColorRGB RGB_Teal     (  0, 128, 128);
static const ColorRGB RGB_Purple   (128,   0, 128);
static const ColorRGB RGB_Olive    (128, 128,   0);

////////////////////////////////////////////////////////////////////////////////
// GLOBAL VARIABLES ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
extern int w;
extern int h;

////////////////////////////////////////////////////////////////////////////////
// KEYBOARD & MOUSE FUNCTIONS //////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
bool keyDown(int scancode);
bool mouseDown(int button);

////////////////////////////////////////////////////////////////////////////////
// BASIC SCREEN FUNCTIONS //////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void initVideo(int width, int height, bool fullscreen, const std::string& title);
void present();
void pset(Uint32* buffer, int x, int y, const ColorRGB& color);
void drawBuffer(Uint32* buffer);

////////////////////////////////////////////////////////////////////////////////
// NON-GRAPHICAL FUNCTIONS /////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
bool done();
void readKeys();

////////////////////////////////////////////////////////////////////////////////
// COLOR CONVERSIONS ///////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
Uint32 RGBtoINT(ColorRGB colorRGB);
ColorRGB INTtoRGB(Uint32 colorINT);

////////////////////////////////////////////////////////////////////////////////
// FILE FUNCTIONS //////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void loadFile(std::vector<unsigned char>& buffer, const std::string& filename);

////////////////////////////////////////////////////////////////////////////////
// IMAGE FUNCTIONS /////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
int loadImage(std::vector<Uint32>& out, unsigned long& w, unsigned long& h, const std::string& filename);
int decodePNG(std::vector<unsigned char>& out_image, unsigned long& image_width, unsigned long& image_height, const unsigned char* in_png, size_t in_size, bool convert_to_rgba32 = true);
int decodePNG(std::vector<unsigned char>& out_image_32bit, unsigned long& image_width, unsigned long& image_height, const std::vector<unsigned char>& in_png);

////////////////////////////////////////////////////////////////////////////////
// TEXT FUNCTIONS //////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
extern bool font[256][8][8];
void drawLetter(Uint32* buffer, unsigned char n, int x, int y, const ColorRGB& textColor = RGB_White, bool bg = 0, const ColorRGB& bgColor = RGB_Black);
int printString(Uint32* buffer, const std::string& text, int x = 0, int y = 0, const ColorRGB& textColor = RGB_White, bool bg = 0, const ColorRGB& bgColor = RGB_Black, int forceLength = 0);

////////////////////////////////////////////////////////////////////////////////
// TEXT INPUT FUNCTIONS ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void decodeBase64(std::vector<unsigned char>& out, const std::string& in);

////////////////////////////////////////////////////////////////////////////////
// AUDIO FUNCTIONS /////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void initAudio(int frequency, Uint16 format, int channels, int chunksize);

}

#endif
