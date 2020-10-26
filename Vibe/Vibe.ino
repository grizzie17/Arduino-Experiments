
const uint8_t k_pinVIBE = 11;


unsigned long g_uTimeCurrent = 0;
unsigned long g_uTimePrevious = 0;

bool    g_bVibe = false;
uint8_t g_intensity = 0;

void
setup()
{
    pinMode( k_pinVIBE, OUTPUT );
    g_bVibe = false;
    g_intensity = 255;
}


void
loop()
{
    g_uTimeCurrent = millis();
    if ( 500 < g_uTimeCurrent - g_uTimePrevious )
    {
        g_uTimePrevious = g_uTimeCurrent;

        if ( g_bVibe )
        {
            g_bVibe = false;
            analogWrite( k_pinVIBE, 0 );
        }
        else
        {
            g_bVibe = true;
            analogWrite( k_pinVIBE, g_intensity );
            g_intensity /= 2;
            if ( g_intensity < 16 )
                g_intensity = 255;
        }
    }
}
