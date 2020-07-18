// Hardware seutp:

#include <MPU9250.h>
// #include <SPI.h>
#include <Sleep_n0m1.h>


#ifdef _MSC_VER
#    define __asm__ __asm
#endif

#define MPU9250_ADDRESS 0x68
#define MAG_ADDRESS     0x0C


const uint8_t kPinAccel = 2;
const uint8_t kPinLED = LED_BUILTIN;

volatile bool g_bAccelInterrupt = false;
unsigned long g_uCountInterrupt = 0;
unsigned long g_uTimeInterrupt = 0;
unsigned long g_uTimeCurrent = 0;
unsigned long g_uTimePrevious = 0;


MPU9250 g_tIMU( Wire, MPU9250_ADDRESS );
// MPU9250 g_tIMU( SPI, 10 );
Sleep g_tSleep;


void
enterSleep()
{
    Serial.println( "Entering Sleep Mode" );

    imuStatus( g_tIMU.enableWakeOnMotion( 50, MPU9250::LP_ACCEL_ODR_0_98HZ ),
            "IMU Wake on Motion" );
    // imuStatus( g_tIMU.enableDataReadyInterrupt(), "IMU Data Ready Interrupt" );
    delay( 500 );

    g_tSleep.pwrDownMode();
    g_tSleep.sleepPinInterrupt( kPinAccel, RISING );

    Serial.println( "Wake Up" );
    // interruptHandler();
    // imuStatus( g_tIMU.disableDataReadyInterrupt(), "IMU Data Ready Interrupt" );
    attachInterrupt(
            digitalPinToInterrupt( kPinAccel ), interruptHandler, RISING );
    // g_uTimeCurrent = millis();
}


void
interruptHandler()
{
    g_bAccelInterrupt = true;
}

void
imuStatus( int status, const char* sMessage )
{
    if ( status < 0 )
    {
        Serial.print( sMessage );
        Serial.println( " unsuccessful" );
        Serial.print( "status=" );
        Serial.println( status );
    }
    else
    {
        Serial.print( sMessage );
        Serial.println( " successful" );
    }
}

void
setup()
{
    Serial.begin( 115200 );
    while ( ! Serial )
        ;

    delay( 500 );

    Serial.println( "MPU9250 Test" );

    imuStatus( g_tIMU.begin(), "IMU begin" );

    // g_tIMU.calibrateAccel();
    // g_tIMU.calibrateGyro();
    // g_tIMU.calibrateMag();

    // imuStatus(
    //         g_tIMU.enableFifo( true, true, false, false ), "IMU enable Fifo" );

    imuStatus( g_tIMU.setDlpfBandwidth( MPU9250::DLPF_BANDWIDTH_5HZ ),
            "IMU Dlpf Bandwidth" );

    g_tIMU.setSrd( 249 );

    imuStatus( g_tIMU.setAccelRange( MPU9250::ACCEL_RANGE_2G ),
            "IMU Accel Range" );

    imuStatus( g_tIMU.setGyroRange( MPU9250::GYRO_RANGE_250DPS ),
            "IMU Gyro Range" );


    // g_tIMU.calibrateAccel();
    // g_tIMU.calibrateAccel();
    // g_tIMU.calibrateAccel();
    // g_tIMU.calibrateAccel();
    // g_tIMU.calibrateAccel();
    // g_tIMU.calibrateAccel();
    // float fScale = g_tIMU.getAccelScaleFactorZ();
    // Serial.print( "Accel Scale = " );
    // Serial.println( fScale );
    // float fBias = g_tIMU.getAccelBiasZ_mss();
    // Serial.print( "Accel Bias = " );
    // Serial.println( fBias );

    // fScale = 1.0;
    // float fBiasX = .00001;
    // float fBiasY = .00001;
    // float fBiasZ = 0.01;
    // float fScaleX = -1.0 / 38.6;
    // float fScaleY = -1.0 / 39.2;
    // float fScaleZ = -1.0 / 7.3;
    // g_tIMU.setAccelCalX( fBiasX, fScaleX );
    // g_tIMU.setAccelCalY( fBiasY, fScaleY );
    // g_tIMU.setAccelCalZ( fBiasZ, fScaleZ );

    // g_tIMU.setMagCalX( 0.0, 1.0 );
    // g_tIMU.setMagCalY( 0.0, 1.0 );
    // g_tIMU.setMagCalZ( 0.0, 1.0 );


    imuStatus( g_tIMU.enableDataReadyInterrupt(), "IMU data ready interrupt" );

    imuStatus( g_tIMU.enableWakeOnMotion( 50, MPU9250::LP_ACCEL_ODR_7_81HZ ),
            "IMU wake on motion" );


    pinMode( kPinLED, OUTPUT );
    digitalWrite( kPinLED, LOW );

    g_bAccelInterrupt = false;
    pinMode( kPinAccel, INPUT );
    // digitalWrite( kPinAccel, LOW );
    attachInterrupt(
            digitalPinToInterrupt( kPinAccel ), interruptHandler, RISING );
    g_uTimeCurrent = millis();
    g_uTimeInterrupt = millis();
    g_uTimePrevious = 0;

    Serial.println( "setup complete" );
}


// void
// printXYZ()
// {
//     g_tIMU.readFifo();
//     float  ax[10];
//     float  ay[10];
//     float  az[10];
//     size_t nSamples;
//     g_tIMU.getFifoAccelX_mss( &nSamples, ax );
//     g_tIMU.getFifoAccelY_mss( &nSamples, ay );
//     g_tIMU.getFifoAccelZ_mss( &nSamples, az );

//     if ( 0 < nSamples )
//     {
//         Serial.print( "Accel: X=" );
//         Serial.print( ax[0] );
//         Serial.print( "; Y=" );
//         Serial.print( ay[0] );
//         Serial.print( "; Z=" );
//         Serial.println( az[0] );
//     }
// }


void
loop()
{
    g_uTimeCurrent = millis();
    if ( g_bAccelInterrupt )
    {
        g_bAccelInterrupt = false;
        Serial.print( "Interrupt: " );
        Serial.println( ++g_uCountInterrupt );
        g_tIMU.readSensor();

        digitalWrite( kPinLED, HIGH );
        float x = g_tIMU.getAccelX_mss();
        float y = g_tIMU.getAccelY_mss();
        float z = g_tIMU.getAccelZ_mss();

        if ( 0.0 != x || 0.0 != y || 0.0 != z )
        {
            Serial.print( "Accel: X=" );
            Serial.print( x );
            Serial.print( "; Y=" );
            Serial.print( y );
            Serial.print( "; Z=" );
            Serial.println( z );
        }

        // printXYZ();

        g_uTimeInterrupt = millis();
        g_uTimePrevious = g_uTimeInterrupt;
    }
    else
    {
        if ( 1000 * 3 < abs( g_uTimeCurrent - g_uTimePrevious ) )
        {
            g_uTimePrevious = g_uTimeCurrent;
            g_uCountInterrupt = 0;
            digitalWrite( kPinLED, LOW );
        }
        if ( 0 < g_uTimeInterrupt )
        {
            if ( 1000 * 30 < abs( g_uTimeCurrent - g_uTimeInterrupt ) )
            {
                // enterSleep();
                g_uTimeInterrupt = millis();
            }
        }
    }
}
