/* M1.2 sketch 4.3 // experiment 004

    Code to controll two stepper motors without blocking other events.
    Recieves a sound level value from puredata.
    adds a rotary switch to change mode between here and not here
    Based on example code by Hannah Elkens

    Thomas Kaufmanas, Eindhoven 2024

*/

#include <TMCStepper.h>
#include <SoftwareSerial.h>     // Software serial for the UART to TMC2209 - https://www.arduino.cc/en/Reference/softwareSerial
#include <AccelStepper.h>

//Magical variables for listening to pure data
int inByte = 0;
char buffer[40];
int index = 0;
int value = 0; //RAW LATEST READING <---

SoftwareSerial TERMINALDEBUG(10, 11);


#define DIR_PIN_1          3 // Direction
#define STEP_PIN_1         4 // Step
#define EN_PIN_1           5 // Enable

#define DIR_PIN_2          6 // Direction
#define STEP_PIN_2         7 // Step
#define EN_PIN_2           8 // Enable

#define switchPin          12 // For rotary switch


#define SW_RX            36 // TMC2208/TMC2224 SoftwareSerial receive pin
#define SW_TX            37 // TMC2208/TMC2224 SoftwareSerial transmit pin
#define DRIVER_ADDRESS 0b00 // TMC2209 Driver address according to MS1 and MS2
#define R_SENSE 0.11f // Match to your driver
#define MICROSTEPS 1
#define STEPS_PER_REVOLUTION 1600*MICROSTEPS //this makes it have 16 more steps = 3200 steps in a full spin
TMC2209Stepper driver(SW_RX, SW_TX, R_SENSE, DRIVER_ADDRESS);

AccelStepper stepper1(AccelStepper::DRIVER, STEP_PIN_1, DIR_PIN_1); // (Typeof driver: with 2 pins, STEP, DIR)
AccelStepper stepper2(AccelStepper::DRIVER, STEP_PIN_2, DIR_PIN_2);

long timer1 = 0; //for first stepper listening loop
long timer2 = 0; //for second stepper listening loop
long timer3 = 0; //for hello world poking in terminal
long timer4 = 0;
int interval1 = 10 * 1000;
int interval2 = 15 * 1000;
int interval3 = 500;
int interval4 = 10 * 1000;
int sensitivity = 6;
int firstRun1 = false;
int firstRun2 = false;

int spikes1 = 0;
int prevSpikes1 = 0;
int spikes2 = 0;
int prevSpikes2 = 0;

int maxSteps = 3200;  //Maximum amount of turns the physicalisation can handle

void setup() {
  pinMode(EN_PIN_1, OUTPUT);
  pinMode(STEP_PIN_1, OUTPUT);
  pinMode(DIR_PIN_1, OUTPUT);

  pinMode(EN_PIN_2, OUTPUT);
  pinMode(STEP_PIN_2, OUTPUT);
  pinMode(DIR_PIN_2, OUTPUT);

  digitalWrite(EN_PIN_1, LOW);
  digitalWrite(EN_PIN_2, LOW);

  pinMode(switchPin, INPUT_PULLUP);

  Serial.begin(9600);
  TERMINALDEBUG.begin (57600); //SW serial for debugging

  TERMINALDEBUG.println("");
  TERMINALDEBUG.println("Run Program -------");
  for (int x = 0; x < 10; x++) {
    TERMINALDEBUG.println(" ");
  }

  driver.beginSerial(115200);
  driver.begin();
  driver.toff(5);
  driver.rms_current(500);        // Set motor RMS current
  driver.microsteps(MICROSTEPS);
  driver.pwm_autoscale(true);       // Needed for stealthChop

  stepper1.setMaxSpeed(200);
  stepper1.setAcceleration(400);
  stepper1.setPinsInverted(false, false, true);

  stepper2.setMaxSpeed(200);
  stepper2.setAcceleration(400);
  stepper2.setPinsInverted(false, false, true);
}

void loop() {


  if (digitalRead(switchPin)) {

    if (firstRun1) { // Zero motors (one time only)
      TERMINALDEBUG.println("");
      TERMINALDEBUG.println("");
      TERMINALDEBUG.println("LISTENING MODE - zero'ing motors...");
      stepper1.moveTo(0);
      stepper2.moveTo(0);

      while ((stepper1.distanceToGo() != 0) && (stepper2.distanceToGo() != 0)) {
        stepper1.run();
        stepper2.run();
      }
      firstRun1 = false;
      TERMINALDEBUG.println("   ...Done, engaging listening mode");
    }

    // MAGIC!! NO CLUE HOW WORKS DONT FUCKING TOUCH--------
    index = 0;                                           //
    do                                                   //
    {
      buffer[index] = Serial.read();                     //
      if (buffer[index] != -1) index = index + 1;        //
    } while (buffer[index - 1] != 32);                   //
    value = atoi(buffer);                                //
    //TERMINALDEBUG.println(value);
    // End of magic ----------------------------------------


    if (value > sensitivity) {    //Notes spikes to both counters
      TERMINALDEBUG.print("|");
      spikes1 += 1;
      spikes2 += 2;
    }

    // TIMER 1: ----------------------------------------------------------------------
    if (millis() - timer1 > interval1) {
      TERMINALDEBUG.println("");
      TERMINALDEBUG.print("[EOR 1. Caught: ");
      TERMINALDEBUG.print(spikes1);
      TERMINALDEBUG.print(". Prev: ");
      TERMINALDEBUG.print(prevSpikes1);
      if ((spikes1 > prevSpikes1) && (stepper1.targetPosition() < maxSteps)) {
        stepper1.move(200);
        TERMINALDEBUG.print(". Adding towards ");
        TERMINALDEBUG.print(stepper1.targetPosition());
        TERMINALDEBUG.println("]");
      }
      else {
        if (stepper1.targetPosition() > 0) {
          stepper1.move(-200);
          TERMINALDEBUG.print(". Subtracting towards ");
          TERMINALDEBUG.print(stepper1.targetPosition());
          TERMINALDEBUG.println("]");
        }
        else {
          TERMINALDEBUG.print(". resting at ");
          TERMINALDEBUG.print(stepper1.targetPosition());
          TERMINALDEBUG.println("]");
        }
      }

      prevSpikes1 = spikes1;
      spikes1 = 0;
      timer1 = millis();
    } //End of FIRST TIMER -----------------------------------------------------------



    // TIMER 2: ----------------------------------------------------------------------
    if (millis() - timer2 > interval2) {
      TERMINALDEBUG.println("");
      for (int y = 0; y < 50; y++) {
        TERMINALDEBUG.print(" ");
      }
      TERMINALDEBUG.print("[EOR 2. Caught: ");
      TERMINALDEBUG.print(spikes2);
      TERMINALDEBUG.print(". Prev: ");
      TERMINALDEBUG.print(prevSpikes2);
      if ((spikes2 > prevSpikes2) && (stepper2.targetPosition() < maxSteps)) {
        stepper2.move(200);
        TERMINALDEBUG.print(". Adding towards ");
        TERMINALDEBUG.print(stepper2.targetPosition());
        TERMINALDEBUG.println("]");
      }
      else {
        if (stepper2.targetPosition() > 0) {
          stepper2.move(-200);
          TERMINALDEBUG.print(". Subtracting towards ");
          TERMINALDEBUG.print(stepper2.targetPosition());
          TERMINALDEBUG.println("]");
        }
        else {
          TERMINALDEBUG.print(". resting at ");
          TERMINALDEBUG.print(stepper2.targetPosition());
          TERMINALDEBUG.println("]");
        }
      }

      prevSpikes2 = spikes2;
      spikes2 = 0;
      timer2 = millis();
    } //End of FIRST TIMER -----------------------------------------------------------


    stepper1.run();
    stepper2.run();


    // TIMER 3: I'm alive -----------------------
    if (millis() - timer3 > interval3) {        //
      TERMINALDEBUG.print(".");                 //
      //
      timer3 = millis(); // reset the timer     //
    } //End of third TIMER -----------------------

    firstRun2 = true;
  } // End of listening loop

  else {

    if (firstRun2) { // Zero motors (one time only)
      TERMINALDEBUG.println("");
      TERMINALDEBUG.println("");
      TERMINALDEBUG.print("NOT HERE MODE - zero'ing motors...");
      stepper1.moveTo(0);
      stepper2.moveTo(0);

      while ((stepper1.distanceToGo() != 0) && (stepper2.distanceToGo() != 0)) {
        stepper1.run();
        stepper2.run();
      }

      firstRun2 = false;
      TERMINALDEBUG.println("   ...Done, showing random location");
      stepper1.moveTo(random(10, 20) * 100);
      stepper2.moveTo(random(16, 32) * 100); //the "slow" listener
      while ((stepper1.distanceToGo() != 0) && (stepper2.distanceToGo() != 0)) {
        stepper1.run();
        stepper2.run();
      }
    }

    // TIMER 4: pick random locations ------------
    if (millis() - timer4 > interval4) {        //
      int randomTravel = (random(-2, 2) * 100);

      if (((randomTravel + stepper1.currentPosition()) < maxSteps) && (randomTravel + stepper1.currentPosition()) > 0) {
        stepper1.move(randomTravel);
        TERMINALDEBUG.print("moving to ");
        TERMINALDEBUG.println(stepper1.targetPosition());
      }
      else {
        TERMINALDEBUG.println("...Out of range!");

      }

      timer4 = millis(); // reset the timer
    } //End of fourth TIMER -----------------------



    stepper1.run();
    stepper2.run();


    firstRun1 = true;
  }// End of switch loop


}// End of void
