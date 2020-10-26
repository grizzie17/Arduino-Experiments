
#include <YogiDebug.h>

#include <YogiDelay.h>
#include <YogiSonic.h>

const uint8_t kPinLED = 3;
const uint8_t kPinTrigger = 4;
const uint8_t kPinEcho = 4;

unsigned long g_uCount = 0;
const long    k_nMaxDistance = 30;

YogiDelay g_tDelay;
YogiSonic g_tSonic( kPinTrigger, kPinEcho );

void
setup()
{
    DEBUG_OPEN();
    pinMode( kPinLED, OUTPUT );
    analogWrite( kPinLED, LOW );
    g_tDelay.init( 2000 );
    g_tSonic.init();
    g_tSonic.setMaxDistance( k_nMaxDistance );

    g_uCount = 0;
}


void
loop()
{
    if ( g_tDelay.timesUp() )
    {
        uint8_t uBrite = 0;
        long    dist = g_tSonic.getDistanceCm();
        if ( 0 < dist )
        {
            uBrite = map( dist, k_nMaxDistance, 0, 0, 255 );
        }
        analogWrite( kPinLED, uBrite );

        DEBUG_VPRINT( "dist = ", dist );
        DEBUG_VPRINTLN( "; #", ++g_uCount );
    }
}