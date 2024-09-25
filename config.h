/*

Add all of your definitions and declarations here. 

*/



const char* host = "iot-web";
const char* ssid     = "TellMyWiFiLoveHer_EXT";
const char* password = "hahasucka!";

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define DHTPIN1   0    //D3 Digital pin connected to the DHT sensor for top sensor
#define DHTPIN2  14    //D5 Digital pin connected to the DHT sensor for bottom sensor
#define humi1 12       //D6 This is top humidifier
#define humi2 13       //D7 This is bottom humidifier
#define button 15      //D8 Button for humi controller 
#define fan1 9         //SD2 Pin for Top section
#define fan2 10        //SD3  Pin for Bottom section

// Uncomment the type of sensor in use:
#define DHTTYPE    DHT11     // DHT 11
//#define DHTTYPE    DHT22     // DHT 22 (AM2302)
//#define DHTTYPE    DHT21     // DHT 21 (AM2301)
