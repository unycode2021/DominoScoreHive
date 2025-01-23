const char PAGE_SETUP[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>

<head>
       <title>Team Setup</title>
       <style>
              body {
                     font-family: Arial, sans-serif;
                     margin: 20px;
                     background-color: #f0f0f0;
                     text-align: center;
              }

              h1 {
                     color: #333;
                     margin-bottom: 30px;
              }

              form {
                     max-width: 400px;
                     margin: 0 auto;
              }

              input[type="text"] {
                     width: 100%;
                     padding: 12px;
                     margin: 10px 0;
                     border: 1px solid #ddd;
                     border-radius: 4px;
                     box-sizing: border-box;
              }

              button {
                     background-color: #4CAF50;
                     color: white;
                     padding: 14px 20px;
                     margin: 10px 0;
                     border: none;
                     border-radius: 4px;
                     cursor: pointer;
                     width: 100%;
                     font-weight: bold;
              }

              button:hover {
                     background-color: #45a049;
              }
       </style>
</head>

<body>
       <h1>Team Setup</h1>
       <form action="/api/init-config" method="POST">
              <input type="text" name="teamA" placeholder="Team A Name" required>
              <input type="text" name="teamB" placeholder="Team B Name" required>
              <button type="submit">Save Teams</button>
       </form>
</body>

</html>
)=====";