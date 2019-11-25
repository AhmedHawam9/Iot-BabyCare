//Includes/Libraries
//Temprature
#include <OneWire.h>
#include <DallasTemperature.h>
//sending data to esp
#include <ArduinoJson.h>


//port definitions and constants
//Temprature
#define ONE_WIRE_BUS 2  // Data wire is plugged into port 2 on the Arduino
//Heart Beat
const int pulsePin = A0;  // Pulse Sensor purple wire connected to analog pin A0
//Cry
const int soundPin = A1;

//Global variables
//Temprature
double temp = 0;
OneWire oneWire(ONE_WIRE_BUS);  // Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
DallasTemperature sensors(&oneWire);  // Pass our oneWire reference to Dallas Temperature. 
//sending data to esp
String message = "";
bool messageReady = false;
//Heart Beat
int beat = 0;
// Volatile Variables, used in the interrupt service routine!
volatile int BPM;                   // int that holds raw Analog in 0. updated every 2mS
volatile int Signal;                // holds the incoming raw data
volatile int IBI = 600;             // int that holds the time interval between beats! Must be seeded! 
volatile boolean Pulse = false;     // "True" when User's live heartbeat is detected. "False" when not a "live beat". 
volatile boolean QS = false;        // becomes true when Arduoino finds a beat.
volatile int rate[10];                      // array to hold last ten IBI values
volatile unsigned long sampleCounter = 0;          // used to determine pulse timing
volatile unsigned long lastBeatTime = 0;           // used to find IBI
volatile int P = 512;                      // used to find peak in pulse wave, seeded
volatile int T = 512;                     // used to find trough in pulse wave, seeded
volatile int thresh = 525;                // used to find instant moment of heart beat, seeded
volatile int amp = 100;                   // used to hold amplitude of pulse waveform, seeded
volatile boolean firstBeat = true;        // used to seed rate array so we startup with reasonable BPM
volatile boolean secondBeat = false;      // used to seed rate array so we startup with reasonable BPM
//Cry
bool cry = 0;
int soundLevel = 0;

void setup(void)
{
  //Start serial port
  Serial.begin(9600);

  //Cry
  pinMode(soundPin, INPUT);
  pinMode(13, OUTPUT);

  //Temprature
  sensors.begin();//Start up the library

  //Heart Beat
  interruptSetup();                 // sets up to read Pulse Sensor signal every 2mS 
}
void loop(void)
{
  send2ESP();
}

/////Cry Detection/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void cryCheck()
{
  unsigned long timeStamp = millis();
  delay(100);
  while((millis() - timeStamp) < 5000){
    soundLevel = analogRead(soundPin);
    if(soundLevel > 44)
    {
      digitalWrite(13, HIGH);
      cry = 1;
      break;
    }
  }
}
/////Heart Beat////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void measureHeartBeat(){
  unsigned long timeStamp = millis();
  while(beat == 0 && (millis() - timeStamp) < 5000)
  {
    arduinoSerialMonitorVisual('-', Signal); 
    if (QS == true) // A Heartbeat Was Found
    {
      beat = BPM;
      QS = false; // reset the Quantified Self flag for next time    
    }
  }
}

void interruptSetup()
{     
  // Initializes Timer2 to throw an interrupt every 2mS.
  TCCR2A = 0x02;     // DISABLE PWM ON DIGITAL PINS 3 AND 11, AND GO INTO CTC MODE
  TCCR2B = 0x06;     // DON'T FORCE COMPARE, 256 PRESCALER 
  OCR2A = 0X7C;      // SET THE TOP OF THE COUNT TO 124 FOR 500Hz SAMPLE RATE
  TIMSK2 = 0x02;     // ENABLE INTERRUPT ON MATCH BETWEEN TIMER2 AND OCR2A
  sei();             // MAKE SURE GLOBAL INTERRUPTS ARE ENABLED      
} 

void arduinoSerialMonitorVisual(char symbol, int data)
{    
  const int sensorMin = 0;      // sensor minimum, discovered through experiment
  const int sensorMax = 1024;    // sensor maximum, discovered through experiment
  int sensorReading = data; // map the sensor range to a range of 12 options:
  int range = map(sensorReading, sensorMin, sensorMax, 0, 11);
  // do something different depending on the 
  // range value:
}



ISR(TIMER2_COMPA_vect) //triggered when Timer2 counts to 124
{  
  cli();                                      // disable interrupts while we do this
  Signal = analogRead(pulsePin);              // read the Pulse Sensor 
  sampleCounter += 2;                         // keep track of the time in mS with this variable
  int N = sampleCounter - lastBeatTime;       // monitor the time since the last beat to avoid noise
                                              //  find the peak and trough of the pulse wave
  if(Signal < thresh && N > (IBI/5)*3) // avoid dichrotic noise by waiting 3/5 of last IBI
    {      
      if (Signal < T) // T is the trough
      {                        
        T = Signal; // keep track of lowest point in pulse wave 
      }
    }

  if(Signal > thresh && Signal > P)
    {          // thresh condition helps avoid noise
      P = Signal;                             // P is the peak
    }                                        // keep track of highest point in pulse wave

  //  NOW IT'S TIME TO LOOK FOR THE HEART BEAT
  // signal surges up in value every time there is a pulse
  if (N > 250)
  {                                   // avoid high frequency noise
    if ( (Signal > thresh) && (Pulse == false) && (N > (IBI/5)*3) )
      {        
        Pulse = true;                               // set the Pulse flag when we think there is a pulse
        IBI = sampleCounter - lastBeatTime;         // measure time between beats in mS
        lastBeatTime = sampleCounter;               // keep track of time for next pulse
  
        if(secondBeat)
        {                        // if this is the second beat, if secondBeat == TRUE
          secondBeat = false;                  // clear secondBeat flag
          for(int i=0; i<=9; i++) // seed the running total to get a realisitic BPM at startup
          {             
            rate[i] = IBI;                      
          }
        }
  
        if(firstBeat) // if it's the first time we found a beat, if firstBeat == TRUE
        {                         
          firstBeat = false;                   // clear firstBeat flag
          secondBeat = true;                   // set the second beat flag
          sei();                               // enable interrupts again
          return;                              // IBI value is unreliable so discard it
        }   
      // keep a running total of the last 10 IBI values
      word runningTotal = 0;                  // clear the runningTotal variable    

      for(int i=0; i<=8; i++)
        {                // shift data in the rate array
          rate[i] = rate[i+1];                  // and drop the oldest IBI value 
          runningTotal += rate[i];              // add up the 9 oldest IBI values
        }

      rate[9] = IBI;                          // add the latest IBI to the rate array
      runningTotal += rate[9];                // add the latest IBI to runningTotal
      runningTotal /= 10;                     // average the last 10 IBI values 
      BPM = 60000/runningTotal;               // how many beats can fit into a minute? that's BPM!
      QS = true;                              // set Quantified Self flag 
      // QS FLAG IS NOT CLEARED INSIDE THIS ISR
    }                       
  }

  if (Signal < thresh && Pulse == true)
    {   // when the values are going down, the beat is over
      Pulse = false;                         // reset the Pulse flag so we can do it again
      amp = P - T;                           // get amplitude of the pulse wave
      thresh = amp/2 + T;                    // set thresh at 50% of the amplitude
      P = thresh;                            // reset these for next time
      T = thresh;
    }

  if (N > 2500)
    {                           // if 2.5 seconds go by without a beat
      thresh = 512;                          // set thresh default
      P = 512;                               // set P default
      T = 512;                               // set T default
      lastBeatTime = sampleCounter;          // bring the lastBeatTime up to date        
      firstBeat = true;                      // set these to avoid noise
      secondBeat = false;                    // when we get the heartbeat back
    }

  sei();                                   // enable interrupts when youre done!
}// end isr


/////Temprature//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void measureTemprature(){
  sensors.requestTemperatures(); // Send the command to get temperatures
  temp = sensors.getTempCByIndex(0);
}

/////Sending Data To ESP/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void send2ESP(){
  while(Serial.available()) { // Monitor serial communication
    message = Serial.readString();
    messageReady = true;
  }
  // Only process message if there's one
  if(messageReady) {
    // The only messages we'll parse will be formatted in JSON
    DynamicJsonDocument doc(1024); // ArduinoJson version 6+
    // Attempt to deserialize the message
    DeserializationError error = deserializeJson(doc,message);
    if(error) {
      messageReady = false;
      return;
    }
    if(doc["type"] == "request") {
      doc["type"] = "response";
      // Get data from analog sensors

      measureTemprature();
      cryCheck();
      measureHeartBeat();

      doc["temprature"] = temp;
      doc["cryState"] = cry;
      doc["heartRate"] = beat;
      
      serializeJson(doc, Serial);
    }
    messageReady = false;
  }
}
//function index
//cryCheck(); the mic listens for 5 seconds for any noise, if the noise level is above a determinted threshold it'll know that the baby is crying
//measureHeartBeat(); going into a loop that doesn't end before detecting a heartbeat
//measureTemprature(); to get the current temprature and store it in it's global variable (temp)
//send2ESP(); to send the three global variables (temp, cry, and beat) to the ESP over serial communication
