#include "header.h"
#include "scoreboard_controller.h"

extern ScrollingText dynamicText[];
extern TeamConfig teamA;
extern TeamConfig teamB;
void updateTeam(const char *team)
{
    if (strcmp(team, "A") == 0)
    {
        initAsyncScrollingText(dynamicText[0],
                               teamA.name, // Dynamic team name
                               teamA.settings,
                               100, // Dynamic score
                               RIGHT_TO_LEFT, -1, true);
        initStaticText(
            dynamicText[1],
            teamA.score.score,
            teamA.score.settings);
    }
    else if (strcmp(team, "B") == 0)
    {
        initAsyncScrollingText(dynamicText[2],
                               teamB.name, // Dynamic team name
                               teamB.settings,
                               100,
                               LEFT_TO_RIGHT,
                               -1,
                               true);
        initStaticText(
            dynamicText[3],
            teamB.score.score,
            teamB.score.settings);
    }
}

// initAsyncScrollingText(dynamicText[1], "DOMINO SCORE BOARD", "V01", CRGB::Red, CRGB::Yellow, FS6, FS3, 100, 0, 10, RIGHT_TO_LEFT, -1, 0, 18, true, -1, 1, colors, 4);
//   initAsyncScrollingText(dynamicText[2], "CENTRAL UNITED", "209", CRGB::Blue, CRGB::Red, FS3, FS2, 100, 5, 0, LEFT_TO_RIGHT, 13, 6, 18, true);