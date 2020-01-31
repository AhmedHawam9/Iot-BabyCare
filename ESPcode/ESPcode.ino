//Libraries
#include <ESP8266WiFi.h>  //the esp library for connecting to the wifi
//website
#include <ESPAsyncWebServer.h> //for sending the web files to the computer as a server
#include <FS.h> //File System for uploading web files to the esp
//sending/recieving data to/from Arduio
#include <ArduinoJson.h>  //for coding data as json when exchanging data with arduino
//Motor
#include <Servo.h>

//Global Variables
//Motor
bool crying = 0;
int shake = 2;
Servo myservo;    // create servo object to control a servo
int pos = 90;    // variable to store the servo position
bool goRight = 1; // determine the direction the servo motor should go
unsigned long servoTimeStamp = millis(); // used to determine when the servo should move
unsigned long stopTimeStamp;
//Arduino communication
DynamicJsonDocument doc(1024);
//Wifi connection
//const char* ssid     = "vast";
//const char* password = "detach33ed";
const char* ssid     = "7W7w98";
const char* password = "ahmed271998271998";
//Sensor values
String cryState = "quit good baby";
int temprature = 0;
int heartRate = 0;
//Webserver
WiFiServer server(80);  // Set web server port number to 80
String header;
unsigned long currentTime = millis(); // Current time
unsigned long previousTime = 0;   // Previous time
const long timeoutTime = 2000;  // Define timeout time in milliseconds (example: 2000ms = 2s)

void setup() {
  Serial.begin(9600);

  // Initialize the output variables as outputs
  pinMode(12, OUTPUT);
  myservo.attach(13);  // attaches the servo on pin 9 to the servo object

  // Connect to Wi-Fi network with SSID and password
  Serial.print("\nConnecting to ");
  Serial.print(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("\nWiFi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop(){

  cycleMotor();
  
  WiFiClient client = server.available();   // Listen for incoming clients
  if (client) {                             // If a new client connects,
    String currentLine = "";                // make a String to hold incoming data from the client
    currentTime = millis();
    previousTime = currentTime;
    while (client.connected() && currentTime - previousTime <= timeoutTime) { // loop while the client's connected
      
      cycleMotor();
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            
          // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
          // and a content-type so the client knows what's coming, then a blank line:
          client.println("HTTP/1.1 200 OK");
          client.println("Content-type:text/html");
          client.println("Connection: close"); //TODO 1
          client.println();
          
          if(!SPIFFS.begin()){
            Serial.println("An Error has occurred while mounting SPIFFS");
            return;
          }
        
          File file = SPIFFS.open("/control.html", "r");
          if(!file){
            Serial.println("Failed to open file for reading");
            return;
          }
          while(file.available()){
            cycleMotor();
            client.write(file.read());
          }
          file.close();

          requestArduino();
          while(!readArduinoResponse())
            cycleMotor();

          if (header.indexOf("GET /light/on") >= 0) {
            digitalWrite(12, HIGH);
          } else if (header.indexOf("GET /light/off") >= 0) {
            digitalWrite(12, LOW);
          } else if (header.indexOf("GET /shake/on") >= 0) {
            shake = 1;
            stopTimeStamp = millis();
          } else if (header.indexOf("GET /shake/off") >= 0) {
            shake = 0;
          }
          
          client.println("");
          client.println("<script>\n");
          client.println("document.getElementById(\"cry\").innerText = \"" + cryState + "\";\n");
          client.println("document.getElementById(\"temp\").innerText = \"Your child's temperature : " + (String)temprature + "\";\n");
          client.println("document.getElementById(\"heart\").innerText = \"Your child's heart rate : " + (String)heartRate + "\";\n");
          client.println("</script>");
          
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
  }
}

/////Motor////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void cycleMotor(){
  if(pos == 90 && 
  (shake == 0 || 
  (shake == 2 && crying == 0) || 
  (millis() - stopTimeStamp) > 60000)){
    goRight = 1;
    return;
  }
    
  if((millis() - servoTimeStamp) >= 15){
    pos += goRight?1:-1;
    myservo.write(pos);

    servoTimeStamp = millis();
    if(pos == 135 || pos == 45)
      goRight = !goRight;
  }
}

/////Communicating With Arduino////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void requestArduino(){
  // Send a JSON-formatted request with key "type" and value "request"
  // then parse the JSON-formatted response with keys "temprature", "cryState", and "heartRate"
  // Sending the request
  doc["type"] = "request";
  serializeJson(doc, Serial);
}
bool readArduinoResponse(){
  if(!Serial.available()) {
    return false;
  }
  
  // Reading the response
  String message = "";
  message = Serial.readString();  //consumes the system completely for 1 second for reading the message, and that explains the bed lag
  
  // Attempt to deserialize the JSON-formatted message
  DeserializationError error = deserializeJson(doc,message);
  if(error) {
    Serial.println("Errrrror");
    return false;
  }

  //extracting variables from json response
  temprature = doc["temprature"];
  heartRate = doc["heartRate"];
  crying = doc["cryState"];
  cryState = crying == 0?"your baby sounds quite":"your baby is crying";
  shake = 2;
  if(crying == 1)
    stopTimeStamp = millis();

  doc.clear();

  return true;
}
