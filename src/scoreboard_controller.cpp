#include "scoreboard_controller.h"
#include "header.h"

extern ScrollingText dynamicText[];

void updateTeamScore(const char* team, int score) {
    if(strcmp(team, "A") == 0) {
        initAsyncScrollingText(dynamicText[0], "Team A", String(score).c_str(), 
                             CRGB::Blue, CRGB::Red, FS3, FS2, 100, 13, 0, 
                             RIGHT_TO_LEFT, -1, 0, 18, true);
    } else if(strcmp(team, "B") == 0) {
        initAsyncScrollingText(dynamicText[1], "Team B", String(score).c_str(), 
                             CRGB::Green, CRGB::Red, FS3, FS2, 100, 5, 0, 
                             LEFT_TO_RIGHT, 13, 6, 18, true);
    }
}
