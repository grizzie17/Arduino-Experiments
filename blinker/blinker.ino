


typedef struct BlinkDelay
{
    int nDelay;
    int nMode;
} BlinkDelay;

BlinkDelay aList[] = { { 1000, HIGH }, { 1000, LOW },                         //
    { 250, HIGH }, { 500, LOW }, { 250, HIGH }, { 500, LOW }, { 250, HIGH },  //
    { 1000, LOW } };

unsigned long g_uTimePrevious = 0;
unsigned long g_uTimeCurrent = 0;
int           g_nIndex = 0;
int           g_nMax = sizeof( aList ) / sizeof( BlinkDelay );
unsigned long g_uTimeDelay = aList[0].nDelay;


void
setup()
{
    pinMode( LED_BUILTIN, OUTPUT );
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
        digitalWrite( LED_BUILTIN, aList[g_nIndex].nMode );
        g_uTimeDelay = aList[g_nIndex].nDelay;
        ++g_nIndex;
        if ( g_nMax <= g_nIndex )
            g_nIndex = 0;
    }
}