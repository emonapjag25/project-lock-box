int const RX_PIN = 3; //this is the RX pin, this receives the bluetooh
int const TX_PIN = 2; //this is the TX pin, this transmits the bluetooth

#define TRIGGERPIN 4 //pin that sends out the pulse, for ultrasonic sensor
#define ECHOPIN 8 //pin that reads the distance, for ultrasonic sensor
#define REDPIN 5
#define YELLOWPIN 6
#define GREENPIN 7
#define RST_PIN 9 //for RFID       
#define SS_PIN 10 //for RFID        
#define DOOR_PIN A5 //make a servo obect

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <SPI.h> //for RFID
#include <MFRC522.h>//for RFID
#include <Servo.h> //tell code to use servo library
SoftwareSerial safe(TX_PIN, RX_PIN); //makes a bluetooth object
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

//set tx and rx pins
//tx goes first, then rx
char lock;
Servo door;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  safe.begin(9600);
  pinMode(REDPIN, OUTPUT);
  pinMode(YELLOWPIN, OUTPUT);
  pinMode(GREENPIN, OUTPUT);
  door.attach(DOOR_PIN); //connecting the servo object to the pin
  door.write(140); //set start of propeller to 140 degrees
  pinMode(TRIGGERPIN, OUTPUT); //sends pulse
  pinMode(ECHOPIN, INPUT); //reads pulse
  while (!Serial);		// Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
	SPI.begin();			// Init SPI bus
	mfrc522.PCD_Init();		// Init MFRC522
	delay(4);				// Optional delay. Some board do need more time after init to be ready, see Readme
	mfrc522.PCD_DumpVersionToSerial();	// Show details of PCD - MFRC522 Card Reader details
	Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));
}//end of void setup

void loop() {
  int alarm;
  int mycard = 0;
  digitalWrite(TRIGGERPIN, LOW); //turn trigger pin off to give clean starting point
  delayMicroseconds(2);//really fast delay
  digitalWrite(TRIGGERPIN, HIGH); //turn trigger pin on
  delayMicroseconds(10);//10 millisecond delay so that the microsensor can read the pulse before the next one
  //make the ECHHOPIN results a float so that it's more precise
  float duration = pulseIn(ECHOPIN, HIGH); //tells the time from pulse sent to pulse received
  // Serial.println(duration); //prints out the duration

  //distance = speed * duration
  float speed = 0.034; //measured in cm/microseconds
  float distance = (speed * duration)/2; //measured in cm, divided by two to account for the sensor pulse forward and back
  Serial.println(distance);
  //added RFID code
  byte authorizedUID[] = {0x73, 0x87, 0x53, 0xF5}; // my UID
  byte uidLength = 4; // Length of the UID
	if (mfrc522.uid.size == uidLength) {
  boolean authorized = true;
  for (byte i = 0; i < uidLength; i++) {
    if (mfrc522.uid.uidByte[i] != authorizedUID[i]||mycard==2) {
      authorized = false;
      break;
      mycard=0;
    }
  }//end of for loop
  if (authorized) {
    mycard=1;
    Serial.println("Access granted.");
    delay(2000);
  } 
	} //added RFID code
  if (safe.available()>0){
    lock = safe.read();
    safe.print("reading new input: ");
    safe.print(lock);
    safe.print(safe);
  }
    if(lock == 'o'|| mycard==1){//when the safe opens
    // Serial.println("Safe open");
    door.write(0);
    digitalWrite(GREENPIN, HIGH);
    digitalWrite(YELLOWPIN, LOW);
    digitalWrite(REDPIN, LOW);
    delay(1000);
    mycard=2;
    
  } else if(lock == 'c'){//when the safe closes
   mycard =0;
    door.write(140);
    // Serial.println("Safe close");    
    digitalWrite(GREENPIN, LOW);
    digitalWrite(REDPIN, LOW);
    digitalWrite(YELLOWPIN, HIGH);
    delay(500);
    digitalWrite(YELLOWPIN, LOW);
    delay(500);
  } 
  if (distance < 10){//when you put in the wrong password
    door.write(140);
    Serial.println("locked");
    digitalWrite(YELLOWPIN, LOW);
    digitalWrite(REDPIN, HIGH);
    delay(1000);
    digitalWrite(REDPIN, LOW);
    delay(1000);
  } 
  else { //when nothing is being inputted
    // door.write(140);
    digitalWrite(GREENPIN, LOW);
    digitalWrite(REDPIN, LOW);
    digitalWrite(YELLOWPIN, HIGH);
    delay(500);
    digitalWrite(YELLOWPIN, LOW);
    delay(500);
    // door.write(0);
  }
  	// Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
	if ( ! mfrc522.PICC_IsNewCardPresent()) {
		return;
	}

	// Select one of the cards
	if ( ! mfrc522.PICC_ReadCardSerial()) {
		return;
	}


	// Dump debug info about the card; PICC_HaltA() is automatically called
	// mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
  MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);


  Serial.print(F("RFID Tag UID:"));

  printHex(mfrc522.uid.uidByte, mfrc522.uid.size);

  Serial.println("");


  mfrc522.PICC_HaltA(); // Halt PICC

}//end of void loop

void printHex(byte *buffer, byte bufferSize) {

 //Serial.begin("reading?");

  for (byte i = 0; i < bufferSize; i++) {
  Serial.print(buffer[i] < 0x10 ? " 0" : " ");
  Serial.print(buffer[i], HEX);
    delay(100);
  }
}