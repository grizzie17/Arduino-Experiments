#define _DEBUG
#include <YogiDebug.h>

// Hardware seutp:
#include <Wire.h>

#include <MPU9250.h>
#include <YogiSleep.h>


#ifdef _MSC_VER
#    define __asm__ __asm
#endif

#define MPU9250_ADDRESS 0x68
#define MAG_ADDRESS     0x0C


const uint8_t kPinAccel = 2;
const uint8_t kPinLED = LED_BUILTIN;

volatile bool g_bAccelInterrupt = false;
unsigned long g_uCountInterrupt = 0;
unsigned long g_uTimeInterrupt = 0;
unsigned long g_uTimeCurrent = 0;
unsigned long g_uTimePrevious = 0;


MPU9250 g_tIMU( Wire, MPU9250_ADDRESS );
// MPU9250 g_tIMU( SPI, 10 );
YogiSleep g_tSleep;


void
interruptHandler()
{
    g_bAccelInterrupt = true;
}


void
imuStatus( int status, const char* sMessage )
{
    if ( status < 0 )
    {
        DEBUG_PRINT( sMessage );
        DEBUG_PRINTLN( " unsuccessful" );
        DEBUG_PRINT( "status=" );
        DEBUG_PRINTLN( status );
    }
    else
    {
        DEBUG_PRINT( sMessage );
        DEBUG_PRINTLN( " successful" );
    }
}


void
enterSleep()
{
    DEBUG_PRINTLN( "Entering Sleep Mode" );

    g_tSleep.prepareSleep();
    // g_tSleep.sleepPinInterrupt( kPinAccel, RISING );
    imuStatus( g_tIMU.enableDataReadyInterrupt(), "IMU Data Ready Interrupt" );
    imuStatus( g_tIMU.enableWakeOnMotion( 50, MPU9250::LP_ACCEL_ODR_0_98HZ ),
            "IMU Wake on Motion" );
    attachInterrupt(
            digitalPinToInterrupt( kPinAccel ), interruptHandler, RISING );
    delay( 500 );
    g_tSleep.sleep();
    g_tSleep.postSleep();

    DEBUG_PRINTLN( "Wake Up" );
    // interruptHandler();
    // imuStatus( g_tIMU.disableDataReadyInterrupt(), "IMU Data Ready Interrupt" );
    // g_uTimeCurrent = millis();
}


void
setup()
{
    DEBUG_OPEN( "MPU9250 Test" );

    imuStatus( g_tIMU.begin(), "IMU begin" );

    delay( 100 );


    // imuStatus( g_tIMU.setAccelRange( MPU9250::ACCEL_RANGE_8G ),
    //         "IMU Accel Range" );

    // imuStatus( g_tIMU.setGyroRange( MPU9250::GYRO_RANGE_500DPS ),
    //         "IMU Gyro Range" );

    // imuStatus( g_tIMU.setDlpfBandwidth( MPU9250::DLPF_BANDWIDTH_5HZ ),
    //         "IMU Dlpf Bandwidth" );

    // g_tIMU.setSrd( 19 );


    // imuStatus( g_tIMU.disableDataReadyInterrupt(), "IMU data ready interrupt" );


    pinMode( kPinLED, OUTPUT );
    digitalWrite( kPinLED, LOW );

    g_bAccelInterrupt = false;
    pinMode( kPinAccel, INPUT );
    // digitalWrite( kPinAccel, LOW );

    imuStatus( g_tIMU.enableWakeOnMotion( 400, MPU9250::LP_ACCEL_ODR_0_24HZ ),
            "IMU wake on motion" );

    attachInterrupt(
            digitalPinToInterrupt( kPinAccel ), interruptHandler, RISING );
    g_uTimeCurrent = millis();
    g_uTimeInterrupt = millis();
    g_uTimePrevious = 0;

    DEBUG_PRINTLN( "setup complete" );
}


// void
// printXYZ()
// {
//     g_tIMU.readFifo();
//     float  ax[10];
//     float  ay[10];
//     float  az[10];
//     size_t nSamples;
//     g_tIMU.getFifoAccelX_mss( &nSamples, ax );
//     g_tIMU.getFifoAccelY_mss( &nSamples, ay );
//     g_tIMU.getFifoAccelZ_mss( &nSamples, az );

//     if ( 0 < nSamples )
//     {
//         Serial.print( "Accel: X=" );
//         Serial.print( ax[0] );
//         Serial.print( "; Y=" );
//         Serial.print( ay[0] );
//         Serial.print( "; Z=" );
//         Serial.println( az[0] );
//     }
// }


void
loop()
{
    g_uTimeCurrent = millis();
    if ( g_bAccelInterrupt )
    {
        g_bAccelInterrupt = false;
        // detachInterrupt( digitalPinToInterrupt( kPinAccel ) );
        DEBUG_PRINT( "Interrupt: " );
        DEBUG_PRINTLN( ++g_uCountInterrupt );
        g_tIMU.readSensor();

        digitalWrite( kPinLED, HIGH );

        float x = g_tIMU.getAccelX_mss();
        float y = g_tIMU.getAccelY_mss();
        float z = g_tIMU.getAccelZ_mss();
        bool  bOutput = false;

        if ( 0.0 != x || 0.0 != y || 0.0 != z )
        {
            bOutput = true;
            DEBUG_PRINT( "Accel: X=" );
            DEBUG_PRINT( x );
            DEBUG_PRINT( "; Y=" );
            DEBUG_PRINT( y );
            DEBUG_PRINT( "; Z=" );
            DEBUG_PRINT( z );
        }

        x = g_tIMU.getGyroX_rads();
        y = g_tIMU.getGyroY_rads();
        z = g_tIMU.getGyroZ_rads();

        if ( 0.0 != x || 0.0 != y || 0.0 != z )
        {
            if ( bOutput )
                DEBUG_PRINT( " ... " );
            bOutput = true;
            DEBUG_PRINT( "Gyro: X=" );
            DEBUG_PRINT( x );
            DEBUG_PRINT( "; Y=" );
            DEBUG_PRINT( y );
            DEBUG_PRINT( "; Z=" );
            DEBUG_PRINT( z );
        }

        if ( bOutput )
            DEBUG_PRINTLN( " ." );

        // printXYZ();

        g_uTimeInterrupt = millis();
        g_uTimePrevious = g_uTimeInterrupt;

        // g_tIMU.enableWakeOnMotion( 400, MPU9250::LP_ACCEL_ODR_0_24HZ );
        // attachInterrupt(
        //         digitalPinToInterrupt( kPinAccel ), interruptHandler, RISING );
    }
    else
    {
        if ( 1000 * 3 < abs( g_uTimeCurrent - g_uTimePrevious ) )
        {
            g_uTimePrevious = g_uTimeCurrent;
            g_uCountInterrupt = 0;
            digitalWrite( kPinLED, LOW );
        }
        if ( 0 < g_uTimeInterrupt )
        {
            if ( 1000 * 10 < abs( g_uTimeCurrent - g_uTimeInterrupt ) )
            {
                // enterSleep();
                g_uTimeInterrupt = millis();
            }
        }
    }
}
