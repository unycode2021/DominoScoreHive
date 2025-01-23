const char PAGE_INDEX[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
    <title>ScoreHive Setup</title>
    <style>
        body { 
            font-family: Arial, sans-serif; 
            margin: 20px; 
            background-color: #f0f0f0; 
        }
        h1 { 
            color: #333; 
            text-align: center; 
        }
        .nav-button { 
            display: block; 
            width: 200px; 
            margin: 10px auto; 
            padding: 15px;
            background-color: #4CAF50; 
            color: white; 
            text-decoration: none; 
            text-align: center;
            border-radius: 5px; 
            font-weight: bold; 
        }
        .nav-button:hover { 
            background-color: #45a049; 
        }
    </style>
</head>
<body>
    <h1>ScoreHive Configuration Portal</h1>
    <div class='nav-container'>
        <a class='nav-button' href='/setup'>Setup Teams</a>
        <a class='nav-button' href='/scores'>Manage Scores</a>
        <a class='nav-button' href='/players'>Manage Players</a>
        <a class='nav-button' href='/config'>View Configuration</a>
    </div>
</body>
</html>
)=====";
