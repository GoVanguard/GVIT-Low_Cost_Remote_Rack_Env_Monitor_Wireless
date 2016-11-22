
// ESP Based WiFI Library
#include <ESP8266WiFi.h>

// ESP Based Mutant Web/DNS Libraries - Will need to replace with standard libraries
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

// Micro SNMP Library - Will add this later after we test for stability
//#include "uSNMP.cpp"
//#include "uSNMP.h"

// DHT Analog Sensor Library
#include "DHT.h"

// SPI
#include <SPI.h>

// Serial Config
#define INT_SERIAL 115200
#define DEBUG

// Wireless Config
#define WIFI_SSID "GVIT-IOT"
#define WIFI_WPA_PSK "HNY332!*$mk"

// HTTP Server Config
#define HTTP_ENABLE true
#define HTTP_PORT 80

// JSON Server Config
#define JSON_ENABLE true
#define JSON_PORT 8080

// SNMP Config
#define SNMP_ENABLE false
#define SNMP_READ_COMMUNITY "public"
#define SNMP_READ_WRITE_COMMUNITY "private"
#define SNMP_TRAP_SINK "public"

//Sensor Config
  // SENSOR 0
  #define SENSOR0_ENABLED true
  #define SENSOR0_PIN 0
  #define SENSOR0_TYPE WATER
  #define SENSOR0_PARAM 400
  // SENSOR 1
  #define SENSOR1_ENABLED true
  #define SENSOR1_PIN 2
  #define SENSOR1_TYPE DHT11
  #define SENSOR1_PARAM 1000
  // SENSOR 2
  #define SENSOR2_ENABLED false
  #define SENSOR2_PIN -1
  #define SENSOR2_TYPE NONE
  #define SENSOR2_PARAM 0
  // SENSOR 3
  #define SENSOR3_ENABLED false
  #define SENSOR3_PIN -1
  #define SENSOR3_TYPE NONE
  #define SENSOR3_PARAM 0
  // SENSOR 4
  #define SENSOR4_ENABLED false
  #define SENSOR4_PIN -1
  #define SENSOR4_TYPE NONE
  #define SENSOR4_PARAM 0
  // SENSOR 5
  #define SENSOR5_ENABLED false
  #define SENSOR5_PIN -1
  #define SENSOR5_TYPE NONE
  #define SENSOR5_PARAM 0
  // SENSOR 6
  #define SENSOR6_ENABLED false
  #define SENSOR6_PIN -1
  #define SENSOR6_TYPE NONE
  #define SENSOR6_PARAM 0
  // SENSOR 7
  #define SENSOR7_ENABLED false
  #define SENSOR7_PIN -1
  #define SENSOR7_TYPE NONE
  #define SENSOR7_PARAM 0

// Working Variables
int INT_HUMID = 0;
int INT_TEMP_F = 0;
int INT_TEMP_C = 0;

// Setup Sensor0
#if SENSOR0_ENABLED == true
  #if SENSOR0_TYPE == DHT11
    DHT SENSOR0_OBJ(SENSOR0_PIN, SENSOR0_TYPE);
  #elif SENSOR0_TYPE == DHT22
    DHT SENSOR0_OBJ(SENSOR0_PIN, SENSOR0_TYPE);
  #elif SENSOR0_TYPE == WATER
    // Nothing for now
  #endif
#endif

// Setup Sensor1
#if SENSOR1_ENABLED == true
  #if SENSOR1_TYPE == DHT11
    DHT SENSOR1_OBJ(SENSOR1_PIN, SENSOR1_TYPE);
  #elif SENSOR1_TYPE == DHT22
    DHT SENSOR1_OBJ(SENSOR1_PIN, SENSOR1_TYPE);
  #elif SENSOR1_TYPE == WATER
    // Nothing for now
  #endif
#endif

// Initialize HTTP Server Object
ESP8266WebServer httpServer(HTTP_PORT);

// Initialize JSON Server Object
ESP8266WebServer jsonServer(JSON_PORT);

void readDHT(DHT SENSOR_Object, int READ_Delay) {
  delay(READ_Delay);
  int INT_HUMID_T = SENSOR_Object.readHumidity();
  int INT_TEMP_C_T = SENSOR_Object.readTemperature();
  int INT_TEMP_F_T = SENSOR_Object.readTemperature(true);

  if (isnan(INT_HUMID)) {
    Serial.println("Failed to read value from sensor");
    INT_HUMID = -1;
  }
  else
  {
    INT_HUMID = INT_HUMID_T;
  }

  if (isnan(INT_TEMP_C)) {
    Serial.println("Failed to read value from sensor");
    INT_TEMP_C = -1;
  }
  else
  {
    INT_TEMP_C = INT_TEMP_C_T;
  }

  if (isnan(INT_TEMP_F)) {
    Serial.println("Failed to read value from sensor");
    INT_TEMP_F = -1;
  }
  else
  {
    INT_TEMP_F = INT_TEMP_F_T;  
  }
}

boolean readWaterSensor(int SENSOR_TRIGGER, int SENSOR_PIN) {
  float senseVal = analogRead(SENSOR_PIN);
  //Serial.println(INT_WET);
  if (senseVal > SENSOR_TRIGGER)
  {
    Serial.println("WET");
    return true;
  }
  else
  {
    Serial.println("Not Wet");
    return false;
  } 
}

void setupHttpServer(void) {
  httpServer.on("/", [](){
    String message = "Sensors:\n\n";
    message += "Humidity: ";
    message += INT_HUMID;
    message += "\n";
    message += "Temperature *F: ";
    message += INT_TEMP_F;
    message += "\n";
    message += "Temperature *C: ";
    message += INT_TEMP_C;
    message += "\n";
    message += "'water':'true''}";
    httpServer.send(200, "text/plain", message);
  });

  httpServer.onNotFound([](){
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += httpServer.uri();
    message += "\nMethod: ";
    message += (httpServer.method() == HTTP_GET)?"GET":"POST";
    message += "\nArguments: ";
    message += httpServer.args();
    message += "\n";
    for (uint8_t i=0; i<httpServer.args(); i++){
      message += " " + httpServer.argName(i) + ": " + httpServer.arg(i) + "\n";
    }
    httpServer.send(404, "text/plain", message);
  });

  httpServer.begin();
  Serial.println("HTTP server started");
}

void setupJsonServer(void) {
  jsonServer.on("/", [](){
    String message = "{'sensors': ";
    message += "{'humidity': '";
    message += INT_HUMID;
    message += "',";
    message += "'temperature_f': '";
    message += INT_TEMP_F;
    message += "',";
    message += "'temperature_c': '";
    message += INT_TEMP_C;
    message += "',}";
    message += "'water':'true''}";
    jsonServer.send(200, "text/plain", message);
  });

  jsonServer.onNotFound([](){
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += jsonServer.uri();
    message += "\nMethod: ";
    message += (jsonServer.method() == HTTP_GET)?"GET":"POST";
    message += "\nArguments: ";
    message += jsonServer.args();
    message += "\n";
    for (uint8_t i=0; i<jsonServer.args(); i++){
      message += " " + jsonServer.argName(i) + ": " + jsonServer.arg(i) + "\n";
    }
    jsonServer.send(404, "text/plain", message);
  });

  jsonServer.begin();
  Serial.println("JSON server started");
}

void setup(void){
  // Initialize Serial
  Serial.begin(115200);

  // Initialize WiFi
  WiFi.begin(WIFI_SSID, WIFI_WPA_PSK);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(WIFI_SSID);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Initialize mDNS
  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }
  
  // Setup HTTP Server
  #if HTTP_ENABLE == true
    setupHttpServer();
  #endif

  // Setup JSON Server
  #if JSON_ENABLE == true
    setupJsonServer();
  #endif
}
  
void loop(void){
  // Check Sensor 0
  #if SENSOR0_ENABLED == true
    #if SENSOR0_TYPE == DHT11
      readDHT(SENSOR0_OBJ, SENSOR0_PARAM);
    #elif SENSOR0_TYPE == DHT22
      readDHT(SENSOR0_OBJ, SENSOR0_PARAM);
    #elif SENSOR0_TYPE == WATER
      readWaterSensor(SENSOR0_PIN, SENSOR0_PARAM);
    #endif
  #endif

  // Check Sensor 1
  #if SENSOR1_ENABLED == true
    #if SENSOR1_TYPE == DHT11
      readDHT(SENSOR1_OBJ, SENSOR1_PARAM);
    #elif SENSOR1_TYPE == DHT22
      readDHT(SENSOR1_OBJ, SENSOR1_PARAM);
    #elif SENSOR1_TYPE == WATER
      readWaterSensor(SENSOR1_PIN, SENSOR1_PARAM);
    #endif
  #endif

  // Process HTTP Requests
  #if HTTP_ENABLE == true
      httpServer.handleClient();
  #endif

  // Process JSON API Requests
  #if JSON_ENABLE == true
      jsonServer.handleClient();
  #endif
}
