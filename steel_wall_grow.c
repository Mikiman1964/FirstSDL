#include "EmeraldMine.h"
#include "sound.h"
#include "steel_wall_grow.h"

extern PLAYFIELD Playfield;

/*----------------------------------------------------------------------------
Name:           ControlSteelGrowLeft
------------------------------------------------------------------------------
Beschreibung:   Steuert Stahl, der sich nach links ausbreiten möchte.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlSteelGrowLeft(uint32_t I) {
    if (Playfield.pLevel[I - 1] == EMERALD_SPACE) {
        Playfield.pLevel[I - 1] = EMERALD_STEEL_GROWING_LEFT;
        PreparePlaySound(SOUND_WALL_STEEL_GROWS,I);
    }
}


/*----------------------------------------------------------------------------
Name:           ControlSteelGrowingLeft
------------------------------------------------------------------------------
Beschreibung:   Steuert Stahl, der sich gerade nach links ausbreitet.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlSteelGrowingLeft(uint32_t I) {
    Playfield.pLevel[I] = EMERALD_STEEL_GROW_LEFT;
    Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
    ControlSteelGrowLeft(I);
}


/*----------------------------------------------------------------------------
Name:           ControlSteelGrowRight
------------------------------------------------------------------------------
Beschreibung:   Steuert Stahl, der sich nach rechts ausbreiten möchte.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlSteelGrowRight(uint32_t I) {
    if (Playfield.pLevel[I + 1] == EMERALD_SPACE) {
        Playfield.pLevel[I + 1] = EMERALD_STEEL_GROWING_RIGHT;
        Playfield.pStatusAnimation[I + 1] = EMERALD_ANIM_RIGHT | EMERALD_ANIM_AVOID_DOUBLE_CONTROL;
        PreparePlaySound(SOUND_WALL_STEEL_GROWS,I);
    }
}


/*----------------------------------------------------------------------------
Name:           ControlSteelGrowingRight
------------------------------------------------------------------------------
Beschreibung:   Steuert Stahl, der sich gerade nach rechts ausbreitet.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlSteelGrowingRight(uint32_t I) {
    // Doppelte Steuerung vermeiden
    if ((Playfield.pStatusAnimation[I] & 0x00FF0000) == EMERALD_ANIM_AVOID_DOUBLE_CONTROL) {
        Playfield.pStatusAnimation[I] = Playfield.pStatusAnimation[I] & 0xFF00FFFF;
        return;
    }
    Playfield.pLevel[I] = EMERALD_STEEL_GROW_RIGHT;
    Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
    ControlSteelGrowRight(I);
}


/*----------------------------------------------------------------------------
Name:           ControlSteelGrowUp
------------------------------------------------------------------------------
Beschreibung:   Steuert Stahl, der sich nach oben ausbreiten möchte.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlSteelGrowUp(uint32_t I) {
    if (Playfield.pLevel[I - Playfield.uLevel_X_Dimension] == EMERALD_SPACE) {
        Playfield.pLevel[I - Playfield.uLevel_X_Dimension] = EMERALD_STEEL_GROWING_UP;
        Playfield.pStatusAnimation[I - Playfield.uLevel_X_Dimension] = EMERALD_ANIM_UP;
        PreparePlaySound(SOUND_WALL_STEEL_GROWS,I);
    }
}


/*----------------------------------------------------------------------------
Name:           ControlSteelGrowingUp
------------------------------------------------------------------------------
Beschreibung:   Steuert Stahl, der sich gerade nach oben ausbreitet.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlSteelGrowingUp(uint32_t I) {
    Playfield.pLevel[I] = EMERALD_STEEL_GROW_UP;
    Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
    ControlSteelGrowUp(I);
}


/*----------------------------------------------------------------------------
Name:           ControlSteelGrowDown
------------------------------------------------------------------------------
Beschreibung:   Steuert Stahl, der sich nach unten ausbreiten möchte.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlSteelGrowDown(uint32_t I) {
    if ( ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_BORN1) || ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_BORN2) ) {
        // SteelGrowDown kann vom Replikator geboren werden, dann hier nichts machen
        return;
    }
    if (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_SPACE) {
        Playfield.pLevel[I + Playfield.uLevel_X_Dimension] = EMERALD_STEEL_GROWING_DOWN;
        Playfield.pStatusAnimation[I + Playfield.uLevel_X_Dimension] = EMERALD_ANIM_DOWN | EMERALD_ANIM_AVOID_DOUBLE_CONTROL;
        PreparePlaySound(SOUND_WALL_STEEL_GROWS,I);
    }
}


/*----------------------------------------------------------------------------
Name:           ControlSteelGrowingDown
------------------------------------------------------------------------------
Beschreibung:   Steuert Stahl, der sich gerade nach unten ausbreitet.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlSteelGrowingDown(uint32_t I) {
    // Doppelte Steuerung vermeiden
    if ((Playfield.pStatusAnimation[I] & 0x00FF0000) == EMERALD_ANIM_AVOID_DOUBLE_CONTROL) {
        Playfield.pStatusAnimation[I] = Playfield.pStatusAnimation[I] & 0xFF00FFFF;
        return;
    }
    Playfield.pLevel[I] = EMERALD_STEEL_GROW_DOWN;
    Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
    ControlSteelGrowDown(I);
}


/*----------------------------------------------------------------------------
Name:           ControlSteelGrowLeftRight
------------------------------------------------------------------------------
Beschreibung:   Steuert Stahl, der sich nach links und rechts ausbreiten möchte.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: -
------------------------------------------------------------------------------*/
void ControlSteelGrowLeftRight(uint32_t I) {
    ControlSteelGrowLeft(I);
    ControlSteelGrowRight(I);
}


/*----------------------------------------------------------------------------
Name:           ControlSteelGrowUpDown
------------------------------------------------------------------------------
Beschreibung:   Steuert Stahl, der sich nach oben und unten ausbreiten möchte.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: -
------------------------------------------------------------------------------*/
void ControlSteelGrowUpDown(uint32_t I) {
    ControlSteelGrowUp(I);
    ControlSteelGrowDown(I);
}


/*----------------------------------------------------------------------------
Name:           ControlSteelGrowAllDirections
------------------------------------------------------------------------------
Beschreibung:   Steuert Stahl, der sich in alle Richtungen ausbreiten möchte.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: -
------------------------------------------------------------------------------*/
void ControlSteelGrowAllDirections(uint32_t I) {
    ControlSteelGrowLeft(I);
    ControlSteelGrowRight(I);
    ControlSteelGrowUp(I);
    ControlSteelGrowDown(I);
}


// Ab hier Mauer-Funktionen


/*----------------------------------------------------------------------------
Name:           ControlWallGrowLeft
------------------------------------------------------------------------------
Beschreibung:   Steuert eine Mauer, der sich nach links ausbreiten möchte.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlWallGrowLeft(uint32_t I) {
    if (Playfield.pLevel[I - 1] == EMERALD_SPACE) {
        Playfield.pLevel[I - 1] = EMERALD_WALL_GROWING_LEFT;
        Playfield.pStatusAnimation[I - 1] = EMERALD_ANIM_LEFT;
        PreparePlaySound(SOUND_WALL_STEEL_GROWS,I);
    }
}


/*----------------------------------------------------------------------------
Name:           ControlWallGrowingLeft
------------------------------------------------------------------------------
Beschreibung:   Steuert eine Mauer, der sich gerade nach links ausbreitet.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlWallGrowingLeft(uint32_t I) {
    Playfield.pLevel[I] = EMERALD_WALL_GROW_LEFT;
    Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
    ControlWallGrowLeft(I);
}


/*----------------------------------------------------------------------------
Name:           ControlWallGrowRight
------------------------------------------------------------------------------
Beschreibung:   Steuert eine Mauer, der sich nach rechts ausbreiten möchte.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlWallGrowRight(uint32_t I) {
    if (Playfield.pLevel[I + 1] == EMERALD_SPACE) {
        Playfield.pLevel[I + 1] = EMERALD_WALL_GROWING_RIGHT;
        Playfield.pStatusAnimation[I + 1] = EMERALD_ANIM_RIGHT | EMERALD_ANIM_AVOID_DOUBLE_CONTROL;
        PreparePlaySound(SOUND_WALL_STEEL_GROWS,I);
    }
}


/*----------------------------------------------------------------------------
Name:           ControlWallGrowingRight
------------------------------------------------------------------------------
Beschreibung:   Steuert eine Mauer, der sich gerade nach rechts ausbreitet.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlWallGrowingRight(uint32_t I) {
    // Doppelte Steuerung vermeiden
    if ((Playfield.pStatusAnimation[I] & 0x00FF0000) == EMERALD_ANIM_AVOID_DOUBLE_CONTROL) {
        Playfield.pStatusAnimation[I] = Playfield.pStatusAnimation[I] & 0xFF00FFFF;
        return;
    }
    Playfield.pLevel[I] = EMERALD_WALL_GROW_RIGHT;
    Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
    ControlWallGrowRight(I);
}


/*----------------------------------------------------------------------------
Name:           ControlWallGrowUp
------------------------------------------------------------------------------
Beschreibung:   Steuert eine Mauer, der sich nach oben ausbreiten möchte.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlWallGrowUp(uint32_t I) {
    if (Playfield.pLevel[I - Playfield.uLevel_X_Dimension] == EMERALD_SPACE) {
        Playfield.pLevel[I - Playfield.uLevel_X_Dimension] = EMERALD_WALL_GROWING_UP;
        Playfield.pStatusAnimation[I - Playfield.uLevel_X_Dimension] = EMERALD_ANIM_UP;
        PreparePlaySound(SOUND_WALL_STEEL_GROWS,I);
    }
}


/*----------------------------------------------------------------------------
Name:           ControlWallGrowingUp
------------------------------------------------------------------------------
Beschreibung:   Steuert eine Mauer, der sich gerade nach oben ausbreitet.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlWallGrowingUp(uint32_t I) {
    Playfield.pLevel[I] = EMERALD_WALL_GROW_UP;
    Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
    ControlWallGrowUp(I);
}


/*----------------------------------------------------------------------------
Name:           ControlWallGrowDown
------------------------------------------------------------------------------
Beschreibung:   Steuert eine Mauer, der sich nach unten ausbreiten möchte.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlWallGrowDown(uint32_t I) {
    if ( ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_BORN1) || ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_BORN2) ) {
        // WallGrowDown kann vom Replikator geboren werden, dann hier nichts machen
        return;
    }
    if (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_SPACE) {
        Playfield.pLevel[I + Playfield.uLevel_X_Dimension] = EMERALD_WALL_GROWING_DOWN;
        Playfield.pStatusAnimation[I + Playfield.uLevel_X_Dimension] = EMERALD_ANIM_DOWN | EMERALD_ANIM_AVOID_DOUBLE_CONTROL;
        PreparePlaySound(SOUND_WALL_STEEL_GROWS,I);
    }
}


/*----------------------------------------------------------------------------
Name:           ControlWallGrowingDown
------------------------------------------------------------------------------
Beschreibung:   Steuert eine Mauer, der sich gerade nach unten ausbreitet.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlWallGrowingDown(uint32_t I) {
    // Doppelte Steuerung vermeiden
    if ((Playfield.pStatusAnimation[I] & 0x00FF0000) == EMERALD_ANIM_AVOID_DOUBLE_CONTROL) {
        Playfield.pStatusAnimation[I] = Playfield.pStatusAnimation[I] & 0xFF00FFFF;
        return;
    }
    Playfield.pLevel[I] = EMERALD_WALL_GROW_DOWN;
    Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
    ControlWallGrowDown(I);
}


/*----------------------------------------------------------------------------
Name:           ControlWallGrowLeftRight
------------------------------------------------------------------------------
Beschreibung:   Steuert eine Mauer, die sich nach links und rechts ausbreiten möchte.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: -
------------------------------------------------------------------------------*/
void ControlWallGrowLeftRight(uint32_t I) {
    ControlWallGrowLeft(I);
    ControlWallGrowRight(I);
}


/*----------------------------------------------------------------------------
Name:           ControlWallGrowUpDown
------------------------------------------------------------------------------
Beschreibung:   Steuert eine mauer, die sich nach oben und unten ausbreiten möchte.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: -
------------------------------------------------------------------------------*/
void ControlWallGrowUpDown(uint32_t I) {
    ControlWallGrowUp(I);
    ControlWallGrowDown(I);
}


/*----------------------------------------------------------------------------
Name:           ControlWallGrowAllDirections
------------------------------------------------------------------------------
Beschreibung:   Steuert eine Mauer, die sich in alle Richtungen ausbreiten möchte.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: -
------------------------------------------------------------------------------*/
void ControlWallGrowAllDirections(uint32_t I) {
    ControlWallGrowLeft(I);
    ControlWallGrowRight(I);
    ControlWallGrowUp(I);
    ControlWallGrowDown(I);
}
