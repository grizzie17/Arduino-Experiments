#define _DEBUG 1
#include <YogiDebug.h>

#include <ADXL345_setup.h>  // includes SparkFun ADXL345 Library
#include <WatchDog.h>
#include <YogiSleep.h>

const uint8_t k_pinINT = 2;
const uint8_t k_pinLED = 5;  //LED_BUILTIN;
const uint8_t k_pinLAY = 6;  // LED indicating laying down
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


unsigned long g_uTimeCurrent = 0;
unsigned long g_uTimePrevious = 0;
unsigned long g_uTimeInterrupt = 0;
unsigned long g_uLaying = 0;

const unsigned long k_uDelaySleep = 680 * ADXL_SLEEP_DELAY;


bool g_bActiveLaydown = false;
bool g_bSleepy = false;


YogiSleep g_tSleep;

//============== ADXL =================

void
adxlAttachInterrupt()
{
    pinMode( k_pinINT, INPUT );
    attachInterrupt( digitalPinToInterrupt( k_pinINT ), adxlIntHandler, RISING );
}

void
adxlDetachInterrupt()
{
    detachInterrupt( digitalPinToInterrupt( k_pinINT ) );
}


void
adxlDrowsy()
{
    g_uCountInterrupt = 0;
    adxl.setInterruptMask( 0 );
    adxl.getInterruptSource();  // clear mInterrupts

    adxl.setInterruptMask( k_maskActivity );
    adxl.setLowPower( true );
    // adxlAttachInterrupt();
}


void
adxlSleep()
{
    adxlDetachInterrupt();
    g_uCountInterrupt = 0;
    adxl.getInterruptSource();  // clear mInterrupts
    adxl.setInterruptMask( 0 );
    adxl.setLowPower( true );
}


void
adxlWakeup()
{
    adxl.setLowPower( false );
    delay( 100 );
    adxl.setInterruptMask( k_maskAll );
    g_uCountInterrupt = 0;
    adxl.getInterruptSource();
    adxlAttachInterrupt();
    adxlIntHandler();
    g_nActivity = 0;
}


//============ WatchDog ===============

volatile bool g_bWatchDogInterrupt = false;

void
watchdogIntHandler()
{
    g_bWatchDogInterrupt = true;
}


void
watchdogSleep()
{
    DEBUG_PRINTLN( "Watchdog Sleep" );
    DEBUG_DELAY( 400 );
    adxlSleep();
    WatchDog::setPeriod( OVF_4000MS );
    WatchDog::start();
    g_tSleep.powerDown();
}


void
watchdogWakeup()
{
    WatchDog::stop();
    adxlWakeup();
    g_uTimeInterrupt = millis();
    DEBUG_PRINTLN( "Watchdog Wakeup" );
}


void
watchdogHandler()
{
    if ( isHorizontal() )
    {
        // go back to sleep
        g_eOrientation = OR_HORIZONTAL;
        adxlSleep();
        WatchDog::start();
        g_tSleep.sleep();
    }
    else
    {
        // cancel watchdog
        watchdogWakeup();
        g_uLaying = 0;
        g_bActiveLaydown = false;
        g_eOrientation = OR_VERTICAL;
    }
}


//============ LED ===============

void
ledOn()
{
    uint8_t m = adxl.getInterruptMask();
    digitalWrite( k_pinLED, HIGH );
    DEBUG_DELAY( 500 );
    adxl.setInterruptMask( m );
}


void
ledOff()
{
    uint8_t m = adxl.getInterruptMask();
    digitalWrite( k_pinLED, LOW );
    DEBUG_DELAY( 500 );
    adxl.getInterruptSource();
    adxl.setInterruptMask( m );
}


//============ states =============

bool
isHorizontal()
{
    int x, y, z;
    adxl.readAccel( &x, &y, &z );
    return abs( z ) < 100 ? true : false;
}


bool
isLayingdown()
{
    if ( isHorizontal() )
    {
        ++g_uLaying;
        return 1 < g_uLaying ? true : false;
    }
    else
    {
        g_uLaying = 0;
        return false;
    }
}


void
orientationUnknown()
{
    g_eOrientation = isHorizontal() ? OR_HORIZONTAL : OR_VERTICAL;
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
        uint8_t mInterrupts = 0;
        if ( 0 < g_uCountInterrupt )
        {
            mInterrupts = adxlGetInterrupts();
            g_uTimeInterrupt = millis();
        }
        else
        {
            if ( k_uDelaySleep < abs( g_uTimeCurrent - g_uTimeInterrupt ) )
            {
                adxl.getInterruptSource();
                g_uTimeInterrupt = g_uTimeCurrent;
                mInterrupts = ADXL_M_INACTIVITY;
            }
        }
        g_uCountInterrupt = 0;
        if ( 0 != mInterrupts )
        {
            ledOn();
            adxlDataHandler( mInterrupts );
            delay( 100 );
            if ( 0 != ( mInterrupts & ADXL_M_INACTIVITY ) )
            {
                DEBUG_PRINTLN( "Sleepy" );
                DEBUG_DELAY( 300 );
                ledOff();
                g_bSleepy = true;
                adxlDrowsy();
                g_tSleep.prepareSleep();
                adxlAttachInterrupt();
                g_tSleep.sleep();
                // wakeup here
                g_tSleep.postSleep();
                DEBUG_PRINTLN( "Wake UP" );
                ledOn();
                g_bSleepy = false;
                adxlWakeup();
                g_uTimePrevious = millis();
                g_uTimeInterrupt = millis();
            }
            g_uTimeInterrupt = millis();
        }

        g_uTimeCurrent = millis();
        if ( 1000 * 5 < abs( g_uTimeCurrent - g_uTimeInterrupt ) )
        {
            ledOff();
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

            ledOff();
            digitalWrite( k_pinLAY, LOW );
            watchdogSleep();
        }
    }
    else
    {
        if ( g_bActiveLaydown )
        {
            watchdogWakeup();

            g_uLaying = 0;
            g_bActiveLaydown = false;
            g_eOrientation = OR_VERTICAL;

            digitalWrite( k_pinLAY, LOW );
            ledOn();
        }
    }
}


/******************** SETUP ********************/
/*          Configure ADXL345 Settings         */
void
setup()
{
    DEBUG_OPEN( "SparkFun ADXL345 Accelerometer Hook Up Guide Example" );

    pinMode( k_pinLED, OUTPUT );
    digitalWrite( k_pinLED, HIGH );

    pinMode( k_pinLAY, OUTPUT );
    digitalWrite( k_pinLAY, LOW );

    // adxl = ADXL345();
    adxlSetup( 15, 18 );
    adxlAttachInterrupt();

    delay( 200 );
    g_eOrientation = isHorizontal() ? OR_HORIZONTAL : OR_VERTICAL;

    WatchDog::init( watchdogIntHandler, OVF_4000MS );
    WatchDog::stop();

    g_uTimePrevious = 0;
    g_uTimeInterrupt = millis();
    g_nActivity = 0;

    g_bActiveLaydown = false;

    DEBUG_PRINTLN( "setup complete" );
}


/****************** MAIN CODE ******************/
/*     Accelerometer Readings and Interrupt    */
void
loop()
{
    if ( g_bWatchDogInterrupt )
    {
        g_bWatchDogInterrupt = false;
        watchdogHandler();
    }
    else
    {
        switch ( g_eOrientation )
        {
        case OR_VERTICAL:
            orientationVertical();
            break;
        case OR_HORIZONTAL:
            orientationHorizontal();
            break;
        case OR_UNKNOWN:
        default:
            orientationUnknown();
            break;
        }
    }
}
