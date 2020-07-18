


void
setup()
{
    Serial.begin( 115200 );
    while ( ! Serial )
        ;  // let serial port startup
    Serial.flush();
    delay( 500 );

    Serial.println( "test" );
}


unsigned long g_uTimeCurrent = 0;
unsigned long g_uTimePrevious = 0;

void
loop()
{
    g_uTimeCurrent = millis();
    if ( 1000 < g_uTimeCurrent - g_uTimePrevious )
    {
        g_uTimePrevious = g_uTimeCurrent;
        Serial.print( "." );
    }
}
