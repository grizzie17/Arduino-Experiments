
const uint8_t k_pinStatus = 13;
const uint8_t k_pinSense = 11;
const uint8_t k_pinGate = 10;
const uint8_t k_pinRelay = 9;


bool
relayIsSet()
{
    bool bResult = false;

    digitalWrite( k_pinGate, HIGH );
    delay( 5 );
    bResult = LOW != digitalRead( k_pinSense );
    digitalWrite( k_pinGate, LOW );

    if ( bResult )
        Serial.println( "Relay is set" );
    else
        Serial.println( "Relay is not set" );

    return bResult;
}


bool
relaySet()
{
    Serial.println( "relaySet" );
    bool bResult = false;
    digitalWrite( k_pinGate, HIGH );
    delay( 5 );
    digitalWrite( k_pinRelay, HIGH );
    for ( int i = 0; i < 10; ++i )
    {
        delay( 5 );
        if ( HIGH == digitalRead( k_pinSense ) )
        {
            Serial.println( "Relay SET" );
            bResult = true;
            break;
        }
    }
    digitalWrite( k_pinGate, LOW );
    return bResult;
}


bool
relayReset()
{
    Serial.println( "relayReset" );
    bool bResult = false;
    digitalWrite( k_pinGate, HIGH );
    delay( 5 );
    digitalWrite( k_pinRelay, LOW );
    for ( int i = 0; i < 10; ++i )
    {
        if ( HIGH != digitalRead( k_pinSense ) )
        {
            Serial.println( "Relay RESET" );
            bResult = true;
            break;
        }
    }
    digitalWrite( k_pinGate, LOW );
    return bResult;
}


unsigned long g_uTimeCurrent = 0;
unsigned long g_uTimePrevious = 0;


void
setup()
{
    Serial.begin( 115200 );
    while ( ! Serial )
        ;
    delay( 300 );

    Serial.println( "Feedback Voltage" );

    pinMode( k_pinSense, INPUT );

    pinMode( k_pinGate, OUTPUT );
    digitalWrite( k_pinGate, LOW );

    pinMode( k_pinRelay, OUTPUT );
    digitalWrite( k_pinRelay, LOW );

    pinMode( k_pinStatus, OUTPUT );
    digitalWrite( k_pinStatus, HIGH );

    g_uTimeCurrent = millis();
}


void
loop()
{
#if false
    if ( LOW == digitalRead( k_pinSense ) )
    {
        digitalWrite( k_pinStatus, LOW );
    }
    else
    {
        digitalWrite( k_pinStatus, HIGH );
    }
    delay( 200 );
#else
    g_uTimeCurrent = millis();
    if ( 3000 < g_uTimeCurrent - g_uTimePrevious )
    {
        g_uTimePrevious = g_uTimeCurrent;

        if ( relayIsSet() )
        {
            digitalWrite( k_pinStatus, LOW );
            relayReset();
        }
        else
        {
            digitalWrite( k_pinStatus, HIGH );
            relaySet();
        }
    }
#endif
}
