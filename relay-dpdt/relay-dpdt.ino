
#include <YogiRelay.h>

const uint8_t k_pinRelayOn = 3;
const uint8_t k_pinRelayOff = 4;

const uint8_t kPinVibeLeft = 5;   // PWM
const uint8_t kPinVibeRight = 6;  // PWM


unsigned long g_uTimeCurrent = 0;
unsigned long g_uTimePrevious = 0;


YogiRelay g_tRelay( k_pinRelayOn, k_pinRelayOff );


void
setup()
{
    Serial.begin( 115200 );
    while ( ! Serial )
        ;
    delay( 400 );
    Serial.println( "Relay Test" );

    g_tRelay.init();
    g_uTimePrevious = millis();

    pinMode( LED_BUILTIN, OUTPUT );


    digitalWrite( kPinVibeLeft, HIGH );
    digitalWrite( kPinVibeRight, HIGH );
}


void
loop()
{
    g_uTimeCurrent = millis();
    if ( 4000 < g_uTimeCurrent - g_uTimePrevious )
    {
        g_uTimePrevious = g_uTimeCurrent;

        if ( g_tRelay.isSet() )
        {
            Serial.println( "set" );
            g_tRelay.reset();
            digitalWrite( LED_BUILTIN, LOW );
        }
        else
        {
            Serial.println( "reset" );
            g_tRelay.set();
            digitalWrite( LED_BUILTIN, HIGH );
        }
    }
}
