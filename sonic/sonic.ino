
#include <YogiDelay.h>
#include <YogiSonic.h>


const uint8_t kPinTrigger = 12;
const uint8_t kPinEcho = 11;

YogiDelay g_tDelay;
YogiSonic g_tSonic;

void
setup()
{
    Serial.begin( 9600 );
    g_tDelay.init( 1000 );
    g_tSonic.init( kPinTrigger, kPinEcho );
}


void
loop()
{
    if ( g_tDelay.timesUp() )
    {
        long dist = g_tSonic.getDistanceCm();

        Serial.print( "dist = " );
        Serial.println( dist );
    }
}