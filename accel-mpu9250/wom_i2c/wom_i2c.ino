/*
WOM_I2C.ino
Brian R Taylor
brian.taylor@bolderflight.com

Copyright (c) 2017 Bolder Flight Systems

Permission is hereby granted, free of charge, to any person obtaining a copy of this software
and associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <MPU9250.h>

// an MPU9250 object with the MPU-9250 sensor on I2C bus 0 with address 0x68
MPU9250       IMU( Wire, 0x68 );
int           status;
unsigned long g_uIntCount = 0;

const uint8_t k_pinINT = 2;
volatile bool g_bINT = false;

void
wakeUp()
{
    g_bINT = true;
}

void
setup()
{
    // serial to display data
    Serial.begin( 115200 );
    while ( ! Serial )
        delay( 10 );
    delay( 500 );
    Serial.println( "WOM_I2C" );

    // start communication with IMU
    status = IMU.begin();
    if ( status < 0 )
    {
        Serial.println( "IMU initialization unsuccessful" );
        Serial.println( "Check IMU wiring or try cycling power" );
        Serial.print( "Status: " );
        Serial.println( status );
        while ( 1 )
            delay( 500 );
    }
    IMU.calibrateAccel();

    // enabling wake on motion low power mode with a threshold of 400 mg and
    // an accelerometer data rate of 15.63 Hz.
    IMU.disableDataReadyInterrupt();
    IMU.setAccelRange( MPU9250::ACCEL_RANGE_16G );
    IMU.setGyroRange( MPU9250::GYRO_RANGE_250DPS );
    IMU.setDlpfBandwidth( MPU9250::DLPF_BANDWIDTH_5HZ );
    IMU.setSrd( 1 + 199 );


    // attaching the interrupt to microcontroller pin 1
    pinMode( k_pinINT, INPUT );
    attachInterrupt( digitalPinToInterrupt( k_pinINT ), wakeUp, RISING );
    g_bINT = false;
    g_uIntCount = 0;
    IMU.enableWakeOnMotion( 400, MPU9250::LP_ACCEL_ODR_0_24HZ );
}


void
loop()
{
    if ( g_bINT )
    {
        g_bINT = false;
        detachInterrupt( digitalPinToInterrupt( k_pinINT ) );
        Serial.print( "INT " );
        Serial.println( ++g_uIntCount );
        attachInterrupt( digitalPinToInterrupt( k_pinINT ), wakeUp, RISING );
        IMU.enableWakeOnMotion( 400, MPU9250::LP_ACCEL_ODR_0_24HZ );
    }
}
