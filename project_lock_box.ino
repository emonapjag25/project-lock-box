int const RX_PIN = 3; //this is the RX pin, this receives the bluetooh
int const TX_PIN = 2; //this is the TX pin, the transmits
#include <SoftwareSerial.h>
#define REDPIN 5
#define YELLOWPIN 6
#define GREENPIN 7
//tell code to use servo library
# include <Servo.h>
# define DOOR_PIN 1
//make a servo obect

SoftwareSerial safe(TX_PIN, RX_PIN); //makes a bluetooth object
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
  door.write(0); //set start of propeller to 90 degrees
}

void loop() {
  // put your main code here, to run repeatedly:
  int alarm = 2;
  Serial.print(alarm);
  Serial.println(safe.available());
  if (safe.available()>0){
    lock = safe.read();
    safe.print("reading new input: ");
    safe.print(lock);
    safe.print(safe);
  }
    if(lock == 'o' && alarm==2){//when the safe opens
    digitalWrite(GREENPIN, HIGH);
    digitalWrite(YELLOWPIN, LOW);
    digitalWrite(REDPIN, LOW);
    door.write(90);
    delay(100);
    alarm = alarm-2;
  } else if(lock == 'c' && alarm == alarm-2){//when the safe closes
    digitalWrite(YELLOWPIN, HIGH);
    delay(1000);
    digitalWrite(REDPIN, LOW);
    digitalWrite(GREENPIN, LOW);
    door.write(0);
    alarm = alarm +2;
  } else if (lock != 'o' && lock != 'c' && safe.available() != 0 && alarm==2){//when the alarm goes off
    alarm = alarm-1;
  } else if (alarm==2){ //when nothing is being inputted
    digitalWrite(GREENPIN, LOW);
    digitalWrite(REDPIN, LOW);
    digitalWrite(YELLOWPIN, HIGH);
    delay(500);
    digitalWrite(YELLOWPIN, LOW);
    delay(500);
  // } else if (alarm ==1 && lock =='stop alarm'){//stop the alarm
  //   alarm=2;
  } else if (safe.available() > 0 && alarm==alarm-1){
    Serial.println("locked");
    digitalWrite(REDPIN, HIGH);
    delay(1000);
  }
  delay(100);
}