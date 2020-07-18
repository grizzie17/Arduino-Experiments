#include <ADXL345_setup.h>  // includes SparkFun ADXL345 Library
#include <WatchDog.h>
#include <YogiSleep.h>

const uint8_t k_pinINT = 2;
const uint8_t k_pinLED = 5;  //LED_BUILTIN;
const uint8_t k_pinLAY = 4;
const uint8_t kPinSDA = A4;
const uint8_t kPinSCL = A5;

#ifdef _MSC_VER
#    define __asm__ __asm
#endif

typedef enum orientation_t
        : uint8_t
{
    OR_UNKNOWN,
    OR_VERTICAL,
    OR_HORIZONTAL
} orientation_t;

orientation_t g_eOrientation = OR_UNKNOWN;


volatile bool g_bWatchDogInterrupt = false;

unsigned long g_uTimeCurrent = 0;
unsigned long g_uTimePrevious = 0;
unsigned long g_uTimeInterrupt = 0;

bool g_bActiveLaydown = false;
bool g_bSleepy = false;


YogiSleep g_tSleep;


void
watchdogIntHandler()
{
    g_bWatchDogInterrupt = true;
}


void
watchdogSleepNow()
{
    Serial.println( "watchdog Sleep Now " );
    delay( 200 );
    g_bSleepy = true;
    WatchDog::setPeriod( OVF_8000MS );
    WatchDog::start();
    g_tSleep.prepareSleep();
    g_tSleep.powerDown();

    // wakeup here
    WatchDog::stop();
}


/******************** SETUP ********************/
/*          Configure ADXL345 Settings         */
void
setup()
{
    Serial.begin( 115200 );
    while ( ! Serial )
        ;  // delay while serial starts up
    delay( 600 );

    Serial.println( "SparkFun ADXL345 Accelerometer Hook Up Guide Example" );


    pinMode( k_pinLED, OUTPUT );
    digitalWrite( k_pinLED, LOW );

    pinMode( k_pinLAY, OUTPUT );
    digitalWrite( k_pinLAY, LOW );

    adxl = ADXL345();
    adxlSetup();

    pinMode( k_pinINT, INPUT );
    g_eOrientation = OR_UNKNOWN;
    attachInterrupt(
            digitalPinToInterrupt( k_pinINT ), adxlIntHandler, CHANGE );

    WatchDog::init( watchdogIntHandler, OVF_4000MS );

    g_uTimePrevious = 0;

    g_bActiveLaydown = false;

    Serial.println( "setup complete" );
}


bool
isLayingdown()
{
    int x, y, z;
    adxl.readAccel( &x, &y, &z );
    return abs( z ) < 100 ? true : false;
}


void
orientationUnknown()
{
    g_eOrientation = isLayingdown() ? OR_HORIZONTAL : OR_VERTICAL;
}


void
orientationVertical()
{
    if ( isLayingdown() )
    {
        g_eOrientation = OR_HORIZONTAL;
    }
    else
    {
        g_uTimeCurrent = millis();
        uint8_t mInterrupts = adxlGetInterrupts();
        if ( 0 != mInterrupts )
        {
            digitalWrite( k_pinLED, HIGH );
            adxlDataHandler( mInterrupts );
            g_uTimeInterrupt = millis();
        }
    }
}

void
orientationHorizontal()
{
    // Check if we are laying-down. If we are not
    // laying-down then switch modes to OR_VERTICAL.
    if ( isLayingdown() )
    {
        if ( ! g_bActiveLaydown )
        {
            g_bActiveLaydown = true;

            digitalWrite( k_pinLED, LOW );
            digitalWrite( k_pinLAY, HIGH );
            adxl.setInterruptMask( 0 );
            watchdogSleepNow();
        }
        else
        {
            g_bWatchDogInterrupt = false;
            WatchDog::start();
        }
    }
    else
    {
        if ( g_bActiveLaydown )
        {
            Serial.println( "watchdog Wakeup" );
            WatchDog::stop();
            g_bActiveLaydown = false;
            g_eOrientation = OR_VERTICAL;

            digitalWrite( k_pinLAY, LOW );
            adxl.setInterruptMask( k_maskAll );
        }
    }
}


/****************** MAIN CODE ******************/
/*     Accelerometer Readings and Interrupt    */
void
loop()
{
    switch ( g_eOrientation )
    {
    case OR_UNKNOWN:
        orientationUnknown();
        break;
    case OR_VERTICAL:
        orientationVertical();
        break;
    case OR_HORIZONTAL:
        orientationHorizontal();
        break;
    default:
        break;
    }


    g_uTimeCurrent = millis();
    if ( 1000 * 5 < abs( g_uTimeCurrent - g_uTimePrevious ) )
    {
        g_uTimePrevious = g_uTimeCurrent;

        digitalWrite( k_pinLED, LOW );
    }
}
