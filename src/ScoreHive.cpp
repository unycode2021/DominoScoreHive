#include "header.h"
#include "webserver.h"
// #include "wifi_manager.h"
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

// extern void wifiManager_init();
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


void displayStaticText(const char *text, TextConfig config)
{
  int cspace = config.charSpacing == -1 ? charSpaceMap[config.fontSize] : config.charSpacing;
  int charWidth = charWidthMap[config.fontSize] + cspace; // Include spacing
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
    drawChar(*text++, config.startRow, config.startCol, config.color, config.fontSize, config.useGradient, config.color, config.gradientEnd);
    config.startCol += charWidth;
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

void initStaticText(
    ScrollingText &text,
    const char *scrollingText,
    TextConfig config
   )
{
  text.scrollingText = scrollingText;
  config.animate = false;
  text.config = config;
}

void initAsyncScrollingText(
    ScrollingText &text,
    const char *scrollingText,
    TextConfig config,
    int scrollSpeed,
    ScrollDirection scrollDirection,
    int scrollTo,
    bool wrapAround)
{
  text.scrollingText = configureScrollingText(scrollingText, scrollDirection);
  config.animate = true;
  text.config = config;
  text.scrollSpeed = scrollSpeed;
  text.scrollDirection = scrollDirection;
  text.scrollTo = scrollTo;
  text.wrapAround = wrapAround;
  text.offset = 0;
  text.lastUpdate = millis();
  text.isComplete = false;
}

// Update the scrolling text asynchronously
void updateAsyncScrollingText(ScrollingText &text)
{
  unsigned long currentTime = millis();
  if (currentTime - text.lastUpdate >= text.scrollSpeed && !text.isComplete)
  {
    text.lastUpdate = currentTime;
    bool noOffset = false;
    int charSpacing = text.config.charSpacing == -1 ? charSpaceMap[text.config.fontSize] : text.config.charSpacing;
    int charWidth = charWidthMap[text.config.fontSize];
    int charHeight = charHeightMap[text.config.fontSize];
    int scrollingTextLength = strlen(text.scrollingText);
    int colStartOffset = text.config.startCol;
    int rowStartOffset = ROWS - text.config.startRow;
    int scrollDistance = (text.scrollDirection == LEFT_TO_RIGHT || text.scrollDirection == RIGHT_TO_LEFT)
                             ? scrollingTextLength * (charWidth + charWidth * 1.3)
                             : scrollingTextLength * charHeight + rowStartOffset;

    int clearStartRow, clearEndRow, clearStartCol, clearEndCol;

    // Clear the specific region affected by the scrolling text
    if (text.scrollDirection == LEFT_TO_RIGHT || text.scrollDirection == RIGHT_TO_LEFT)
    {
      // For horizontal scrolling
      clearStartRow = text.config.startRow;
      clearEndRow = text.config.startRow + charHeightMap[text.config.fontSize] - 1;

      // Previous position plus character width
      int prevOffset = text.offset - 1;
      if (text.scrollDirection == LEFT_TO_RIGHT)
      {
        clearStartCol = text.config.startCol;
        int scrollTo = text.scrollTo > -1 ? text.scrollTo : scrollDistance;
        clearEndCol = (scrollTo + text.offset) + charWidth;
      }
      else
      {
        clearStartCol = (text.config.startCol - text.offset) - charWidth;
        clearEndCol = prevOffset + scrollingTextLength * charWidth;
      }
    }
    else
    {
      // For vertical scrolling
      clearStartCol = text.config.startCol;
      clearEndCol = text.config.startCol + charWidthMap[text.config.fontSize] - 1;

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

    // Draw the scrolling text
    for (int i = 0; i < scrollingTextLength; i++)
    {
      int startCol = 0;
      int startRow = 0;

      if (text.scrollDirection == LEFT_TO_RIGHT)
      {
        // startCol = (text.startY + text.offset) - i * charWidth;
        startCol = text.config.startCol + text.offset;
        for (int j = 0; j < i; j++)
        {
          if (text.scrollingText[j] == ' ')
          {
            startCol -= text.config.wordSpacing;
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

        if (startCol <= COLS && startCol >= text.config.startCol)
        {
          
          drawChar(text.scrollingText[i], text.config.startRow, startCol, text.config.color, text.config.fontSize);
        }
      }
      else if (text.scrollDirection == RIGHT_TO_LEFT)
      {
        // startCol = (colStartOffset - text.offset) + i * charWidth;
        startCol = text.config.startCol - text.offset;
        for (int j = 0; j < i; j++)
        {
          if (text.scrollingText[j] == ' ')
          {
            startCol += text.config.wordSpacing;
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

        if (startCol >= 0 && startCol < text.config.startCol)
        {

          drawChar(text.scrollingText[i], text.config.startRow, startCol, text.config.color, text.config.fontSize);
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
            startRow += text.config.wordSpacing;
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
          drawChar(text.scrollingText[i], startRow, text.config.startCol, text.config.color, text.config.fontSize);
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
            startRow -= text.config.wordSpacing;
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
            startRow = (rowStartOffset - text.offset) + i * text.config.wordSpacing;
          }
          drawChar(text.scrollingText[i], startRow, text.config.startCol, text.config.color, text.config.fontSize);
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


void setup()
{
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  // Default display configuration

  if (!isConfigured)
  {
    CRGB colors[] = {CRGB::Red, CRGB::Green, CRGB::Blue, CRGB::Yellow};
    TextConfig config = {
        FS6, // fontSize
        1,  // startRow
        25, // startCol
        2,  // wordSpacing
        1,  // charSpacing
        CRGB::Blue, // color
        CRGB::Black, // gradientEnd
        colors, // charColors
        false, // isGradient
        0}; // colorsLength
    initAsyncScrollingText(
        dynamicText[0],
        "SCOREHIVE BY DOMINO101",
        config,
        100, // scrollSpeed
        RIGHT_TO_LEFT, // scrollDirection
        -1, // scrollTo
        true // wrapAround
        );
  }

  WiFi.softAP("ScoreHive-Setup");
  WiFi.setHostname("score_hive");
  dnsServer.start(53, "*", WiFi.softAPIP());

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
        if (dynamicText[i].config.animate)
        {
          updateAsyncScrollingText(dynamicText[i]);
        }
        else
        {
          displayStaticText(dynamicText[i].scrollingText, dynamicText[i].config);
        }
      }
    }
  }
  dnsServer.processNextRequest();
}

