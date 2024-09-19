
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define DHT1PIN 0      //D3 Digital pin connected to the DHT sensor for top sensor
#define DHT2PIN 14     //D5 Digital pin connected to the DHT sensor for bottom sensor
#define humi1 12       //D6 This is top humidifier
#define humi2 13       //D7 This is bottom humidifier
#define button 15      //D8 Button for humi controller  

// Uncomment the type of sensor in use:
#define DHTTYPE    DHT11     // DHT 11
//#define DHTTYPE    DHT22     // DHT 22 (AM2302)
//#define DHTTYPE    DHT21     // DHT 21 (AM2301)

DHT dht1(DHT1PIN, DHTTYPE);
DHT dht2(DHT2PIN, DHTTYPE);

void setup() {
  Serial.begin(9600);

  dht1.begin();
  dht2.begin();
  pinMode(humi1,OUTPUT);
  pinMode(humi2,OUTPUT);
  pinMode(button,INPUT);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(2000);
  display.clearDisplay();
  display.setTextColor(WHITE);
}

void loop() {
  delay(5000);

  //read temperature and humidity
  float t1 = dht1.readTemperature();
  float h1 = dht1.readHumidity();
  if (isnan(h1)) {
    Serial.println("Failed to read from DHT1 sensor!");
  }
  // clear display
  display.clearDisplay();

    //read temperature and humidity
  float t2 = dht2.readTemperature();
  float h2 = dht2.readHumidity();
  if (isnan(h2)) {
    Serial.println("Failed to read from DHT2 sensor!");
  }
  // clear display
  display.clearDisplay();
  
if(h1<64){
  digitalWrite(humi1,LOW);
  Serial.println("TOP humidity low, beware the mist!");
  delay(5000);
}else
{
  digitalWrite(humi1,HIGH);
  Serial.println("TOP humidity is g2g!");
  delay(5000);
}

if(h2<64){
  digitalWrite(humi2,LOW);
  Serial.println("BOTTOM humidity low, beware the mist!");
  delay(5000);
}else
{
  digitalWrite(humi2,HIGH);
  Serial.println("BOTTOM humidity is g2g!");
  delay(5000);

}

  // display humidity
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Humidity 1: ");
  display.setTextSize(2);
  display.setCursor(0, 10);
  display.print(h1);
  display.print(" %"); 

 
  // display humidity
  display.setTextSize(1);
  display.setCursor(0, 35);
  display.print("Humidity 2: ");
  display.setTextSize(2);
  display.setCursor(0, 45);
  display.print(h2);
  display.print(" %"); 
  
  
  display.display(); 
}
