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
Rückgabewert:  int32_t, 0 = Alles OK, sonst Fehler (Savegame.pPlayfield = NULL)
Seiteneffekte: Playfield.x, Savegame.x
------------------------------------------------------------------------------*/
int32_t QuickSaveGame(void) {
    int32_t nErrorCode = -1;

    FreeSaveGame();     // Falls es bereits ein SaveGame gibt, dieses freigeben
    // Speicher für Savegame allozieren
    Savegame.pLevel = (uint16_t*)malloc(Playfield.uLevel_XY_Dimension * sizeof(uint16_t));
    Savegame.pInvalidElement = (uint16_t*)malloc(Playfield.uLevel_XY_Dimension * sizeof(uint16_t));
    Savegame.pSlimeElement = (uint16_t*)malloc(Playfield.uLevel_XY_Dimension * sizeof(uint16_t));
    Savegame.pStatusAnimation = (uint32_t*)malloc(Playfield.uLevel_XY_Dimension * sizeof(uint32_t));
    Savegame.pLastStatusAnimation = (uint32_t*)malloc(Playfield.uLevel_XY_Dimension * sizeof(uint32_t));
    Savegame.pLastYamSlimeDirection = (uint8_t*)malloc(Playfield.uLevel_XY_Dimension);

    if ((Savegame.pLevel != NULL) && (Savegame.pInvalidElement != NULL) && (Savegame.pSlimeElement != NULL) &&
       (Savegame.pStatusAnimation != NULL) && (Savegame.pLastStatusAnimation != NULL) && (Savegame.pLastYamSlimeDirection != NULL)) {
        nErrorCode = 0;
        // Aktuelles Spiel speichern
        memcpy(Savegame.pLevel,Playfield.pLevel,Playfield.uLevel_XY_Dimension * sizeof(uint16_t));
        memcpy(Savegame.pInvalidElement,Playfield.pInvalidElement,Playfield.uLevel_XY_Dimension * sizeof(uint16_t));
        memcpy(Savegame.pSlimeElement,Playfield.pSlimeElement,Playfield.uLevel_XY_Dimension * sizeof(uint16_t));
        memcpy(Savegame.pStatusAnimation,Playfield.pStatusAnimation,Playfield.uLevel_XY_Dimension * sizeof(uint32_t));
        memcpy(Savegame.pLastStatusAnimation,Playfield.pLastStatusAnimation,Playfield.uLevel_XY_Dimension * sizeof(uint32_t));
        memcpy(Savegame.pLastYamSlimeDirection,Playfield.pLastYamSlimeDirection,Playfield.uLevel_XY_Dimension);
        Savegame.bReadyToGo = Playfield.bReadyToGo;
        Savegame.bHasRedKey = Playfield.bHasRedKey;
        Savegame.bHasGreenKey = Playfield.bHasGreenKey;
        Savegame.bHasBlueKey = Playfield.bHasBlueKey;
        Savegame.bHasYellowKey = Playfield.bHasYellowKey;
        Savegame.bHasGeneralKey = Playfield.bHasGeneralKey;
        Savegame.bWheelRunning = Playfield.bWheelRunning;
        Savegame.bMagicWallRunning = Playfield.bMagicWallRunning;
        Savegame.bMagicWallWasOn = Playfield.bMagicWallWasOn;
        Savegame.bLightOn = Playfield.bLightOn;
        Savegame.bPushStone = Playfield.bPushStone;
        Savegame.bManDead = Playfield.bManDead;
        Savegame.bWellDone = Playfield.bWellDone;
        Savegame.bManProtected = Playfield.bManProtected;
        Savegame.bLightBarrierRedOn = Playfield.bLightBarrierRedOn;
        Savegame.bLightBarrierGreenOn = Playfield.bLightBarrierGreenOn;
        Savegame.bLightBarrierBlueOn = Playfield.bLightBarrierBlueOn;
        Savegame.bLightBarrierYellowOn = Playfield.bLightBarrierYellowOn;
        Savegame.bTimeDoorOpen = Playfield.bTimeDoorOpen;
        Savegame.bSwitchDoorState = Playfield.bSwitchDoorState;
        Savegame.bSwitchDoorImpluse = Playfield.bSwitchDoorImpluse;
        Savegame.bReplicatorRedOn = Playfield.bReplicatorRedOn;
        Savegame.bReplicatorGreenOn = Playfield.bReplicatorGreenOn;
        Savegame.bReplicatorBlueOn = Playfield.bReplicatorBlueOn;
        Savegame.bReplicatorYellowOn = Playfield.bReplicatorYellowOn;
        Savegame.uConveybeltRedState = Playfield.uConveybeltRedState;
        Savegame.uConveybeltRedDirection = Playfield.uConveybeltRedDirection;
        Savegame.uConveybeltGreenState = Playfield.uConveybeltGreenState;
        Savegame.uConveybeltGreenDirection = Playfield.uConveybeltGreenDirection;
        Savegame.uConveybeltBlueState = Playfield.uConveybeltBlueState;
        Savegame.uConveybeltBlueDirection = Playfield.uConveybeltBlueDirection;
        Savegame.uConveybeltYellowState = Playfield.uConveybeltYellowState;
        Savegame.uConveybeltYellowDirection = Playfield.uConveybeltYellowDirection;
        strcpy(Savegame.szMd5String,Playfield.szMd5String);
        Savegame.uEmeraldsToCollect = Playfield.uEmeraldsToCollect;
        Savegame.uTimeToPlay = Playfield.uTimeToPlay;
        Savegame.uTimeWheelRotationLeft = Playfield.uTimeWheelRotationLeft;
        Savegame.uTimeDoorTimeLeft = Playfield.uTimeDoorTimeLeft;
        Savegame.uShieldCoinTimeLeft = Playfield.uShieldCoinTimeLeft;
        Savegame.uWheelRunningXpos = Playfield.uWheelRunningXpos;
        Savegame.uWheelRunningYpos = Playfield.uWheelRunningYpos;
        Savegame.uTimeMagicWallLeft = Playfield.uTimeMagicWallLeft;
        Savegame.uTimeLightLeft = Playfield.uTimeLightLeft;
        Savegame.uDynamiteCount = Playfield.uDynamiteCount;
        Savegame.uHammerCount = Playfield.uHammerCount;
        Savegame.uWhiteKeyCount = Playfield.uWhiteKeyCount;
        Savegame.uManXpos = Playfield.uManXpos;
        Savegame.uManYpos = Playfield.uManYpos;
        Savegame.uYamExplosion = Playfield.uYamExplosion;
        Savegame.uDynamitePos = Playfield.uDynamitePos;
        Savegame.uDynamiteStatusAnim = Playfield.uDynamiteStatusAnim;
        Savegame.uPlayTimeStart = Playfield.uPlayTimeStart;
        Savegame.uPlayTimeEnd = Playfield.uPlayTimeEnd;
        Savegame.uTotalScore = Playfield.uTotalScore;
        // SDL_Log("%s: game saved: MD5: %s",__FUNCTION__,Savegame.szMd5String);
    } else {
        SDL_Log("%s: Error: can not allocate memory for savegame!",__FUNCTION__);
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           IsQuickLoadGameAvailable
------------------------------------------------------------------------------
Beschreibung: Prüft, ob ein gespeichertes Quicksave-Game zur Verfügung steht und
              zum aktuellen Level passt.
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  bool, true = Game steht zur Verfügung, sonst nicht
Seiteneffekte: Playfield.x, Savegame.x
------------------------------------------------------------------------------*/
bool IsQuickLoadGameAvailable(void) {
    bool bGameAvailable;

    bGameAvailable = (strcmp(Savegame.szMd5String,Playfield.szMd5String) == 0) &&
                     (Savegame.pLevel != NULL) && (Savegame.pInvalidElement != NULL) && (Savegame.pSlimeElement != NULL) &&
                     (Savegame.pStatusAnimation != NULL) && (Savegame.pLastStatusAnimation != NULL) && (Savegame.pLastYamSlimeDirection != NULL);

    return bGameAvailable;
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
Rückgabewert:  int32_t, 0 = Alles OK, sonst Fehler
Seiteneffekte: Playfield.x, Savegame.x
------------------------------------------------------------------------------*/
int32_t QuickLoadGame(void) {
    int32_t nErrorCode = -1;

    if (IsQuickLoadGameAvailable()) {
        memcpy(Playfield.pLevel,Savegame.pLevel,Playfield.uLevel_XY_Dimension * sizeof(uint16_t));
        memcpy(Playfield.pInvalidElement,Savegame.pInvalidElement,Playfield.uLevel_XY_Dimension * sizeof(uint16_t));
        memcpy(Playfield.pSlimeElement,Savegame.pSlimeElement,Playfield.uLevel_XY_Dimension * sizeof(uint16_t));
        memcpy(Playfield.pStatusAnimation,Savegame.pStatusAnimation,Playfield.uLevel_XY_Dimension * sizeof(uint32_t));
        memcpy(Playfield.pLastStatusAnimation,Savegame.pLastStatusAnimation,Playfield.uLevel_XY_Dimension * sizeof(uint32_t));
        memcpy(Playfield.pLastYamSlimeDirection,Savegame.pLastYamSlimeDirection,Playfield.uLevel_XY_Dimension);
        Playfield.bReadyToGo = Savegame.bReadyToGo;
        Playfield.bHasRedKey = Savegame.bHasRedKey;
        Playfield.bHasGreenKey = Savegame.bHasGreenKey;
        Playfield.bHasBlueKey = Savegame.bHasBlueKey;
        Playfield.bHasYellowKey = Savegame.bHasYellowKey;
        Playfield.bHasGeneralKey = Savegame.bHasGeneralKey;
        Playfield.bWheelRunning = Savegame.bWheelRunning;
        Playfield.bMagicWallRunning = Savegame.bMagicWallRunning;
        Playfield.bMagicWallWasOn = Savegame.bMagicWallWasOn;
        Playfield.bLightOn = Savegame.bLightOn;
        Playfield.bPushStone = Savegame.bPushStone;
        Playfield.bManDead = Savegame.bManDead;
        Playfield.bWellDone = Savegame.bWellDone;
        Playfield.bManProtected = Savegame.bManProtected;
        Playfield.bLightBarrierRedOn = Savegame.bLightBarrierRedOn;
        Playfield.bLightBarrierGreenOn = Savegame.bLightBarrierGreenOn;
        Playfield.bLightBarrierBlueOn = Savegame.bLightBarrierBlueOn;
        Playfield.bLightBarrierYellowOn = Savegame.bLightBarrierYellowOn;
        Playfield.bTimeDoorOpen = Savegame.bTimeDoorOpen;
        Playfield.bSwitchDoorState = Savegame.bSwitchDoorState;
        Playfield.bSwitchDoorImpluse = Savegame.bSwitchDoorImpluse;
        Playfield.bReplicatorRedOn = Savegame.bReplicatorRedOn;
        Playfield.bReplicatorGreenOn = Savegame.bReplicatorGreenOn;
        Playfield.bReplicatorBlueOn = Savegame.bReplicatorBlueOn;
        Playfield.bReplicatorYellowOn = Savegame.bReplicatorYellowOn;
        Playfield.uConveybeltRedState = Savegame.uConveybeltRedState;
        Playfield.uConveybeltRedDirection = Savegame.uConveybeltRedDirection;
        Playfield.uConveybeltGreenState = Savegame.uConveybeltGreenState;
        Playfield.uConveybeltGreenDirection = Savegame.uConveybeltGreenDirection;
        Playfield.uConveybeltBlueState = Savegame.uConveybeltBlueState;
        Playfield.uConveybeltBlueDirection = Savegame.uConveybeltBlueDirection;
        Playfield.uConveybeltYellowState = Savegame.uConveybeltYellowState;
        Playfield.uConveybeltYellowDirection = Savegame.uConveybeltYellowDirection;
        Playfield.uEmeraldsToCollect = Savegame.uEmeraldsToCollect;
        Playfield.uTimeToPlay = Savegame.uTimeToPlay;
        Playfield.uTimeWheelRotationLeft = Savegame.uTimeWheelRotationLeft;
        Playfield.uTimeDoorTimeLeft = Savegame.uTimeDoorTimeLeft;
        Playfield.uShieldCoinTimeLeft = Savegame.uShieldCoinTimeLeft;
        Playfield.uWheelRunningXpos = Savegame.uWheelRunningXpos;
        Playfield.uWheelRunningYpos = Savegame.uWheelRunningYpos;
        Playfield.uTimeMagicWallLeft = Savegame.uTimeMagicWallLeft;
        Playfield.uTimeLightLeft = Savegame.uTimeLightLeft;
        Playfield.uDynamiteCount = Savegame.uDynamiteCount;
        Playfield.uHammerCount = Savegame.uHammerCount;
        Playfield.uWhiteKeyCount = Savegame.uWhiteKeyCount;
        Playfield.uManXpos = Savegame.uManXpos;
        Playfield.uManYpos = Savegame.uManYpos;
        Playfield.uYamExplosion = Savegame.uYamExplosion;
        Playfield.uDynamitePos = Savegame.uDynamitePos;
        Playfield.uDynamiteStatusAnim = Savegame.uDynamiteStatusAnim;
        Playfield.uPlayTimeStart = Savegame.uPlayTimeStart;
        Playfield.uPlayTimeEnd = Savegame.uPlayTimeEnd;
        Playfield.uTotalScore = Savegame.uTotalScore;
        SetVisibleArea();
        // SDL_Log("%s: game loaded: MD5: %s",__FUNCTION__,Savegame.szMd5String);
        nErrorCode = 0;
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           FreeSaveGame
------------------------------------------------------------------------------
Beschreibung: Gibt den Speicher eines gespeicherten Spiels frei.

Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x, Actualplayer.x, SelectedLevelgroup.x
------------------------------------------------------------------------------*/
void FreeSaveGame() {
    SAFE_FREE(Savegame.pLevel);
    SAFE_FREE(Savegame.pInvalidElement);
    SAFE_FREE(Savegame.pSlimeElement);
    SAFE_FREE(Savegame.pStatusAnimation);
    SAFE_FREE(Savegame.pLastStatusAnimation);
    SAFE_FREE(Savegame.pLastYamSlimeDirection);
    memset(&Savegame,0,sizeof(Savegame));
}
