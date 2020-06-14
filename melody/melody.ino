
#include <YogiDelay.h>
#include <YogiPitches.h>

#define REST 0

const uint8_t kPinTone = 5;

unsigned int tempo = 100;
unsigned int wholeNote = ( 60000 * 4 ) / tempo;


typedef struct Note
{
    unsigned int frequency;
    int          duration;
} Note;

// star wars melody
Note melody[] = {  //
    // { NOTE_A4, -4 }, { NOTE_A4, -4 }, { NOTE_A4, -4 },  //
    // { NOTE_A4, 4 },
    // // { NOTE_A4, 16 },
    // // { NOTE_A4, 16 }, { NOTE_A4, 16 }, { NOTE_F4, 16 },
    // { REST, 8 },  //
    // { NOTE_A4, -4 }, { NOTE_A4, -4 }, { NOTE_A4, 16 }, { NOTE_A4, 16 },
    // { NOTE_A4, 16 }, { NOTE_A4, 16 }, { NOTE_F4, 8 }, { REST, 8 },
    { NOTE_A4, 4 }, { NOTE_A4, 4 }, { NOTE_A4, 4 }, { NOTE_F4, -8 },
    { NOTE_C5, 16 },

    { NOTE_A4, 4 }, { NOTE_F4, -8 }, { NOTE_C5, 16 }, { NOTE_A4, 2 },  //4
    { NOTE_E5, 4 }, { NOTE_E5, 4 }, { NOTE_E5, 4 }, { NOTE_F5, -8 },
    { NOTE_C5, 16 }, { NOTE_A4, 4 }, { NOTE_F4, -8 }, { NOTE_C5, 16 },
    { NOTE_A4, 2 },

    { NOTE_A5, 4 }, { NOTE_A4, -8 }, { NOTE_A4, 16 }, { NOTE_A5, 4 },
    { NOTE_GS5, -8 }, { NOTE_G5, 16 },  //7
    { NOTE_DS5, 16 }, { NOTE_D5, 16 }, { NOTE_DS5, 8 }, { REST, 8 },
    { NOTE_A4, 8 }, { NOTE_DS5, 4 }, { NOTE_D5, -8 }, { NOTE_CS5, 16 },

    { NOTE_C5, 16 }, { NOTE_B4, 16 }, { NOTE_C5, 16 }, { REST, 8 },
    { NOTE_F4, 8 }, { NOTE_GS4, 4 }, { NOTE_F4, -8 }, { NOTE_A4, -16 },  //9
    { NOTE_C5, 4 }, { NOTE_A4, -8 }, { NOTE_C5, 16 }, { NOTE_E5, 2 },

    { NOTE_A5, 4 }, { NOTE_A4, -8 }, { NOTE_A4, 16 }, { NOTE_A5, 4 },
    { NOTE_GS5, -8 }, { NOTE_G5, 16 },  //7
    { NOTE_DS5, 16 }, { NOTE_D5, 16 }, { NOTE_DS5, 8 }, { REST, 8 },
    { NOTE_A4, 8 }, { NOTE_DS5, 4 }, { NOTE_D5, -8 }, { NOTE_CS5, 16 },

    { NOTE_C5, 16 }, { NOTE_B4, 16 }, { NOTE_C5, 16 }, { REST, 8 },
    { NOTE_F4, 8 }, { NOTE_GS4, 4 }, { NOTE_F4, -8 }, { NOTE_A4, -16 },  //9
    { NOTE_A4, 4 }, { NOTE_F4, -8 }, { NOTE_C5, 16 }, { NOTE_A4, 2 },
    { REST, 1 }
};

// notes in the melody:
// int melody[] = { NOTE_C5, NOTE_D5, NOTE_E5, NOTE_F5, NOTE_G5, NOTE_A5, NOTE_B5,
//     NOTE_C6 };
int melodyCount = sizeof( melody ) / sizeof( melody[0] );
// int duration = 500;  // 500 miliseconds


unsigned long g_uTimeCurrent = 0;
int           g_nNoteCurrent = 0;

YogiDelay g_tDelay;


int
beep( unsigned int frequency, int noteType )
{
    int duration;
    if ( 0 < noteType )
    {
        duration = wholeNote / noteType;
    }
    else
    {
        duration = wholeNote / abs( noteType ) * 1.5;
    }
    //noTone( kPinTone );
    if ( REST != frequency )
        tone( kPinTone, frequency, duration * 0.9 );
    else
        noTone( kPinTone );

    return duration;
}


void
setup()
{
    pinMode( kPinTone, OUTPUT );
    g_tDelay.init( 1000 );
    g_nNoteCurrent = 0;
}


void
loop()
{
    g_uTimeCurrent = millis();
    if ( g_tDelay.timesUp( g_uTimeCurrent ) )
    {
        int delay = beep( melody[g_nNoteCurrent].frequency,
                melody[g_nNoteCurrent].duration );
        g_tDelay.newDelay( delay );
        //tone( kPinTone, melody[g_nNoteCurrent], duration );
        ++g_nNoteCurrent;
        if ( melodyCount <= g_nNoteCurrent )
        {
            g_nNoteCurrent = 0;
        }
    }
}
