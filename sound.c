#include "config.h"
#include "sound.h"
#include "EmeraldMine.h"
#include "miniz.h"
#include "mySDL.h"
#include "sdlmixer_SDL_mixer.h"
#include "sfx/gamesound.h"

GAMESOUND GameSound;
extern PLAYFIELD Playfield;
extern CONFIG Config;
extern uint8_t _binary_gamesound_compressed_bin_start;extern uint8_t _binary_gamesound_compressed_bin_end;

// Alle verfügbaren Gamesounds
// Bit
/*01*/      // Ping, Emerald oder Saphir fallen auf etwas Hartes
/*02*/      // Mine
/*03*/      // Käfer
/*04*/      // Rad
/*05*/      // Alien
/*06*/      // Magic Wall
/*07*/      // Man geht in Space
/*08*/      // Man geht durch Tür
/*09*/      // Nuss fällt auf Etwas
/*10*/      // Nuss wird geknackt
/*11*/      // Replikator läuft
/*12*/      // Man schiebt Etwas
/*13*/      // Yam
/*14*/      // Stein fällt auf Etwas
/*15*/      // Explosion
/*16*/      // Dynamit aktiv
/*17*/      // Etwas ist ins Säurebecken gefallen
/*18*/      // Man nimmt etwas auf
/*19*/      // Man gräbt Sand
/*20*/      // Man geht in die Endtür
/*21*/      // Man schreit
/*22*/      // Käse breitet sich aus
/*23*/      // Zeit ist bald um
/*24*/      // Saphir gequetscht / Objekt durch blaue Wand
/*25*/      // Ein Schalter wird betätigt
/*26*/      // Maulwurf
/*27*/      // Rad für Zeit-Tür
/*28*/      // Tür schließen / öffnen
/*29*/      // Replikator erzeugt neues Objekt
/*30*/      // Dynamit wird gestartet
/*31*/      // Laufband
/*32*/      // Teleporter
/*33*/      // Mauer und Stahl wächst
/*34*/      // ferngesteuerte Bombe
/*35*/      // Man läuft in Röhre
/*36*/      // Schutzschild geht zu Ende
/*37*/      // Schleim frisst etwas
/*38*/      // Schleim bewegt sich

Mix_Chunk *g_pChunk[MAX_WAV_CHUNKS];


/*----------------------------------------------------------------------------
Name:           InitGameSound
------------------------------------------------------------------------------
Beschreibung: Initialisiert das Soundsystem für das Spiel.
              1) Auspacken der WAV-Dateien aus dem komprimierten Gesamtblock
              2) Öffnen des Audio devices
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  int, 0 = alles OK, sonst Fehler
Seiteneffekte: GameSound.x, g_pChunk[]
------------------------------------------------------------------------------*/
int InitGameSound(void) {
    uint32_t I;
    uint32_t uCompressedWavSize;
    uint32_t uUnCompressedWavSize;
    uint8_t *pCompressedWavStart;
    int nMiniz;

    memset(&GameSound,0,sizeof(GAMESOUND));
    for (I = 0; I < MAX_WAV_CHUNKS; I++) {
        g_pChunk[I] = NULL;
    }
    GameSound.uAllSounds = 0;
    // Alle WAV-dateien entpacken und im Speicher Audioplayer.pMusicAll ablegen
    pCompressedWavStart = &_binary_gamesound_compressed_bin_start;;
    uCompressedWavSize = &_binary_gamesound_compressed_bin_end - &_binary_gamesound_compressed_bin_start - 4;  // -4, da am Anfang die unkomprimierte Größe eingetragen wurde
    uUnCompressedWavSize = *(uint32_t*)pCompressedWavStart;
    GameSound.pWavAll = malloc(uUnCompressedWavSize);
    if (GameSound.pWavAll == NULL) {
        SDL_Log("%s: can not allocate memory for decompressed wav sounds (%u bytes)",__FUNCTION__,uUnCompressedWavSize);
        return -1;
    }
    nMiniz = mz_uncompress(GameSound.pWavAll,(mz_ulong*)&uUnCompressedWavSize,pCompressedWavStart + 4,(mz_ulong)uCompressedWavSize);
    if (nMiniz != MZ_OK) {
        SDL_Log("%s: can not decompress wav sounds, error: %d",__FUNCTION__,nMiniz);
        SAFE_FREE(GameSound.pWavAll);
        return -1;
    }
    // Audio device für game sounds öffnen
    if (Mix_OpenAudio(44100,AUDIO_S16,2,1024) == 0) {       // Samplerate, Audioformat,Kanäle,Chunksize die in einem Stück verarbeitet wird
        Mix_AllocateChannels(MAX_WAV_CHUNKS);  // Die Allocation muss nach Mix_OpenAudio erfolgen, ansonsten bleibt es bei 8 Kanälen
        // SDL_Log("%s: allocated channels: %d",__FUNCTION__,Mix_AllocateChannels(-2));
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
      Eingang: uSound, uint64_t, Ein oder mehrere Sounds die abgespielt werden sollen, siehe sound.h
                    Beispiel: SOUND_PING | SOUND_MINE   spielt PING und MINE ab
               uCoordinate, uint32_t, lineare Level-Koordinate, wo Sound entstanden ist.
                    Einige Sounds werden nur im sichtbaren Bereich abgespielt
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: GameSound.x, Playfield.x
------------------------------------------------------------------------------*/
void PreparePlaySound(uint64_t uSound, uint32_t uCoordinate) {
    uint32_t uTopLeftXpos;   // obere linke Ecke, X-Position (Level-Koordinate)
    uint32_t uTopLeftYpos;   // obere linke Ecke, Y-Position (Level-Koordinate)
    uint32_t uElementXpos;
    uint32_t uElementYpos;
    bool     bAlwaysOn;      // Sound ist unabhängig vom sichtbaren Bereich aktiv
    uint32_t uAddX,uAddY;

    // Bei "ungeraden" Auflösung wird der hörbare Bereich um ein Feld nach unten bzw. rechts erweitert.
    if ((Config.uResX % FONT_W) != 0) {
        uAddX = 1;
    } else {
        uAddX = 0;
    }
    if ((Config.uResY % FONT_H) != 0) {
        uAddY = 1;
    } else {
        uAddY = 0;
    }
    bAlwaysOn = ((uSound & 0x8000000000000000) != 0);
    uSound = uSound & 0x7FFFFFFFFFFFFFFF;               // Always-Bit ausblenden
    // Falls Sound bisher noch nicht aktiv ist, muss hier die Koordinate geprüft werden, ob wir uns im sichtbaren Bereich befinden
    if ((GameSound.uAllSounds & uSound) == 0) {
        if (bAlwaysOn) {   // unabhängig vom sichtbaren Bereich aktiv ?
            GameSound.uAllSounds |= uSound;
        } else {
            // Pixel-Koordinate in Level-Koordinate umrechnen
            uTopLeftXpos = Playfield.nTopLeftXpos / FONT_W;
            uTopLeftYpos = Playfield.nTopLeftYpos / FONT_H;
            uElementXpos = uCoordinate % Playfield.uLevel_X_Dimension;
            uElementYpos = uCoordinate / Playfield.uLevel_X_Dimension;
            if ( ((uElementXpos >= uTopLeftXpos) && (uElementXpos < (uTopLeftXpos + Playfield.uVisibleX + uAddX))) &&
                 ((uElementYpos >= uTopLeftYpos) && (uElementYpos < (uTopLeftYpos + Playfield.uVisibleY + uAddY))) ) {
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
Seiteneffekte: GameSound.x, g_pChunk[]
------------------------------------------------------------------------------*/
int PlayAllSounds(void) {
    uint64_t uBit;
    uint64_t uMask;
    uint32_t uS;    // Wave-Chunk und Channel
    int nRet;

    if (GameSound.uAllSounds == 0) {
        return 0;       // Beenden, wenn es nichts abzuspielen gibt
    }
    uMask = 0x01;
    nRet = 0;
    uS = 0;
    for (uBit = 0; (uBit < 63) && (nRet == 0); uBit++) {
        if ((uMask & GameSound.uAllSounds) != 0) {          // Soll der Sound abgespielt werden?
            if (Mix_PlayChannel(uS, g_pChunk[uS],0) == -1) {
                nRet = -1;
                SDL_Log("%s: Mix_PlayChannel() failed, channel: %u",__FUNCTION__,uS);
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
Beschreibung: Erzeugt WAV-Chunks aus WAV-Dateien. Die WAV-Chunks können mit
              SDL_Mixer direkt auf einem Kanal/Track ausgegeben werden.
              Vor Aufruf dieser Funktion muss der Audio-Mixer mit Mix_OpenAudio
              geöffnet worden sein und die Anzahl der Tracks mit Mix_AllocateChannels()
              ggf. erhöht worden sein.
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  int, 0 = Alles OK, sonst Fehler
Seiteneffekte: g_pChunk[], gamesound[], GameSound.x
------------------------------------------------------------------------------*/
int CreateWavChunks(void) {
    uint32_t I;
    uint32_t uSoundSize;
    int nErrorCode = 0;
    Mix_Chunk * pChunk;
    SDL_RWops* pSDLStreamPointer; // Streampointer, um Sounds einzulesen

    I = 0;
    nErrorCode = 0;
    while ((gamesound[I * 2] != 0xFFFFFFFF) && (I < MAX_WAV_CHUNKS) && (nErrorCode == 0)) {
        uSoundSize = gamesound[I * 2 + 1];
        pSDLStreamPointer = SDL_RWFromMem((void*)GameSound.pWavAll + gamesound[I * 2],uSoundSize);// Erzeugt SDL-Speicherstruktur für Speicher (Stream)
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
Rückgabewert:  -
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
