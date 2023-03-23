#include "EmeraldMine.h"
#include "replicator.h"
#include "sound.h"

extern PLAYFIELD Playfield;

/*----------------------------------------------------------------------------
Name:           ControlRedReplicator
------------------------------------------------------------------------------
Beschreibung: Steuert den roten Repliktor
Parameter
      Eingang: I, uint32_t, Position der oberen Mittelteils des Replikators
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlRedReplicator(uint32_t I) {
    if (Playfield.bReplicatorRedOn) {          // Laufen die roten Replikatoren?
        PreparePlaySound(SOUND_REPLICATOR,I);
        if (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_SPACE) {   // Ist der Replikator frei?
            Playfield.pLevel[I + Playfield.uLevel_X_Dimension] = Playfield.uReplicatorRedObject;
            Playfield.pStatusAnimation[I + Playfield.uLevel_X_Dimension] = EMERALD_ANIM_BORN1;
            PreparePlaySound(SOUND_REPLICATOR_PLOP,I);
        }
    }
}


/*----------------------------------------------------------------------------
Name:           ControlYellowReplicator
------------------------------------------------------------------------------
Beschreibung: Steuert den gelben Repliktor
Parameter
      Eingang: I, uint32_t, Position der oberen Mittelteils des Replikators
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlYellowReplicator(uint32_t I) {
    if (Playfield.bReplicatorYellowOn) {          // Laufen die gelben Replikatoren?
        PreparePlaySound(SOUND_REPLICATOR,I);
        if (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_SPACE) {   // Ist der Replikator frei?
            Playfield.pLevel[I + Playfield.uLevel_X_Dimension] = Playfield.uReplicatorYellowObject;
            Playfield.pStatusAnimation[I + Playfield.uLevel_X_Dimension] = EMERALD_ANIM_BORN1;
        }
    }
}


/*----------------------------------------------------------------------------
Name:           ControlGreenReplicator
------------------------------------------------------------------------------
Beschreibung: Steuert den grünen Repliktor
Parameter
      Eingang: I, uint32_t, Position der oberen Mittelteils des Replikators
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlGreenReplicator(uint32_t I) {
    if (Playfield.bReplicatorGreenOn) {          // Laufen die grünen Replikatoren?
        PreparePlaySound(SOUND_REPLICATOR,I);
        if (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_SPACE) {   // Ist der Replikator frei?
            Playfield.pLevel[I + Playfield.uLevel_X_Dimension] = Playfield.uReplicatorGreenObject;
            Playfield.pStatusAnimation[I + Playfield.uLevel_X_Dimension] = EMERALD_ANIM_BORN1;
        }
    }
}


/*----------------------------------------------------------------------------
Name:           ControlBlueReplicator
------------------------------------------------------------------------------
Beschreibung: Steuert den blauen Repliktor
Parameter
      Eingang: I, uint32_t, Position der oberen Mittelteils des Replikators
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlBlueReplicator(uint32_t I) {
    if (Playfield.bReplicatorBlueOn) {          // Laufen die blauen Replikatoren?
        PreparePlaySound(SOUND_REPLICATOR,I);
        if (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_SPACE) {   // Ist der Replikator frei?
            Playfield.pLevel[I + Playfield.uLevel_X_Dimension] = Playfield.uReplicatorBlueObject;
            Playfield.pStatusAnimation[I + Playfield.uLevel_X_Dimension] = EMERALD_ANIM_BORN1;
        }
    }
}

