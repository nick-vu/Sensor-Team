#include <Wire.h>
#include <VL53L0X.h>

VL53L0X sensor1; 
VL53L0X sensor2;
VL53L0X sensor3;
#define sensor1_shut 4 //shutdown pin for sensor 1
#define sensor2_shut 5 //shutdown pin for sensor 2
#define sensor3_shut 6 //shutdown pin for sensor 3
#define sensor1_address 22 //defines address for sensor 1
#define sensor2_address 25 //defines address for sensor 2
#define sensor3_address 28 //defines address for sensor 3
void setup()
{

  pinMode(sensor1_shut, OUTPUT);
  pinMode(sensor2_shut, OUTPUT);
  pinMode(sensor3_shut, OUTPUT);
  digitalWrite(sensor1_shut, LOW);
  digitalWrite(sensor2_shut, LOW);
  digitalWrite(sensor3_shut, LOW);

  delay(500);
  Wire.begin();


  Serial.begin (115200);

  pinMode(sensor1_shut, INPUT);
  delay(150);
  Serial.println("00");
  sensor1.init(true);

  delay(100);
  sensor1.setAddress((uint8_t)sensor1_address);
  Serial.println("01");

  pinMode(sensor2_shut, INPUT);
  delay(150);
  sensor2.init(true);
  delay(100);
  sensor2.setAddress((uint8_t)sensor2_address);
  Serial.println("02");

  pinMode(sensor3_shut, INPUT);
  delay(150);
  sensor3.init(true);
  delay(100);
  sensor3.setAddress((uint8_t)sensor3_address);
  Serial.println("03");

  Serial.println("addresses set");

  Serial.println ("I2C scanner. Scanning ...");
  byte count = 0;


  for (byte i = 1; i < 120; i++)
  {

    Wire.beginTransmission (i);
    if (Wire.endTransmission () == 0)
    {
      Serial.print ("Found address: ");
      Serial.print (i, DEC);
      Serial.print (" (0x");
      Serial.print (i, HEX);
      Serial.println (")");
      count++;
      delay (1);  // maybe unneeded?
    } // end of good response
  } // end of for loop
  Serial.print ("Found ");
  Serial.print (count, DEC);
  Serial.println (" device(s).");



}

void loop()
{
  
  Serial.print ("Sensor 1: ");
  Serial.print(sensor1.readRangeSingleMillimeters());
  Serial.print (" mm");
  Serial.println();
  
  Serial.print ("Sensor 2: ");
  Serial.print(sensor2.readRangeSingleMillimeters());
  Serial.print (" mm");
  Serial.println();

  Serial.print ("Sensor 3: ");
  Serial.print(sensor3.readRangeSingleMillimeters());
  Serial.print (" mm");
  Serial.println();

  delay(100);
}
