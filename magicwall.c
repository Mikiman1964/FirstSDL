#include "EmeraldMine.h"
#include "magicwall.h"
#include "sound.h"

extern PLAYFIELD Playfield;

/*----------------------------------------------------------------------------
Name:           CheckRunningMagicWall
------------------------------------------------------------------------------
Beschreibung: Prüft, ob MagicWall angehalten werden muss, da Zeit abgelaufen.
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void CheckRunningMagicWall(void) {
    if (Playfield.bMagicWallRunning) {          // Läuft bereits Magic Wall?
        if (Playfield.uTimeMagicWallLeft > 0) {
            Playfield.uTimeMagicWallLeft--;
        }
        if (Playfield.uTimeMagicWallLeft == 0) {
            // Magic Wall anhalten, da Zeit abgelaufen ist
            Playfield.bMagicWallRunning = false;
        }
    }
}


/*----------------------------------------------------------------------------
Name:           CheckLight
------------------------------------------------------------------------------
Beschreibung: Prüft, ob Licht wieder ausgeschaltet werden muss, da Zeit abgelaufen.
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void CheckLight(void) {
    if (Playfield.bLightOn) {          // Licht noch an ?
        if (Playfield.uTimeLightLeft > 0) {
            Playfield.uTimeLightLeft--;
        }
        if (Playfield.uTimeLightLeft == 0) {
            // Licht ausschalten, da Zeit abgelaufen ist
            Playfield.bLightOn = false;
        }
    }
}


/*----------------------------------------------------------------------------
Name:           CheckTimeDoorOpen
------------------------------------------------------------------------------
Beschreibung: Prüft, ob Zeit-Tür wieder geschlossen werden muss, da Zeit abgelaufen.
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void CheckTimeDoorOpen(void) {
    if (Playfield.uTimeDoorTimeLeft > 0) {          // Noch Zeit übrig ?
        Playfield.uTimeDoorTimeLeft--;
    }
}


/*----------------------------------------------------------------------------
Name:           ElementGoesMagicWall
------------------------------------------------------------------------------
Beschreibung: Prüft, ob unter der Magic Wall frei ist. Wenn ja, wird dort ein
              neues temporäres Element (EMERALD_INVALID->Zielelement) entstehen.
Parameter
      Eingang: I, uint32_t, aktuelle Position des Elements, das in Magic Wall eintaucht
               DestElement, uint32_t, Zielelement, z.B. EMERALD_EMERALD
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ElementGoesMagicWall(uint32_t I, uint32_t uDestElement) {
    if (Playfield.pLevel[I + 2 * Playfield.uLevel_X_Dimension] == EMERALD_SPACE) {
        // neuen Platz mit ungültigem Element besetzen
        Playfield.pLevel[I + 2 * Playfield.uLevel_X_Dimension] = EMERALD_INVALID;
        // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
        Playfield.pInvalidElement[I + 2 * Playfield.uLevel_X_Dimension] = uDestElement;
        Playfield.pStatusAnimation[I + 2 * Playfield.uLevel_X_Dimension] = EMERALD_ANIM_DOWN_SELF | EMERALD_ANIM_CLEAN_UP_DOUBLESPEED;
        // Aktuelles Element auf Animation "unten"
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_DOWN;
        // Magic Wall auf neues Element initialisieren
        Playfield.pInvalidElement[I + Playfield.uLevel_X_Dimension] = uDestElement;
        Playfield.pStatusAnimation[I + Playfield.uLevel_X_Dimension] = EMERALD_ANIM_SAG_OUT_MAGIC_WALL | EMERALD_ANIM_AVOID_DOUBLE_CONTROL;
    } else {
        SDL_Log("%s: Magic wall is not free for element %x with StatusAnim: %x",__FUNCTION__,uDestElement,Playfield.pStatusAnimation[I]);
    }
}


/*----------------------------------------------------------------------------
Name:           ControlMagicWall
------------------------------------------------------------------------------
Beschreibung: Setzt die Magic Wall nach Erzeugung eines gewandelten Elements zurück.
Parameter
      Eingang: I, uint32_t, aktuelle Position des Elements, das aus Magic Wall kommt
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlMagicWall(uint32_t I) {
    if (Playfield.bMagicWallRunning) {
        PreparePlaySound(SOUND_MAGIC_WALL,I);
    }
    // Doppelte Steuerung vermeiden
    if ((Playfield.pStatusAnimation[I] & 0x00FF0000) == EMERALD_ANIM_AVOID_DOUBLE_CONTROL) {
        Playfield.pStatusAnimation[I] = Playfield.pStatusAnimation[I] & 0xFF00FFFF;
        SDL_Log("%s: ack double control",__FUNCTION__);
        return;
    } else {
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
    }
}
