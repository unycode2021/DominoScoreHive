#include "header.h"
#include "webserver.h"


TeamConfig teamA;
TeamConfig teamB;

PlayerConfig players[4];

DNSServer dnsServer;
AsyncWebServer server(80);
void setupWebServer()
{
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(200, "text/html", PAGE_INDEX); });
    server.on("/setup", HTTP_GET, [](AsyncWebServerRequest *request)
              {
               AsyncResponseStream *response = request->beginResponseStream("text/html");
               response->print("<!DOCTYPE html><html><head><title>Team Setup</title>");
               response->print("<style>/* Add your CSS here */</style></head><body>");
               response->print("<h1>Team Setup</h1>");
               response->print("<form action='/api/init-config' method='POST'>");
               response->print("<input type='text' name='teamA' placeholder='Team A Name'><br>");
               response->print("<input type='text' name='teamB' placeholder='Team B Name'><br>");
               response->print("<button type='submit'>Save Teams</button>");
               response->print("</form></body></html>");
               request->send(response); });

    server.on("/scores", HTTP_GET, [](AsyncWebServerRequest *request)
              {
    AsyncResponseStream *response = request->beginResponseStream("text/html");
    response->print("<!DOCTYPE html><html><head>");
    response->print("<title>Score Management</title>");
    response->print("<style>");
    response->print("body { font-family: Arial; padding: 20px; }");
    response->print(".team-box { border: 1px solid #ddd; padding: 20px; margin: 10px; border-radius: 5px; }");
    response->print(".score-btn { padding: 10px; margin: 5px; cursor: pointer; }");
    response->print("</style></head><body>");
    
    // Team A Controls
    response->print("<div class='team-box'>");
    response->printf("<h2>%s</h2>", teamA.name);
    response->printf("<h3>Current Score: <span id='scoreA'>%s</span></h3>", teamA.score.score);
    response->print("<button class='score-btn' onclick='updateScore(\"A\", 1)'>+1</button>");
    response->print("<button class='score-btn' onclick='updateScore(\"A\", -1)'>-1</button>");
    response->print("</div>");
    
    // Team B Controls
    response->print("<div class='team-box'>");
    response->printf("<h2>%s</h2>", teamB.name);
    response->printf("<h3>Current Score: <span id='scoreB'>%s</span></h3>", teamB.score.score);
    response->print("<button class='score-btn' onclick='updateScore(\"B\", 1)'>+1</button>");
    response->print("<button class='score-btn' onclick='updateScore(\"B\", -1)'>-1</button>");
    response->print("</div>");

    // Add JavaScript for AJAX score updates
    response->print("<script>");
    response->print("function updateScore(team, change) {");
    response->print("  fetch('/api/score/update', {");
    response->print("    method: 'POST',");
    response->print("    headers: {'Content-Type': 'application/x-www-form-urlencoded'},");
    response->print("    body: `team=${team}&score=${change}`");
    response->print("  })");
    response->print("  .then(response => response.json())");
    response->print("  .then(data => {");
    response->print("    if(data.status === 'updated') {");
    response->print("      location.reload();");
    response->print("    }");
    response->print("  });");
    response->print("}");
    response->print("</script>");
    
    response->print("</body></html>");
                request->send(response); });

    server.on("/api/init-config", HTTP_POST, [](AsyncWebServerRequest *request)
              {
    if (request->hasParam("teamA", true) && request->hasParam("teamB", true)) {
        String teamAName = request->getParam("teamA", true)->value();
        String teamBName = request->getParam("teamB", true)->value();
        
        // Initialize Team A
        strncpy(teamA.name, teamAName.c_str(), sizeof(teamA.name));
        strncpy(teamA.pid, "ScoreHive", sizeof(teamA.pid));
        strncpy(teamA.score.score, "0", sizeof(teamA.score.score));
        strncpy(teamA.score.pid, "ScoreHive", sizeof(teamA.score.pid));
        
        // Set default text config for Team A
        teamA.settings.color = CRGB::Blue;
        teamA.settings.fontSize = FS5;
        teamA.settings.wordSpacing = 1;
        teamA.settings.charSpacing = 1;
        teamA.settings.useGradient = false;
        
        // Score text config for Team A
        teamA.score.settings = teamA.settings;
        teamA.score.settings.color = CRGB::Blue;
        
        // Initialize Team B with similar structure
        strncpy(teamB.name, teamBName.c_str(), sizeof(teamB.name));
        strncpy(teamB.pid, "ScoreHive", sizeof(teamB.pid));
        strncpy(teamB.score.score, "0", sizeof(teamB.score.score));
        strncpy(teamB.score.pid, "ScoreHive", sizeof(teamB.score.pid));
        
        // Set default text config for Team B
        teamB.settings.color = CRGB::Green;
        teamB.settings.fontSize = FS5;
        teamB.settings.wordSpacing = 1;
        teamB.settings.charSpacing = 1;
        teamB.settings.useGradient = false;
        
        // Score text config for Team B
        teamB.score.settings = teamB.settings;
        teamB.score.settings.color = CRGB::Green;
        
        // Mark as configured
        isConfigured = true;
        
        // Initialize the LED matrix display for both teams
        updateTeam("A");
        updateTeam("B");
        
        AsyncWebServerResponse *response = request->beginResponse(302);
        response->addHeader("Location", "/scores");
        request->send(response);
    } });

    // Score update endpoint
    server.on("/api/score/update", HTTP_POST, [](AsyncWebServerRequest *request)
              {
    if (request->hasParam("team", true) && request->hasParam("score", true)) {
        String team = request->getParam("team", true)->value();
        String scoreChange = request->getParam("score", true)->value();
        int change = scoreChange.toInt();
        
        if(team == "A") {
            int currentScore = String(teamA.score.score).toInt();
            currentScore += change;
            String newScore = String(currentScore);
            strncpy(teamA.score.score, newScore.c_str(), sizeof(teamA.score.score));
        } else if (team == "B") {
            int currentScore = String(teamB.score.score).toInt();
            currentScore += change;
            String newScore = String(currentScore);
            strncpy(teamB.score.score, newScore.c_str(), sizeof(teamB.score.score));
        }
        
        updateTeam(team.c_str());
        request->send(200, "application/json", "{\"status\":\"updated\"}");
    } });

    // Player management endpoint
    server.on("/api/players", HTTP_POST, [](AsyncWebServerRequest *request)
              {
           if (request->hasParam("team", true) && request->hasParam("players", true)) {
               String team = request->getParam("team", true)->value();
               String playersJson = request->getParam("players", true)->value();
            
               // Parse and store player data
               JsonDocument doc;
               deserializeJson(doc, playersJson);
            
               TeamConfig* targetTeam = (team == "A") ? &teamA : &teamB;
               JsonArray players = doc["players"];
               int i = 0;
               for(JsonVariant player : players) {
                   if(i < 4) {
                       strncpy(targetTeam->players[i].name, player.as<const char*>(), 32);
                       i++;
                   }
               }
               request->send(200, "application/json", "{\"status\":\"players_updated\"}");
           } });
}
