
/* IMU Breakout Board - ADXL345 & IMU3000
   Example Arduino Sketch to read the Gyro and Accelerometer Data
  
   Written by www.hobbytronics.co.uk 
   See the latest version at www.hobbytronics.co.uk/arduino-adxl345-imu3000
   08-Apr-2011
   
   Modified by Justin Weber
   12-May-2011
   
*/

#include <Wire.h>

#define GYRO 0x68         // gyro I2C address
#define REG_GYRO_X 0x1D   // IMU-3000 Register address for GYRO_XOUT_H
#define REG_TEMP 0x1B     // IMU-3000 Register address for 
#define ACCEL 0x53        // Accel I2c Address
#define ADXL345_POWER_CTL 0x2D


#define PI 3.14159265358979

char buffer[14];     // Array to store ADC values 
float gyro_x;
float gyro_y;
float gyro_z;
float accel_x;
float accel_y;
float accel_z;
float RwAcc[3];       //normalized accel vector(x,y,z)
float RwGyro[3];      //Gyro data (x,y,z)
float temp;
unsigned long curtime;
unsigned long oldTime=0;
unsigned long newTime;
unsigned long interval;
int i;




void setup()
{
    Serial.begin(57600); 
    
    Wire.begin();
    // Set Gyro settings
    // Sample Rate 1kHz, Filter Bandwidth 42Hz, Gyro Range 500 d/s 
    writeTo(GYRO, 0x16, 0x0B);       
    //set accel register data address
    writeTo(GYRO, 0x18, 0x32);     
    // set accel i2c slave address
    writeTo(GYRO, 0x14, ACCEL);     
    
    // Set passthrough mode to Accel so we can turn it on
    writeTo(GYRO, 0x3D, 0x08);     
    // set accel power control to 'measure'
    writeTo(ACCEL, ADXL345_POWER_CTL, 8);
    // set accel range to +-2 g, and change accel output to MSB
    writeTo(ACCEL, 0x31, 0x04);
    //cancel pass through to accel, gyro will now read accel for us   
    writeTo(GYRO, 0x3D, 0x28);    

}

// Write a value to address register on device
void writeTo(int device, byte address, byte val) {
  Wire.beginTransmission(device); // start transmission to device 
  Wire.write(address);             // send register address
  Wire.write(val);                 // send value to write
  Wire.endTransmission();         // end transmission
}

void loop()
{
    // Read the Gyro X, Y and Z and Accel X, Y and Z all through the gyro
    
    // First set the register start address for X on Gyro  
    Wire.beginTransmission(GYRO);
    Wire.write(REG_TEMP); //Register Address TEMP_OUT_H
    Wire.endTransmission();

    // New read the 12 data bytes
    Wire.beginTransmission(GYRO);
    Wire.requestFrom(GYRO,14); // Read 14 bytes
    i = 0;
    while(Wire.available())
    {
        buffer[i] = Wire.read();
        i++;
    }
    Wire.endTransmission();
    
    //record time
    curtime=micros();
    newTime=curtime;
    interval=newTime-oldTime;
    oldTime=newTime;

    //Combine bytes into integers
    //Temp is in MSB first
    temp = buffer[0] << 16 | buffer[1];
    // Gyro format is MSB first
    gyro_x = buffer[2] << 16 | buffer[3];
    gyro_y = buffer[4] << 16 | buffer[5];
    gyro_z = buffer[6] << 16 | buffer[7];
    // Because of orientation of chips
    // accel y output is in same orientation as gyro x
    // and accel x is gyro -y
    accel_y = buffer[8] << 10 | buffer[9];
    accel_x = buffer[10] << 10 | buffer[11];
    accel_z = buffer[12] << 10 | buffer[13];

   //gyro
   
   //accel vector
   RwAcc[0]=accel_x;
   RwAcc[1]=accel_y;
   RwAcc[2]=accel_z;
   normalize3DVector(RwAcc);
   

    // Print out what we have
    Serial.print(gyro_x);    // echo the number received to screen
    Serial.print(",");
    Serial.print(gyro_y);    // echo the number received to screen
    Serial.print(",");    
    Serial.print(gyro_z);   // echo the number received to screen 
    Serial.print(",");     
    Serial.print(RwAcc[0]);  // echo the number received to screen
    Serial.print(",");
    Serial.print(RwAcc[1]);  // echo the number received to screen
    Serial.print(",");   
    Serial.print(RwAcc[2]);   // echo the number received to screen
    Serial.print(",");
    Serial.print(interval);  // echo time interval
    Serial.print(",");
    Serial.print(temp);     // echo temp

    Serial.println("");     // prints carriage return
    delay(400);             // wait for a second   
}

void normalize3DVector(float* vector){
  static float R;  
  R = sqrt(vector[0]*vector[0] + vector[1]*vector[1] + vector[2]*vector[2]);
  vector[0] /= R;
  vector[1] /= R;  
  vector[2] /= R;  
}
