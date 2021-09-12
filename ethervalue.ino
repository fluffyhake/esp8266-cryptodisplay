#include <ESP8266HTTPClient.h> // For sending HTTP requests
#include <ESP8266WiFi.h> // For connecting to WiFi
#include <ArduinoJson.h> // For parsing json
#include <Arduino_JSON.h>


// Display stuff

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
// endof display stuff
  

StaticJsonDocument<2000> doc;
float payloadArr[3];
WiFiClientSecure wifiClient;

const char* ssid = "xxx";
const char* password = "xxx";


void setup() {
 // JSON STUFF
 // NON JSON STUFF
  // put your setup code here, to run once:
  Serial.begin(115200);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("Connecting to WiFi");
    delay(1000);
    Serial.print("\n");
    Serial.print("Connecting to SSID: ");
    Serial.print(ssid);
    Serial.print("\n");
    Serial.print("Connecting with password: ");
    Serial.print(password);
    Serial.print("\n");
    Serial.print("Current status from WiFi.status(): ");
    Serial.print(WiFi.status());
  }
  Serial.println();
  Serial.println();
  Serial.println();

  Serial.println("Current IP-address on this LAN: ");
  Serial.print(WiFi.localIP());

// Display stuff
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();

  // Draw a single pixel in white
  display.drawPixel(10, 10, SSD1306_WHITE);

  // Show the display buffer on the screen. You MUST call display() after
  // drawing commands to make them visible on screen!
  display.display();

  display.clearDisplay();
  display.display();


// endof display stuff
  
}

void loop() {
 
  // put your main code her, to run repeatedly:
  const char* host = "https://api.bittrex.com/v3/markets/ETH-USD/ticker";
  const uint16_t httpsPort = 443;
  //Check if the wifi connection is up
  if(WiFi.status() == WL_CONNECTED) {

    HTTPClient http; // Declare HTTPClient as a variable???

    wifiClient.setInsecure(); //the magic line, use with caution
    wifiClient.connect(host, httpsPort);

    http.begin(wifiClient, host); //Specifies the request destination
    int httpCode = http.GET(); //Send the HTTP Request and bind it to the int httpCode?
  
    String payload = "{}"; 
//    Serial.print(httpCode);
    if (httpCode > 0) { // Check if we get a HTTP return code
      payload = http.getString(); // Get the request response and assign it to the string "payload"
//      Serial.println();
//      Serial.println(payload);  //Print the response payload
//
//      Serial.println();

      delay(1000);
      //{"symbol":"ETH-USD","lastTradeRate":"3326.55900000","bidRate":"3327.50200000","askRate":"3329.20100000"}
      http.end(); //Close the HTTP connection  
// JSON STUFF :(

      JSONVar myObject = JSON.parse(payload);
      
      // JSON.typeof(jsonVar) can be used to get the type of the var
      if (JSON.typeof(myObject) == "undefined") {
        Serial.println("Parsing input failed!");
        return;
      }
      
//      Serial.print("JSON object = ");
//      Serial.println(myObject);
      
      // myObject.keys() can be used to get an array of all the keys in the object
      JSONVar keys = myObject.keys();
      
//      for (int i = 0; i < keys.length(); i++) {
//        JSONVar value = myObject[keys[i]];
//        Serial.print(keys[i]);
//        Serial.print(" = ");
//        Serial.println(value);
//        payloadArr[i] = double(value);
//      }
      
      JSONVar value = myObject[keys[1]];
      String myvalue = JSON.stringify(value);
      // makes the value from key 1 into a string called myvalue
      //it now looks like this "3326.55900000"
      
//      Serial.println(value);
//      Serial.println(myvalue);

      myvalue.remove(0,1);
// now looks like this      3329.91100000"
      myvalue.remove(10,4);
// now looks like this 3324.39100
      Serial.println(myvalue);

      display.clearDisplay();
      display.setCursor(1,0);
      display.setTextSize(2);             // Draw 2X-scale text
      display.setTextColor(SSD1306_WHITE);
      display.print(myvalue);
    
      display.display();
      delay(200);

  }else{
    Serial.print("NO VALID HTTP RESPONSE!!");
      display.clearDisplay();
      display.setCursor(1,0);
      display.setTextSize(2);             // Draw 2X-scale text
      display.setTextColor(SSD1306_WHITE);
      display.print(" NO VALID HTTP RESPONSE :( ");
    
      display.display();
      delay(200);
    }


  }
  delay(1000); //Wait 1s before sending a new request
}
