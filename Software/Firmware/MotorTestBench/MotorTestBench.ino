// Motor Test Bench V1.0
// Developer: Alan Carvalho
// Date: 14/04/2024

#include "AccelStepper.h"  // Library to control the stepper motor
#include "HX711.h"         // Library to control the HX711 module

// HX711 pins
#define LOADCELL_DT 10
#define LOADCELL_SCK 11

// Relay pin
#define RELAY_S 4

// Optical switch pin
#define OPTICAL_SWITCH 3

// Stepper driver pins
#define STEPPER_DIR 6
#define STEPPER_STEP 7

// Bench parameters
#define STEPPER_SPEED -100    // Stepper motor speed
#define ARM_LENGTH 0.035      // Lever arm length (m)
#define ENCODER_POINTS 20.0   // Number of windows in the encoder disk
#define MAX_PULSES 100        // Number of pulses to restart the counting

HX711 loadCell;                                                       // HX711 library object
AccelStepper stepper = AccelStepper(1, STEPPER_STEP, STEPPER_DIR);    // AccelStepper library object

bool running = false;                   // Indicates the test is running
float mass = 0;                         // Mass on the load cell (g)
float torque = 0;                       // Torque generated by the motor (mN.m)
float speed = 0;                        // Motor shaft speed (RPM)
volatile int pulseCount = 0;            // Encoder pulse count
unsigned long currentTime = 0;          // Current time on speed calculation (ms)
unsigned long initialTime = 0;          // Initial time on speed calculation (ms)
const long readingInterval = 100;        // Interval between readings (ms)
unsigned long lastReading = 0;          // Time of the last reading (ms)
const long maximumPulseInterval = 1000; // Maximum time between two encoder pulse to assume it is not running (ms)
unsigned long lastPulseTime = 0;        // Time when the last encoder pulse happened (ms)
const long testFinishInterval = 3000;   // Interval to stop the test after the motor speed gets to zero (ms)
unsigned long zeroSpeedTime = 0;        // Time when the motor speed gets to zero (ms)


void setup() 
{
  Serial.begin(115200);  // Open the serial port

  // Configure pins
  pinMode(OPTICAL_SWITCH, INPUT_PULLUP);
  pinMode(RELAY_S, OUTPUT);

  // Configure the HX711 sensor
  loadCell.begin(LOADCELL_DT, LOADCELL_SCK);
  loadCell.set_scale(440);
  loadCell.tare();

  // Configure the interrupt to detect encoder pulses
  attachInterrupt(digitalPinToInterrupt(OPTICAL_SWITCH), countPulse, RISING);

  // Start with the motor turned off
  digitalWrite(RELAY_S, LOW);

  // Set stepper maximum speed
  stepper.setMaxSpeed(100);
}


void loop() 
{
  if (Serial.available() > 0) 
  {
    String command = Serial.readString();

    if (command.indexOf("START") > -1) startTest();
    else if (command.indexOf("STOP") > -1) stopTest(false);
  }

  if (running) 
  {
    stepper.runSpeed();

    if ((millis() - lastReading) > readingInterval) 
    {
      // Get all the necessary data
      getSpeed();
      getTorque();

      // Print the collected data on the serial port
      printData();

      lastReading = millis();
    }

    // Encoder is not moving
    if ((millis() - lastPulseTime) > maximumPulseInterval) pulseCount = 0;
  }
}


void getSpeed() 
{
  currentTime = millis() - initialTime;
  speed = 60000 * ((pulseCount / ENCODER_POINTS) / currentTime);
  if (speed < 0) speed = 0;

  if (pulseCount > MAX_PULSES) 
  {
    pulseCount = 0;
    initialTime = millis();
  }

  //if(speed == 0)
  //{
    //if(zeroSpeedTime == 0) zeroSpeedTime = millis();
    //else if ((millis() - zeroSpeedTime) > testFinishInterval) stopTest(true);
  //}
}

void getTorque() 
{
  mass = loadCell.get_units();
  torque = 9.81 * mass * ARM_LENGTH;
  if (torque < 0) torque = 0;
}

void countPulse() 
{
  pulseCount++;
  lastPulseTime = millis();
}

void printData() 
{
  Serial.print(speed);
  Serial.print("|");
  Serial.print(torque);
  Serial.println("");
}

void startTest()
{
  running = true;
  
  digitalWrite(RELAY_S, HIGH);
  
  Serial.println("MOTOR TEST STARTED");
  Serial.println("");
  Serial.println("Speed (RPM)|Torque (mN.m)");
     
  stepper.setSpeed(STEPPER_SPEED);
}

void stopTest(bool status)
{
  running = false;
  pulseCount = 0;
  zeroSpeedTime = 0;
  
  digitalWrite(RELAY_S, LOW);
  
  if (status) Serial.println("MOTOR TEST FINISHED");
  else Serial.println("MOTOR TEST STOPPED");
  Serial.println("");

  stepper.stop();
}