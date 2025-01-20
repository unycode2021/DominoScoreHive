#include "header.h"
#include "webserver.h"
// Constants
#define NUM_LEDS 300  // Total LEDs
#define LED_PIN 2     // Data pin
#define ROWS 10       // Number of rows
#define COLS 30       // Number of columns
#define BRIGHTNESS 30 // LED brightness (0-255)
// LED Array
CRGB leds[NUM_LEDS];
ScrollingText dynamicText[20] = {{0}};
bool isConfigured = false;

const int numFonts = sizeof(charFontMap) / sizeof(charFontMap[0]);
const int ALPHBETH_LENGTH = 38;
// Map grid position to LED index in zigzag pattern
int mapLED(int row, int col)
{
  if (row % 2 == 0)
  {
    return (row + 1) * COLS - 1 - col; // Reverse direction
  }
  else
  {
    return row * COLS + col; // Normal direction
  }
}

CRGB getGradientColor(CRGB start, CRGB end, float progress)
{
  return CRGB(
      start.r + (end.r - start.r) * progress,
      start.g + (end.g - start.g) * progress,
      start.b + (end.b - start.b) * progress);
}

CRGB getCharsColorORDefault(CRGB *charColors, int charIndex, CRGB defaultColor, int colorsLength = 0)
{
  // If no color array provided, return default
  if (!charColors)
  {
    return defaultColor;
  }

  if (charIndex >= 0 && charIndex < colorsLength)
  {
    return charColors[charIndex];
  }
  // Return default if index out of bounds
  return defaultColor;
}

void clearScreen()
{
  FastLED.clear();
}

void clearScreen(int startRow, int endRow, int startCol, int endCol)
{
  // Ensure boundaries are within the valid range
  startRow = max(0, min(startRow, ROWS - 1));
  endRow = max(0, min(endRow, ROWS - 1));
  startCol = max(0, min(startCol, COLS - 1));
  endCol = max(0, min(endCol, COLS - 1));

  // Clear only the specified region
  for (int row = startRow; row <= endRow; row++)
  {
    for (int col = startCol; col <= endCol; col++)
    {
      int ledIndex = mapLED(row, col);
      if (ledIndex >= 0 && ledIndex < NUM_LEDS)
      {
        leds[ledIndex] = CRGB::Black;
      }
    }
  }
}

void drawToScreen()
{
  FastLED.show();
}

// Draw a character at a specific position
void drawChar(char character, int startRow, int startCol, CRGB color, FontSize size = FS5, bool useGradient = false, CRGB gradientStart = CRGB::Black, CRGB gradientEnd = CRGB::Black)
{
  int charHeight = charHeightMap[size];
  const uint8_t(*font)[charHeight] = nullptr;
  for (int i = 0; i < numFonts; i++)
  {
    if (charFontMap[i].size == size)
    {
      font = (const uint8_t(*)[charHeight])charFontMap[i].font;
      break;
    }
  }
  // Determine character dimensions based on font size
  int charWidth = charWidthMap[size];
  int charIndex = -1;
  // Determine the index for the character
  if (character >= 'A' && character <= 'Z')
    charIndex = character - 'A'; // Uppercase letters
  // else if (character >= 'a' && character <= 'z')
  //   charIndex = (character - 'a') + 26; // Lowercase letters after uppercase
  else if (character >= '0' && character <= '9')
    charIndex = (character - '0') + 26; // Numbers after letters
  else if (character == ' ')
    charIndex = 36; // Space (you can adjust based on the font array)
  else if (character == '|')
    charIndex = 37;

  if (charIndex < 0 || charIndex >= ALPHBETH_LENGTH)
    return; // Ignore unsupported characters

  // Loop through each row and column of the character
  for (int row = 0; row < charHeight; row++)
  {
    uint8_t rowData = font[charIndex][row]; // Get the bit pattern for the current row

    for (int col = 0; col < charWidth; col++)
    {
      // Check if the bit for the current column is set
      if (rowData & (1 << (charWidth - 1 - col)))
      {
        // Map grid position to LED index
        int ledIndex = mapLED(startRow + row, startCol + col);
        if (ledIndex >= 0 && ledIndex < NUM_LEDS)
        {
          if (useGradient)
          {
            float progress = (float)(row) / charHeight;
            leds[ledIndex] = getGradientColor(gradientStart, gradientEnd, progress);
          }
          else
          {
            leds[ledIndex] = color; // normal color
          }
        }
      }
    }
  }
}

// void displayStaticText(const char *text, int startRow, int startCol, CRGB color, FontSize size = FS5)
// {
//   int charWidth = charWidthMap[size] + charSpaceMap[size]; // Include spacing
//   while (*text)
//   {
//     drawChar(*text++, startRow, startCol, color, size);
//     startCol += charWidth;
//   }
//   drawToScreen();
// }
// Display static text
void displayStaticText(const char *text, int startRow, int startCol, CRGB color, FontSize size = FS5, int charSpacing = -1, int wordSpacing = 1, CRGB *charColors = nullptr, int colorsLength = 0, bool useGradient = false, CRGB gradientStart = CRGB::Black, CRGB gradientEnd = CRGB::Black)
{
  int cspace = charSpacing == -1 ? charSpaceMap[size] : charSpacing;
  int charWidth = charWidthMap[size] + cspace; // Include spacing
  // int cc = 0;
  // while (*text)
  // {
  //   if (*text == ' ')
  //   {
  //     startCol += wordSpacing; // Use smaller spacing for spaces
  //   }
  //   else
  //   {
  //     CRGB text_color = getCharsColorORDefault(charColors, cc, text_color, colorsLength);
  //     drawChar(*text++, startRow, startCol, text_color, size, useGradient, gradientStart, gradientEnd);
  //     startCol += charWidth;
  //     cc++;
  //   }
  // }
  //  int charWidth = charWidthMap[size] + charSpaceMap[size]; // Include spacing
  while (*text)
  {
    drawChar(*text++, startRow, startCol, color, size, useGradient, gradientStart, gradientEnd);
    startCol += charWidth;
  }
  //   drawToScreen();
  drawToScreen();
}

const char *configureScrollingText(const char *text, ScrollDirection direction)
{
  if (direction != LEFT_TO_RIGHT)
    return text;

  //  reverse the scrolling text
  // Get length of input text
  int len = strlen(text);

  // Allocate memory for reversed string
  char *reversed = new char[len + 1];

  // Copy characters in reverse order
  for (int i = 0; i < len; i++)
  {
    reversed[i] = text[len - 1 - i];
  }

  // Add null terminator
  reversed[len] = '\0';

  return reversed;
}

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
    int scrollTo,
    int staticStartX,
    int staticStartY,
    bool wrapAround,
    int charSpacing,
    int wordSpacing,
    CRGB *charColors,
    int colorsLength,
    bool useGradient,
    CRGB gradientStart,
    CRGB gradientEnd){
  text.scrollingText = configureScrollingText(scrollingText, scrollDirection);
  text.staticText = staticText;
  text.scrollColor = scrollColor;
  text.staticColor = staticColor;
  text.scrollSize = scrollSize;
  text.staticSize = staticSize;
  text.scrollSpeed = scrollSpeed;
  text.startX = startX;
  text.startY = startY;
  text.scrollDirection = scrollDirection;
  text.scrollTo = scrollTo;
  text.staticStartX = staticStartX;
  text.staticStartY = staticStartY;
  text.wrapAround = wrapAround;
  text.offset = 0;
  text.wordSpacing = wordSpacing;
  text.charSpacing = charSpacing;
  text.lastUpdate = millis();
  text.isComplete = false;
  text.charColors = charColors;
  text.useGradient = useGradient;
  text.gradientStart = gradientStart;
  text.gradientEnd = gradientEnd;
  text.colorsLength = colorsLength;
}

// Update the scrolling text asynchronously
void updateAsyncScrollingText(ScrollingText &text)
{
  unsigned long currentTime = millis();
  if (currentTime - text.lastUpdate >= text.scrollSpeed && !text.isComplete)
  {
    text.lastUpdate = currentTime;
    bool noOffset = false;
    int charSpacing = text.charSpacing == -1 ? charSpaceMap[text.scrollSize] : text.charSpacing;
    int charWidth = charWidthMap[text.scrollSize] + charSpacing;
    int charHeight = charHeightMap[text.scrollSize];
    int scrollingTextLength = strlen(text.scrollingText);
    int colStartOffset = COLS - text.startY;
    int rowStartOffset = ROWS - text.startX;
    int scrollDistance = (text.scrollDirection == LEFT_TO_RIGHT || text.scrollDirection == RIGHT_TO_LEFT)
                             ? scrollingTextLength * charWidth + colStartOffset
                             : scrollingTextLength * charHeight + rowStartOffset;

    int clearStartRow, clearEndRow, clearStartCol, clearEndCol;

    // Clear the specific region affected by the scrolling text
    if (text.scrollDirection == LEFT_TO_RIGHT || text.scrollDirection == RIGHT_TO_LEFT)
    {
      // For horizontal scrolling
      clearStartRow = text.startX;
      clearEndRow = text.startX + charHeightMap[text.scrollSize] - 1;

      // Previous position plus character width
      int prevOffset = text.offset - 1;
      if (text.scrollDirection == LEFT_TO_RIGHT)
      {
        clearStartCol = text.startY;
        clearEndCol = (text.startY + text.offset) + charWidth;
      }
      else
      {
        clearStartCol = (text.startY - text.offset) - charWidth;
        clearEndCol = prevOffset + scrollingTextLength * charWidth;
      }
    }
    else
    {
      // For vertical scrolling
      clearStartCol = text.startY;
      clearEndCol = text.startY + charWidthMap[text.scrollSize] - 1;

      int prevOffset = text.offset - 1;
      if (text.scrollDirection == TOP_TO_BOTTOM)
      {
        clearStartRow = (rowStartOffset + prevOffset) - scrollingTextLength * charHeight;
        clearEndRow = (rowStartOffset + text.offset) + charHeight;
      }
      else
      {
        clearStartRow = (rowStartOffset - text.offset) - charHeight;
        clearEndRow = (rowStartOffset - prevOffset) + scrollingTextLength * charHeight;
      }
    }

    // Clear only the necessary region
    clearScreen(clearStartRow, clearEndRow, clearStartCol, clearEndCol);

    // Draw the static text
    displayStaticText(text.staticText, text.staticStartX, text.staticStartY, text.staticColor, text.staticSize, text.charSpacing, text.wordSpacing, text.charColors, text.colorsLength, text.useGradient, text.gradientStart, text.gradientEnd);

    // Draw the scrolling text
    for (int i = 0; i < scrollingTextLength; i++)
    {
      int startCol = 0;
      int startRow = 0;

      if (text.scrollDirection == LEFT_TO_RIGHT)
      {
        // startCol = (text.startY + text.offset) - i * charWidth;
        startCol = text.startY + text.offset;
        for (int j = 0; j < i; j++)
        {
          if (text.scrollingText[j] == ' ')
          {
            startCol -= text.wordSpacing;
          }
          else
          {
            startCol -= (charWidth + charSpacing);
          }
        }
        if (text.scrollTo != -1 && startCol >= text.scrollTo)
        {
          if (text.wrapAround)
          {
            startCol = -charWidth;
          }
          else
          {
            startCol = text.scrollTo + COLS;
            text.isComplete = true;
          }
        }

        if (startCol <= COLS && startCol >= text.startY)
        {
          drawChar(text.scrollingText[i], text.startX, startCol, text.scrollColor, text.scrollSize);
        }
      }
      else if (text.scrollDirection == RIGHT_TO_LEFT)
      {
        // startCol = (colStartOffset - text.offset) + i * charWidth;
        startCol = text.startY - text.offset;
        for (int j = 0; j < i; j++)
        {
          if (text.scrollingText[j] == ' ')
          {
            startCol += text.wordSpacing;
          }
          else
          {
            startCol += (charWidth + charSpacing);
          }
        }
        if (text.scrollTo != -1 && startCol < text.scrollTo)
        {
          if (text.wrapAround)
          {
            startCol = COLS;
          }
          else
          {
            startCol = text.scrollTo;
            text.isComplete = true;
          }
        }

        if (startCol >= 0 && startCol < text.startY)
        {

          drawChar(text.scrollingText[i], text.startX, startCol, text.scrollColor, text.scrollSize);
        }
      }
      else if (text.scrollDirection == TOP_TO_BOTTOM)
      {
        // startRow = (rowStartOffset + text.offset) - i * charHeight;
        startRow = rowStartOffset + text.offset;
        for (int j = 0; j < i; j++)
        {
          if (text.scrollingText[j] == ' ')
          {
            startRow += text.wordSpacing;
          }
          else
          {
            startRow += (charHeight + charSpacing);
          }
        }
        if (text.scrollTo != -1 && startRow >= text.scrollTo)
        {
          if (text.wrapAround)
          {
            startRow = -charHeight;
          }
          else
          {
            startRow = text.scrollTo;
            text.isComplete = true;
          }
        }

        if (startRow < ROWS && startRow >= 0)
        {

          drawChar(text.scrollingText[i], startRow, text.startY, text.scrollColor, text.scrollSize);
        }
      }
      else if (text.scrollDirection == BOTTOM_TO_TOP)
      {
        // startRow = (rowStartOffset - text.offset) + i * charHeight;
        startRow = rowStartOffset - text.offset;
        for (int j = 0; j < i; j++)
        {
          if (text.scrollingText[j] == ' ')
          {
            startRow -= text.wordSpacing;
          }
          else
          {
            startRow -= (charHeight + charSpacing);
          }
        }
        if (text.scrollTo != -1 && startRow <= text.scrollTo)
        {
          if (text.wrapAround)
          {
            startRow = ROWS;
          }
          else
          {
            startRow = text.scrollTo;
            text.isComplete = true;
          }
        }

        if (startRow >= 0 && startRow < ROWS)
        {
          if (text.scrollingText[i] == ' ')
          {
            startRow = (rowStartOffset - text.offset) + i * text.wordSpacing;
          }
          drawChar(text.scrollingText[i], startRow, text.startY, text.scrollColor, text.scrollSize);
        }
      }
    }

    // Increment the offset
    text.offset++;
    if (text.offset > scrollDistance)
    {
      if (text.wrapAround)
      {
        text.offset = 0; // Reset to the starting position
      }
      else
      {
        text.isComplete = true; // Mark as complete
      }
    }

    // Render the updates
    drawToScreen();
  }
}

// void setup()
// {
//   FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
//   FastLED.setBrightness(BRIGHTNESS);
//   CRGB colors[] = {CRGB::Red, CRGB::Green, CRGB::Orange, CRGB::Yellow};

//   // Horizontal layout settings
//   initAsyncScrollingText(dynamicText[1], "DOMINO SCORE BOARD", "V01", CRGB::Red, CRGB::Yellow, FS6, FS3, 100, 0, 10, RIGHT_TO_LEFT, -1, 0, 18, true, -1, 1, colors, 4);
//   // initAsyncScrollingText(dynamicText[2], "CENTRAL UNITED", "209", CRGB::Blue, CRGB::Red, FS3, FS2, 100, 5, 0, LEFT_TO_RIGHT, 13, 6, 18, true);

//   // Vertical layout settings
//   // initAsyncScrollingText(text1, "TEAM A", "O", CRGB::Blue, CRGB::Blue, 3, 2, 100, 0, 18, RIGHT_TO_LEFT, -1, 5, 3, true);
//   // initAsyncScrollingText(text2, "TEAM B", "O", CRGB::Green, CRGB::Red, 3, 2, 100, 0, 0, RIGHT_TO_LEFT, 18, 5, 22, true);

//   WiFi.softAP("esp-captive");
//   dnsServer.start(53, "*", WiFi.softAPIP());
//   server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER); // only when requested from AP
//   // more handlers...
//   server.begin();

void setup()
{
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);

  // Default display configuration
  if (!isConfigured)
  {
    CRGB colors[] = {CRGB::Red, CRGB::Green, CRGB::Blue, CRGB::Yellow};
    initAsyncScrollingText(
        dynamicText[0],
        "SCOREHIVE BY DOMINO101",
        "",
        CRGB::Yellow,
        CRGB::Yellow,
        FS6,
        FS3,
        100, // scroll speed
        1,   // startX
        25,  // startY
        RIGHT_TO_LEFT,
        0,   // scrollTo
        0,    // staticStartX
        18,   // staticStartY
        true, // wrapAround
        -1,   // charSpacing
        1,    // wordSpacing
        colors,
        4 // colors length
    );
  }

  WiFi.softAP("ScoreHive-Setup");
  WiFi.setHostname("ScoreHive");
  dnsServer.start(53, "*", WiFi.softAPIP());
  // Add this line to initialize all web routes
  setupWebServer();
  server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);
  server.begin();
}

void loop()
{
  if (!isConfigured)
  {
    // Show default welcome message
    updateAsyncScrollingText(dynamicText[0]);
  }
  else
  {
    // Count valid entries in dynamicText array
    int validEntries = 0;
    for (int i = 0; i < 20; i++)
    {
      if (dynamicText[i].scrollingText != nullptr)
      {
        validEntries++;
      }
    }

    // Only loop through if we have valid entries
    if (validEntries > 0)
    {
      for (int i = 0; i < validEntries; i++)
      {
        updateAsyncScrollingText(dynamicText[i]);
      }
    }
  }
  dnsServer.processNextRequest();
}
