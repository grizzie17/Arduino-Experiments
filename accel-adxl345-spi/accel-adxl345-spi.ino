#include <SparkFun_ADXL345.h>

#ifdef _MSC_VER
#    define __asm__ __asm
#endif


const uint8_t k_pinCS = 9;
const uint8_t k_pinSDA = 11;
const uint8_t k_pinSDO = 12;
const uint8_t k_pinSCL = 13;

const uint8_t k_pinINT = 2;  // INT0
const uint8_t k_pinLED = 7;

volatile unsigned int g_uInterrupt = 0;

// ADXL345 instantiation
ADXL345 adxl = ADXL345( k_pinCS );


void
debugStatus( char* sMessage )
{
    bool bSts = adxl.status;
    byte eCode = adxl.error_code;

    Serial.print( sMessage );
    if ( ADXL345_ERROR == bSts )
    {
        Serial.print( ": error-code=" );
        Serial.print( eCode, HEX );
    }
    else
    {
        Serial.print( ": Success" );
    }
    Serial.println( "" );
}


void
setup()
{
    Serial.begin( 115200 );
    while ( ! Serial )
        ;  // wait for serial startup

    pinMode( k_pinLED, OUTPUT );
    digitalWrite( k_pinLED, LOW );

    delay( 500 );
    Serial.println( "ADXL345 SPI test" );

    g_uInterrupt = 0;
    pinMode( k_pinINT, INPUT );
    attachInterrupt(
            digitalPinToInterrupt( k_pinINT ), adxlInterruptHandler, RISING );

    adxlSetup();

    Serial.println( "setup complete" );
}


void
loop()
{
    if ( 0 < g_uInterrupt )
    {
        g_uInterrupt = 0;
        Serial.println( "Interrupt" );
    }

    // int x, y, z;
    // adxl.readAccel( &x, &y, &z );
    // Serial.print( x );
    // Serial.print( "," );
    // Serial.print( y );
    // Serial.print( "," );
    // Serial.println( z );
}


void
adxlInterruptHandler()
{
    ++g_uInterrupt;
}


void
adxlSetup()
{
    Serial.println( "adxlSetup begin" );

    adxl.powerOn();
    debugStatus( "adxl.powerOn" );
    delay( 500 );

    adxl.setSpiBit( 0 );  // four wire mode
    debugStatus( "adxl.setSpiBit" );
    delay( 500 );

    // adxl.setRate( 50 );

    adxl.setRangeSetting( 2 );  // 2g
    debugStatus( "adxl.setRangeSetting" );

    adxl.setActivityXYZ( 1, 1, 1 );
    debugStatus( "adxl.setActivityXYZ" );
    adxl.setActivityThreshold( 20 );
    debugStatus( "adxl.setActivityThreshold" );

    adxl.setInactivityXYZ( 1, 1, 1 );
    debugStatus( "adxl.setInactivityXYZ" );
    adxl.setInactivityThreshold( 20 );
    debugStatus( "adxl.setInactivityThreshold" );
    adxl.setTimeInactivity( 20 );
    debugStatus( "adxl.setTimeInactivity" );

    adxl.setImportantInterruptMapping( 2, 2, 2, 2, 2 );

    adxl.InactivityINT( 1 );
    adxl.ActivityINT( 1 );
    adxl.FreeFallINT( 1 );
    adxl.doubleTapINT( 1 );
    adxl.singleTapINT( 1 );

    debugStatus( "adxlSetup end" );
}
