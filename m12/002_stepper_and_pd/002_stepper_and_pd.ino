/* M1.2 sketch 1.0 // experiment 002

    Code to listen to pd, and transmitting back to a software serial for debugging
    Rotates steppermotor accordingly in listening for spikes


    Thomas Kaufmanas, Eindhoven 2024

*/

//Magical variables for listening to pure data
int inByte = 0;
char buffer[40];
int index = 0;
int value = 0; //RAW LATEST READING <---

//Setup debug-to-terminal serial port (via U-ART)
#include<SoftwareSerial.h>
SoftwareSerial SUART(2, 3);

//Stepper motor stuff
#define stepPin 10
#define directionPin 11
int actualPos = 0;
int virtualPos = 0;

//Internal Variables
int spikes = 0;
int prevSpikes = 0;
long timer = 0;
long timer2 = 0;

//Adjustable Variables
int volThreshold = 12;
int interval = 9000;
int interval2 = 500;
int stepSpeed = 500; //in microseconds



void setup() {
  Serial.begin(9600); //HW serial for listening to PD part
  SUART.begin (115200); //SW serial for debugging

  pinMode(stepPin, OUTPUT);
  pinMode(directionPin, OUTPUT);
  SUART.println("HI!__");
  for (int x = 0; x < 10; x++) {
    SUART.println(" ");
  }


}

void loop() {

  // MAGIC!! NO CLUE HOW WORKS DONT FUCKING TOUCH-----------
  index = 0;                                       //|
  do                                               //|
  {
    buffer[index] = Serial.read();                 //|
    if (buffer[index] != -1) index = index + 1;    //|
  } while (buffer[index - 1] != 32);               //|
  value = atoi(buffer);                            //|
  //SUART.println(value); //debug raw data
  // End of magic ----------------------------------------


  if (value > volThreshold) {
    spikes += 1;
    SUART.print("!");
  }




  digitalWrite(directionPin, HIGH);
  for (int p = 0; p < 10; p++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(stepSpeed);    // by changing this time delay between the steps we can change the rotation speed
    digitalWrite(stepPin, LOW);
  }
  
  delay(1000);
  digitalWrite(directionPin, LOW);
  
  for (int q = 0; q <10; q++){
  digitalWrite(stepPin, HIGH);
  delayMicroseconds(stepSpeed);    // by changing this time delay between the steps we can change the rotation speed
  digitalWrite(stepPin, LOW);
  }






  delay(1);
} //end of void
