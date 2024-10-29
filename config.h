/*

Add all of your definitions and declarations here. 
For the reed switch -> D7 to NC, GRND to COM on board!


*/
const char* host = "iot-web";
const char* ssid = "YOUR WIFI SSID";
const char* password = "YOUR WIFI PASSWORD";
const char* host2 = "script.google.com";
const int httpsPort = 443;
Adafruit_MCP23X17 mcp;

WiFiClientSecure client;  //--> Create a WiFiClientSecure object.

String GAS_ID = "AKfycbwxnClSyi8TkCkuYXStro811g-gblgZ3SsAkBxkKizL9iFLdZaDgJqPaUyoO1EtcRJw";  //--> spreadsheet script ID

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define DHTPIN1 10  //SD3 Digital pin connected to the DHT sensor for top sensor
#define DHTPIN2 9   //SD2 Digital pin connected to the DHT sensor for bottom sensor
#define humi1 0     //MCP pin A0, Top humidifier
#define humi2 1     //MCP pin A1, bottom humidifier
#define fan1 2      //MCP pin A2
#define fan2 3      //MCP pin A3


//Turn off serial.println. 0 = Off, 1 = On
//#define DEBUG 0
#define DEBUG 1

//These lines will turn of serial printing of errors for final code to make it smaller and faster to load
#if DEBUG == 1
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)
#else
#define debug(x)
#define debugln(x)
#endif

// Set GPIO for reedswitch
const int reedSwitch = 13;  // D7 on NodeMCU. Couldn't get the MCP to work with this interrupt.


// Set number of relays
#define NUM_RELAYS 2

// Assign each GPIO to a relay for the fans to run without the humidifier
int relayGPIOs[NUM_RELAYS] = { 12, 14 };  //12 is D6, 14 is D5

// Uncomment the type of sensor in use:
//#define DHTTYPE DHT11  // DHT 11
#define DHTTYPE DHT22  // DHT 22 (AM2302)
//#define DHTTYPE DHT21 // DHT 21 (AM2301)
