
// Import required libraries

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
//#include <rExcel.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "config.h"


DHT dht1(DHTPIN1, DHTTYPE);
DHT dht2(DHTPIN2, DHTTYPE);

// current temperature & humidity, updated in loop()
float t1 = 0.0;
float h1 = 0.0;
float t2 = 0.0;
float h2 = 0.0;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0;    // will store last time DHT was updated

// Updates DHT readings every 10 seconds
const long interval = 4000;  

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
      padding-bottom: 15px;
    }
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
</script>
</html>)rawliteral";

// Replaces placeholder with DHT values
String processor(const String& var){
  //Serial.println(var);
  if(var == "TEMPERATURE1"){
    return String(t1);
  }
  else if(var == "HUMIDITY1"){
    return String(h1);
  }
  else if(var == "TEMPERATURE2"){
    return String(t2);
  }
  else if(var == "HUMIDITY2"){
    return String(h2);
  }
  return String();
}

void setup(){
  
  // Serial port for debugging purposes
  Serial.begin(115200);
  dht1.begin();
  dht2.begin();
  pinMode(humi1,OUTPUT);
  pinMode(humi2,OUTPUT);
  pinMode(button,INPUT);
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting...");
  }

    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(2000);
  display.clearDisplay();
  display.setTextColor(WHITE);

  // Print ESP8266 Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });
  server.on("/temperature1", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(t1).c_str());
  });
  server.on("/humidity1", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(h1).c_str());
  });
    server.on("/temperature2", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(t2).c_str());
  });
  server.on("/humidity2", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(h2).c_str());
  });
  
  // Start server
  server.begin();
}
 
void loop(){  


  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    
    // save the last time you updated the DHT values
    previousMillis = currentMillis;


    // Read temperature as Celsius (the default)
    //float newT1 = dht1.readTemperature();
    //float newT2 = dht2.readTemperature();
  
    // Read temperature as Fahrenheit (isFahrenheit = true)
    float newT1 = dht1.readTemperature(true);
    float newT2 = dht2.readTemperature(true);
    
    // if temperature read failed, don't change t value
    if (isnan(newT1)) {
      Serial.println("Failed to read from DHT1 sensor!");
    }
    else {
      t1 = newT1;
      Serial.println(t1);
    }
    if (isnan(newT2)) {
      Serial.println("Failed to read from DHT2 sensor!");
    }
    else {
      t2 = newT2;
      Serial.println(t2);
    }
  
    // Read Humidity
    float newH1 = dht1.readHumidity();
    float newH2 = dht2.readHumidity();

      h1 = newH1;
      h2 = newH2;
      Serial.println(h1);
      Serial.println(h2);
    

      // clear display
  display.clearDisplay();

    }

//Turn the top humidifier off if the humidity is above 65% or the sensor fails
//Else turn the humidifier on
if(h1>65 || isnan(h1) || (h1) == 0){
  digitalWrite(humi1,HIGH);
  Serial.println("TOP humidity is g2g!");
  delay(5000);
}else{
  digitalWrite(humi1,LOW);
  Serial.println("TOP humidity low, beware the mist!");
  delay(5000);
}

//Turn the bottom humidifier off if the humidity is above 65% or the sensor fails
//Else turn the humidifier on
if(h2>65 || isnan(h2) || (h2) == 0){
  digitalWrite(humi2,HIGH);
  Serial.println("BOTTOM humidity is g2g!");
  delay(5000);
}else{
  digitalWrite(humi2,LOW);
  Serial.println("BOTTOM humidity low, beware the mist!");
  delay(5000);
}





  // display humidity
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Humidity Top: ");
  display.setTextSize(2);
  display.setCursor(0, 10);
  display.print(h1);
  display.print(" %"); 

 
  // display humidity
  display.setTextSize(1);
  display.setCursor(0, 35);
  display.print("Humidity Bottom: ");
  display.setTextSize(2);
  display.setCursor(0, 45);
  display.print(h2);
  display.print(" %"); 
  
  
  display.display(); 


}
