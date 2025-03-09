#ifndef QUICKSAVELOADGAME_H_INCLUDED
#define QUICKSAVELOADGAME_H_INCLUDED

#include "EmeraldMine.h"
#include "loadlevel.h"


typedef struct {
    PLAYFIELD *pPlayfield;                                      // nur gültig, wenn != NULL
    ACTUALPLAYER Actualplayer;                                  // enthält Playerinfo, Level und Levelgruppeninfo -> Savegame nur für diesen Player in aktueller Levelgruppe ladbar
} SAVEGAME;


void InitSaveGame(void);                                            // Struktur Savegame.x initialisieren
int QuickSaveGame(void);                                            // Aktuelles Spiel speichern
int QuickLoadGame(void);                                            // Aktuelles Spiel mit gespeichertem Spiel überschreiben
void FreeSaveGame(void);                                            // Gespeichertes Savegame wieder freigeben

#endif // QUICKSAVELOADGAME_H_INCLUDED

