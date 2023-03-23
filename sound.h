#ifndef SOUND_H_INCLUDED
#define SOUND_H_INCLUDED

#include <SDL2/SDL.h>

#define MAX_SOUND_SAMPLES   32768

// Bit 31 = 1, unabhängig vom sichtbaren Bereich aktiv
#define SOUND_PING              0b00000000000000000000000000000001      // Diamant oder Saphir fällt auf etwas Hartes
#define SOUND_MINE              0b00000000000000000000000000000010      // Mine
#define SOUND_BEETLE            0b00000000000000000000000000000100      // Käfer
#define SOUND_WHEEL             0b10000000000000000000000000001000      // Rad  (unabhängig vom sichtbaren Bereich aktiv)
#define SOUND_ALIEN             0b00000000000000000000000000010000      // Alien
#define SOUND_MAGIC_WALL        0b00000000000000000000000000100000      // Magic Wall
#define SOUND_MAN               0b00000000000000000000000001000000      // Man geht im Space
#define SOUND_DOOR              0b00000000000000000000000010000000      // Man geht durch Tür
#define SOUND_NUT_FALL          0b00000000000000000000000100000000      // Nuss fällt auf Etwas
#define SOUND_NUT_CRACK         0b00000000000000000000001000000000      // Nuss wird geknackt
#define SOUND_REPLICATOR        0b00000000000000000000010000000000      // Replikator läuft
#define SOUND_MAN_PUSH          0b00000000000000000000100000000000      // Man schiebt Etwas
#define SOUND_YAM               0b00000000000000000001000000000000      // Yam
#define SOUND_STONE_FALL        0b00000000000000000010000000000000      // Stein fällt auf Etwas
#define SOUND_EXPLOSION         0b10000000000000000100000000000000      // Explosion (unabhängig vom sichtbaren Bereich aktiv)
#define SOUND_DYNAMITE          0b00000000000000001000000000000000      // Dynamit aktiv
#define SOUND_POOL_BLUB         0b00000000000000010000000000000000      // Etwas ist ins Säurebecken gefallen
#define SOUND_MAN_TAKE          0b00000000000000100000000000000000      // Man nimmt etwas auf
#define SOUND_DIG_SAND          0b00000000000001000000000000000000      // Man gräbt Sand
#define SOUND_ENDDOOR           0b00000000000010000000000000000000      // Man geht in die Endtür
#define SOUND_MAN_CRIES         0b00000000000100000000000000000000      // Man schreit
#define SOUND_CHEESE            0b00000000001000000000000000000000      // Käse breitet sich aus
#define SOUND_END_BELL          0b10000000010000000000000000000000      // Zeit ist bald um (unabhängig vom sichtbaren Bereich aktiv)
#define SOUND_SQUEAK            0b00000000100000000000000000000000      // Saphir gequetscht / Objekt durch blaue Wand / Perle zerbricht
#define SOUND_SWITCH            0b00000001000000000000000000000000      // Ein Schalter wird betätigt
#define SOUND_MOLE              0b00000010000000000000000000000000      // Maulwurf
#define SOUND_WHEEL_TIMEDOOR    0b10000100000000000000000000000000      // Rad für Zeit-Tür
#define SOUND_DOOR_OPEN_CLOSE   0b00001000000000000000000000000000      // Tür öffnen / schließen
#define SOUND_REPLICATOR_PLOP   0b00010000000000000000000000000000      // Replikator erzeugt neues Objekt
#define SOUND_DYNAMITE_START    0b00100000000000000000000000000000      // Dynamit wird gestartet
#define SOUND_FREE_6            0b01000000000000000000000000000000      // -



typedef struct {
    SDL_AudioSpec sdl_audio;
    SDL_AudioDeviceID audio_device;
    short sAudiobuffer[MAX_SOUND_SAMPLES];   // Maximale Anzahl von Samples, 1 Sample = 2 Bytes (short)
    uint32_t uAudioSize;                    // in Bytes, nicht in Samples -> Eingabe für SDL_QueueAudio()
    uint32_t uAllSounds;                    // OR-verknüpfte Sounds, die abgespielt werden sollen
} GAMESOUND;

int InitGameSound(void);
void PreparePlaySound(uint32_t uSound, uint32_t uCoordinate);
int PlayAllSounds(void);                    // Mixt alle Sounds in GameSound.uAllSounds und spielt diese ab.

#endif // SOUND_H_INCLUDED
