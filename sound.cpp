#include "sound.h"
#include "EmeraldMine.h"
#include "mySDL.h"

GAMESOUND GameSound;
extern PLAYFIELD Playfield;

extern uint8_t _binary_Sound_Ping_raw_start;extern uint8_t _binary_Sound_Ping_raw_end;
extern uint8_t _binary_Sound_Mine_raw_start;extern uint8_t _binary_Sound_Mine_raw_end;
extern uint8_t _binary_Sound_Beetle_raw_start;extern uint8_t _binary_Sound_Beetle_raw_end;
extern uint8_t _binary_Sound_Wheel_raw_start;extern uint8_t _binary_Sound_Wheel_raw_end;
extern uint8_t _binary_Sound_Alien_raw_start;extern uint8_t _binary_Sound_Alien_raw_end;
extern uint8_t _binary_Sound_MagicWall_raw_start;extern uint8_t _binary_Sound_MagicWall_raw_end;
extern uint8_t _binary_Sound_ManSpace_raw_start;extern uint8_t _binary_Sound_ManSpace_raw_end;
extern uint8_t _binary_Sound_Door_raw_start;extern uint8_t _binary_Sound_Door_raw_end;
extern uint8_t _binary_Sound_Nut_raw_start;extern uint8_t _binary_Sound_Nut_raw_end;
extern uint8_t _binary_Sound_Nut_Knack_raw_start;extern uint8_t _binary_Sound_Nut_Knack_raw_end;
extern uint8_t _binary_Sound_Replicator_raw_start;extern uint8_t _binary_Sound_Replicator_raw_end;
extern uint8_t _binary_Sound_ManPush_raw_start;extern uint8_t _binary_Sound_ManPush_raw_end;
extern uint8_t _binary_Sound_Yam_raw_start;extern uint8_t _binary_Sound_Yam_raw_end;
extern uint8_t _binary_Sound_Stone_raw_start;extern uint8_t _binary_Sound_Stone_raw_end;
extern uint8_t _binary_Sound_Explosion_raw_start;extern uint8_t _binary_Sound_Explosion_raw_end;
//#define SOUND_DYNAMIT       0b00000000000000001000000000000000      // Dynamit aktiv
extern uint8_t _binary_Sound_PoolBlub_raw_start;extern uint8_t _binary_Sound_PoolBlub_raw_end;
extern uint8_t _binary_Sound_ManTake_raw_start;extern uint8_t _binary_Sound_ManTake_raw_end;
extern uint8_t _binary_Sound_ManDig_raw_start;extern uint8_t _binary_Sound_ManDig_raw_end;
extern uint8_t _binary_Sound_Enddoor_raw_start;extern uint8_t _binary_Sound_Enddoor_raw_end;
extern uint8_t _binary_Sound_ManCries_raw_start;extern uint8_t _binary_Sound_ManCries_raw_end;
extern uint8_t _binary_Sound_Cheese_raw_start;extern uint8_t _binary_Sound_Cheese_raw_end;
extern uint8_t _binary_Sound_Gong_raw_start;extern uint8_t _binary_Sound_Gong_raw_end;
extern uint8_t _binary_Sound_Quetschen_raw_start;extern uint8_t _binary_Sound_Quetschen_raw_end;
extern uint8_t _binary_Sound_Switch_raw_start;extern uint8_t _binary_Sound_Switch_raw_end;



uint8_t* g_pSfxPointer[] = {
// Bit
/*01*/    &_binary_Sound_Ping_raw_start,&_binary_Sound_Ping_raw_end,            // Ping, Emerald oder Saphir fallen auf etwas Hartes
/*02*/    &_binary_Sound_Mine_raw_start,&_binary_Sound_Mine_raw_end,            // Mine
/*03*/    &_binary_Sound_Beetle_raw_start,&_binary_Sound_Beetle_raw_end,        // Käfer
/*04*/    &_binary_Sound_Wheel_raw_start,&_binary_Sound_Wheel_raw_end,          // Rad
/*05*/    &_binary_Sound_Alien_raw_start,&_binary_Sound_Alien_raw_end,          // Alien
/*06*/    &_binary_Sound_MagicWall_raw_start,&_binary_Sound_MagicWall_raw_end,  // Magic Wall
/*07*/    &_binary_Sound_ManSpace_raw_start,&_binary_Sound_ManSpace_raw_end,    // Man geht in Space
/*08*/    &_binary_Sound_Door_raw_start,&_binary_Sound_Door_raw_end,            // Man geht durch Tür
/*09*/    &_binary_Sound_Nut_raw_start,&_binary_Sound_Nut_raw_end,              // Nuss fällt auf Etwas
/*10*/    &_binary_Sound_Nut_Knack_raw_start,&_binary_Sound_Nut_Knack_raw_end,  // Nuss wird geknackt
/*11*/    &_binary_Sound_Replicator_raw_start,&_binary_Sound_Replicator_raw_end,// Replikator läuft
/*12*/    &_binary_Sound_ManPush_raw_start,&_binary_Sound_ManPush_raw_end,      // Man schiebt Etwas
/*13*/    &_binary_Sound_Yam_raw_start,&_binary_Sound_Yam_raw_end,              // Yam
/*14*/    &_binary_Sound_Stone_raw_start,&_binary_Sound_Stone_raw_end,          // Stein fällt auf Etwas
/*15*/    &_binary_Sound_Explosion_raw_start,&_binary_Sound_Explosion_raw_end,  // Explosion
/*16*/    NULL,NULL,                                                            // Dynamit aktiv
/*17*/    &_binary_Sound_PoolBlub_raw_start,&_binary_Sound_PoolBlub_raw_end,    // Etwas ist ins Säurebecken gefallen
/*18*/    &_binary_Sound_ManTake_raw_start,&_binary_Sound_ManTake_raw_end,      // Man nimmt etwas auf
/*19*/    &_binary_Sound_ManDig_raw_start,&_binary_Sound_ManDig_raw_end,        // Man gräbt Sand
/*20*/    &_binary_Sound_Enddoor_raw_start,&_binary_Sound_Enddoor_raw_end,      // Man geht in die Endtür
/*21*/    &_binary_Sound_ManCries_raw_start,&_binary_Sound_ManCries_raw_end,    // Man schreit
/*22*/    &_binary_Sound_Cheese_raw_start,&_binary_Sound_Cheese_raw_end,        // Käse breitet sich aus
/*23*/    &_binary_Sound_Gong_raw_start,&_binary_Sound_Gong_raw_end,            // Zeit ist bald um
/*24*/    &_binary_Sound_Quetschen_raw_start,&_binary_Sound_Quetschen_raw_end,  // Saphir gequetscht / Objekt durch blaue Wand
/*25*/    &_binary_Sound_Switch_raw_start,&_binary_Sound_Switch_raw_end,        // Ein Schalter wird betätigt
/*26*/    NULL,NULL,                                                            // Frei 1
/*27*/    NULL,NULL,                                                            // Frei 2
/*28*/    NULL,NULL,                                                            // Frei 3
/*29*/    NULL,NULL,                                                            // Frei 4
/*30*/    NULL,NULL,                                                            // Frei 5
/*31*/    NULL,NULL,                                                            // Frei 6
};


/*----------------------------------------------------------------------------
Name:           InitGameSound
------------------------------------------------------------------------------
Beschreibung: Initialisiert das Soundsystem für das Spiel.
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: GameSound.x
------------------------------------------------------------------------------*/
int InitGameSound(void) {
    GameSound.sdl_audio.freq = 44100;
    GameSound.sdl_audio.format = AUDIO_S16;
    GameSound.sdl_audio.channels = 1;
    GameSound.sdl_audio.samples = 64; //4096;
    GameSound.sdl_audio.callback = NULL;
    GameSound.audio_device = SDL_OpenAudioDevice(NULL, 0, &GameSound.sdl_audio, NULL, 0);
    GameSound.uAllSounds = 0;
    if (GameSound.audio_device == 0) {
        SDL_Log("%s: SDL_OpenAudioDevice() failed: %s",__FUNCTION__,SDL_GetError());
        return -1;
    } else {
        SDL_PauseAudioDevice(GameSound.audio_device, 0);
        return 0;
    }
}


/*----------------------------------------------------------------------------
Name:           PreparePlaySound
------------------------------------------------------------------------------
Beschreibung: Bereitet das Apspielen eines Sounds vor.
Parameter
      Eingang: uSound, uint32_t, Ein oder mehrere Sounds die abgespielt werden sollen, siehe sound.h
                    Beispiel: SOUND_PING | SOUND_MINE   spielt PING und MINE ab
               uCoordinate, uint32_t, lineare Level-Koordinate, wo Sound entstanden ist.
                    Einige Sounds werden nur im sichtbaren Bereich abgespielt
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: GameSound.x, Playfield.x
------------------------------------------------------------------------------*/
void PreparePlaySound(uint32_t uSound, uint32_t uCoordinate) {
    uint32_t uTopLeftXpos;   // obere linke Ecke, X-Position (Level-Koordinate)
    uint32_t uTopLeftYpos;   // obere linke Ecke, Y-Position (Level-Koordinate)
    uint32_t uElementXpos;
    uint32_t uElementYpos;

    // Falls Sound bisher noch nicht aktiv ist, muss hier die Koordinate geprüft werden, ob wir uns im sichtbaren Bereich befinden
    if ((GameSound.uAllSounds & uSound) == 0) {
        if ((uSound & 0x80000000) != 0) {   // unabhängig vom sichtbaren Bereich aktiv ?
            GameSound.uAllSounds |= uSound;
        }
        else {
            // Pixel-Koordinate in Level-Koordinate umrechnen
            uTopLeftXpos = Playfield.nTopLeftXpos / FONT_W;
            uTopLeftYpos = Playfield.nTopLeftYpos / FONT_H;
            uElementXpos = uCoordinate % Playfield.uLevel_X_Dimension;
            uElementYpos = uCoordinate / Playfield.uLevel_X_Dimension;
            if ( ((uElementXpos >= uTopLeftXpos) && (uElementXpos < (uTopLeftXpos + Playfield.uVisibleX))) &&
                 ((uElementYpos >= uTopLeftYpos) && (uElementYpos < (uTopLeftYpos + Playfield.uVisibleY))) ) {
                GameSound.uAllSounds |= uSound;
            }
        }
    }
}


/*----------------------------------------------------------------------------
Name:           PlayAllSounds
------------------------------------------------------------------------------
Beschreibung: Bereitet das Apspielen eines Sounds vor.
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: GameSound.x, g_pSfxPointer[]
------------------------------------------------------------------------------*/
int PlayAllSounds(void) {
    uint32_t uBit;
    uint32_t uMask;
    uint8_t *pStart;
    uint8_t *pEnd;
    uint32_t uSize;     // Samples
    uint32_t uMaxLen;   // Samples
    uint32_t uS;
    short sSample;
    uint32_t uSounds;   // Anzahl der gleichzeitigen Sounds, dient später als Teiler
    int nRet;

    if (GameSound.uAllSounds == 0) {
        return 0;       // Sofort beenden hier, wenn es nichts abzuspielen gibt
    }
    memset(GameSound.sAudiobuffer,0,sizeof(GameSound.sAudiobuffer));
    uMaxLen = 0;
    uSounds = 0;
    uMask = 0x01;
    for (uBit = 0; uBit < 31; uBit++) {
        if ((uMask & GameSound.uAllSounds) != 0) {          // Soll der Sound abgespielt werden
            pStart = g_pSfxPointer[uBit * 2];
            pEnd = g_pSfxPointer[uBit * 2 + 1];
            if ((pStart != NULL) && (pEnd != NULL)) {
                uSize = pEnd - pStart;                      // Anzahl der Bytes
                for (uS = 0; uS < uSize / 2; uS++) {
                    sSample = *(short*)(pStart + uS * 2);   // 16 Bit-Sample aus Original lesen
                    GameSound.sAudiobuffer[uS] += sSample;  // Sample zum bisherigen Mix dazu addieren
                }
                if ( (uSize / 2) > uMaxLen) {
                    uMaxLen = uSize / 2;
                }
                uSounds++;
            } else {
                SDL_Log("%s: null pointer found, can't play sound no. %u",__FUNCTION__,uBit);
            }
        }
        uMask = uMask << 1;
    }
    //SDL_Log("Sounds: %u  Maxlen = %u    In Queue: %u",uSounds,uMaxLen,SDL_GetQueuedAudioSize(GameSound.audio_device));
    SDL_ClearQueuedAudio(GameSound.audio_device);
    nRet =  SDL_QueueAudio(GameSound.audio_device,GameSound.sAudiobuffer, uMaxLen * 2);  // 1 channel, 2 bytes/sample
    if (nRet != 0) {
        SDL_Log("%s: SDL_QueueAudio() failed: %s",__FUNCTION__,SDL_GetError());
    }
    GameSound.uAllSounds = 0;
    return nRet;
}

