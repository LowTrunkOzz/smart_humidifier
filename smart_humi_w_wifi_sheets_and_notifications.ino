
// Import required libraries

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <WiFiClientSecure.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_MCP23X17.h>
#include "config.h"

#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

DHT dht1(DHTPIN1, DHTTYPE);
DHT dht2(DHTPIN2, DHTTYPE);

// current temperature & humidity, updated in loop()
float t1 = 0.0;
float h1 = 0.0;
float t2 = 0.0;
float h2 = 0.0;

// Set to true to define Relay as Normally Open (NO)
#define RELAY_NO true

// Detects whenever the door changed state
bool changeState = false;

// Holds reedswitch state (1=opened, 0=close)
bool state;
String doorState;

const char *PARAM_INPUT_1 = "relay";
const char *PARAM_INPUT_2 = "state";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0;        // will store last time DHT was updated
unsigned long previousupdateMillis = 0;  //will store the last time Google Sheets was updated
unsigned long previousTELEMillis = 0;    // will store the last time Telegram was updated

const long interval = 4000;     // Updates DHT readings every 4000 milliseconds
long updateInterval = 900000;   //Updates Google Sheets every 15 mins
const long TELEinterval = 500;  // Detect changes that are 500 milliseconds apart for Telegram

// Initialize Telegram BOT
#define BOTtoken ""  // your Bot Token (Get from Botfather)
#define CHAT_ID ""                                       // Use @myidbot to find out the chat ID of an individual or a group

X509List cert(TELEGRAM_CERTIFICATE_ROOT);
UniversalTelegramBot bot(BOTtoken, client);


// Runs whenever the reedswitch changes state
ICACHE_RAM_ATTR void changeDoorStatus() {
  //debugln("State changed");
  changeState = true;
}


const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <style>
    html {
     font-family: Arial;
     display: inline-block;
     margin: 0px auto;
     text-align: center;
    }
    h2 { font-size: 2.0rem; }
    p { font-size: 2.0rem; }
    .units { font-size: 1rem; }
    .dht-labels{
      font-size: 1rem;
      vertical-align:middle;
      padding-bottom: 10px;
    }
    .door-labels{
      font-size: 1.5rem;
      vertical-align:middle;
      padding-bottom: 10px;
    }
	.switch {position: relative; display: inline-block; width: 60px; height: 34px} 
    .switch input {display: none}
    .slider {position: absolute; top: 0; left: 0; right: 0; bottom: 0; background-color: #ccc; border-radius: 17px}
    .slider:before {position: absolute; content: ""; height: 26px; width: 26px; left: 4px; bottom: 4px; background-color: #fff; -webkit-transition: .4s; transition: .4s; border-radius: 34px}
    input:checked+.slider {background-color: #2196F3}
    input:checked+.slider:before {-webkit-transform: translateX(26px); -ms-transform: translateX(26px); transform: translateX(26px)}
  </style>
</head>
<body>                
  <h2>Wineador Top:</h2>
    <p>
    <i class="fas fa-thermometer-half" style="color:#059e8a;"></i> 
    <span class="dht-labels">Temperature</span> 
    <span id="temperature1">%TEMPERATURE1%</span>
    <sup class="units">&deg;F</sup>
  </p>
   <p>
    <i class="fas fa-tint" style="color:#00add6;"></i> 
    <span class="dht-labels">Humidity</span>
    <span id="humidity1">%HUMIDITY1%</span>
    <sup class="units">&#37</sup>
  </p>
  
  <h2>Wineador Bottom:</h2>
   <p>
    <i class="fas fa-thermometer-half" style="color:#059e8a;"></i> 
    <span class="dht-labels">Temperature</span> 
    <span id="temperature2">%TEMPERATURE2%</span>
    <sup class="units">&deg;F</sup>
  </p>
    <p>
    <i class="fas fa-tint" style="color:#00add6;"></i> 
    <span class="dht-labels">Humidity</span>
    <span id="humidity2">%HUMIDITY2%</span>
    <sup class="units">&#37</sup>
  </p>
   <p>
   <i class="fas fa-door-open"></i>   
   <span class="door-labels">The door is</span>
   <span id="doorState">%DOORSTATE%</span>
  </p>
    <h2>Fan Relays</h2>
  %BUTTONPLACEHOLDER%
<script>function toggleCheckbox(element) {
  var xhr = new XMLHttpRequest();
  if(element.checked){ xhr.open("GET", "/update?relay="+element.id+"&state=1", true); }
  else { xhr.open("GET", "/update?relay="+element.id+"&state=0", true); }
  xhr.send();
}</script>
</body>
<script>
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("temperature1").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/temperature1", true);
  xhttp.send();
}, 4000 ) ; // webpage refresh rate

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("humidity1").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/humidity1", true);
  xhttp.send();
}, 4000 ) ;
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("temperature2").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/temperature2", true);
  xhttp.send();
}, 4000 ) ; // webpage refresh rate

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("humidity2").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/humidity2", true);
  xhttp.send();
}, 4000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("doorState").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/doorState", true);
  xhttp.send();
}, 4000 ) ;
</script>
</html>)rawliteral";

String relayState(int numRelay) {
  if (RELAY_NO) {
    if (digitalRead(relayGPIOs[numRelay - 1])) {
      return "";
    } else {
      return "checked";
    }
  } else {
    if (digitalRead(relayGPIOs[numRelay - 1])) {
      return "checked";
    } else {
      return "";
    }
  }
  return "";
}

// Replaces placeholder with button section in your web page
String processor(const String &var) {
  //debugln(var);
  if (var == "BUTTONPLACEHOLDER") {
    String buttons = "";
    for (int i = 1; i <= NUM_RELAYS; i++) {
      String relayStateValue = relayState(i);
      buttons += "<h4>Fan #" + String(i) + "</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"" + String(i) + "\" " + relayStateValue + "><span class=\"slider\"></span></label>";
    }
    return buttons;
  }
  if (var == "TEMPERATURE1") {
    return String(t1);
  } else if (var == "HUMIDITY1") {
    return String(h1);
  } else if (var == "TEMPERATURE2") {
    return String(t2);
  } else if (var == "HUMIDITY2") {
    return String(h2);
  } else if (var == "DOORSTATE") {
    return String(doorState);
  }

  return String();
}


void setup() {

  // Serial port for debugging purposes
  Serial.begin(115200);

  // Add root certificate for api.telegram.org
  client.setTrustAnchors(&cert);

  // uncomment appropriate mcp.begin
  if (!mcp.begin_I2C()) {
    //if (!mcp.begin_SPI(CS_PIN)) {
    debugln("Error.");
    while (1)
      ;
  }

  //Turn off relay outputs while waiting for WiFi in case there's an issue before readings start:
  mcp.digitalWrite(humi1, HIGH);
  mcp.digitalWrite(fan1, HIGH);
  mcp.digitalWrite(humi2, HIGH);
  mcp.digitalWrite(fan2, HIGH);


  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  debugln("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(4000);
    debugln("Connecting...");
  }

  bot.sendMessage(CHAT_ID, "Bot started up", "");

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    debugln(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }
  delay(2000);
  display.clearDisplay();
  display.setTextColor(WHITE);

  // uncomment appropriate mcp.begin
  if (!mcp.begin_I2C()) {
    //if (!mcp.begin_SPI(CS_PIN)) {
    debugln("Error.");
    while (1)
      ;
  }

  // Print ESP8266 Local IP Address
  debugln(WiFi.localIP());

  dht1.begin();
  dht2.begin();
  mcp.pinMode(humi1, OUTPUT);
  mcp.pinMode(humi2, OUTPUT);
  mcp.pinMode(fan1, OUTPUT);
  mcp.pinMode(fan2, OUTPUT);

  pinMode(reedSwitch, INPUT_PULLUP);

  // Set the reedswitch pin as interrupt, assign interrupt function and set CHANGE mode
  attachInterrupt(digitalPinToInterrupt(reedSwitch), changeDoorStatus, CHANGE);


  // Set all relays to off when the program starts - if set to Normally Open (NO), the relay is off when you set the relay to HIGH
  for (int i = 1; i <= NUM_RELAYS; i++) {
    pinMode(relayGPIOs[i - 1], OUTPUT);
    if (RELAY_NO) {
      digitalWrite(relayGPIOs[i - 1], HIGH);
    } else {
      digitalWrite(relayGPIOs[i - 1], LOW);
    }
  }


  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html, processor);
  });
  server.on("/temperature1", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/plain", String(t1).c_str());
  });
  server.on("/humidity1", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/plain", String(h1).c_str());
  });
  server.on("/temperature2", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/plain", String(t2).c_str());
  });
  server.on("/humidity2", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/plain", String(h2).c_str());
  });
  server.on("/doorState", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/plain", String(doorState).c_str());
  });

  // Send a GET request to <ESP_IP>/update?relay=<inputMessage>&state=<inputMessage2>
  server.on("/update", HTTP_GET, [](AsyncWebServerRequest *request) {
    String inputMessage;
    String inputParam;
    String inputMessage2;
    String inputParam2;
    // GET input1 value on <ESP_IP>/update?relay=<inputMessage>
    if (request->hasParam(PARAM_INPUT_1) & request->hasParam(PARAM_INPUT_2)) {
      inputMessage = request->getParam(PARAM_INPUT_1)->value();
      inputParam = PARAM_INPUT_1;
      inputMessage2 = request->getParam(PARAM_INPUT_2)->value();
      inputParam2 = PARAM_INPUT_2;
      if (RELAY_NO) {
        debug("NO ");
        digitalWrite(relayGPIOs[inputMessage.toInt() - 1], !inputMessage2.toInt());
      } else {
        debug("NC ");
        digitalWrite(relayGPIOs[inputMessage.toInt() - 1], inputMessage2.toInt());
      }
    } else {
      inputMessage = "No message sent";
      inputParam = "none";
    }
    debugln(inputMessage + inputMessage2);
    request->send(200, "text/plain", "OK");
  });

  // Start server
  server.begin();

  client.setInsecure();
}

void loop() {

  //Read the state of the door switch
  state = digitalRead(reedSwitch);

  //Check if that state has changed and send a notification
  if (changeState) {
    unsigned long currentTELEMillis = millis();
    if (currentTELEMillis - previousTELEMillis >= TELEinterval) {
      previousTELEMillis = currentTELEMillis;
      // If a state has occured, invert the current door state
      state = !state;
      if (state) {
        doorState = "closed";
      } else {
        doorState = "open";
      }
      changeState = false;
      debugln(state);
      debugln(doorState);

      //Send notification
      bot.sendMessage(CHAT_ID, "The door is " + doorState, "");
    }
    yield();
  }


  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {

    // save the last time you updated the DHT values
    previousMillis = currentMillis;

    // Read temperature as Fahrenheit (isFahrenheit = true)
    float newT1 = dht1.readTemperature(true);
    float newT2 = dht2.readTemperature(true);

    // if temperature read failed, don't change t value
    if (isnan(newT1)) {
      debugln("Failed to read from DHT1 sensor!");
    } else {
      t1 = newT1;
      debugln(t1);
    }
    if (isnan(newT2)) {
      debugln("Failed to read from DHT2 sensor!");
    } else {
      t2 = newT2;
      debugln(t2);
    }

    // Read Humidity
    float newH1 = dht1.readHumidity();
    float newH2 = dht2.readHumidity();

    h1 = newH1;
    h2 = newH2;
    debugln(h1);
    debugln(h2);

    // clear display
    display.clearDisplay();
  }



  //Turn the top humidifier and fans off if the humidity is above 65%, the sensor fails, or the door is open
  //Else turn the humidifier on
  if (h1 > 64 || isnan(h1) || (h1) == 0 || doorState == "open") {
    mcp.digitalWrite(humi1, HIGH);
    mcp.digitalWrite(fan1, HIGH);
    debugln("TOP humidity is g2g or the door is open!");
    delay(2000);
  } else {
    mcp.digitalWrite(humi1, LOW);
    mcp.digitalWrite(fan1, LOW);
    debugln("TOP humidity low, beware the mist!");
    delay(2000);
  }

  //Turn the bottom humidifier and fans off if the humidity is above 65%, the sensor fails, or the door is open
  //Else turn the humidifier on
  if (h2 > 64 || isnan(h2) || (h2) == 0 || doorState == "open") {
    mcp.digitalWrite(humi2, HIGH);
    mcp.digitalWrite(fan2, HIGH);
    debugln("BOTTOM humidity is g2g or the door is open!");
    delay(2000);
  } else {
    mcp.digitalWrite(humi2, LOW);
    mcp.digitalWrite(fan2, LOW);
    debugln("BOTTOM humidity low, beware the mist!");
    delay(2000);
  }

  //Use LCD to tell people to shut the door if they opened it or print the Humidities
  if (doorState == "open") {
    display.setTextSize(2);
    display.setCursor(0, 0);
    display.print("Shut the");
    display.setTextSize(2);
    display.setCursor(0, 35);
    display.print("Damn Door!");
  } else {

    // display top humidity
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print("Humidity Top: ");
    display.setTextSize(2);
    display.setCursor(0, 10);
    display.print(h1);
    display.print(" %");

    // display bottom humidity
    display.setTextSize(1);
    display.setCursor(0, 35);
    display.print("Humidity Bottom: ");
    display.setTextSize(2);
    display.setCursor(0, 45);
    display.print(h2);
    display.print(" %");
  }

  display.display();

  unsigned long currentupdateMillis = millis();
  if (currentupdateMillis - previousupdateMillis >= updateInterval) {

    previousupdateMillis = currentupdateMillis;  // save the last time you updated the Google Sheets values

    //----------------------------------------Connect to Google host
    if (!client.connect(host2, httpsPort)) {
      debugln("connection failed");
      return;
    }

    //----------------------------------------Processing data and sending data

    String url = "/macros/s/" + GAS_ID + "/exec?temperature_top=" + (t1) + "&temperature_bottom=" + (t2) + "&humidity_top=" + (h1) + "&humidity_bottom=" + (h2);
    debug("requesting URL: ");
    debugln(url);

    client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host2 + "\r\n" + "User-Agent: BuildFailureDetectorESP32 \r\n" + "Connection: close\r\n\r\n");

    debugln("request sent");
    //----------------------------------------

    //----------------------------------------Checking whether the data was sent successfully or not
    while (client.connected()) {
      String line = client.readStringUntil('\n');
      if (line == "\r") {
        debugln("headers received");
        break;
      }
    }
    yield();
  }
}
