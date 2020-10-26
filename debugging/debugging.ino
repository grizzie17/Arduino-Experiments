#include <app_api.h>
#include <avr8-stub.h>


const uint8_t k_pinLED = 5;


void
setup()
{
    debug_init();
    pinMode( k_pinLED, OUTPUT );
}


void
loop()
{
    digitalWrite( k_pinLED, HIGH );
    delay( 1000 );
    digitalWrite( k_pinLED, LOW );
    delay( 500 );
}
