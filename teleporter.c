#include <stdio.h>
#include "EmeraldMine.h"
#include "mystd.h"
#include "teleporter.h"

extern PLAYFIELD Playfield;


/*----------------------------------------------------------------------------
Name:           PrintTeleporters
------------------------------------------------------------------------------
Beschreibung: Zeigt die Anzahl und Koordinaten der Teleporter an.
Parameter
      Eingang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void PrintTeleporters(void) {
    uint32_t I;

    printf("TeleporterRedCounter:        %u\r\n",Playfield.uTeleporterRedCounter);
    for (I = 0; I < Playfield.uTeleporterRedCounter; I++) {
        printf("   Coord[%02u]:                %u\r\n",I,Playfield.puTeleporterRedCoordinates[I]);
    }
    printf("TeleporterYellowCounter:     %u\r\n",Playfield.uTeleporterYellowCounter);
    for (I = 0; I < Playfield.uTeleporterYellowCounter; I++) {
        printf("   Coord[%02u]:                %u\r\n",I,Playfield.puTeleporterYellowCoordinates[I]);
    }
    printf("TeleporterGreenCounter:      %u\r\n",Playfield.uTeleporterGreenCounter);
    for (I = 0; I < Playfield.uTeleporterGreenCounter; I++) {
        printf("   Coord[%02u]:                %u\r\n",I,Playfield.puTeleporterGreenCoordinates[I]);
    }
    printf("TeleporterBlueCounter:       %u\r\n",Playfield.uTeleporterBlueCounter);
    for (I = 0; I < Playfield.uTeleporterBlueCounter; I++) {
        printf("   Coord[%02u]:                %u\r\n",I,Playfield.puTeleporterBlueCoordinates[I]);
    }
}


/*----------------------------------------------------------------------------
Name:           InitTeleporter
------------------------------------------------------------------------------
Beschreibung: Initialisiert die Werte für die Teleporter in der Struktur Playfield.x.
              Diese Funktion wird einmalig aus EmeraldMineMainMenu() heraus
              aufgerufen.

Parameter
      Eingang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void InitTeleporter(void) {
    Playfield.uTeleporterRedCounter = 0;                      // Anzahl roter Teleporter
    Playfield.puTeleporterRedCoordinates = NULL;              // lineare Koordinaten, rote Teleporter
    Playfield.uTeleporterYellowCounter = 0;                   // Anzahl gelber Teleporter
    Playfield.puTeleporterYellowCoordinates = NULL;           // lineare Koordinaten, gelbe Teleporter
    Playfield.uTeleporterGreenCounter = 0;                    // Anzahl grüner Teleporter
    Playfield.puTeleporterGreenCoordinates = NULL;            // lineare Koordinaten, grüne Teleporter
    Playfield.uTeleporterBlueCounter = 0;                     // Anzahl blauer Teleporter
    Playfield.puTeleporterBlueCoordinates = NULL;             // lineare Koordinaten, blaue Teleporter
}


/*----------------------------------------------------------------------------
Name:           FreeTeleporterCoordinates
------------------------------------------------------------------------------
Beschreibung: Gibt den Speicher für alle Teleporter-Koordinaten wieder frei und
              setzt die Anzahl der Teleporter auf 0.
Parameter
      Eingang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void FreeTeleporterCoordinates(void) {
    Playfield.uTeleporterRedCounter = 0;                      // Anzahl roter Teleporter
    SAFE_FREE(Playfield.puTeleporterRedCoordinates);          // lineare Koordinaten, rote Teleporter
    Playfield.uTeleporterYellowCounter = 0;                   // Anzahl gelber Teleporter
    SAFE_FREE(Playfield.puTeleporterYellowCoordinates);       // lineare Koordinaten, gelbe Teleporter
    Playfield.uTeleporterGreenCounter = 0;                    // Anzahl grüner Teleporter
    SAFE_FREE(Playfield.puTeleporterGreenCoordinates);        // lineare Koordinaten, grüne Teleporter
    Playfield.uTeleporterBlueCounter = 0;                     // Anzahl blauer Teleporter
    SAFE_FREE(Playfield.puTeleporterBlueCoordinates);         // lineare Koordinaten, blaue Teleporter
}


/*----------------------------------------------------------------------------
Name:           SearchTeleporter
------------------------------------------------------------------------------
Beschreibung: Sucht und zählt im Spielfeld die Teleporter und legt die Koordinaten
              in der Struktur Playfield.x ab.
              Die Struktur Playfield.x muss hierzu bereits ein initialisiertes
              Spielfeld enthalten und die Level-Dimension muss bekannt sein.
Parameter
      Eingang: -
Rückgabewert:  int, 0 = OK, sonst Fehler
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
int SearchTeleporter(void) {
    int nErrorCode = 0;
    uint32_t I;             // lineare Koordinate
    uint16_t uElement;

    FreeTeleporterCoordinates();
    for (I = 0; (I < Playfield.uLevel_X_Dimension * Playfield.uLevel_Y_Dimension) && (nErrorCode == 0); I++) {
        uElement = Playfield.pLevel[I];
        switch (uElement) {
            case(EMERALD_TELEPORTER_RED):
            case(EMERALD_TELEPORTER_YELLOW):
            case(EMERALD_TELEPORTER_GREEN):
            case(EMERALD_TELEPORTER_BLUE):
                nErrorCode = AddTeleporterCoordinate(uElement,I);
                break;
        }
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           AddTeleporterCoordinate
------------------------------------------------------------------------------
Beschreibung: Fügt eine neue Koordinate für einen Teleporter hinzu.
Parameter
      Eingang: uTeleporterElement, uint16_t, Teleporter
                    EMERALD_TELEPORTER_RED,
                    EMERALD_TELEPORTER_YELLOW,
                    EMERALD_TELEPORTER_GREEN,
                    EMERALD_TELEPORTER_BLUE
Rückgabewert:  int, 0 = OK, sonst Fehler
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
int AddTeleporterCoordinate(uint16_t uTeleporterElement,uint32_t uCoordinate) {
    int nErrorCode;

    if (uCoordinate < (Playfield.uLevel_X_Dimension * Playfield.uLevel_Y_Dimension)) {
        switch (uTeleporterElement) {
            case(EMERALD_TELEPORTER_RED):
                Playfield.puTeleporterRedCoordinates = realloc(Playfield.puTeleporterRedCoordinates,(Playfield.uTeleporterRedCounter + 1) * sizeof(uint32_t));
                if (Playfield.puTeleporterRedCoordinates != NULL) {
                    Playfield.puTeleporterRedCoordinates[Playfield.uTeleporterRedCounter] = uCoordinate;
                    Playfield.uTeleporterRedCounter++;
                    nErrorCode = 0;
                } else {
                    SDL_Log("%s: realloc() for red teleporter failed. Teleporters: %u",__FUNCTION__,Playfield.uTeleporterRedCounter);
                    nErrorCode = -1;
                }
                break;
            case(EMERALD_TELEPORTER_YELLOW):
                Playfield.puTeleporterYellowCoordinates = realloc(Playfield.puTeleporterYellowCoordinates,(Playfield.uTeleporterYellowCounter + 1) * sizeof(uint32_t));
                if (Playfield.puTeleporterYellowCoordinates != NULL) {
                    Playfield.puTeleporterYellowCoordinates[Playfield.uTeleporterYellowCounter] = uCoordinate;
                    Playfield.uTeleporterYellowCounter++;
                    nErrorCode = 0;
                } else {
                    SDL_Log("%s: realloc() for yellow teleporter failed. Teleporters: %u",__FUNCTION__,Playfield.uTeleporterYellowCounter);
                    nErrorCode = -1;
                }
                break;
            case(EMERALD_TELEPORTER_GREEN):
                Playfield.puTeleporterGreenCoordinates = realloc(Playfield.puTeleporterGreenCoordinates,(Playfield.uTeleporterGreenCounter + 1) * sizeof(uint32_t));
                if (Playfield.puTeleporterGreenCoordinates != NULL) {
                    Playfield.puTeleporterGreenCoordinates[Playfield.uTeleporterGreenCounter] = uCoordinate;
                    Playfield.uTeleporterGreenCounter++;
                    nErrorCode = 0;
                } else {
                    SDL_Log("%s: realloc() for green teleporter failed. Teleporters: %u",__FUNCTION__,Playfield.uTeleporterGreenCounter);
                    nErrorCode = -1;
                }
                break;
            case(EMERALD_TELEPORTER_BLUE):
                Playfield.puTeleporterBlueCoordinates = realloc(Playfield.puTeleporterBlueCoordinates,(Playfield.uTeleporterBlueCounter + 1) * sizeof(uint32_t));
                if (Playfield.puTeleporterBlueCoordinates != NULL) {
                    Playfield.puTeleporterBlueCoordinates[Playfield.uTeleporterBlueCounter] = uCoordinate;
                    Playfield.uTeleporterBlueCounter++;
                    nErrorCode = 0;
                } else {
                    SDL_Log("%s: realloc() for blue teleporter failed. Teleporters: %u",__FUNCTION__,Playfield.uTeleporterBlueCounter);
                    nErrorCode = -1;
                }
                break;
            default:
                SDL_Log("%s: bad element: 0x%X",__FUNCTION__,uTeleporterElement);
                nErrorCode = -1;
        }
    } else {
        SDL_Log("%s: bad coordinate: %u, Level-X: %u   Level-Y: %u",__FUNCTION__,uCoordinate,Playfield.uLevel_X_Dimension,Playfield.uLevel_Y_Dimension);
        nErrorCode = -1;
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           GetDestinationTeleporterCoordinate
------------------------------------------------------------------------------
Beschreibung: Ermittelt anhand einer Teleporter-Quell-Koordinate eine Teleporter-Ziel-Koordinate.
              Auf der Quell-Koordinate muss sich ein Teleporter befinden.
Parameter
      Eingang: uSourceCoordinate, uint32_t, lineare Quell-Teleporter-Koordinate
               uAnimation, uint32_t, gewünschte Richtung des Man
Rückgabewert:  uint32_t, Teleporter-Ziel-Koordinate, ansonsten EMERALD_INVALID_TELEPORTER_COORDINATE
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
uint32_t GetDestinationTeleporterCoordinate(uint32_t uSourceCoordinate, uint32_t uAnimation) {
    uint32_t uDestinationCoordinate = EMERALD_INVALID_TELEPORTER_COORDINATE;
    int nIndex;

    if (uSourceCoordinate < (Playfield.uLevel_X_Dimension * Playfield.uLevel_Y_Dimension)) {
        switch (Playfield.pLevel[uSourceCoordinate]) {
            case(EMERALD_TELEPORTER_RED):
                if (Playfield.uTeleporterRedCounter == 2) {
                    if (Playfield.puTeleporterRedCoordinates[0] != uSourceCoordinate) {
                        uDestinationCoordinate = Playfield.puTeleporterRedCoordinates[0];
                    } else if (Playfield.puTeleporterRedCoordinates[1] != uSourceCoordinate) {
                        uDestinationCoordinate = Playfield.puTeleporterRedCoordinates[1];
                    }
                } else if (Playfield.uTeleporterRedCounter > 2) {
                    do {
                        nIndex = randn(0, Playfield.uTeleporterRedCounter - 1);
                        uDestinationCoordinate = Playfield.puTeleporterRedCoordinates[nIndex];
                    } while (uSourceCoordinate == uDestinationCoordinate);
                }
                break;
            case(EMERALD_TELEPORTER_YELLOW):
                if (Playfield.uTeleporterYellowCounter == 2) {
                    if (Playfield.puTeleporterYellowCoordinates[0] != uSourceCoordinate) {
                        uDestinationCoordinate = Playfield.puTeleporterYellowCoordinates[0];
                    } else if (Playfield.puTeleporterYellowCoordinates[1] != uSourceCoordinate) {
                        uDestinationCoordinate = Playfield.puTeleporterYellowCoordinates[1];
                    }
                } else if (Playfield.uTeleporterYellowCounter > 2) {
                    do {
                        nIndex = randn(0, Playfield.uTeleporterYellowCounter - 1);
                        uDestinationCoordinate = Playfield.puTeleporterYellowCoordinates[nIndex];
                    } while (uSourceCoordinate == uDestinationCoordinate);
                }
                break;
            case(EMERALD_TELEPORTER_GREEN):
                if (Playfield.uTeleporterGreenCounter == 2) {
                    if (Playfield.puTeleporterGreenCoordinates[0] != uSourceCoordinate) {
                        uDestinationCoordinate = Playfield.puTeleporterGreenCoordinates[0];
                    } else if (Playfield.puTeleporterGreenCoordinates[1] != uSourceCoordinate) {
                        uDestinationCoordinate = Playfield.puTeleporterGreenCoordinates[1];
                    }
                } else if (Playfield.uTeleporterGreenCounter > 2) {
                    do {
                        nIndex = randn(0, Playfield.uTeleporterGreenCounter - 1);
                        uDestinationCoordinate = Playfield.puTeleporterGreenCoordinates[nIndex];
                    } while (uSourceCoordinate == uDestinationCoordinate);
                }
                break;
            case(EMERALD_TELEPORTER_BLUE):
                 if (Playfield.uTeleporterBlueCounter == 2) {
                    if (Playfield.puTeleporterBlueCoordinates[0] != uSourceCoordinate) {
                        uDestinationCoordinate = Playfield.puTeleporterBlueCoordinates[0];
                    } else if (Playfield.puTeleporterBlueCoordinates[1] != uSourceCoordinate) {
                        uDestinationCoordinate = Playfield.puTeleporterBlueCoordinates[1];
                    }
                } else if (Playfield.uTeleporterBlueCounter > 2) {
                    do {
                        nIndex = randn(0, Playfield.uTeleporterBlueCounter - 1);
                        uDestinationCoordinate = Playfield.puTeleporterBlueCoordinates[nIndex];
                    } while (uSourceCoordinate == uDestinationCoordinate);
                }
                break;
        }
    }
    if (uDestinationCoordinate != EMERALD_INVALID_TELEPORTER_COORDINATE) {
        // Ziel-Teleporter konnte gewählt werden -> jetzt prüfen, ob Feld neben Teleporter frei ist
        switch (uAnimation) {
            case (EMERALD_ANIM_UP):
                // Ist über Ziel-Teleporter besetzt?
                if (Playfield.pLevel[uDestinationCoordinate - Playfield.uLevel_X_Dimension] != EMERALD_SPACE) {
                    uDestinationCoordinate = EMERALD_INVALID_TELEPORTER_COORDINATE;
                }
                break;
            case (EMERALD_ANIM_DOWN):
                // Ist unter Ziel-Teleporter besetzt?
                if (Playfield.pLevel[uDestinationCoordinate + Playfield.uLevel_X_Dimension] != EMERALD_SPACE) {
                    uDestinationCoordinate = EMERALD_INVALID_TELEPORTER_COORDINATE;
                }
                break;
            case (EMERALD_ANIM_LEFT):
                // Ist links neben Ziel-Teleporter besetzt?
                if (Playfield.pLevel[uDestinationCoordinate - 1] != EMERALD_SPACE) {
                    uDestinationCoordinate = EMERALD_INVALID_TELEPORTER_COORDINATE;
                }
                break;
            case (EMERALD_ANIM_RIGHT):
                // Ist rechts neben Ziel-Teleporter besetzt?
                if (Playfield.pLevel[uDestinationCoordinate + 1] != EMERALD_SPACE) {
                    uDestinationCoordinate = EMERALD_INVALID_TELEPORTER_COORDINATE;
                }
                break;
            default:
                uDestinationCoordinate = EMERALD_INVALID_TELEPORTER_COORDINATE;
                break;
        }
    }
    return uDestinationCoordinate;
}
