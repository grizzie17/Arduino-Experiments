#include "WatchDog.h"


const uint8_t k_pinLED = 4;


void
watchdogHandler()
{
    digitalWrite( k_pinLED, ! digitalRead( k_pinLED ) );
}


void
setup()
{
    Serial.begin( 115200 );
    while ( ! Serial )
        ;  // wait for Serial to start

    pinMode( k_pinLED, OUTPUT );
    digitalWrite( k_pinLED, LOW );
    WatchDog::init( watchdogHandler );
    WatchDog::setPeriod( OVF_1000MS );
    WatchDog::start();
}


void
loop()
{}
