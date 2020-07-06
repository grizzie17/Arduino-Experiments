#include <SparkFun_ADXL345.h>  // SparkFun ADXL345 Library

#ifdef _MSC_VER
#    define __asm__ __asm
#endif

const uint8_t ADXL_M_WATERMARK = ( 1 << ADXL345_INT_WATERMARK_BIT );
const uint8_t ADXL_M_DATA_READY = ( 1 << ADXL345_INT_DATA_READY_BIT );
const uint8_t ADXL_M_OVERRUNY = ( 1 << ADXL345_INT_OVERRUNY_BIT );
const uint8_t ADXL_M_ACTIVITY = ( 1 << ADXL345_INT_ACTIVITY_BIT );
const uint8_t ADXL_M_INACTIVITY = ( 1 << ADXL345_INT_INACTIVITY_BIT );
const uint8_t ADXL_M_SINGLE_TAP = ( 1 << ADXL345_INT_SINGLE_TAP_BIT );
const uint8_t ADXL_M_DOUBLE_TAP = ( 1 << ADXL345_INT_DOUBLE_TAP_BIT );
const uint8_t ADXL_M_FREE_FALL = ( 1 << ADXL345_INT_FREE_FALL_BIT );

const uint8_t k_maskInterrupt
        = ~uint8_t( ADXL_M_WATERMARK | ADXL_M_DATA_READY | ADXL_M_OVERRUNY );

const uint8_t k_maskActivity = ADXL_M_ACTIVITY | ADXL_M_SINGLE_TAP
        | ADXL_M_DOUBLE_TAP | ADXL_M_FREE_FALL;

const uint8_t k_pinINT = 2;
const uint8_t k_pinLED = LED_BUILTIN;

// volatile bool          g_bAccelInterrupt = false;
volatile unsigned int g_uCountInterrupt = 0;
volatile unsigned int g_uCopyInterrupt = 0;

unsigned long g_uTimeCurrent = 0;
unsigned long g_uTimePrevious = 0;
unsigned long g_uTimeInterrupt = 0;

unsigned long g_nActivity = 0;

/*********** COMMUNICATION SELECTION ***********/
/*    Comment Out The One You Are Not Using    */
// ADXL345 adxl = ADXL345( 10 );  // USE FOR SPI COMMUNICATION, ADXL345(CS_PIN);
ADXL345 adxl = ADXL345();  // USE FOR I2C COMMUNICATION

/****************** INTERRUPT ******************/
/*      Uncomment If Attaching Interrupt       */
// Setup pin 2 to be the interrupt pin (for most Arduino Boards)


void
accelIntHandler()
{
    ++g_uCountInterrupt;
}

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

    g_uCountInterrupt = 0;
    adxlSetup();

    g_nActivity = 0;
    pinMode( k_pinINT, INPUT );
    interrupts();
    attachInterrupt(
            digitalPinToInterrupt( k_pinINT ), accelIntHandler, RISING );

    g_uTimePrevious = 0;

    Serial.println( "setup complete" );
}


/****************** MAIN CODE ******************/
/*     Accelerometer Readings and Interrupt    */
void
loop()
{
    g_uTimeCurrent = millis();
    uint8_t mInterrupts = adxlGetInterrupts();

    if ( 0 != mInterrupts )
    {
        // Serial.print( "interrupt mask = " );
        // Serial.println( mInterrupts, BIN );
        adxlDataHandler( mInterrupts );


        g_uTimeInterrupt = millis();
        digitalWrite( k_pinLED, HIGH );
    }
    else
    {
        if ( 1000 * 2 < abs( g_uTimeCurrent - g_uTimeInterrupt ) )
        {
            digitalWrite( k_pinLED, LOW );
            g_nActivity = 0;
        }
    }
}

/********************* ISR *********************/
/* Look for Interrupts and Triggered Action    */

void
adxlSetup()
{
    Serial.println( "adxlSetup" );

    adxl.powerOn();  // Power on the ADXL345
    debugStatus( "adxl.powerOn" );


    adxl.setRangeSetting( 2 );
    debugStatus( "adxl.setRangeSetting(2)" );
    // Give the range settings
    // Accepted values are 2g, 4g, 8g or 16g
    // Higher Values = Wider Measurement Range
    // Lower Values = Greater Sensitivity


    // adxl.setRate( 100 );

    adxl.setInterruptLevelBit( 0 );
    // set interrupts to High

    // adxl.setSpiBit( 0 );
    // Configure the device to be in 4 wire SPI mode when set to '0' or 3 wire SPI mode when set to 1
    // Default: Set to 1
    // SPI pins on the ATMega328: 11, 12 and 13 as reference in SPI Library

    adxl.setActivityXYZ( 1, 1, 1 );
    debugStatus( "adxl.setActivityXYZ( 1, 1, 1 )" );
    // Set to activate movement detection in the axes "adxl.setActivityXYZ(X, Y, Z);" (1 == ON, 0 == OFF)
    adxl.setActivityThreshold( 20 );
    debugStatus( "adxl.setActivityThreshold( 16 )" );
    // 62.5mg per increment   // Set activity   // Inactivity thresholds (0-255)

    adxl.setInactivityXYZ( 1, 1, 1 );
    debugStatus( "adxl.setInactivityXYZ( 1, 1, 1 )" );
    // Set to detect inactivity in all the axes "adxl.setInactivityXYZ(X, Y, Z);" (1 == ON, 0 == OFF)
    adxl.setInactivityThreshold( 16 );
    debugStatus( "adxl.setInactivityThreshold( 20 )" );
    // 62.5mg per increment
    // Set inactivity
    // Inactivity thresholds (0-255)
    adxl.setTimeInactivity( 16 );
    // How many seconds of no activity is inactive?

    adxl.setTapDetectionOnXYZ( 1, 1, 1 );
    // Detect taps in the directions turned ON "adxl.setTapDetectionOnX(X, Y, Z);" (1 == ON, 0 == OFF)

    // Set values for what is considered a TAP and what is a DOUBLE TAP (0-255)
    adxl.setTapThreshold( 20 );      // 62.5 mg per increment
    adxl.setTapDuration( 15 );       // 625 μs per increment
    adxl.setDoubleTapLatency( 80 );  // 1.25 ms per increment
    adxl.setDoubleTapWindow( 200 );  // 1.25 ms per increment

    // Set values for what is considered FREE FALL (0-255)
    adxl.setFreeFallThreshold( 7 );
    // (5 - 9) recommended - 62.5mg per increment
    adxl.setFreeFallDuration( 30 );
    // (20 - 70) recommended - 5ms per increment

    // Setting all interupts to take place on INT2 pin
    adxl.setImportantInterruptMapping( 2, 2, 2, 2, 2 );
    // Sets "adxl.setEveryInterruptMapping(single tap, double tap, free fall, activity, inactivity);"
    // Accepts only 1 or 2 values for pins INT1 and INT2. This chooses the pin on the ADXL345 to use for Interrupts.
    // Default to INT1 pin.

    // Turn on Interrupts for each mode (1 == ON, 0 == OFF)
    adxl.InactivityINT( 1 );
    adxl.ActivityINT( 1 );
    adxl.FreeFallINT( 1 );
    adxl.doubleTapINT( 1 );
    adxl.singleTapINT( 1 );


    debugStatus( "adxl setup complete" );
}


uint8_t
adxlGetInterrupts()
{
    unsigned int n = 0;
    // noInterrupts();
    n = g_uCountInterrupt;
    if ( 0 != n )
    {
        adxl.InactivityINT( 0 );
        adxl.ActivityINT( 0 );
        adxl.FreeFallINT( 0 );
        adxl.doubleTapINT( 0 );
        adxl.singleTapINT( 0 );

        // noInterrupts();
        g_uCountInterrupt = 0;
        // interrupts();

        // Serial.print( "interrupt count = " );
        // Serial.println( n );
        int     x, y, z;
        uint8_t mInterrupts = 0;
        while ( 0 < n-- )
        {
            mInterrupts |= adxl.getInterruptSource();
            adxl.readAccel( &x, &y, &z );
        }

        if ( mInterrupts & k_maskActivity )
            adxl.InactivityINT( 1 );
        adxl.ActivityINT( 1 );
        adxl.FreeFallINT( 1 );
        adxl.doubleTapINT( 1 );
        adxl.singleTapINT( 1 );


        attachInterrupt(
                digitalPinToInterrupt( k_pinINT ), accelIntHandler, RISING );

        return mInterrupts & (uint8_t)k_maskInterrupt;
    }
    else
    {
        // interrupts();
        return 0;
    }
}


void
adxlDataHandler( uint8_t mInterrupts )
{
    int x, y, z;
    adxl.readAccel( &x, &y, &z );

    ++g_nActivity;
    // Free Fall Detection
    if ( adxl.triggered( mInterrupts, ADXL345_FREE_FALL ) )
    {
        Serial.print( "*** FREE FALL *** " );
        Serial.println( g_nActivity );
        //add code here to do when free fall is sensed
    }

    // Inactivity
    if ( adxl.triggered( mInterrupts, ADXL345_INACTIVITY ) )
    {
        Serial.print( "*** INACTIVITY *** " );
        Serial.println( g_nActivity );
        //add code here to do when inactivity is sensed
    }

    // Activity
    if ( adxl.triggered( mInterrupts, ADXL345_ACTIVITY ) )
    {
        Serial.print( "*** ACTIVITY *** " );
        Serial.println( g_nActivity );
        //add code here to do when activity is sensed
    }

    // Double Tap Detection
    if ( adxl.triggered( mInterrupts, ADXL345_DOUBLE_TAP ) )
    {
        Serial.print( "*** DOUBLE TAP *** " );
        Serial.println( g_nActivity );
        //add code here to do when a 2X tap is sensed
    }

    // Tap Detection
    if ( adxl.triggered( mInterrupts, ADXL345_SINGLE_TAP ) )
    {
        Serial.print( "*** TAP *** " );
        Serial.println( g_nActivity );
        //add code here to do when a tap is sensed
    }
}
