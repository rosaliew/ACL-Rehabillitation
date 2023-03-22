/* Student Information 
 *  IBEHS 3P04 
 *  Group 26
 *  Rosalie Wang      | wangr200 | 400305904
 *  Saina Seddighpour | seddighs | 400305602
 *  Daniel Wong       | wongd43  | 400308249
 *  Matthew Milkovich | milkovim | 400308871
*/



/*--------------------------------Variable Declarations & Definitions--------------------------------*/

//Arduino Libraries for BNO55 Sensors
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>



//First I2C for BNO55
#define SDA_1 21  //Data is the blue wire connecting to Pin 21
#define SCL_1 22  //Clock is the yellow wire connecting to Pin 22

//Second I2C for BNO55
#define SDA_2 19  //Data is the blue wire connecting to Pin 19
#define SCL_2 23  //Clock is the yellow wire connecting to Pin 23

TwoWire I2Cone = TwoWire(0);  //Assists in I2C communication
TwoWire I2Ctwo = TwoWire(1);

//Setting up the 2 BNO55's to communicate on different lines; same address is okay because of this
Adafruit_BNO055 BNO1 = Adafruit_BNO055(55, 0x28, &I2Cone);
Adafruit_BNO055 BNO2 = Adafruit_BNO055(55, 0x28, &I2Ctwo);



//Setting up the variables and parameters for monitoring tibial lateral rotation
bool isInLast30 = false;
int numSamples = 0;
float runningTotal = 0.0;
float instantaneousFlexAngle = 0.0;
float averageExtAngle = 0.0;



//Defining 3 force sensors and the pins they connect to
#define FORCE_SENSOR_1 12
#define FORCE_SENSOR_2 13
#define FORCE_SENSOR_3 14



/*--------------------------------------------Void Setup--------------------------------------------*/

void setup() {
 Serial.begin(115200);                //Set Baud rate to 115200, ensure the same rate in serial monitor
 Serial.println(F("Initializing the two BNO55 sensors..."));  //Prints when beginning I2C communication
 I2Cone.begin(SDA_1, SCL_1, 100000);
 I2Ctwo.begin(SDA_2, SCL_2, 100000);  //Tells I2Cone and I2Ctwo which lines are being used
 


 //Checks wiring of the first BNO55
 bool status1 = BNO1.begin();
 if (!status1) {
 Serial.println("Could not find a valid BNO55_1 sensor, check wiring!");
 while (1);
 }

 //Checks wiring of the second BNO55
 bool status2 = BNO2.begin();
 if (!status2) {
 Serial.println("Could not find a valid BNO55_2 sensor, check wiring!");
 while (1);
 }
 Serial.println();

}

/*
Note: After Void Setup, you can simply use BNO1 when you want to refer to the first BNO55 and BNO2 when you want to refer to the second BNO55. You don't need to think about the 2 seperate lines anymore.
*/



/*---------------------------------------------Void Loop---------------------------------------------*/

void loop() {
  //Getting new sensor events for the BNO55 sensors
  sensors_event_t event1;
  sensors_event_t event2;
  BNO1.getEvent(&event1);
  BNO2.getEvent(&event2);

  //Calculating the knee-joint angle (saggital plane)
  float angle1 = event1.orientation.z-event2.orientation.z;  //BNO55 z-angle difference of sensors
  // Serial.println(angle1, 4);

  //Calculating the tibial-rotation angle (transverse plane)
  float angle2 = event1.orientation.y-event2.orientation.y; //BNO55 y-angle difference of sensors
  // Serial.println(angle2, 4);

  Serial.print("Average Knee Angle: ");
  Serial.println(smoothKneeAngle(angle1));  //Uses "smoothKneeAngle" to print rolling avg



  //If knee angle (angle1) is less than 30 deg, set the following variables and print to user
  if (!isInLast30 && angle1 <= 30) {
    isInLast30 = true;
    runningTotal = 0.0;
  }

  if (isInLast30 && numSamples < 5) {
    runningTotal += event2.orientation.y;
    averageExtAngle = smoothTibAngle(event2.orientation.y); //Uses "smoothTibAngle" to print rolling avg
    numSamples += 1;
    if (numSamples == 5) {
      instantaneousFlexAngle = runningTotal / 5;  //Calculates avg angle as soon as knee enters 30 deg
    }
    Serial.println("Patient is in the last 30 degrees of knee extension.");
  }
  
  if (isInLast30 && numSamples == 5) {
    Serial.println("Patient is in the last 30 degrees of knee extension.");
    averageExtAngle = smoothTibAngle(event2.orientation.y);
    float angle2 = averageExtAngle - instantaneousFlexAngle;
    Serial.print("Average Tibial Lateral Rotation: ");  //Prints continuous difference of tibial rotation
    Serial.println(angle2);
  }

  if (angle1 > 30) {
    isInLast30 = false;
    numSamples = 0;     //If knee angle becomes greater than 30 deg, reset tibial rotation angle
  }


  //Data interpretation for Force Sensor 1 (monitors foot's left side)
  int analogReading1 = analogRead(FORCE_SENSOR_1);

  Serial.print("FS1 Reading = ");
  Serial.println(analogReading1); //Printing the raw analog reading

  if (analogReading1 < 3500){
    Serial.println(" -> FS1 Undetected - Foot Lifted on Medial Side"); //Any force less than 3500
  }
  else{
    Serial.println(" -> Force Applied on Medial Side"); //Any force more than 3500
  }


  //Data interpretation for Force Sensor 2 (monitors foot's right side)
  int analogReading2 = analogRead(FORCE_SENSOR_2);

  Serial.print("FS2 Reading = ");
  Serial.println(analogReading2); //Printing the raw analog reading

  if (analogReading2 < 3500){
    Serial.println(" -> FS2 Undetected - Foot Lifted on Lateral Side"); //Any force less than 3500
  }
  else{
    Serial.println(" -> Force Applied on Lateral Side"); //Any force more than 3500
  }


  //Data interpretation for Force Sensor 3 (monitors ball of foot)
  int analogReading3 = analogRead(FORCE_SENSOR_3);

  Serial.print("FS3 Reading = ");
  Serial.println(analogReading3); //Printing the raw analog reading

  if (analogReading3 < 3500){
    Serial.println(" -> FS3 Undetected - Heel is Lifted"); //Any force less than 3500
  }
  else{
    Serial.println(" -> Force Applied at the Heel"); //Any force more than 3500
  }



  Serial.println("-----------------------------------------");

  //For serial plotter in Arduino
  int force1 = int(analogReading1 > 3500)*50;
  int force2 = int(analogReading2 > 3500)*50; 
  int force3 = int(analogReading3 > 3500)*50;

  float averaged_angle = smoothKneeAngle(angle1);
  int MIN = -45; 
  int MAX = 135; 

  Serial.print(MIN);
  Serial.print(","); 
  Serial.print(MAX);
  Serial.print(","); 
  Serial.print(angle1, 4); 
  Serial.print(","); 
  Serial.print(averaged_angle); 
  Serial.print(","); 
  Serial.print(force1);
  Serial.print(","); 
  Serial.print(force2); 
  Serial.print(","); 
  Serial.println(force3);

  delay(1000);

}



/*--------------------------------Data Filtration for the BNO55 Sensors--------------------------------*/

//Functions for BNO55 sensor data filtration with local parameters and variables 

//Low-pass, rolling average filter for knee-joint angle
float smoothKneeAngle(float inputAngle) { 
  static const int numReadings = 5;
  static float total = 0;
  static float readings[numReadings];
  static int readIndex = 0;

  total -= readings[readIndex];           //Subtract the previous reading
  readings[readIndex] = inputAngle;       //Read the sensor
  total += readings[readIndex];           //Add value to total
  readIndex = readIndex + 1;              //Update the indexing

  if (readIndex >= numReadings) {
    readIndex = 0;
  }
  
  float average = total / numReadings;          //Calculate the running average

  return average;
}

//Low-pass, rolling average filter for tibial lateral rotation angle (when knee-angle < 30 deg)
float smoothTibAngle(float inputAngle) { 
  static const int numReadings = 5;
  static long total = 0;
  static int readings[numReadings];
  static int readIndex = 0;

  total -= readings[readIndex];           //Subtract the previous reading
  readings[readIndex] = inputAngle;       //Read the sensor
  total += readings[readIndex];           //Add value to total
  readIndex = readIndex + 1;              //Update the indexing

  if (readIndex >= numReadings) {
    readIndex = 0;
  }
  
  float average = total / numReadings;          //Calculate the running average

  return average;
}
