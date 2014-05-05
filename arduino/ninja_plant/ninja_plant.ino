

#include <Ninja.h>
#include <DHT22.h>
#include <I2C.h>
#include <aJSON.h>
#include <MMA8453Q.h>
#include <RCSwitch.h>
#include <Wire.h>
#include <NinjaPi.h>

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include <SoftwareSerial.h>      //we have to include the SoftwareSerial library, or else we can't use it.  
#define rx 2                     //define what pin rx is going to be.
#define tx 13                     //define what pin Tx is going to be.
SoftwareSerial myserial(rx, tx); //define how the soft serial port is going to work. 
char ph_data[20];                  //we make a 20 byte character array to hold incoming data from the pH. 
byte received_from_sensor=0;       //we need to know how many characters have been received.
float ph=0;  
const int TIMEDELAY = 5000;
byte string_received=0;
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#define DEVICES_MAX          10
Device *devices[DEVICES_MAX + 1]; //null terminated
static void updateFromCloud(Device *pMyDevice);
double myDATA = 0;
const int INPUT_LEN = 20;
char strData[INPUT_LEN+1];
unsigned long lastRead = millis();

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~



//most important thing to change here is the device ID, list is here: http://ninjablocks.com/pages/device-ids   ph=226, text=240
Device myDevice = {"0", 0, 226
    , strData, INPUT_LEN, myDATA, false
    , &updateFromCloud
};              


//struct Device {
//	char * strGUID;
//	const int intVID;
//	const int intDID;
//	char * strDATA;
//	const int strDATALenMax;
//	double intDATA;
//	const bool IsString;
//	void (*didUpdate)(Device*);
//};





static void updateFromCloud(Device *pMyDevice) {
  Serial.println("update from cloud received");
}

void initDevices() {
  myserial.begin(38400);       //enable the software serial port
  delay(50);
  myserial.print("e\r");       // put in continuous update mode (twice for good measure)              
  delay(50);
  myserial.print("e\r");  
  delay(50);      
  
  for (int i=0; i<DEVICES_MAX + 1; i++) {
    devices[i] = (Device*)0;
  }
 
  devices[0] = &myDevice;
  updateFromCloud(&myDevice);
}



void initDefaultPins() {
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(BLUE_LED_PIN, OUTPUT);
  digitalWrite(RED_LED_PIN, HIGH);            // set the RED LED  Off
  digitalWrite(GREEN_LED_PIN, HIGH);          // set the GREEN LED Off
  digitalWrite(BLUE_LED_PIN, HIGH);           // set the BLUE LED Off
  pinMode(RED_STAT_LED_PIN, OUTPUT);
  pinMode(GREEN_STAT_LED_PIN, OUTPUT);
  pinMode(BLUE_STAT_LED_PIN, OUTPUT);
  digitalWrite(RED_STAT_LED_PIN, HIGH);		// set the RED STAT LED  Off
  digitalWrite(GREEN_STAT_LED_PIN, HIGH);	        // set the GREEN STAT LED Off
  digitalWrite(BLUE_STAT_LED_PIN, LOW);	        // Power on Status
}

void setup()
{
  Serial.begin(9600);
  initDefaultPins();
  initDevices();
  nOBJECTS.connectDevices(devices);
}


void loop()
{
  unsigned long timeNow = millis();
  if(timeNow >= lastRead + TIMEDELAY) {
   myserial.print("R\r");             
//   Serial.print("taking reading... ");
   received_from_sensor=myserial.readBytesUntil(13,ph_data,20); //we read the data sent from pH Circuit until we see a <CR>. We also count how many character have been received.  
   ph_data[received_from_sensor]=0; 
//   Serial.print("received: ");
//   Serial.println(ph_data);
   ph=atof(ph_data);
   myDevice.intDATA = ph;
//myDevice.intDATA = 9;
   lastRead = timeNow;
  }


    
    nOBJECTS.sendObjects();				// Send Ninja Objects 
    nOBJECTS.doReactors();				// Receive Ninja Objects reactors  
  }

