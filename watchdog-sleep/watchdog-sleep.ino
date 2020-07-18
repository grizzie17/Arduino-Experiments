#include <WatchDog.h>
#include <YogiSleep.h>

const uint8_t k_pinLED = 4;

volatile bool g_bWdInterrupt = false;
volatile bool g_bSleepy = false;

unsigned long g_uTimeCurrent = 0;
unsigned long g_uTimePrevious = 0;

YogiSleep g_tSleep;

void
watchdogHandler()
{
    g_bWdInterrupt = true;
}


void
sleepNow()
{
    Serial.println( "Sleep Now" );
    delay( 200 );
    g_bSleepy = true;
    digitalWrite( k_pinLED, LOW );
    WatchDog::setPeriod( OVF_4000MS );
    WatchDog::start();
    g_tSleep.prepareSleep();
    g_tSleep.powerDown();

    // we will resume here
    Serial.println( "Wakeup" );
    WatchDog::stop();
}


void
wakeupNow()
{
    g_bSleepy = false;
    Serial.println( "Wakeup Now" );
    digitalWrite( k_pinLED, HIGH );
    WatchDog::stop();
}


void
setup()
{
    Serial.begin( 115200 );
    while ( ! Serial )
        ;  // wait for Serial

    pinMode( k_pinLED, OUTPUT );
    digitalWrite( k_pinLED, HIGH );
    WatchDog::init( watchdogHandler, OVF_4000MS );
    // WatchDog::start();

    g_uTimePrevious = 0;
}


void
loop()
{
    if ( g_bWdInterrupt )
    {
        g_bWdInterrupt = false;
        if ( g_bSleepy )
            wakeupNow();
    }
    else
    {
        g_uTimeCurrent = millis();
        if ( 2000 < abs( g_uTimeCurrent - g_uTimePrevious ) )
        {
            g_uTimePrevious = g_uTimeCurrent;
            if ( ! g_bSleepy )
                sleepNow();
        }
    }
}
