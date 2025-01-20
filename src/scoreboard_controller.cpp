#include "scoreboard_controller.h"
#include "header.h"

extern ScrollingText dynamicText[];
extern TeamConfig teamA;
extern TeamConfig teamB;
void updateTeam(const char* team)
{
    if (strcmp(team, "A") == 0)
    {
        initAsyncScrollingText(dynamicText[0],
                               teamA.name,      // Dynamic team name
                               teamA.score,     // Dynamic score
                               teamA.teamColor, // Dynamic team color
                               teamA.scoreColor, // Dynamic team color
                               FS5, FS2, 100, 0, 12,
                               RIGHT_TO_LEFT, -1, 0, 19, true,0);
    }
    
    else if (strcmp(team, "B") == 0)
    {
        initAsyncScrollingText(dynamicText[1],
                               teamB.name,         // Dynamic team name
                               teamB.score,        // Dynamic score
                               teamB.teamColor,    // Dynamic team color
                               teamB.scoreColor, // Dynamic team color
                               FS4, FS2, 100, 5, 0,
                               LEFT_TO_RIGHT, 14, 6, 19, true,0);
    }
}

// initAsyncScrollingText(dynamicText[1], "DOMINO SCORE BOARD", "V01", CRGB::Red, CRGB::Yellow, FS6, FS3, 100, 0, 10, RIGHT_TO_LEFT, -1, 0, 18, true, -1, 1, colors, 4);
//   initAsyncScrollingText(dynamicText[2], "CENTRAL UNITED", "209", CRGB::Blue, CRGB::Red, FS3, FS2, 100, 5, 0, LEFT_TO_RIGHT, 13, 6, 18, true);