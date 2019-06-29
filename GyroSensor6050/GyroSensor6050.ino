/* 
I2C interface

MPU6050 -- Arduino
3.3V -------- 3.3V
SDA ----------- A4
SCL ----------- A5
GND ---------- GND
*/

#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#include "Wire.h"

MPU6050 mpu(0x68);

uint8_t mpuIntStatus;
uint16_t packetSize;
uint16_t fifoCount;
uint8_t fifoBuffer[64];
volatile bool mpuInterrupt = false;
unsigned long timing;

Quaternion q;

void setup() 
{
    Wire.begin();

    Serial.begin(115200);   

    mpu.initialize();

    delay(1000);

    uint8_t devStatus = mpu.dmpInitialize();

    if (devStatus == 0) 
    {
        mpu.setDMPEnabled(true);
        mpuIntStatus = mpu.getIntStatus();
        packetSize = mpu.dmpGetFIFOPacketSize();
    }
}

void loop() 
{
    mpuInterrupt = false;

    mpuIntStatus = mpu.getIntStatus();

    fifoCount = mpu.getFIFOCount();

    if ((mpuIntStatus & 0x10) || fifoCount == 1024) 
    {
        mpu.resetFIFO();
    } 
    else if (mpuIntStatus & 0x02) 
    {
        while (fifoCount < packetSize) 
            fifoCount = mpu.getFIFOCount();

        mpu.getFIFOBytes(fifoBuffer, packetSize);

        fifoCount -= packetSize;

        mpu.dmpGetQuaternion(&q, fifoBuffer);

        Serial.flush();
        Serial.print("gyro"); Serial.print("|");
        Serial.print("GyroSensor00"); Serial.print("|");
        Serial.print(q.x); Serial.print("|");
        Serial.print(q.y); Serial.print("|");
        Serial.print(q.z); Serial.print("|");
        Serial.print(q.w); Serial.println("|");
    }
}

void dmpDataReady() 
{
    mpuInterrupt = true;
}
