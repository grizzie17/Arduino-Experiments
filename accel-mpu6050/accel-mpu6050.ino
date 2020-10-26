
// #include <I2Cdev.h>
#include <MPU6050_6Axis_MotionApps_V6_12.h>
//#include "MPU6050.h" // not necessary if using MotionApps include file
// #include <Wire.h>


#define MASK( btx ) ( 0x01 << btx )

const uint8_t k_pinINT = 2;
const uint8_t k_pinLED = LED_BUILTIN;

const float k_PI = 3.14159265358979323846f;

// MPU control/status vars
MPU6050 mpu;

bool     g_bDumpReady = false;  // set true if DMP init was successful
uint8_t  g_uMpuIntStatus = 0;   // holds actual interrupt status byte from MPU
uint8_t  g_uDeviceStatus = 0;   // device operation status (0=success, !0=error)
uint16_t g_uPacketSize = 0;     // expected DMP packet size (default == 42 bytes)
uint16_t g_uFifoCount = 0;      // count of all bytes currently in FIFO
uint8_t  g_aFifoBuffer[128];

// orientation/motion vars
Quaternion  g_q;
VectorInt16 g_aa;
VectorInt16 g_gy;
VectorInt16 g_aaReal;
VectorFloat g_gravity;
float       g_ypr[3];  // yaw/pitch/roll


// Interrupt
volatile bool g_bINT = false;

void
interruptHandler()
{
    g_bINT = true;
}

void
printData()
{
    int16_t ax, ay, az;
    int16_t gx, gy, gz;

    mpu.getMotion6( &ax, &ay, &az, &gx, &gy, &gz );
    int16_t temp = mpu.getTemperature();

    /* Print out the values */
    Serial.print( "Acceleration X: " );
    Serial.print( ax );
    Serial.print( ", Y: " );
    Serial.print( ay );
    Serial.print( ", Z: " );
    Serial.print( az );
    Serial.println( " m/s^2" );

    Serial.print( "Rotation X: " );
    Serial.print( gx );
    Serial.print( ", Y: " );
    Serial.print( gy );
    Serial.print( ", Z: " );
    Serial.print( gz );
    Serial.println( " rad/s" );

    Serial.print( "Temperature: " );
    Serial.print( temp );
    Serial.println( " degC" );

    Serial.println( "" );
}

unsigned long g_uIntCount = 0;

void
setup()
{
    g_bDumpReady = false;
    Wire.begin();
    // Wire.setClock( 400000 );


    Serial.begin( 115200 );
    while ( ! Serial )
        delay( 10 );

    delay( 600 );
    Serial.println( "MPU6050 w/INT" );


    mpu.initialize();
    g_bINT = false;
    pinMode( k_pinINT, INPUT );

    Serial.println( "Testing device connections..." );
    Serial.println( mpu.testConnection() ? "MPU6050 connection successful"
                                         : "MPU6050 connection failed" );

    // mpu.setIntEnabled( MASK( MPU6050_INTERRUPT_DMP_INT_BIT ) );
    g_uDeviceStatus = mpu.dmpInitialize();
    Serial.println();

    mpu.setXGyroOffset( -5 );
    mpu.setYGyroOffset( 18 );
    mpu.setZGyroOffset( -46 );

    // mpu.setXAccelOffset( -534 );
    // mpu.setYAccelOffset( -359 );
    mpu.setZAccelOffset( 1157 );


    if ( 0 == g_uDeviceStatus )
    {
        Serial.println( "initializing DMP" );

        mpu.setMotionDetectionThreshold( 20 );
        mpu.setMotionDetectionDuration( 40 );
        mpu.setMotionDetectionCounterDecrement( 0x15 );
        mpu.setAccelXSelfTest( true );
        mpu.setAccelYSelfTest( true );
        mpu.setAccelZSelfTest( true );
        mpu.CalibrateAccel( 6 );
        mpu.CalibrateGyro( 6 );

        // mpu.PrintActiveOffsets();

        mpu.setDMPEnabled( true );
        g_uMpuIntStatus = mpu.getIntStatus();

        g_bDumpReady = true;

        g_uPacketSize = mpu.dmpGetFIFOPacketSize();
        if ( sizeof( g_aFifoBuffer ) < g_uPacketSize )
        {
            Serial.println( "potential buffer overflow" );
            g_uPacketSize = sizeof( g_aFifoBuffer );
        }
    }

    attachInterrupt( digitalPinToInterrupt( k_pinINT ), interruptHandler, RISING );

    g_uIntCount = 0;
    pinMode( k_pinLED, OUTPUT );
    digitalWrite( k_pinLED, LOW );

    // Serial.println( "Waiting for internal calibration" );
    // delay( 20 * 1000 );
    Serial.println( "done" );
}


void
loop()
{
    if ( ! g_bDumpReady )
        return;

    if ( g_bINT )
    {
        ++g_uIntCount;
        g_bINT = false;
        detachInterrupt( digitalPinToInterrupt( k_pinINT ) );
        if ( 80 < g_uIntCount )
        {
            Serial.println();
            g_uIntCount = 0;
        }
        // Serial.print( "." );
        // Serial.println( g_uIntCount );

        g_uMpuIntStatus = mpu.getIntStatus();
        g_uFifoCount = mpu.getFIFOCount();

        // clear overflow condition
        if ( ( g_uMpuIntStatus & MASK( MPU6050_INTERRUPT_FIFO_OFLOW_BIT ) )
                || 1024 <= g_uFifoCount )
        {
            Serial.println( "+" );
            g_uIntCount = 0;
            while ( g_uMpuIntStatus & MASK( MPU6050_INTERRUPT_FIFO_OFLOW_BIT ) )
            {
                mpu.setIntDMPEnabled( false );
                mpu.resetFIFO();
                g_uFifoCount = mpu.getFIFOCount();
                mpu.setIntDMPEnabled( true );
                g_uMpuIntStatus = mpu.getIntStatus();
            }
        }

        if ( g_uMpuIntStatus & MASK( MPU6050_INTERRUPT_DMP_INT_BIT ) )
        {
            g_uIntCount = 0;
            while ( g_uFifoCount < g_uPacketSize )
                g_uFifoCount = mpu.getFIFOCount();

            mpu.getFIFOBytes( g_aFifoBuffer, g_uPacketSize );
            g_uFifoCount -= g_uPacketSize;

            mpu.dmpGetQuaternion( &g_q, g_aFifoBuffer );
            mpu.dmpGetAccel( &g_aa, g_aFifoBuffer );
            mpu.dmpGetGravity( &g_gravity, &g_q );
            mpu.dmpGetYawPitchRoll( g_ypr, &g_q, &g_gravity );
            mpu.dmpGetLinearAccel( &g_aaReal, &g_aa, &g_gravity );

            // Serial.println();
            Serial.print( "ypr: " );
            Serial.print( g_ypr[0] * 180.0f / k_PI );
            Serial.print( ", " );
            Serial.print( g_ypr[1] * 180.0f / k_PI );
            Serial.print( ", " );
            Serial.print( g_ypr[2] * 180.0f / k_PI );
            Serial.print( ";  " );

            Serial.print( "accel: " );
            Serial.print( g_aaReal.x );
            Serial.print( ", " );
            Serial.print( g_aaReal.y );
            Serial.print( ", " );
            Serial.print( g_aaReal.z );
            Serial.println();
        }
        attachInterrupt( digitalPinToInterrupt( k_pinINT ), interruptHandler, RISING );
    }
}
