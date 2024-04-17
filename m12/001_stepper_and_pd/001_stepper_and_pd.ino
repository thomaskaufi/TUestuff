/* M1.2 sketch 2.0 // experiment 001

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

  if (millis() - timer > interval) { //FIRST TIMER ----------------------
    SUART.println(" ");
    SUART.print("End of reading!   ");
    SUART.print(spikes);
    SUART.print(" spikes were recorded, so...");

    if (spikes > prevSpikes) {   // Tighten the physicalisation
      if (virtualPos < 200) {
        SUART.print("Adding steps");
        virtualPos += 12;
      }
      else {
        SUART.print("Resting at max!!");
      }

    }
    if (spikes < prevSpikes || spikes == 0) {   // Loosen the physicalisation
      if (virtualPos > 0) {
        SUART.print("Subtracting steps");
        virtualPos -= 12;
      }
      else {
        SUART.print("Resting at min");
      }
    }
    SUART.print("    Virtual Position: ");
    SUART.print(virtualPos);
    SUART.print("     Actual Position: ");
    SUART.println(actualPos);
    SUART.println("  ");
    SUART.println("  ");

    //Update previous counter and next
    prevSpikes = spikes;
    spikes = 0;
    timer = millis(); // reset the timer
  } //End of FIRST TIMER -----------------------------------------------------


  if (millis() - timer2 > interval2) { // SECOND TIMER ----------
    if (actualPos != virtualPos) {
      //PULL ENABLE PIN TO LOW?!

      if (actualPos < virtualPos) {
        digitalWrite(directionPin, HIGH);

        digitalWrite(stepPin, HIGH);
        delayMicroseconds(stepSpeed);    // by changing this time delay between the steps we can change the rotation speed
        digitalWrite(stepPin, LOW);
        actualPos += 1;
      }
      if (actualPos > virtualPos) {
        digitalWrite(directionPin, LOW);

        digitalWrite(stepPin, HIGH);
        delayMicroseconds(stepSpeed);    // by changing this time delay between the steps we can change the rotation speed
        digitalWrite(stepPin, LOW);
        actualPos -= 1;
      }

    }
    timer2 = millis(); // reset the timer
  } // End of SECOND TIMER


  delay(1);
} //end of void





/*
  digitalWrite(directionPin, HIGH);
  for (int x = 0; x < 50; x++) {
  digitalWrite(stepPin, HIGH);
  delayMicroseconds(stepSpeed);    // by changing this time delay between the steps we can change the rotation speed
  digitalWrite(stepPin, LOW);
  delayMicroseconds(stepSpeed);
  stepsTaken += 1;
  }
*/




/*
  digitalWrite(directionPin, LOW);
  for (int x = 0; x < 50; x++) {
  digitalWrite(stepPin, HIGH);
  delayMicroseconds(stepSpeed);    // by changing this time delay between the steps we can change the rotation speed
  digitalWrite(stepPin, LOW);
  delayMicroseconds(stepSpeed);
  stepsTaken -= 1;
  }
*/
