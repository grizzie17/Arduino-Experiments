#define USE_SW_SERIAL

#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>


const uint8_t k_pinRX = 7;
const uint8_t k_pinTX = 8;

SoftwareSerial g_tSerial( k_pinRX, k_pinTX );
Adafruit_GPS   g_tGPS( &g_tSerial );


// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences
#define GPSECHO false


unsigned long g_uTimeCurrent = 0;
unsigned long g_uTimePrevious = 0;


void
print2digit( int n )
{
    if ( n < 10 )
        Serial.print( "0" );
    Serial.print( n, DEC );
}


void
print3digit( int n )
{
    if ( n < 10 )
        Serial.print( "00" );
    else if ( n < 100 )
        Serial.print( "0" );
    Serial.print( n, DEC );
}


void
serialEvent()
{
    if ( Serial.available() )
        g_tSerial.write( Serial.read() );
}

unsigned long g_uCount = 0;

void
setup()
{
    Serial.begin( 115200 );
    while ( ! Serial )
        ;  // wait for serial port to startup

    // g_tSerial.begin( 9600 );
    // while ( ! g_tSerial )
    //     ;  // wait for software-serial to startup

    g_tGPS.begin( 9600 );

    // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
    //g_tGPS.sendCommand( PMTK_SET_NMEA_OUTPUT_RMCGGA );
    // uncomment this line to turn on only the "minimum recommended" data
    //g_tGPS.sendCommand( PMTK_SET_NMEA_OUTPUT_RMCONLY );
    // For parsing data, we don't suggest using anything but either RMC only or RMC+GGA since
    // the parser doesn't care about other sentences at this time

    // Set the update rate
    //g_tGPS.sendCommand( PMTK_SET_NMEA_UPDATE_1HZ );  // 1 Hz update rate
    // For the parsing code to work nicely and have time to sort thru the data, and
    // print it out we don't suggest using anything higher than 1 Hz

    // Request updates on antenna status, comment out to keep quiet
    //g_tGPS.sendCommand( PGCMD_ANTENNA );

    //delay( 1000 );
    // Ask for firmware version
    //g_tSerial.println( PMTK_Q_RELEASE );


    g_uTimePrevious = 0;
    g_uCount = 0;
}


void
loop()
{
    // if ( g_tSerial.available() )
    //     Serial.write( g_tSerial.read() );

    char c = g_tGPS.read();
    // if you want to debug, this is a good time to do it!
    if ( ( c ) && ( GPSECHO ) )
        Serial.write( c );


    // if a sentence is received, we can check the checksum, parse it...
    if ( g_tGPS.newNMEAreceived() )
    {
        // a tricky thing here is if we print the NMEA sentence, or data
        // we end up not listening and catching other sentences!
        // so be very wary if using OUTPUT_ALLDATA and trytng to print out data
        //Serial.println(GPS.lastNMEA());   // this also sets the newNMEAreceived() flag to false

        // this also sets the newNMEAreceived() flag to false
        if ( ! g_tGPS.parse( g_tGPS.lastNMEA() ) )
            return;  // we can fail to parse a sentence in which case we should just wait for another
    }

    g_uTimeCurrent = millis();
    if ( 1000 < g_uTimeCurrent - g_uTimePrevious )
    {
        g_uTimePrevious = g_uTimeCurrent;

        Serial.print( "20" );
        print2digit( g_tGPS.year );
        Serial.print( "-" );
        print2digit( g_tGPS.month );
        Serial.print( "-" );
        print2digit( g_tGPS.day );

        Serial.print( "T" );
        print2digit( g_tGPS.hour );
        Serial.print( ":" );
        print2digit( g_tGPS.minute );
        Serial.print( ":" );
        print2digit( g_tGPS.seconds );
        Serial.print( "." );
        print3digit( g_tGPS.minute );


        int nFix = g_tGPS.fix;
        Serial.print( " Fix: " );
        Serial.print( nFix );
        Serial.print( " quality: " );
        Serial.println( (int)g_tGPS.fixquality );
        if ( 0 != nFix )
        {
            Serial.print( "Location: " );
            Serial.print( g_tGPS.latitude, 4 );
            Serial.print( g_tGPS.lat );
            Serial.print( ", " );
            Serial.print( g_tGPS.longitude, 4 );
            Serial.println( g_tGPS.lon );

            Serial.print( "Speed (knots): " );
            Serial.println( g_tGPS.speed );
            Serial.print( "Angle: " );
            Serial.println( g_tGPS.angle );
            Serial.print( "Altitude: " );
            Serial.println( g_tGPS.altitude );
            Serial.print( "Satellites: " );
            Serial.println( (int)g_tGPS.satellites );
        }
        Serial.print( " #" );
        Serial.println( ++g_uCount );
    }
}
