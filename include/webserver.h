#include <DNSServer.h>
#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include "ESPAsyncWebServer.h"
#include "scoreboard_controller.h"
#include <ArduinoJson.h>

extern bool isConfigured;
// Global state management

TeamConfig teamA;
TeamConfig teamB;

PlayerConfig players[4];

DNSServer dnsServer;
AsyncWebServer server(80);

class CaptiveRequestHandler : public AsyncWebHandler
{
public:
    CaptiveRequestHandler() {}
    virtual ~CaptiveRequestHandler() {}

    bool canHandle(AsyncWebServerRequest *request)
    {
        // Only handle root and captive portal requests
        String host = request->host();
        return !request->url().startsWith("/api/") &&
               !request->url().startsWith("/setup") &&
               !request->url().startsWith("/scores") &&
               !request->url().startsWith("/players") &&
               !request->url().startsWith("/config");
    }

    void handleRequest(AsyncWebServerRequest *request)
    {
        AsyncResponseStream *response = request->beginResponseStream("text/html");
        response->print("<!DOCTYPE html><html><head>");
        response->print("<title>ScoreHive Setup</title>");
        response->print("<style>");
        response->print("body { font-family: Arial, sans-serif; margin: 20px; background-color: #f0f0f0; }");
        response->print("h1 { color: #333; text-align: center; }");
        response->print(".nav-button { display: block; width: 200px; margin: 10px auto; padding: 15px; ");
        response->print("background-color: #4CAF50; color: white; text-decoration: none; text-align: center; ");
        response->print("border-radius: 5px; font-weight: bold; }");
        response->print(".nav-button:hover { background-color: #45a049; }");
        response->print("</style></head><body>");
        response->print("<h1>ScoreHive Configuration Portal</h1>");
        response->print("<div class='nav-container'>");
        response->print("<a class='nav-button' href='/setup'>Setup Teams</a>");
        response->print("<a class='nav-button' href='/scores'>Manage Scores</a>");
        response->print("<a class='nav-button' href='/players'>Manage Players</a>");
        response->print("<a class='nav-button' href='/config'>View Configuration</a>");
        response->print("</div></body></html>");
        request->send(response);
    }
};

void setupWebServer()
{
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
    response->printf("<h3>Current Score: <span id='scoreA'>%s</span></h3>", teamA.score);
    response->print("<button class='score-btn' onclick='updateScore(\"A\", 1)'>+1</button>");
    response->print("<button class='score-btn' onclick='updateScore(\"A\", -1)'>-1</button>");
    response->print("</div>");
    
    // Team B Controls
    response->print("<div class='team-box'>");
    response->printf("<h2>%s</h2>", teamB.name);
    response->printf("<h3>Current Score: <span id='scoreB'>%s</span></h3>", teamB.score);
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

    server.on("/players", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        AsyncResponseStream *response = request->beginResponseStream("text/html");
        response->print("<!DOCTYPE html><html><head><title>Player Management</title></head><body>");
        response->print("<h1>Player Management</h1>");
        // Add player management form
        request->send(response); });

    // WiFi Configuration endpoint
    server.on("/wifi/configure", HTTP_POST, [](AsyncWebServerRequest *request)
              {
           String ssid = request->arg("ssid");
           String password = request->arg("password");
           // Store WiFi credentials and attempt connection
           request->send(200, "application/json", "{\"status\":\"configuring\"}");
       });

    server.on("/api/init-config", HTTP_POST, [](AsyncWebServerRequest *request)
              {
            if (request->hasParam("teamA", true) && request->hasParam("teamB", true)) {
                String teamAName = request->getParam("teamA", true)->value();
                String teamBName = request->getParam("teamB", true)->value();
                
                // Store team names
                strncpy(teamA.name, teamAName.c_str(), sizeof(teamA.name));
                strncpy(teamB.name, teamBName.c_str(), sizeof(teamB.name));
                
                // Initialize scores to 0
                const char *zero = "0";

                strncpy(teamA.score, zero, sizeof(zero));
                strncpy(teamB.score, zero, sizeof(zero));
                // Initialize team colors defaults for now until user configuration is added
                teamA.teamColor = CRGB::Blue;
                teamA.scoreColor = CRGB::Blue;
                teamB.teamColor = CRGB::Green;
                teamB.scoreColor = CRGB::Green;
                // Initialize the LED matrix display for both teams
                updateTeam("A");  // This will set up Team A's display
                updateTeam("B");  // This will set up Team B's display
                
                // Mark as configured
                isConfigured = true;

                AsyncWebServerResponse *response = request->beginResponse(302);
                response->addHeader("Location", "/scores");
                request->send(response);
            } });

    // Team configuration endpoint
    server.on("/api/teams", HTTP_POST, [](AsyncWebServerRequest *request)
              {
           if (request->hasParam("team", true) && request->hasParam("name", true)) {
               String team = request->getParam("team", true)->value();
               String name = request->getParam("name", true)->value();
            
               if (team == "A") {
                   strncpy(teamA.name, name.c_str(), sizeof(teamA.name));
               } else if (team == "B") {
                   strncpy(teamB.name, name.c_str(), sizeof(teamB.name));
               }
               request->send(200, "application/json", "{\"status\":\"success\"}");
           } });

    // Score update endpoint
    server.on("/api/score/update", HTTP_POST, [](AsyncWebServerRequest *request)
              {
    if (request->hasParam("team", true) && request->hasParam("score", true)) {
        String team = request->getParam("team", true)->value();
        String scoreChange = request->getParam("score", true)->value();
        int change = scoreChange.toInt();
        
        if(team == "A") {
            int currentScore = String(teamA.score).toInt();
            currentScore += change;
            String newScore = String(currentScore);
            strncpy(teamA.score, newScore.c_str(), sizeof(teamA.score));
        } else if (team == "B") {
            int currentScore = String(teamB.score).toInt();
            currentScore += change;
            String newScore = String(currentScore);
            strncpy(teamB.score, newScore.c_str(), sizeof(teamB.score));
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
