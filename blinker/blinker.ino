
const uint8_t kPinLed = 13;  // PWM

typedef struct BlinkDelay
{
    int     nDelay;
    uint8_t nMode;
} BlinkDelay;

BlinkDelay aList[] = {
    //
    { 2000, 255 }, { 1000, 0 },  //
    { 1000, 127 }, { 1000, 0 },  //
    { 1000, 31 }, { 1000, 0 },   //
    { 250, 255 }, { 500, 0 },    //
    { 250, 127 }, { 500, 0 },    //
    { 2000, 63 }, { 1000, 0 },   //
    { 720, 17 }, { 1000, 0 }
};

unsigned long g_uTimePrevious = 0;
unsigned long g_uTimeCurrent = 0;
int           g_nIndex = 0;
int           g_nMax = sizeof( aList ) / sizeof( BlinkDelay );
unsigned long g_uTimeDelay = aList[0].nDelay;


void
setup()
{
    pinMode( kPinLed, OUTPUT );
    analogWrite( kPinLed, 0 );
    g_uTimePrevious = 0;
    g_nIndex = 0;
    g_uTimeDelay = aList[0].nDelay;
}


void
loop()
{
    g_uTimeCurrent = millis();
    if ( g_uTimeDelay < g_uTimeCurrent - g_uTimePrevious )
    {
        g_uTimePrevious = g_uTimeCurrent;
        analogWrite( kPinLed, aList[g_nIndex].nMode );
        g_uTimeDelay = aList[g_nIndex].nDelay;
        ++g_nIndex;
        if ( g_nMax <= g_nIndex )
            g_nIndex = 0;
    }
}