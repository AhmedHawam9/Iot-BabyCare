#include <ESP8266WiFi.h>
#include <FS.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
//motor
#include <Servo.h>


String cryState = "quit good baby";
int temprature = 0;
int heartRate = 0;
//motor
bool active = 0;
Servo myservo;    // create servo object to control a servo
int pos = 0;    // variable to store the servo position

const char* ssid     = "7W7w98";
const char* password = "ahmed271998271998";

WiFiServer server(80);  // Set web server port number to 80

String header;

unsigned long currentTime = millis(); // Current time
unsigned long previousTime = 0;   // Previous time
const long timeoutTime = 2000;  // Define timeout time in milliseconds (example: 2000ms = 2s)

void setup() {
  Serial.begin(9600);

  // Initialize the output variables as outputs
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);

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

  if(active == 1)
    {cycleMotor();}
  
  WiFiClient client = server.available();   // Listen for incoming clients
  if (client) {                             // If a new client connects,
    String currentLine = "";                // make a String to hold incoming data from the client
    currentTime = millis();
    previousTime = currentTime;
    while (client.connected() && currentTime - previousTime <= timeoutTime) { // loop while the client's connected
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

          if (header.indexOf("GET /light/on") >= 0) {
            digitalWrite(12, HIGH);
          } else if (header.indexOf("GET /light/off") >= 0) {
            digitalWrite(12, LOW);
          } else if (header.indexOf("GET /shake/on") >= 0) {
            active = 1;
          } else if (header.indexOf("GET /shake/off") >= 0) {
            active = 0;
          }
        
          File file = SPIFFS.open("/control.html", "r");
          if(!file){
            Serial.println("Failed to open file for reading");
            return;
          }
          while(file.available()){
            client.write(file.read());
          }
          file.close();


          // Send a JSON-formatted request with key "type" and value "request"
          // then parse the JSON-formatted response with keys "temprature", "cryState", and "heartRate"
          DynamicJsonDocument doc(1024);
          
          // Sending the request
          doc["type"] = "request";
          serializeJson(doc, Serial);
          
          // Reading the response
          boolean messageReady = false;
          String message = "";
          while(messageReady == false) { // blocking but that's ok
            if(Serial.available()) {
              message = Serial.readString();
              messageReady = true;
            }
          }
          // Attempt to deserialize the JSON-formatted message
          DeserializationError error = deserializeJson(doc,message);
          if(error) {
            Serial.println("Errrrror");
            return;
          }
          
          temprature = doc["temprature"];
          int crying = doc["cryState"];
          heartRate = doc["heartRate"];
        
          cryState = crying == 0?"your baby sounds quite":"your baby is crying";
          active = crying == 0?0:1;
          


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

void cycleMotor(){
  for (pos = 0; pos <= 90; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  for (pos = 90; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
}
