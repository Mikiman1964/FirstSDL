#ifndef SOUND_H_INCLUDED
#define SOUND_H_INCLUDED

#include <SDL2/SDL.h>

// Bit 63 = 1, unabh�ngig vom sichtbaren Bereich aktiv
#define SOUND_PING              0b0000000000000000000000000000000000000000000000000000000000000001      // Diamant oder Saphir f�llt auf etwas Hartes
#define SOUND_MINE              0b0000000000000000000000000000000000000000000000000000000000000010      // Mine
#define SOUND_BEETLE            0b0000000000000000000000000000000000000000000000000000000000000100      // K�fer
#define SOUND_WHEEL             0b1000000000000000000000000000000000000000000000000000000000001000      // Rad  (unabh�ngig vom sichtbaren Bereich aktiv)
#define SOUND_ALIEN             0b0000000000000000000000000000000000000000000000000000000000010000      // Alien
#define SOUND_MAGIC_WALL        0b0000000000000000000000000000000000000000000000000000000000100000      // Magic Wall
#define SOUND_MAN               0b0000000000000000000000000000000000000000000000000000000001000000      // Man geht im Space
#define SOUND_DOOR              0b0000000000000000000000000000000000000000000000000000000010000000      // Man geht durch T�r
#define SOUND_NUT_FALL          0b0000000000000000000000000000000000000000000000000000000100000000      // Nuss f�llt auf Etwas
#define SOUND_NUT_CRACK         0b0000000000000000000000000000000000000000000000000000001000000000      // Nuss wird geknackt
#define SOUND_REPLICATOR        0b0000000000000000000000000000000000000000000000000000010000000000      // Replikator l�uft
#define SOUND_MAN_PUSH          0b0000000000000000000000000000000000000000000000000000100000000000      // Man schiebt Etwas
#define SOUND_YAM               0b0000000000000000000000000000000000000000000000000001000000000000      // Yam
#define SOUND_STONE_FALL        0b0000000000000000000000000000000000000000000000000010000000000000      // Stein f�llt auf Etwas
#define SOUND_EXPLOSION         0b1000000000000000000000000000000000000000000000000100000000000000      // Explosion (unabh�ngig vom sichtbaren Bereich aktiv)
#define SOUND_DYNAMITE          0b0000000000000000000000000000000000000000000000001000000000000000      // Dynamit aktiv
#define SOUND_POOL_BLUB         0b0000000000000000000000000000000000000000000000010000000000000000      // Etwas ist ins S�urebecken gefallen
#define SOUND_MAN_TAKE          0b0000000000000000000000000000000000000000000000100000000000000000      // Man nimmt etwas auf
#define SOUND_DIG_SAND          0b0000000000000000000000000000000000000000000001000000000000000000      // Man gr�bt Sand
#define SOUND_ENDDOOR           0b0000000000000000000000000000000000000000000010000000000000000000      // Man geht in die Endt�r
#define SOUND_MAN_CRIES         0b0000000000000000000000000000000000000000000100000000000000000000      // Man schreit
#define SOUND_CHEESE            0b0000000000000000000000000000000000000000001000000000000000000000      // K�se breitet sich aus
#define SOUND_END_BELL          0b1000000000000000000000000000000000000000010000000000000000000000      // Zeit ist bald um (unabh�ngig vom sichtbaren Bereich aktiv)
#define SOUND_SQUEAK            0b0000000000000000000000000000000000000000100000000000000000000000      // Saphir gequetscht / Objekt durch blaue Wand / Perle zerbricht
#define SOUND_SWITCH            0b1000000000000000000000000000000000000001000000000000000000000000      // Ein Schalter wird bet�tigt
#define SOUND_MOLE              0b0000000000000000000000000000000000000010000000000000000000000000      // Maulwurf
#define SOUND_WHEEL_TIMEDOOR    0b1000000000000000000000000000000000000100000000000000000000000000      // Rad f�r Zeit-T�r
#define SOUND_DOOR_OPEN_CLOSE   0b0000000000000000000000000000000000001000000000000000000000000000      // T�r �ffnen / schlie�en
#define SOUND_REPLICATOR_PLOP   0b0000000000000000000000000000000000010000000000000000000000000000      // Replikator erzeugt neues Objekt
#define SOUND_DYNAMITE_START    0b0000000000000000000000000000000000100000000000000000000000000000      // Dynamit wird gestartet
#define SOUND_CONVEYORBELT      0b0000000000000000000000000000000001000000000000000000000000000000      // Laufband
#define SOUND_TELEPORTER        0b0000000000000000000000000000000010000000000000000000000000000000      // Teleporter
#define SOUND_WALL_STEEL_GROWS  0b0000000000000000000000000000000100000000000000000000000000000000      // Mauer und Stahl w�chst
#define SOUND_REMOTE_BOMB       0b1000000000000000000000000000001000000000000000000000000000000000      // Mindestens eine ferngesteuerte Bombe hat sich bewegt
#define SOUND_PIPE              0b0000000000000000000000000000010000000000000000000000000000000000      // Man l�uft in R�hre


#define MAX_WAV_CHUNKS          64          // Maximale Anzahl von WAV-Chunks

typedef struct {
    uint64_t uAllSounds;                    // OR-verkn�pfte Sounds, die abgespielt werden sollen
} GAMESOUND;

int InitGameSound(void);
void PreparePlaySound(uint64_t uSound, uint32_t uCoordinate);
int PlayAllSounds(void);                    // Mixt alle Sounds in GameSound.uAllSounds und spielt diese ab.


void FreeWavChunks(void);
int CreateWavChunks(void);

#endif // SOUND_H_INCLUDED
