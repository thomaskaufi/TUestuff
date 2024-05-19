/* M1.2 sketch 3.2 // experiment 003

    Code to controll two stepper motors without blocking other events
    Based on example code by Hannah Elkens
   
    Thomas Kaufmanas, Eindhoven 2024

*/

#include <TMCStepper.h>
#include <SoftwareSerial.h>     // Software serial for the UART to TMC2209 - https://www.arduino.cc/en/Reference/softwareSerial
#include <AccelStepper.h>


#define DIR_PIN_1          3 // Direction
#define STEP_PIN_1         4 // Step
#define EN_PIN_1           5 // Enable

#define DIR_PIN_2          6 // Direction
#define STEP_PIN_2         7 // Step
#define EN_PIN_2           8 // Enable


#define SW_RX            12 // TMC2208/TMC2224 SoftwareSerial receive pin
#define SW_TX            10 // TMC2208/TMC2224 SoftwareSerial transmit pin
#define DRIVER_ADDRESS 0b00 // TMC2209 Driver address according to MS1 and MS2
#define R_SENSE 0.11f // Match to your driver
#define MICROSTEPS 1
#define STEPS_PER_REVOLUTION 1600*MICROSTEPS //this makes it have 16 more steps = 3200 steps in a full spin
TMC2209Stepper driver(SW_RX, SW_TX, R_SENSE, DRIVER_ADDRESS);

AccelStepper stepper1(AccelStepper::DRIVER, STEP_PIN_1, DIR_PIN_1); // (Typeof driver: with 2 pins, STEP, DIR)
AccelStepper stepper2(AccelStepper::DRIVER, STEP_PIN_2, DIR_PIN_2);

void setup() {
  pinMode(EN_PIN_1, OUTPUT);
  pinMode(STEP_PIN_1, OUTPUT);
  pinMode(DIR_PIN_1, OUTPUT);

  pinMode(EN_PIN_2, OUTPUT);
  pinMode(STEP_PIN_2, OUTPUT);
  pinMode(DIR_PIN_2, OUTPUT);

  digitalWrite(EN_PIN_1, LOW);
  digitalWrite(EN_PIN_2, LOW);

  Serial.begin(9600);

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
  stepper2.setAcceleration(100);
  stepper2.setPinsInverted(false, false, true);
}

void loop() {


  if (stepper1.distanceToGo() == 0) {
    stepper1.runToNewPosition(0);

    if (stepper1.currentPosition() == 0) {
      stepper1.runToNewPosition(1600);
    }
  }


  if (stepper2.distanceToGo() == 0) {
    stepper2.runToNewPosition(0);

    if (stepper2.currentPosition() == 0) {
      stepper2.runToNewPosition(1600);
    }
  }

  Serial.println("runnin-");

}
