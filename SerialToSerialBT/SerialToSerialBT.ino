
//This example code is in the Public Domain (or CC0 licensed, at your option.)
//By Evandro Copercini - 2018
//
//This example creates a bridge between Serial and Classical Bluetooth (SPP)
//and also demonstrate that SerialBT have the same functionalities of a normal Serial

// ---------------- BNO055 Setup
#define BNO055_SAMPLERATE_DELAY_MS (100)
#include "BluetoothSerial.h" //includes the blue tooth serial library 
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

Adafruit_BNO055 bno = Adafruit_BNO055(55);
BluetoothSerial SerialBT; //an instance of bluetoothserial called SerialBT

// -------------- Force Sensor Setup ------------
#define FORCE_SENSOR_PIN 14    // the FSR and 10K pulldown are connected to a0
int fsrReading;     // the analog reading from the FSR resistor divider
int fsrVoltage;     // the analog reading converted to voltage
unsigned long fsrResistance;  // The voltage converted to resistance, can be very big so make "long"
unsigned long fsrConductance; 
long fsrForce;       // Finally, the resistance converted to force

//
//checks if bluetooth is properly enabled
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif


void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32_ROMbell"); //Bluetooth device name
  Wire.begin(); 
  if(!bno.begin())
  {
    /* There was a problem detecting the BNO055 ... check your connections */
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }
    delay(1000);
  Serial.println("The device started, now you can pair it with bluetooth!");
}

// sends and receives data via bluetooth serial 
// SerialBT.write() sends data using bluetooth serial 
// Serial.read() returns the data received in the serial port

void loop() {
  fsrReading = analogRead(FORCE_SENSOR_PIN);  //analog reading
 
  // analog voltage reading ranges from about 0 to 1023 which maps to 0V to 5V (= 5000mV)
  fsrVoltage = map(fsrReading, 0, 4095, 0, 3300); // Voltage reading in mV
  
  if (SerialBT.available()) {
    // Getting data from sensors 
    imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
    if (fsrVoltage == 0) { //no pressure
      fsrForce = 0;
    } else {
      // The voltage = Vcc * R / (R + FSR) where R = 1K and Vcc = 3.3V
      // so FSR = ((Vcc - V) * R) / V        yay math!
      fsrResistance = 3300 - fsrVoltage;     // fsrVoltage is in millivolts so 3.3V = 3300mV
      fsrResistance *= 1000;                // 1K resistor
      fsrResistance /= fsrVoltage; //resistance in ohms
   
      fsrConductance = 1000000;           // we measure in micromhos so 
      fsrConductance /= fsrResistance;  // conductance in microohms
  
  
      // Use the two FSR guide graphs to approximate the force
      if (fsrConductance <= 1000) {
        fsrForce = fsrConductance / 80;     
      } else {
        fsrForce = fsrConductance - 1000;
        fsrForce /= 30;         
      }
    }
    SerialBT.print(euler.x());
    SerialBT.print(" , ");
    SerialBT.print(euler.z());
    SerialBT.print(" , ");
    SerialBT.println(fsrForce); // getting z-value and printing to serial monitor
    delay(600);
//    SerialBT.write(Serial.read()); //checks to see if there are bytes being received in the serial port. If there are, send that information via Bluetooth to the connected device
  }
}
