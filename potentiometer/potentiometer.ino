

// GND - right hand pin
// VCC - (5V) left hand pin
// signal - middle pin hooked up to analog-in

int kPinPot = A0;

void
setup()
{
    Serial.begin( 9600 );
}


void
loop()
{
    long nPotValue = analogRead( kPinPot );

    Serial.print( "pot-value = " );
    Serial.println( nPotValue * 256 / 1023 );

    delay( 500 );
}
