#include <string.h>
#include "EmeraldMine.h"
#include "quicksaveloadgame.h"

SAVEGAME Savegame;
extern ACTUALPLAYER Actualplayer;
extern LEVELGROUP SelectedLevelgroup;
extern PLAYFIELD Playfield;

/*----------------------------------------------------------------------------
Name:           InitSaveGame
------------------------------------------------------------------------------
Beschreibung: Initialisiert die Struktur Game.x.
              Diese Funktion sollte nur einmalig zum Programmstart aufgerufen werden.
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Savegame.x
------------------------------------------------------------------------------*/
void InitSaveGame(void) {
    memset(&Savegame,0,sizeof(Savegame));

}


/*----------------------------------------------------------------------------
Name:           QuickSaveGame
------------------------------------------------------------------------------
Beschreibung: Führt das "Schnell-Speichern" während eines Spiels aus.
              Das Schnell-Speichern wird durch Tastendruck (F9) aufgerufen.
              Das gespeicherte Spiel wird nur im Speicher abgelegt, nicht als Datei.
              Wenn es geklappt hat, wird der Pointer Savegame.pPlayfield gesetzt.
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  int, 0 = Alles OK, sonst Fehler (Savegame.pPlayfield = NULL)
Seiteneffekte: Playfield.x, Actualplayer.x, SelectedLevelgroup.x, Savegame.x
------------------------------------------------------------------------------*/
int QuickSaveGame(void) {
    int nErrorCode = -1;
/*
    FreeSaveGame();     // Falls es bereits ein SaveGame gibt, dieses freigeben
    Savegame.pPlayfield = malloc(sizeof(Playfield));
    if (Savegame.pPlayfield != NULL) {

    uint16_t        *pLevel;
    uint16_t        *pPipeLevel;
    uint32_t        *pStatusAnimation;
    uint32_t        *pLastStatusAnimation;                      // Wird für Explosionen benötigt, damit entsprechende Felder auf Space gesetzt werden können
    POSTANIMATION   *pPostAnimation;                            // wird bisher nur für "Splash" des Säurebeckens verwendet
    uint16_t        *pInvalidElement;                           // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
    uint16_t        *pSlimeElement;                             // Element, welches der Schleim gefressen hat
    uint8_t         *pLastYamSlimeDirection;
    char            *pMessage[EMERALD_MAX_MESSAGES];            // 8 Nachrichtentexte
    uint32_t        *puTeleporterRedCoordinates;                // lineare Koordinaten, rote Teleporter
    uint32_t        *puTeleporterYellowCoordinates;             // lineare Koordinaten, gelbe Teleporter
    uint32_t        *puTeleporterGreenCoordinates;              // lineare Koordinaten, grüne Teleporter
    uint32_t        *puTeleporterBlueCoordinates;               // lineare Koordinaten, blaue Teleporter



    Playfield.pLevel = (uint16_t*)malloc(Playfield.uLevel_XY_Dimension * sizeof(uint16_t));
    Playfield.pPipeLevel = (uint16_t*)malloc(Playfield.uLevel_XY_Dimension * sizeof(uint16_t));
    Playfield.pInvalidElement = (uint16_t*)malloc(Playfield.uLevel_XY_Dimension * sizeof(uint16_t));
    Playfield.pSlimeElement = (uint16_t*)malloc(Playfield.uLevel_XY_Dimension * sizeof(uint16_t));
    Playfield.pStatusAnimation = (uint32_t*)malloc(Playfield.uLevel_XY_Dimension * sizeof(uint32_t));
    Playfield.pLastStatusAnimation = (uint32_t*)malloc(Playfield.uLevel_XY_Dimension * sizeof(uint32_t));
    Playfield.pPostAnimation = (POSTANIMATION*)malloc(Playfield.uLevel_XY_Dimension * sizeof(POSTANIMATION));
    Playfield.pLastYamSlimeDirection = (uint8_t*)malloc(Playfield.uLevel_XY_Dimension);



    }

*/
    return nErrorCode;

}


/*----------------------------------------------------------------------------
Name:           QuickLoadGame
------------------------------------------------------------------------------
Beschreibung: Führt das "Schnell-Laden" während eines Spiels aus.
              Das aktuelle Spiel-Geschehen wird überschrieben.
              Das Schnell-Laden wird durch Tastendruck (F10) aufgerufen.
              Falls kein passendes gespeichertes Spiel vorliegt, wird keine
              Änderung des aktuellen Spiels durchgeführt.

Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  int, 0 = Alles OK, sonst Fehler
Seiteneffekte: Playfield.x, Actualplayer.x, SelectedLevelgroup.x
------------------------------------------------------------------------------*/
int QuickLoadGame(void) {
    SDL_Log("%s",__FUNCTION__);
/*
typedef struct {
    bool        bValid;                                                         // diese Struktur enthält einen validen Spieler
    char        szPlayername[EMERALD_PLAYERNAME_LEN + 1];                       // Aktueller Spieler
    uint8_t     uLevelgroupMd5Hash[16];                                         // aktuell gewählte Levelgruppe als MD5 Hash
    uint32_t    uLevel;                                                         // aktuell gewähltes Level (wird beim Init. auf Handicap gestellt)
    uint32_t    uHandicap;                                                      // aktuelles Handicap in der gewählten Levelgruppe
    uint32_t    uGamesPlayed;                                                   // Anzahl gespielter Level in der gewählten Levelgruppe
    uint32_t    uGamesWon;                                                      // Anzahl gewonnener Spiele in der gewählten Levelgruppe
    uint32_t    uTotalScore;                                                    // Gesamtpunktezahl in der gewählten Levelgruppe
    uint32_t    uPlayTimeS;                                                     // Gesamtspielzeit in Sekunden in der gewählten Levelgruppe
} ACTUALPLAYER;
*/

    return -1;

}


/*----------------------------------------------------------------------------
Name:           FreeSaveGame
------------------------------------------------------------------------------
Beschreibung: Gibt den Speicher eines gespeicherten Spiels frei.

Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  int, 0 = Alles OK, sonst Fehler
Seiteneffekte: Playfield.x, Actualplayer.x, SelectedLevelgroup.x
------------------------------------------------------------------------------*/
void FreeSaveGame() {


}
