#include "sound.h"
#include "EmeraldMine.h"
#include "mySDL.h"
#include "SDL2/SDL_mixer.h"

GAMESOUND GameSound;
extern PLAYFIELD Playfield;

extern uint8_t _binary_Sound_Ping_wav_start;extern uint8_t _binary_Sound_Ping_wav_end;
extern uint8_t _binary_Sound_Mine_wav_start;extern uint8_t _binary_Sound_Mine_wav_end;
extern uint8_t _binary_Sound_Beetle_wav_start;extern uint8_t _binary_Sound_Beetle_wav_end;
extern uint8_t _binary_Sound_Wheel_wav_start;extern uint8_t _binary_Sound_Wheel_wav_end;
extern uint8_t _binary_Sound_Alien_wav_start;extern uint8_t _binary_Sound_Alien_wav_end;
extern uint8_t _binary_Sound_MagicWall_wav_start;extern uint8_t _binary_Sound_MagicWall_wav_end;
extern uint8_t _binary_Sound_ManSpace_wav_start;extern uint8_t _binary_Sound_ManSpace_wav_end;
extern uint8_t _binary_Sound_Door_wav_start;extern uint8_t _binary_Sound_Door_wav_end;
extern uint8_t _binary_Sound_Nut_wav_start;extern uint8_t _binary_Sound_Nut_wav_end;
extern uint8_t _binary_Sound_Nut_Knack_wav_start;extern uint8_t _binary_Sound_Nut_Knack_wav_end;
extern uint8_t _binary_Sound_Replicator_wav_start;extern uint8_t _binary_Sound_Replicator_wav_end;
extern uint8_t _binary_Sound_ManPush_wav_start;extern uint8_t _binary_Sound_ManPush_wav_end;
extern uint8_t _binary_Sound_Yam_wav_start;extern uint8_t _binary_Sound_Yam_wav_end;
extern uint8_t _binary_Sound_Stone_wav_start;extern uint8_t _binary_Sound_Stone_wav_end;
extern uint8_t _binary_Sound_Explosion_wav_start;extern uint8_t _binary_Sound_Explosion_wav_end;
extern uint8_t _binary_Sound_DynamiteTick_wav_start;extern uint8_t _binary_Sound_DynamiteTick_wav_end;
extern uint8_t _binary_Sound_PoolBlub_wav_start;extern uint8_t _binary_Sound_PoolBlub_wav_end;
extern uint8_t _binary_Sound_ManTake_wav_start;extern uint8_t _binary_Sound_ManTake_wav_end;
extern uint8_t _binary_Sound_ManDig_wav_start;extern uint8_t _binary_Sound_ManDig_wav_end;
extern uint8_t _binary_Sound_Enddoor_wav_start;extern uint8_t _binary_Sound_Enddoor_wav_end;
extern uint8_t _binary_Sound_ManCries_wav_start;extern uint8_t _binary_Sound_ManCries_wav_end;
extern uint8_t _binary_Sound_Cheese_wav_start;extern uint8_t _binary_Sound_Cheese_wav_end;
extern uint8_t _binary_Sound_Gong_wav_start;extern uint8_t _binary_Sound_Gong_wav_end;
extern uint8_t _binary_Sound_Quetschen_wav_start;extern uint8_t _binary_Sound_Quetschen_wav_end;
extern uint8_t _binary_Sound_Switch_wav_start;extern uint8_t _binary_Sound_Switch_wav_end;
extern uint8_t _binary_Sound_Mole_wav_start;extern uint8_t _binary_Sound_Mole_wav_end;
extern uint8_t _binary_Sound_WheelTimeDoor_wav_start;extern uint8_t _binary_Sound_WheelTimeDoor_wav_end;
extern uint8_t _binary_Sound_DoorCloseOpen_wav_start;extern uint8_t _binary_Sound_DoorCloseOpen_wav_end;
extern uint8_t _binary_Sound_ReplicatorPlop_wav_start;extern uint8_t _binary_Sound_ReplicatorPlop_wav_end;
extern uint8_t _binary_Sound_DynamiteStart_wav_start;extern uint8_t _binary_Sound_DynamiteStart_wav_end;

uint8_t* g_pSfxPointer_wav[] = {
// Bit
/*01*/    &_binary_Sound_Ping_wav_start,&_binary_Sound_Ping_wav_end,                    // Ping, Emerald oder Saphir fallen auf etwas Hartes
/*02*/    &_binary_Sound_Mine_wav_start,&_binary_Sound_Mine_wav_end,                    // Mine
/*03*/    &_binary_Sound_Beetle_wav_start,&_binary_Sound_Beetle_wav_end,                // K�fer
/*04*/    &_binary_Sound_Wheel_wav_start,&_binary_Sound_Wheel_wav_end,                  // Rad
/*05*/    &_binary_Sound_Alien_wav_start,&_binary_Sound_Alien_wav_end,                  // Alien
/*06*/    &_binary_Sound_MagicWall_wav_start,&_binary_Sound_MagicWall_wav_end,          // Magic Wall
/*07*/    &_binary_Sound_ManSpace_wav_start,&_binary_Sound_ManSpace_wav_end,            // Man geht in Space
/*08*/    &_binary_Sound_Door_wav_start,&_binary_Sound_Door_wav_end,                    // Man geht durch T�r
/*09*/    &_binary_Sound_Nut_wav_start,&_binary_Sound_Nut_wav_end,                      // Nuss f�llt auf Etwas
/*10*/    &_binary_Sound_Nut_Knack_wav_start,&_binary_Sound_Nut_Knack_wav_end,          // Nuss wird geknackt
/*11*/    &_binary_Sound_Replicator_wav_start,&_binary_Sound_Replicator_wav_end,        // Replikator l�uft
/*12*/    &_binary_Sound_ManPush_wav_start,&_binary_Sound_ManPush_wav_end,              // Man schiebt Etwas
/*13*/    &_binary_Sound_Yam_wav_start,&_binary_Sound_Yam_wav_end,                      // Yam
/*14*/    &_binary_Sound_Stone_wav_start,&_binary_Sound_Stone_wav_end,                  // Stein f�llt auf Etwas
/*15*/    &_binary_Sound_Explosion_wav_start,&_binary_Sound_Explosion_wav_end,          // Explosion
/*16*/    &_binary_Sound_DynamiteTick_wav_start,&_binary_Sound_DynamiteTick_wav_end,    // Dynamit aktiv
/*17*/    &_binary_Sound_PoolBlub_wav_start,&_binary_Sound_PoolBlub_wav_end,            // Etwas ist ins S�urebecken gefallen
/*18*/    &_binary_Sound_ManTake_wav_start,&_binary_Sound_ManTake_wav_end,              // Man nimmt etwas auf
/*19*/    &_binary_Sound_ManDig_wav_start,&_binary_Sound_ManDig_wav_end,                // Man gr�bt Sand
/*20*/    &_binary_Sound_Enddoor_wav_start,&_binary_Sound_Enddoor_wav_end,              // Man geht in die Endt�r
/*21*/    &_binary_Sound_ManCries_wav_start,&_binary_Sound_ManCries_wav_end,            // Man schreit
/*22*/    &_binary_Sound_Cheese_wav_start,&_binary_Sound_Cheese_wav_end,                // K�se breitet sich aus
/*23*/    &_binary_Sound_Gong_wav_start,&_binary_Sound_Gong_wav_end,                    // Zeit ist bald um
/*24*/    &_binary_Sound_Quetschen_wav_start,&_binary_Sound_Quetschen_wav_end,          // Saphir gequetscht / Objekt durch blaue Wand
/*25*/    &_binary_Sound_Switch_wav_start,&_binary_Sound_Switch_wav_end,                // Ein Schalter wird bet�tigt
/*26*/    &_binary_Sound_Mole_wav_start,&_binary_Sound_Mole_wav_end,                    // Maulwurf
/*27*/    &_binary_Sound_WheelTimeDoor_wav_start,&_binary_Sound_WheelTimeDoor_wav_end,  // Rad f�r Zeit-T�r
/*28*/    &_binary_Sound_DoorCloseOpen_wav_start,&_binary_Sound_DoorCloseOpen_wav_end,  // T�r schlie�en / �ffnen
/*29*/    &_binary_Sound_ReplicatorPlop_wav_start,&_binary_Sound_ReplicatorPlop_wav_end,// Replikator erzeugt neues Objekt
/*30*/    &_binary_Sound_DynamiteStart_wav_start,&_binary_Sound_DynamiteStart_wav_end,  // Dynamit wird gestartet
/*31*/    NULL,NULL,                                                                    // Frei 6
};

Mix_Chunk *g_pChunk[MAX_WAV_CHUNKS];


/*----------------------------------------------------------------------------
Name:           InitGameSound
------------------------------------------------------------------------------
Beschreibung: Initialisiert das Soundsystem f�r das Spiel.
Parameter
      Eingang: -
      Ausgang: -
R�ckgabewert:  -
Seiteneffekte: GameSound.x, g_pChunk[]
------------------------------------------------------------------------------*/
int InitGameSound(void) {
    uint32_t I;

    for (I = 0; I < MAX_WAV_CHUNKS; I++) {
        g_pChunk[I] = NULL;
    }
    GameSound.uAllSounds = 0;
    if (Mix_OpenAudio(44100,AUDIO_S16,2,1024) == 0) {       // Samplerate, Audioformat,Kan�le,Chunksize die in einem St�ck verarbeitet wird
        Mix_AllocateChannels(MAX_WAV_CHUNKS);  // Die Allocation muss nach Mix_OpenAudio erfolgen, ansonsten bleibt es bei 8 Kan�len
        SDL_Log("%s: allocated channels: %d",__FUNCTION__,Mix_AllocateChannels(-2));
        return CreateWavChunks();
    } else {
        SDL_Log("%s: Mix_OpenAudio() failed, Error: %s",__FUNCTION__,SDL_GetError());
        return -1;
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
R�ckgabewert:  -
Seiteneffekte: GameSound.x, Playfield.x
------------------------------------------------------------------------------*/
void PreparePlaySound(uint32_t uSound, uint32_t uCoordinate) {
    uint32_t uTopLeftXpos;   // obere linke Ecke, X-Position (Level-Koordinate)
    uint32_t uTopLeftYpos;   // obere linke Ecke, Y-Position (Level-Koordinate)
    uint32_t uElementXpos;
    uint32_t uElementYpos;

    // Falls Sound bisher noch nicht aktiv ist, muss hier die Koordinate gepr�ft werden, ob wir uns im sichtbaren Bereich befinden
    if ((GameSound.uAllSounds & uSound) == 0) {
        if ((uSound & 0x80000000) != 0) {   // unabh�ngig vom sichtbaren Bereich aktiv ?
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
R�ckgabewert:  -
Seiteneffekte: GameSound.x, g_pChunk[]
------------------------------------------------------------------------------*/
int PlayAllSounds(void) {
    uint32_t uBit;
    uint32_t uMask;
    uint32_t uS;
    int nRet;

    if (GameSound.uAllSounds == 0) {
        return 0;       // Sofort raus hier, wenn es nichts abzuspielen gibt
    }
    uMask = 0x01;
    nRet = 0;
    uS = 0;
    for (uBit = 0; (uBit < 31) && (nRet == 0); uBit++) {
        if ((uMask & GameSound.uAllSounds) != 0) {          // Soll der Sound abgespielt werden
            if (Mix_PlayChannel(uS, g_pChunk[uS],0) == -1) {
                nRet = -1;
            }
        }
        uMask = uMask << 1;
        uS++;
    }
    GameSound.uAllSounds = 0;
    return nRet;
}


/*----------------------------------------------------------------------------
Name:           CreateWavChunks
------------------------------------------------------------------------------
Beschreibung: Erzeugt WAV-Chunks aus WAV-Dateien. Die WAV-Chunks k�nnen mit
              SDL_Mixer direkt auf einem Kanal/Track ausgegeben werden.
              Programms aufgerufen.
              Vor Aufruf dieser Funktion muss der Audio-Mixer mit Mix_OpenAudio
              ge�ffnet worden sein und die Anzahl der Tracks mit Mix_AllocateChannels()
              ggf. erh�ht worden sein.
Parameter
      Eingang: -
      Ausgang: -
R�ckgabewert:  int, 0 = Alles OK, sonst Fehler
Seiteneffekte: g_pChunk[], g_pSfxPointer_wav[]
------------------------------------------------------------------------------*/
int CreateWavChunks(void) {
    uint32_t I;
    uint32_t uSoundSize;
    int nErrorCode = 0;
    Mix_Chunk * pChunk;
    SDL_RWops* pSDLStreamPointer; // Streampointer, um Sounds einzulesen

    I = 0;
    nErrorCode = 0;
    while ((g_pSfxPointer_wav[I * 2] != NULL) && (g_pSfxPointer_wav[I * 2 + 1] != NULL) && (I < MAX_WAV_CHUNKS) && (nErrorCode == 0)) {
        uSoundSize = g_pSfxPointer_wav[I * 2 + 1] - g_pSfxPointer_wav[I * 2];
        pSDLStreamPointer = SDL_RWFromMem((void*)g_pSfxPointer_wav[I * 2],uSoundSize);// Erzeugt SDL-Speicherstruktur f�r Speicher (Stream)
        if (pSDLStreamPointer != NULL) {
            pChunk = Mix_LoadWAV_RW(pSDLStreamPointer,1);
            if (pChunk != NULL) {
                // SDL_Log("%s: Chunk[%02d] OK, size: %u",__FUNCTION__,I,uSoundSize);
                g_pChunk[I] = pChunk;
            } else {
                SDL_Log("%s: Mix_LoadWAV_RW() failed, Error: %s",__FUNCTION__,SDL_GetError());
                nErrorCode = -1;
            }
        } else {
            SDL_Log("%s: SDL_RWFromMem() failed, Error: %s",__FUNCTION__,SDL_GetError());
            nErrorCode = -1;
        }
        I++;
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           FreeWavChunks
------------------------------------------------------------------------------
Beschreibung: Gibt alle erzeugten WAV-Chunks wieder frei. Wird am Ende des
              Programms aufgerufen.
Parameter
      Eingang: -
      Ausgang: -
R�ckgabewert:  -
Seiteneffekte: g_pChunk[]
------------------------------------------------------------------------------*/
void FreeWavChunks(void) {
    uint32_t I;

    for (I = 0; I < MAX_WAV_CHUNKS; I++) {
        if (g_pChunk[I] != NULL) {
            Mix_FreeChunk(g_pChunk[I]);
        }
    }
}
