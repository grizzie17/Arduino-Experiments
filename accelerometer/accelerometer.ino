#include <MPU9250.h>
#include <Sleep_n0m1.h>


#ifdef _MSC_VER
#    define __asm__ __asm
#endif

#define MPU9250_ADDRESS 0x68
#define MAG_ADDRESS     0x0C


const uint8_t kPinAccel = 2;

volatile bool g_bAccelInterrupt = false;
unsigned long g_uCountInterrupt = 0;
unsigned long g_uTimeInterrupt = 0;
unsigned long g_uTimeCurrent = 0;
unsigned long g_uTimePrevious = 0;


MPU9250 g_tIMU( Wire, 0x68 );
Sleep   g_tSleep;


void
enterSleep()
{
    Serial.println( "Entering Sleep Mode" );
    delay( 500 );
    g_tSleep.pwrDownMode();
    g_tSleep.sleepPinInterrupt( kPinAccel, RISING );
    Serial.println( "Wake Up" );
    interruptHandler();
    attachInterrupt(
            digitalPinToInterrupt( kPinAccel ), interruptHandler, RISING );
    g_uTimeCurrent = millis();
    g_uCountInterrupt = 0;
}


void
interruptHandler()
{
    g_bAccelInterrupt = true;
}

void
setup()
{
    Serial.begin( 115200 );
    while ( ! Serial )
        ;

    int status = g_tIMU.begin();
    if ( status < 0 )
    {
        Serial.println( "IMU initialization unsuccessful" );
    }

    g_tIMU.setAccelRange( MPU9250::ACCEL_RANGE_2G );
    g_tIMU.setSrd( 249 );

    // g_tIMU.setSrd( 19 );
    // g_tIMU.enableDataReadyInterrupt();
    status = g_tIMU.enableWakeOnMotion( 400, MPU9250::LP_ACCEL_ODR_0_98HZ );
    if ( status < 0 )
    {
        Serial.println( "IMU enable wake on motion failure" );
    }


    g_bAccelInterrupt = false;
    pinMode( kPinAccel, INPUT );
    digitalWrite( kPinAccel, LOW );
    // interrupts();
    attachInterrupt(
            digitalPinToInterrupt( kPinAccel ), interruptHandler, RISING );
    g_uTimeCurrent = millis();
}


void
loop()
{
    g_uTimeCurrent = millis();
    if ( g_bAccelInterrupt )
    {
        // g_tIMU.readSensor();
        g_bAccelInterrupt = false;
        g_uTimeInterrupt = millis();
        if ( 1000 * 2 < g_uTimeCurrent - g_uTimePrevious )
        {
            g_uTimePrevious = g_uTimeCurrent;
            Serial.print( "Interrupt: " );
            Serial.println( ++g_uCountInterrupt );
        }
    }
    else
    {
        if ( 1000 * 10 < g_uTimeCurrent - g_uTimeInterrupt )
        {
            enterSleep();
            g_uTimeInterrupt = millis();
        }
    }
}
