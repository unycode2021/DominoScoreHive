// This file includes the function prototypes for the LED mapping functions
#ifndef HEADER_H
#define HEADER_H

#include <map>
#include <Arduino.h>
#include <FastLED.h>

enum ScrollDirection
{
       LEFT_TO_RIGHT,
       RIGHT_TO_LEFT,
       TOP_TO_BOTTOM,
       BOTTOM_TO_TOP
};

enum FontSize
{
       FS1 = 1,
       FS2,
       FS3,
       FS4,
       FS5,
       FS6,
       FS7,
       FS8,
       FS9,
       FS10,
};

struct FontMapping
{
       uint8_t size;            // Font size
       const uint8_t (*font)[]; // Pointer to the font array
};

struct ScrollingText
{
       const char *scrollingText;
       const char *staticText;
       CRGB scrollColor;
       CRGB staticColor;
       FontSize scrollSize;
       FontSize staticSize;
       int scrollSpeed;
       int startX;
       int startY;
       ScrollDirection scrollDirection;
       int scrollTo;
       int staticStartX;
       int staticStartY;
       bool wrapAround;
       int offset; // Tracks the current offset position
       int wordSpacing;
       int charSpacing;
       unsigned long lastUpdate;
       bool isComplete;    // Indicates when the scrolling is complete
       CRGB *charColors;   // Array of colors for each character
       bool useGradient;   // Flag to enable gradient effect
       CRGB gradientStart; // Starting color for gradient
       CRGB gradientEnd;   // Ending color for gradient
       int colorsLength;
};


extern std::map<int, int> charSpaceMap;
extern std::map<int, int> charHeightMap;
extern std::map<int, int> charWidthMap;
extern const uint8_t font3x4[][4];
extern const uint8_t font3x5[][5];
extern const uint8_t font4x5[][5];
extern const uint8_t font5x5[][5];
extern const uint8_t font5x7[][7];
extern const FontMapping charFontMap[6];

int mapLED(int row, int col);
CRGB getGradientColor(CRGB start, CRGB end, float progress);
CRGB getCharsColorORDefault(CRGB *charColors, int charIndex, CRGB defaultColor, int colorsLength);
void clearScreen();
void clearScreen(int startRow, int endRow, int startCol, int endCol);
void drawToScreen();
void drawChar(char character, int startRow, int startCol, CRGB color, FontSize size, bool useGradient, CRGB gradientStart, CRGB gradientEnd);
void displayStaticText(const char *text, int startRow, int startCol, CRGB color, FontSize size, int charSpacing, int wordSpacing, CRGB *charColors, int colorsLength, bool useGradient, CRGB gradientStart, CRGB gradientEnd);
const char *configureScrollingText(const char *text, ScrollDirection direction);
// void initAsyncScrollingText(ScrollingText &text, const char *scrollingText, const char *staticText, CRGB scrollColor, CRGB staticColor, FontSize scrollSize, FontSize staticSize, int scrollSpeed, int startX, int startY, ScrollDirection scrollDirection, int scrollTo, int staticStartX, int staticStartY, bool wrapAround, int charSpacing, int wordSpacing, CRGB *charColors, int colorsLength, bool useGradient, CRGB gradientStart, CRGB gradientEnd);

void initAsyncScrollingText(
    ScrollingText &text,
    const char *scrollingText,
    const char *staticText,
    CRGB scrollColor,
    CRGB staticColor,
    FontSize scrollSize,
    FontSize staticSize,
    int scrollSpeed,
    int startX,
    int startY,
    ScrollDirection scrollDirection,
    int scrollTo = -1,
    int staticStartX = 0,
    int staticStartY = 0,
    bool wrapAround = false,
    int charSpacing = -1,
    int wordSpacing = 1,
    CRGB *charColors = nullptr,
    int colorsLength = 0,
    bool useGradient = false,
    CRGB gradientStart = CRGB::Black,
    CRGB gradientEnd = CRGB::Black);

void updateScrollingText(ScrollingText &text);

#endif
