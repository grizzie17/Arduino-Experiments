#include <ADXL345_setup.h>  // includes SparkFun ADXL345 Library

const uint8_t k_pinINT = 2;
const uint8_t k_pinLED = 5;  //LED_BUILTIN;
const uint8_t k_pinLAY = 4;
const uint8_t kPinSDA = A4;
const uint8_t kPinSCL = A5;

#ifdef _MSC_VER
#    define __asm__ __asm
#endif


typedef enum orientation_t
{
    OR_UNKNOWN,
    OR_VERTICAL,
    OR_HORIZONTAL
} orientation_t;

orientation_t g_eOrientation = OR_UNKNOWN;


unsigned long g_uTimeCurrent = 0;
unsigned long g_uTimePrevious = 0;
unsigned long g_uTimeInterrupt = 0;


/*********** COMMUNICATION SELECTION ***********/
/*    Comment Out The One You Are Not Using    */
// ADXL345 adxl = ADXL345( 10 );  // USE FOR SPI COMMUNICATION, ADXL345(CS_PIN);
// ADXL345 adxl;  // USE FOR I2C COMMUNICATION
// instantiation done in ADXL345_setup


bool g_bActiveLaydown = false;


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

    g_uCountInterrupt = 0;
    adxl = ADXL345();
    adxlSetup();

    pinMode( k_pinINT, INPUT );
    g_eOrientation = OR_UNKNOWN;
    attachInterrupt(
            digitalPinToInterrupt( k_pinINT ), adxlIntHandler, CHANGE );

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
        }
    }
    else
    {
        if ( g_bActiveLaydown )
        {
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
