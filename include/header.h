#ifndef HEADER_H
#define HEADER_H
#define VERSION 2412040

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

struct TextConfig {
       FontSize fontSize;
       int startRow;
       int startCol;
       int wordSpacing;
       int charSpacing;
       CRGB color;
       CRGB gradientEnd;
       CRGB *charColors;
       bool useGradient;
       int colorsLength;
       bool animate;
};

struct ScoreConfig
{
       char pid[32];
       char score[32];
       TextConfig settings;
};

struct PlayerConfig
{
       char pid[32]; // platform id::Domino101, DominoScores, ScoreHive...etc....
       char tid[32]; // team id
       char alias[32];
       char name[155];
       ScoreConfig score;
       TextConfig settings;
};


struct TeamConfig
{
       char pid[32];
       char name[32];
       ScoreConfig score;
       TextConfig settings;
       PlayerConfig players[2];
};

struct TableConfig
{
       char pid[32];
       char tag[32];
       PlayerConfig players[4];
};

struct MatchConfig
{
       char pid[32];
       char name[155];
       TableConfig (*tables)[];
       TextConfig settings;
};

struct ScrollingText
{
       const char *scrollingText;
       TextConfig config;
       int scrollSpeed;
       ScrollDirection scrollDirection;
       int scrollTo;
       bool wrapAround;
       int offset; // Tracks the current offset position
       unsigned long lastUpdate;
       bool isComplete;    // Indicates when the scrolling is complete
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
void setupWebServer();
void drawChar(char character, int startRow, int startCol, CRGB color, FontSize size, bool useGradient, CRGB gradientStart, CRGB gradientEnd);
void displayStaticText(const char *text, TextConfig config);
const char *configureScrollingText(const char *text, ScrollDirection direction);

void initAsyncScrollingText(
        ScrollingText &text,
        const char *scrollingText,
        TextConfig config,
        int scrollSpeed,
        ScrollDirection scrollDirection,
        int scrollTo = -1,
        bool wrapAround = false);

void initStaticText(
    ScrollingText &text,
    const char *staticText,
    TextConfig config);

void updateScrollingText(ScrollingText &text);

#endif
