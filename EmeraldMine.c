/*
TODO
* Element 0x3B, Dynamit an
* Explosionen vor regulärer Levelsteuerung behandeln
* Bei angeschaltetem Licht auch graue Türen mit Farbe anzeigen
*/
#include <SDL2/SDL.h>
#include <stdio.h>
#include <math.h>
#include "EmeraldMine.h"
#include "alien.h"
#include "beetle.h"
#include "bomb.h"
#include "crystal.h"
#include "emerald.h"
#include "explosion.h"
#include "greendrop.h"
#include "KeyboardMouse.h"
#include "loadlevel.h"
#include "lightbarrier.h"
#include "magicwall.h"
#include "man.h"
#include "megabomb.h"
#include "mine.h"
#include "mole.h"
#include "mySDL.h"
#include "mystd.h"
#include "nut.h"
#include "panel.h"
#include "perl.h"
#include "replicator.h"
#include "ruby.h"
#include "sand.h"
#include "saphir.h"
#include "sound.h"
#include "stone.h"
#include "yam.h"


PLAYFIELD Playfield;
extern GAMESOUND GameSound;
extern INPUTSTATES InputStates;
extern MANKEY ManKey;
extern uint8_t ge_uLevel[];
extern uint8_t ge_uBeamColors[];


/*----------------------------------------------------------------------------
Name:           RunGame
------------------------------------------------------------------------------
Beschreibung: Hauptschleifen-Funktion für das Spielen eines Levels.
Parameter
      Eingang: pWindow, SDL_Window, Fenster-Handle
               pRenderer, SDL_Renderer *, Zeiger auf Renderer
      Ausgang: -

Rückgabewert:  int, 0 = kein Fehler, sonst Fehler
Seiteneffekte: Playfield.x, InputStates, ManKey.x, ge_uLevel[], GameSound.x
------------------------------------------------------------------------------*/
int RunGame(SDL_Window *pWindow, SDL_Renderer *pRenderer) {
    bool bLevelRun;
    bool bPrepareLevelExit;
    int nColorDimm;
    int nCheckLevelCount;
    nCheckLevelCount = 0;
    nColorDimm = 100; // war 0
    uint32_t uManDirection = EMERALD_ANIM_STAND;     // Rückgabe von CheckLevel() -> Wohin ist der Man gelaufen?
    uint32_t uKey;
    uint32_t I;
    bool bDimmIn = true;
    bool bDebug = false;

    if (InitGameSound() != 0) {
        return -1;
    }
    SetAllTextureColors(0);
    bLevelRun = (InitialisePlayfield(LEVEL_XML_FILENAME) == 0);
    // Renderer mit schwarz löschen
    SDL_SetRenderDrawColor(pRenderer,0,0,0,SDL_ALPHA_OPAQUE);
    SDL_RenderClear(pRenderer);
    SDL_RenderPresent(pRenderer);
    bPrepareLevelExit = false;
    while (bLevelRun) {
        UpdateManKey();
        if ((InputStates.pKeyboardArray[SDL_SCANCODE_SPACE]) || (InputStates.pKeyboardArray[SDL_SCANCODE_ESCAPE]) || (InputStates.bQuit) || (Playfield.uTimeToPlay == 0)) {
            bPrepareLevelExit = true;
        }

        if (InputStates.pKeyboardArray[SDL_SCANCODE_X]) {
            bDebug = !bDebug;
        }

        if (InputStates.pKeyboardArray[SDL_SCANCODE_P]) {
            PrintPlayfieldValues();
        }
        if (nCheckLevelCount == 0) {
            if ((ManKey.uDirection == MANKEY_NONE) && ((Playfield.uFrameCounter - ManKey.uLastDirectionFrameCount) <= 15)) {
                SDL_Log("%s: use buffered key: dir: %u   dif:%u",__FUNCTION__,ManKey.uLastActiveDirection,Playfield.uFrameCounter - ManKey.uLastDirectionFrameCount);
                uKey = ManKey.uLastActiveDirection;
            } else {
                uKey = ManKey.uDirection;
            }
            uManDirection = ControlGame(uKey);
            PlayAllSounds();
        }
        ScrollAndCenterLevel(uManDirection);
        RenderLevel(pRenderer,&Playfield.nTopLeftXpos,&Playfield.nTopLeftYpos,nCheckLevelCount);  // nCheckLevelCount 0 ... 15
        nCheckLevelCount++;
        if (nCheckLevelCount == 16) {
            nCheckLevelCount = 0;
        }
        if (bPrepareLevelExit) {
            if (nColorDimm > 0) {
                nColorDimm = nColorDimm - 2;
                if (nColorDimm < 0) {
                    nColorDimm = 0;
                }
                SetAllTextureColors(nColorDimm);
            } else {
                bLevelRun = false;
            }
        }
        if ((bDimmIn) && (!bPrepareLevelExit)) {
            nColorDimm = nColorDimm + 1;
            if (nColorDimm >= 100) {
                nColorDimm = 100;
                bDimmIn = false;;
            }
            SetAllTextureColors(nColorDimm);
        }
        if (Playfield.uShowMessageNo != 0) {
            ConfirmMessage(pRenderer);  // Spiel pausiert hier, bis Nachricht bestätigt wurde
        }
        SDL_RenderPresent(pRenderer);   // Renderer anzeigen, lässt Hauptschleife mit ~ 60 Hz (Bild-Wiederholfrequenz) laufen
        SDL_RenderClear(pRenderer);     // Renderer für nächstes Frame löschen
        if (bDebug) {
            WaitForKey(SDL_SCANCODE_RETURN);
        }
    }
    SetAllTextureColors(100);           // Farben beim Verlassen wieder auf volle Helligekit
    SAFE_FREE(Playfield.pLevel);
    SAFE_FREE(Playfield.pStatusAnimation);
    SAFE_FREE(Playfield.pPostAnimation);
    for (I = 0; I < EMERALD_MAX_MESSAGES; I++) {
        SAFE_FREE(Playfield.pMessage[I]);
    }
    SDL_CloseAudioDevice(GameSound.audio_device);
    WaitNoKey();
    return 0;
}


/*----------------------------------------------------------------------------
Name:           ControlGame
------------------------------------------------------------------------------
Beschreibung: Alle 16 Frames bzw. alle 16 Aniomationsschritte wird diese Funktion aufgerufen, um das Spiel zu steuern.
Parameter
      Eingang: uDirection, uint32_t, gewünschte Richtung des Man
      Ausgang: -

Rückgabewert:  uint32_t, tatsächliche Richtung des Man (damit wird Levelscrolling gesteuert)
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
uint32_t ControlGame(uint32_t uDirection) {
    uint32_t uManDirection;
    uint32_t I;
    uint16_t uLevelElement;
    uint32_t uAnimStatus;
    uint32_t uCleanStatus;

    uManDirection = EMERALD_ANIM_STAND;
    // Ab hier das Level und die Status für alle Elemente aus voriger Steuerung vorbereiten
    for (I = 0; I < Playfield.uLevel_X_Dimension * Playfield.uLevel_Y_Dimension; I++) {
        // Dieser Block sorgt bei bewegten Objekten dafür, dass diese
        // ihren neuen Platz (invalides Feld) einnehmen.
        if (Playfield.pLevel[I] == EMERALD_INVALID) {
            uCleanStatus = Playfield.pStatusAnimation[I] & 0x00FF0000;
            if (uCleanStatus != EMERALD_ANIM_CLEAN_NOTHING) {

                if (Playfield.pInvalidElement[I] == EMERALD_NONE) {
                    SDL_Log("%s: warning, set element EMERALD_NONE at position %d",__FUNCTION__,I);
                }
                Playfield.pLevel[I] = Playfield.pInvalidElement[I]; // von invalides auf nächstes Element setzen
                Playfield.pInvalidElement[I] = EMERALD_NONE;
                // Neue Element-Koordinaten des Man berechnen
                if (Playfield.pLevel[I] == EMERALD_MAN) {
                    Playfield.uManXpos = I % Playfield.uLevel_X_Dimension;
                    Playfield.uManYpos = I / Playfield.uLevel_X_Dimension;
                }
                switch (uCleanStatus) {
                    case (EMERALD_ANIM_CLEAN_UP):
                        Playfield.pLevel[I - Playfield.uLevel_X_Dimension] = EMERALD_SPACE;
                        Playfield.pStatusAnimation[I - Playfield.uLevel_X_Dimension] = 0x00;
                        break;
                    case (EMERALD_ANIM_CLEAN_RIGHT):
                        Playfield.pLevel[I + 1] = EMERALD_SPACE;
                        Playfield.pStatusAnimation[I + 1] = 0x00;
                        break;
                    case (EMERALD_ANIM_CLEAN_DOWN):
                        Playfield.pLevel[I + Playfield.uLevel_X_Dimension] = EMERALD_SPACE;
                        Playfield.pStatusAnimation[I + Playfield.uLevel_X_Dimension] = 0x00;
                        break;
                    case (EMERALD_ANIM_CLEAN_LEFT):
                        Playfield.pLevel[I - 1] = EMERALD_SPACE;
                        Playfield.pStatusAnimation[I - 1] = 0x00;
                        break;
                    case (EMERALD_ANIM_CLEAN_UP_DOUBLESPEED):
                        Playfield.pLevel[I - 2 * Playfield.uLevel_X_Dimension] = EMERALD_SPACE;
                        Playfield.pStatusAnimation[I - 2 * Playfield.uLevel_X_Dimension] = 0x00;
                        break;
                    case (EMERALD_ANIM_CLEAN_RIGHT_DOUBLESPEED):
                        Playfield.pLevel[I + 2] = EMERALD_SPACE;
                        Playfield.pStatusAnimation[I + 2] = 0x00;
                        break;
                    case (EMERALD_ANIM_CLEAN_DOWN_DOUBLESPEED):
                        Playfield.pLevel[I + 2 * Playfield.uLevel_X_Dimension] = EMERALD_SPACE;
                        Playfield.pStatusAnimation[I + 2 * Playfield.uLevel_X_Dimension] = 0x00;
                        break;
                    case (EMERALD_ANIM_CLEAN_LEFT_DOUBLESPEED):
                        Playfield.pLevel[I - 2] = EMERALD_SPACE;
                        Playfield.pStatusAnimation[I - 2] = 0x00;
                        break;
                    default:
                        SDL_Log("%s: invalid clean status  %x",__FUNCTION__,uCleanStatus);
                        break;
                }
            }
        } else if (Playfield.pLevel[I] == EMERALD_ACIDPOOL_DESTROY) {
            // EMERALD_ACIDPOOL_DESTROY muss im Vorwege "behandelt" werden, da sonst Elemente auf EMERALD_ACIDPOOL_DESTROY fallen können.
            Playfield.pLevel[I] = EMERALD_SPACE;
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
        }
        // Dieser Block sorgt bei drehenden Objekten (Minen+Käfer+Mole) dafür, dass diese
        // nach der Drehung in die richtige Richtung zeigen.
        uAnimStatus = Playfield.pStatusAnimation[I] & 0x0000FF00;
        if ((uAnimStatus == EMERALD_ANIM_SPIN_UP_TO_RIGHT) || (uAnimStatus == EMERALD_ANIM_SPIN_DOWN_TO_RIGHT)) {
            // Ab hier auf Mine, Käfer und Mole prüfen
            if ((Playfield.pLevel[I] == EMERALD_MINE_UP) || (Playfield.pLevel[I] == EMERALD_MINE_DOWN)) {
                Playfield.pLevel[I] = EMERALD_MINE_RIGHT;
            } else if ((Playfield.pLevel[I] == EMERALD_BEETLE_UP) || (Playfield.pLevel[I] == EMERALD_BEETLE_DOWN)) {
                Playfield.pLevel[I] = EMERALD_BEETLE_RIGHT;
            } else if ((Playfield.pLevel[I] == EMERALD_MOLE_UP) || (Playfield.pLevel[I] == EMERALD_MOLE_DOWN)) {
                Playfield.pLevel[I] = EMERALD_MOLE_RIGHT;
            }
        } else if ((uAnimStatus == EMERALD_ANIM_SPIN_RIGHT_TO_DOWN) || (uAnimStatus == EMERALD_ANIM_SPIN_LEFT_TO_DOWN)) {
            // Ab hier auf Mine, Käfer und Mole prüfen
            if ((Playfield.pLevel[I] == EMERALD_MINE_RIGHT) || (Playfield.pLevel[I] == EMERALD_MINE_LEFT)) {
                Playfield.pLevel[I] = EMERALD_MINE_DOWN;
            } else if ((Playfield.pLevel[I] == EMERALD_BEETLE_RIGHT) || (Playfield.pLevel[I] == EMERALD_BEETLE_LEFT)) {
                Playfield.pLevel[I] = EMERALD_BEETLE_DOWN;
            } else if ((Playfield.pLevel[I] == EMERALD_MOLE_RIGHT) || (Playfield.pLevel[I] == EMERALD_MOLE_LEFT)) {
                Playfield.pLevel[I] = EMERALD_MOLE_DOWN;
            }
        } else if ((uAnimStatus == EMERALD_ANIM_SPIN_DOWN_TO_LEFT) || (uAnimStatus == EMERALD_ANIM_SPIN_UP_TO_LEFT)) {
            // Ab hier auf Mine, Käfer und Mole prüfen
            if ((Playfield.pLevel[I] == EMERALD_MINE_DOWN) || (Playfield.pLevel[I] == EMERALD_MINE_UP)) {
                Playfield.pLevel[I] = EMERALD_MINE_LEFT;
            } else if ((Playfield.pLevel[I] == EMERALD_BEETLE_DOWN) || (Playfield.pLevel[I] == EMERALD_BEETLE_UP)) {
                Playfield.pLevel[I] = EMERALD_BEETLE_LEFT;
            } else if ((Playfield.pLevel[I] == EMERALD_MOLE_DOWN) || (Playfield.pLevel[I] == EMERALD_MOLE_UP)) {
                Playfield.pLevel[I] = EMERALD_MOLE_LEFT;
            }
        } else if ((uAnimStatus == EMERALD_ANIM_SPIN_LEFT_TO_UP) || (uAnimStatus == EMERALD_ANIM_SPIN_RIGHT_TO_UP)) {
            // Ab hier auf Mine, Käfer und Mole prüfen
            if ((Playfield.pLevel[I] == EMERALD_MINE_LEFT) || (Playfield.pLevel[I] == EMERALD_MINE_RIGHT)) {
                Playfield.pLevel[I] = EMERALD_MINE_UP;
            } else if ((Playfield.pLevel[I] == EMERALD_BEETLE_LEFT) || (Playfield.pLevel[I] == EMERALD_BEETLE_RIGHT)) {
                Playfield.pLevel[I] = EMERALD_BEETLE_UP;
            } else if ((Playfield.pLevel[I] == EMERALD_MOLE_LEFT) || (Playfield.pLevel[I] == EMERALD_MOLE_RIGHT)) {
                Playfield.pLevel[I] = EMERALD_MOLE_UP;
            }
        }
        uCleanStatus = Playfield.pStatusAnimation[I] & 0xFF000000;
        // Es muss hier auch mit dem Levelelement UND-Verknüpft werden, da der Man unter Umständen auf das Objekt zuläuft und es aufnimmt
        if (    ((uCleanStatus == EMERALD_ANIM_KEY_RED_SHRINK) && (Playfield.pLevel[I] == EMERALD_KEY_RED)) ||
                ((uCleanStatus == EMERALD_ANIM_KEY_GREEN_SHRINK) && (Playfield.pLevel[I] == EMERALD_KEY_GREEN)) ||
                ((uCleanStatus == EMERALD_ANIM_KEY_BLUE_SHRINK) && (Playfield.pLevel[I] == EMERALD_KEY_BLUE)) ||
                ((uCleanStatus == EMERALD_ANIM_KEY_YELLOW_SHRINK) && (Playfield.pLevel[I] == EMERALD_KEY_YELLOW))  ||
                ((uCleanStatus == EMERALD_ANIM_KEY_WHITE_SHRINK) && (Playfield.pLevel[I] == EMERALD_KEY_WHITE))  ||
                ((uCleanStatus == EMERALD_ANIM_KEY_GENERAL_SHRINK) && (Playfield.pLevel[I] == EMERALD_KEY_GENERAL))  ||
                ((uCleanStatus == EMERALD_ANIM_SAND_SHRINK) && (Playfield.pLevel[I] == EMERALD_SAND)) ||
                ((uCleanStatus == EMERALD_ANIM_EMERALD_SHRINK) && (Playfield.pLevel[I] == EMERALD_EMERALD)) ||
                ((uCleanStatus == EMERALD_ANIM_RUBY_SHRINK) && (Playfield.pLevel[I] == EMERALD_RUBY)) ||
                ((uCleanStatus == EMERALD_ANIM_PERL_SHRINK) && (Playfield.pLevel[I] == EMERALD_PERL)) ||
                ((uCleanStatus == EMERALD_ANIM_CRYSTAL_SHRINK) && (Playfield.pLevel[I] == EMERALD_CRYSTAL)) ||
                ((uCleanStatus == EMERALD_ANIM_TIME_COIN_SHRINK) && (Playfield.pLevel[I] == EMERALD_TIME_COIN)) ||
                ((uCleanStatus == EMERALD_ANIM_HAMMER_SHRINK) && (Playfield.pLevel[I] == EMERALD_HAMMER)) ||
                ((uCleanStatus == EMERALD_ANIM_DYNAMITE_SHRINK) && (Playfield.pLevel[I] == EMERALD_DYNAMITE_OFF)) ||
                ((uCleanStatus == EMERALD_ANIM_MESSAGE_SHRINK) && (Playfield.pLevel[I] >= EMERALD_MESSAGE_1) && (Playfield.pLevel[I] <= EMERALD_MESSAGE_8)) ||
                ((uCleanStatus == EMERALD_ANIM_SAPPHIRE_SHRINK) && (Playfield.pLevel[I] == EMERALD_SAPPHIRE)) ) {
            Playfield.pLevel[I] = EMERALD_SPACE;
            Playfield.pStatusAnimation[I] = EMERALD_NO_ADDITIONAL_ANIMSTATUS;
        }
        // "Geboren" wird über 2 Steuerungsrunden, da sonst zu schnell geboren wird
        if (uCleanStatus == EMERALD_ANIM_BORN1) {
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_BORN2;
        } else if (uCleanStatus == EMERALD_ANIM_BORN2) {
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
        }
        // "Selbststeuernde" Animationen und Animationsstatus nicht zurücksetzen,
        // Clean-Status und invalid Element zurücksetzen
        Playfield.pStatusAnimation[I] = Playfield.pStatusAnimation[I] & 0xFF00FF00;
    }
    if (Playfield.uTimeDoorTimeLeft == 0) {
        // Zeit-Tür schließen, da Zeit abgelaufen ist
        Playfield.bTimeDoorOpen = false;
    }
    // Ab hier beginnt eine neue Steuerung für alle Elemente
    ControlPreElements();   // ggf. Spaces für Elemente einsetzen, die sich auflösen, Molen-Sand in normalen Sand wandeln
    // Man als Zweites steuern !
    uManDirection = ControlMan(Playfield.uManYpos * Playfield.uLevel_X_Dimension + Playfield.uManXpos,uDirection);
    ControlLightBarriers();
    for (I = 0; I < Playfield.uLevel_X_Dimension * Playfield.uLevel_Y_Dimension; I++) {
        uLevelElement = Playfield.pLevel[I];
        switch (uLevelElement)
        {
            case (EMERALD_SWITCHDOOR_OPEN):
                // wird nach der Steuerungsrunde durchgeführt in ControlSwitchDoors();
                break;
            case (EMERALD_SWITCHDOOR_CLOSED):
                // wird nach der Steuerungsrunde durchgeführt in ControlSwitchDoors();
                break;
            case (EMERALD_DOOR_TIME):
                ControlTimeDoor(I);
                break;
            case (EMERALD_WHEEL_TIMEDOOR):
                if (Playfield.bTimeDoorOpen) {
                    PreparePlaySound(SOUND_WHEEL_TIMEDOOR,I);
                }
                break;
            case (EMERALD_GREEN_CHEESE_GOES):
                // wird oben im Vorwege bereits behandelt und kommt hier nicht mehr vor, da bereits in Space gewandelt
                break;
            case (EMERALD_SAND_MOLE):
                // ControlMoleSand(I);
                break;
            case (EMERALD_STANDMINE):
                ControlStandMine(I);
                break;
            case (EMERALD_GREEN_DROP_COMES):
                ControlSpreadCheese(I);
                break;
            case (EMERALD_GREEN_CHEESE):
                ControlGreenCheese(I);
                break;
            case (EMERALD_GREEN_DROP):
                ControlGreenDrop(I);
                break;
            case (EMERALD_YAM):
                ControlYam(I);
                break;
            case (EMERALD_DOOR_END_NOT_READY):
                if (Playfield.uEmeraldsToCollect == 0) {
                    Playfield.pLevel[I] = EMERALD_DOOR_END_READY;
                }
                break;
            case (EMERALD_DOOR_END_NOT_READY_STEEL):
                if (Playfield.uEmeraldsToCollect == 0) {
                    Playfield.pLevel[I] = EMERALD_DOOR_END_READY_STEEL;
                }
                break;
            case (EMERALD_EXPLOSION_PHASE_1):
            case (EMERALD_EXPLOSION_PHASE_2):
                //ControlExplosion(I);
                break;
            case (EMERALD_MAGIC_WALL):
            case (EMERALD_MAGIC_WALL_STEEL):
                ControlMagicWall(I);
                break;
            case (EMERALD_ACIDPOOL_DESTROY):
                // wird oben im Vorwege bereits behandelt und kommt hier nicht mehr vor, da bereits in Space gewandelt
                break;
            case (EMERALD_STONE_SAG):
                ControlStoneSag(I);
                break;
            case (EMERALD_SWAMP_STONE):
                ControlSwampStone(I);
                break;
            case (EMERALD_STONE_SINK):
                ControlStoneSink(I);
                break;
            case (EMERALD_EMERALD):
                ControlEmerald(I);
                break;
            case (EMERALD_RUBY):
                ControlRuby(I);
                break;
            case (EMERALD_PERL):
                ControlPerl(I);
                break;
            case (EMERALD_CRYSTAL):
                ControlCrystal(I);
                break;
            case (EMERALD_SAPPHIRE):
                ControlSaphir(I);
                break;
            case (EMERALD_BOMB):
                ControlBomb(I);
                break;
            case (EMERALD_MEGABOMB):
                ControlMegaBomb(I);
                break;
            case (EMERALD_NUT):
                ControlNut(I);
                break;
            case (EMERALD_STONE):
                ControlStone(I);
                break;
            case (EMERALD_SAND):
                ControlSand(I);
                break;
            case (EMERALD_REPLICATOR_RED_TOP_MID):
                ControlRedReplicator(I);
                break;
            case (EMERALD_REPLICATOR_YELLOW_TOP_MID):
                ControlYellowReplicator(I);
                break;
            case (EMERALD_REPLICATOR_GREEN_TOP_MID):
                ControlGreenReplicator(I);
                break;
            case (EMERALD_REPLICATOR_BLUE_TOP_MID):
                ControlBlueReplicator(I);
                break;
            case (EMERALD_MAN):
                // Man wird vorab gesteuert, siehe oben
                break;
            case (EMERALD_MOLE_UP):
                ControlMoleUp(I);
                PreparePlaySound(SOUND_MOLE,I);
                break;
            case (EMERALD_MOLE_RIGHT):
                ControlMoleRight(I);
                PreparePlaySound(SOUND_MOLE,I);
                break;
            case (EMERALD_MOLE_DOWN):
                ControlMoleDown(I);
                PreparePlaySound(SOUND_MOLE,I);
                break;
            case (EMERALD_MOLE_LEFT):
                ControlMoleLeft(I);
                PreparePlaySound(SOUND_MOLE,I);
                break;
            case (EMERALD_MINE_UP):
                ControlMineUp(I);
                PreparePlaySound(SOUND_MINE,I);
                break;
            case (EMERALD_MINE_RIGHT):
                ControlMineRight(I);
                PreparePlaySound(SOUND_MINE,I);
                break;
            case (EMERALD_MINE_DOWN):
                ControlMineDown(I);
                PreparePlaySound(SOUND_MINE,I);
                break;
            case (EMERALD_MINE_LEFT):
                ControlMineLeft(I);
                PreparePlaySound(SOUND_MINE,I);
                break;
            case (EMERALD_BEETLE_UP):
                ControlBeetleUp(I);
                PreparePlaySound(SOUND_BEETLE,I);
                break;
            case (EMERALD_BEETLE_RIGHT):
                ControlBeetleRight(I);
                PreparePlaySound(SOUND_BEETLE,I);
                break;
            case (EMERALD_BEETLE_DOWN):
                ControlBeetleDown(I);
                PreparePlaySound(SOUND_BEETLE,I);
                break;
            case (EMERALD_BEETLE_LEFT):
                ControlBeetleLeft(I);
                PreparePlaySound(SOUND_BEETLE,I);
                break;
            case (EMERALD_ALIEN):
                ControlAlien(I);
                break;
            case (EMERALD_WHEEL):
                if (Playfield.bWheelRunning) {
                    PreparePlaySound(SOUND_WHEEL,I);
                }
                break;
        }
    }
    PostControlSwitchDoors();
    return uManDirection;
}


/*----------------------------------------------------------------------------
Name:           ScrollAndCenterLevel
------------------------------------------------------------------------------
Beschreibung: Scrollt und zentriert das Level, wenn sich der Man bewegt.
Parameter
      Eingang: uManDirection, uint32_t, aktuelle Richtung des Man
      Ausgang: -

Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ScrollAndCenterLevel(uint32_t uManDirection) {
    switch (uManDirection) {
        case (EMERALD_ANIM_UP):
            if (Playfield.uManYpos < (Playfield.uLevel_Y_Dimension - Playfield.uVisibleCenterY)) {
                Playfield.nTopLeftYpos -= 2;
            }
            break;
        case (EMERALD_ANIM_RIGHT):
            if (Playfield.uManXpos >= Playfield.uVisibleCenterX) {
                Playfield.nTopLeftXpos += 2;
            }
            break;
        case (EMERALD_ANIM_DOWN):
            if (Playfield.uManYpos >= Playfield.uVisibleCenterY) {
                Playfield.nTopLeftYpos += 2;
            }
            break;
        case (EMERALD_ANIM_LEFT):
            if (Playfield.uManXpos < (Playfield.uLevel_X_Dimension - Playfield.uVisibleCenterX)) {
                Playfield.nTopLeftXpos -= 2;
            }
            break;
        case (EMERALD_ANIM_UP_DOUBLESPEED):
            if (Playfield.uManYpos < (Playfield.uLevel_Y_Dimension - Playfield.uVisibleCenterY)) {
                Playfield.nTopLeftYpos -= 4;
            }
            break;
        case (EMERALD_ANIM_RIGHT_DOUBLESPEED):
            if (Playfield.uManXpos >= Playfield.uVisibleCenterX) {
                Playfield.nTopLeftXpos += 4;
            }
            break;
        case (EMERALD_ANIM_DOWN_DOUBLESPEED):
            if (Playfield.uManYpos >= Playfield.uVisibleCenterY) {
                Playfield.nTopLeftYpos += 4;
            }
            break;
        case (EMERALD_ANIM_LEFT_DOUBLESPEED):
            if (Playfield.uManXpos < (Playfield.uLevel_X_Dimension - Playfield.uVisibleCenterX)) {
                Playfield.nTopLeftXpos -= 4;
            }
            break;
    }
}


/*----------------------------------------------------------------------------
Name:           RenderLevel
------------------------------------------------------------------------------
Beschreibung: Kopiert den sichtbaren Teil des Levels in den Renderer
Parameter
      Eingang: pRenderer, SDL_Renderer *, Zeiger auf Renderer
               pnXpos, int *, Pixel-Positionierung X (obere linke Ecke des Levelausschnitts)
               pnYpos, int *, Pixel-Positionierung Y (obere linke Ecke des Levelausschnitts)
               nAnimationCount, int, 0 - 15 für Animationsstufe
      Ausgang: pnXpos, int *, ggf. korrigierte Pixel-Positionierung X (obere linke Ecke des Levelausschnitts)
               pnYpos, int *, ggf. korrigierte Pixel-Positionierung Y (obere linke Ecke des Levelausschnitts)
Rückgabewert:  int , 0 = OK, sonst Fehler
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
int RenderLevel(SDL_Renderer *pRenderer, int *pnXpos, int *pnYpos, int nAnimationCount)
{
    int nErrorCode;
    uint32_t uX;
    uint32_t uY;
    uint32_t I;                         // Levelindex
    uint32_t Y;                         // Für YAM-Animation und Post-Animation
    uint32_t uReplicatorAnimation;      // Animationsschritt für Replikator
    uint32_t uUpperLeftLevelIndex;      // oberstes linkes Element, welches entweder komplett oder dessen untere rechte Ecke noch gerade sichtbar ist
    uint32_t uPostAnimationIndex;       // Zeiger auf Playfield.pPostAnimation für Post-Animationen
    uint32_t uAnimStatus;
    uint32_t uSelfStatus;
    uint32_t uNewMagicElement;
    SDL_Rect DestR;                     // Zum Kopieren in den Renderer
    /////// Standard
    uint32_t uTextureIndex;
    uint16_t uLevelElement;
    int nXoffs;                         // Animations-Offset X
    int nYoffs;                         // Animations-Offset Y
    SDL_RendererFlip Flip;
    float fAngle;
    float fAngleOffs;
    float fScaleW;
    float fScaleH;
    /////// Erweitert: dieses Element wird >vor<, also unter das Standard-Element auf derselben Position gezeichnet
    uint32_t uTextureIndex_0;
    bool bExtendedElement;
    int nXoffs_0;                        // Animations-Offset X
    int nYoffs_0;                        // Animations-Offset Y
    SDL_RendererFlip Flip_0;
    float fAngle_0;
    float fAngleOffs_0;
    float fScaleW_0;
    float fScaleH_0;
    ///////
    uint8_t uVerticalBeamColor;
    uint8_t uHorizontalBeamColor;

    uPostAnimationIndex = 0;
    // Die Eingangsparameter "grob" prüfen, damit nichts Schlimmes passiert
    if ((pRenderer == NULL) || (Playfield.pLevel == NULL)) {
        SDL_Log("%s: bad input parameters",__FUNCTION__);
        return -1;
    }
    // Positionsüberläufe abfangen
    if (*pnXpos < 0) {
        *pnXpos = 0;
    } else if (*pnXpos > Playfield.nMaxXpos) {
        *pnXpos = Playfield.nMaxXpos;
    }
    if (*pnYpos < 0) {
        *pnYpos = 0;
    } else if (*pnYpos > Playfield.nMaxYpos) {
        *pnYpos = Playfield.nMaxYpos;
    }
    uUpperLeftLevelIndex = (*pnXpos / FONT_W) + (*pnYpos / FONT_H) * Playfield.uLevel_X_Dimension;
    CheckRunningWheel();
    CheckRunningMagicWall();
    CheckLight();
    CheckTimeDoorOpen();
    CheckPlayTime();
    nErrorCode = 0;
    // Den sichtbaren Teil des Levels in den Renderer kopieren.
    for (uY = 0; (uY <= ((WINDOW_H - PANEL_H) / FONT_H)) && (uY < Playfield.uLevel_Y_Dimension) && (nErrorCode == 0); uY++) {
        for (uX = 0; (uX <= (WINDOW_W / FONT_W)) && (uX < Playfield.uLevel_X_Dimension) && (nErrorCode == 0); uX++) {
            // Levelindex berechnen
            I = uUpperLeftLevelIndex + uY * Playfield.uLevel_X_Dimension + uX;
            if (I > ((Playfield.uLevel_X_Dimension * Playfield.uLevel_Y_Dimension) - 1)) {
                // SDL_Log("%s: Warning: Level-Overflow, V:%u   X:%u   Y:%u  T:%u",__FUNCTION__,I,uX,uY,SDL_GetTicks());
                break;
            }
            uLevelElement = Playfield.pLevel[I];
            // Standard
            fAngle = 0;
            nXoffs = 0;
            nYoffs = 0;
            fAngleOffs = 0;
            fScaleW = 1;
            fScaleH = 1;
            Flip = SDL_FLIP_NONE;
            // Erweitert
            uTextureIndex_0 = 0; // Space
            bExtendedElement = false;
            fAngle_0 = 0;
            fAngleOffs_0 = 0;
            nXoffs_0 = 0;
            nYoffs_0 = 0;
            fAngleOffs_0 = 0;
            fScaleW_0 = 1;
            fScaleH_0 = 1;
            Flip_0 = SDL_FLIP_NONE;

            uAnimStatus = Playfield.pStatusAnimation[I] & 0x0000FF00;
            uSelfStatus = Playfield.pStatusAnimation[I] & 0xFF000000;
            uReplicatorAnimation = Playfield.uFrameCounter % 12;
            switch (uLevelElement) {
                case (EMERALD_DYNAMITE_ON): // TODO
                    uTextureIndex = 555 + nAnimationCount / 4;
                    break;
                case (EMERALD_WALL_WITH_CRYSTAL):
                    uTextureIndex = 533;
                    break;
                case (EMERALD_WALL_WITH_KEY_RED):
                    uTextureIndex = 534;
                    break;
                case (EMERALD_WALL_WITH_KEY_GREEN):
                    uTextureIndex = 535;
                    break;
                case (EMERALD_WALL_WITH_KEY_BLUE):
                    uTextureIndex = 536;
                    break;
                case (EMERALD_WALL_WITH_KEY_YELLOW):
                    uTextureIndex = 537;
                    break;
                case (EMERALD_WALL_WITH_KEY_WHITE):
                    uTextureIndex = 538;
                    break;
                case (EMERALD_WALL_WITH_KEY_GENERAL):
                    uTextureIndex = 539;
                    break;
                case (EMERALD_WALL_WITH_BOMB):
                    uTextureIndex = 540;
                    break;
                case (EMERALD_WALL_WITH_MEGABOMB):
                    uTextureIndex = 541;
                    break;
                case (EMERALD_WALL_WITH_STONE):
                    uTextureIndex = 542;
                    break;
                case (EMERALD_WALL_WITH_NUT):
                    uTextureIndex = 543;
                    break;
                case (EMERALD_WALL_WITH_WHEEL):
                    uTextureIndex = 544;
                    break;
                case (EMERALD_WALL_WITH_DYNAMITE):
                    uTextureIndex = 545;
                    break;
                case (EMERALD_WALL_WITH_ENDDOOR):
                    uTextureIndex = 546;
                    break;
                case (EMERALD_WALL_WITH_ENDDOOR_READY):
                    uTextureIndex = 547;
                    break;
                case (EMERALD_WALL_WITH_MINE_UP):
                    uTextureIndex = 548;
                    break;
                case (EMERALD_WALL_WITH_BEETLE_UP):
                    uTextureIndex = 549;
                    break;
                case (EMERALD_WALL_WITH_YAM):
                    uTextureIndex = 550;
                    break;
                case (EMERALD_WALL_WITH_ALIEN):
                    uTextureIndex = 551;
                    break;
                case (EMERALD_WALL_WITH_MOLE):
                    uTextureIndex = 552;
                    break;
                case (EMERALD_WALL_WITH_GREEN_CHEESE):
                    uTextureIndex = 553;
                    break;
                case (EMERALD_WALL_WITH_EMERALD):
                    uTextureIndex = 529;
                    break;
                case (EMERALD_WALL_WITH_RUBY):
                    uTextureIndex = 530;
                    break;
                case (EMERALD_WALL_WITH_SAPPHIRE):
                    uTextureIndex = 531;
                    break;
                case (EMERALD_WALL_WITH_PERL):
                    uTextureIndex = 532;
                    break;
                case (EMERALD_SWITCHDOOR_OPEN):
                    if (uSelfStatus == EMERALD_ANIM_DOOR_CLOSE) {   // Soll Tür sich schließen?
                        if (nAnimationCount < 15) {
                            uTextureIndex = 506 - nAnimationCount / 3;
                        } else {
                            uTextureIndex = 502;    // Tür geschlossen
                        }
                    } else {
                        uTextureIndex = 506;        // Schalttür offen
                    }
                    break;
                case (EMERALD_SWITCHDOOR_CLOSED):
                    if (uSelfStatus == EMERALD_ANIM_DOOR_OPEN) {   // Soll Tür sich öffnen?
                        if (nAnimationCount < 15) {
                            uTextureIndex = 502 + nAnimationCount / 3;
                        } else {
                            uTextureIndex = 506;    // Tür offen
                        }
                    } else {
                        uTextureIndex = 502;        // Schalttür geschlossen
                    }
                    break;
               case (EMERALD_SWITCH_SWITCHDOOR):
                    if (Playfield.bSwitchDoorState) {
                        uTextureIndex = 507;
                    } else {
                        uTextureIndex = 508;
                    }
                    break;
                case (EMERALD_DOOR_TIME):
                    if (uSelfStatus == EMERALD_ANIM_DOOR_OPEN) {   // Tür öffnet sich
                        if (nAnimationCount < 15) {
                            uTextureIndex = 497 + nAnimationCount / 3;
                        } else {
                                uTextureIndex = 501;
                        }
                    } else if (uSelfStatus == EMERALD_ANIM_DOOR_CLOSE) {    // Tür schließt sich
                        if (nAnimationCount < 15) {
                            uTextureIndex = 501 - nAnimationCount / 3;
                        } else {
                            uTextureIndex = 497;    // Tür geschlossen
                        }
                    } else if (Playfield.bTimeDoorOpen) {
                        uTextureIndex = 501;    // Tür offen
                    } else {
                        uTextureIndex = 497;    // Tür geschlossen
                    }
                    break;
                case (EMERALD_WHEEL_TIMEDOOR):
                    if (Playfield.bTimeDoorOpen) {
                        uTextureIndex = 493 + nAnimationCount / 4;
                    } else {
                        uTextureIndex = 493;
                    }
                    break;
                case (EMERALD_DOOR_EMERALD):
                    uTextureIndex = 487;
                    break;
                case (EMERALD_DOOR_MULTICOLOR):
                    uTextureIndex = 488;
                    break;
                case (EMERALD_DOOR_ONLY_UP):
                    uTextureIndex = 489;
                    break;
                case (EMERALD_DOOR_ONLY_DOWN):
                    uTextureIndex = 490;
                    break;
                case (EMERALD_DOOR_ONLY_LEFT):
                    uTextureIndex = 491;
                    break;
                case (EMERALD_DOOR_ONLY_RIGHT):
                    uTextureIndex = 492;
                    break;
                case (EMERALD_STEEL_INVISIBLE):
                    if (Playfield.bLightOn) {
                        uTextureIndex = 484;
                    } else {
                        uTextureIndex = 0;
                    }
                    break;
                case (EMERALD_WALL_INVISIBLE):
                    if (Playfield.bLightOn) {
                        uTextureIndex = 485;
                    } else {
                        uTextureIndex = 0;
                    }
                    break;
                case (EMERALD_LIGHT_SWITCH):
                    uTextureIndex = 486;
                    break;
                case (EMERALD_GREEN_CHEESE_GOES):
                    uTextureIndex = 343 + I % 3;
                    nXoffs = nAnimationCount;
                    nYoffs = nAnimationCount;
                    fScaleW = 1 - nAnimationCount * 0.06;
                    fScaleH = fScaleW;
                    break;
                case (EMERALD_SAND_MOLE):
                    uTextureIndex = 171;        // Sand
                    nXoffs = 15 - nAnimationCount;
                    nYoffs = 15 - nAnimationCount;
                    fScaleW = nAnimationCount * 0.06;
                    fScaleH = fScaleW;
                    break;
                case (EMERALD_STANDMINE):
                    uTextureIndex = 447;
                    if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN1) {
                        nXoffs = 15 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN2) {
                        nXoffs = 7 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = 0.5 + nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if ((((Playfield.uFrameCounter & 0xFFFFFFFC) >> 2) % 32) == 0) {
                        uTextureIndex = 448;
                    } else if ((((Playfield.uFrameCounter & 0xFFFFFFFC) >> 2) % 16) == 0) {
                        uTextureIndex = 449;
                    } else {
                        uTextureIndex = 447;
                    }
                    break;
                case (EMERALD_MAGIC_WALL_SWITCH):
                    uTextureIndex = 435;
                    break;
                case (EMERALD_LIGHTBARRIER_RED_UP):
                    if (Playfield.bLightBarrierRedOn) {
                        uTextureIndex = 423;
                    } else {
                        uTextureIndex = 419;
                    }
                    break;
                case (EMERALD_LIGHTBARRIER_RED_DOWN):
                    if (Playfield.bLightBarrierRedOn) {
                        uTextureIndex = 424;
                    } else {
                        uTextureIndex = 420;
                    }
                    break;
                case (EMERALD_LIGHTBARRIER_RED_LEFT):
                    if (Playfield.bLightBarrierRedOn) {
                        uTextureIndex = 425;
                    } else {
                        uTextureIndex = 421;
                    }
                    break;
                case (EMERALD_LIGHTBARRIER_RED_RIGHT):
                    if (Playfield.bLightBarrierRedOn) {
                        uTextureIndex = 426;
                    } else {
                        uTextureIndex = 422;
                    }
                    break;
                case (EMERALD_LIGHTBARRIER_GREEN_UP):
                    if (Playfield.bLightBarrierGreenOn) {
                        uTextureIndex = 399;
                    } else {
                        uTextureIndex = 395;
                    }
                    break;
                case (EMERALD_LIGHTBARRIER_GREEN_DOWN):
                    if (Playfield.bLightBarrierGreenOn) {
                        uTextureIndex = 400;
                    } else {
                        uTextureIndex = 396;
                    }
                    break;
                case (EMERALD_LIGHTBARRIER_GREEN_LEFT):
                    if (Playfield.bLightBarrierGreenOn) {
                        uTextureIndex = 401;
                    } else {
                        uTextureIndex = 397;
                    }
                    break;
                case (EMERALD_LIGHTBARRIER_GREEN_RIGHT):
                    if (Playfield.bLightBarrierGreenOn) {
                        uTextureIndex = 402;
                    } else {
                        uTextureIndex = 398;
                    }
                    break;
                case (EMERALD_LIGHTBARRIER_BLUE_UP):
                    if (Playfield.bLightBarrierBlueOn) {
                        uTextureIndex = 407;
                    } else {
                        uTextureIndex = 403;
                    }
                    break;
                case (EMERALD_LIGHTBARRIER_BLUE_DOWN):
                    if (Playfield.bLightBarrierBlueOn) {
                        uTextureIndex = 408;
                    } else {
                        uTextureIndex = 404;
                    }
                    break;
                case (EMERALD_LIGHTBARRIER_BLUE_LEFT):
                    if (Playfield.bLightBarrierBlueOn) {
                        uTextureIndex = 409;
                    } else {
                        uTextureIndex = 405;
                    }
                    break;
                case (EMERALD_LIGHTBARRIER_BLUE_RIGHT):
                    if (Playfield.bLightBarrierBlueOn) {
                        uTextureIndex = 410;
                    } else {
                        uTextureIndex = 406;
                    }
                    break;
                case (EMERALD_LIGHTBARRIER_YELLOW_UP):
                    if (Playfield.bLightBarrierYellowOn) {
                        uTextureIndex = 415;
                    } else {
                        uTextureIndex = 411;
                    }
                    break;
                case (EMERALD_LIGHTBARRIER_YELLOW_DOWN):
                    if (Playfield.bLightBarrierYellowOn) {
                        uTextureIndex = 416;
                    } else {
                        uTextureIndex = 412;
                    }
                    break;
                case (EMERALD_LIGHTBARRIER_YELLOW_LEFT):
                    if (Playfield.bLightBarrierYellowOn) {
                        uTextureIndex = 417;
                    } else {
                        uTextureIndex = 413;
                    }
                    break;
                case (EMERALD_LIGHTBARRIER_YELLOW_RIGHT):
                    if (Playfield.bLightBarrierYellowOn) {
                        uTextureIndex = 418;
                    } else {
                        uTextureIndex = 414;
                    }
                    break;
                case (EMERALD_BEAM_RED_VERTICAL):
                    uTextureIndex = 433;
                    break;
                case (EMERALD_BEAM_RED_HORIZONTAL):
                    uTextureIndex = 434;
                    break;
                case (EMERALD_BEAM_GREEN_VERTICAL):
                    uTextureIndex = 427;
                    break;
                case (EMERALD_BEAM_GREEN_HORIZONTAL):
                    uTextureIndex = 428;
                    break;
                case (EMERALD_BEAM_BLUE_VERTICAL):
                    uTextureIndex = 429;
                    break;
                case (EMERALD_BEAM_BLUE_HORIZONTAL):
                    uTextureIndex = 430;
                    break;
                case (EMERALD_BEAM_YELLOW_VERTICAL):
                    uTextureIndex = 431;
                    break;
                case (EMERALD_BEAM_YELLOW_HORIZONTAL):
                    uTextureIndex = 432;
                    break;
                case (EMERALD_BEAM_CROSS):
                    uTextureIndex = 0;
                    bExtendedElement = false;
                    uVerticalBeamColor = (Playfield.pStatusAnimation[I] >> 4) & 0x3;
                    uHorizontalBeamColor = Playfield.pStatusAnimation[I] & 0x03;
                    // Horizontale Linien zeichnen
                    SDL_SetRenderDrawColor(pRenderer,
                                           ge_uBeamColors[uHorizontalBeamColor * 3 + 0],        // rot
                                           ge_uBeamColors[uHorizontalBeamColor * 3 + 1],        // grün
                                           ge_uBeamColors[uHorizontalBeamColor * 3 + 2],        // blau
                                           SDL_ALPHA_OPAQUE); // Farbe für Line setzen
                    SDL_RenderDrawLine(pRenderer,
                                       uX * FONT_W - (*pnXpos % FONT_W) + Playfield.uShiftLevelXpix,
                                       uY * FONT_H - (*pnYpos % FONT_H) + Playfield.uShiftLevelYpix + 2,
                                       uX * FONT_W - (*pnXpos % FONT_W) + Playfield.uShiftLevelXpix + FONT_W,
                                       uY * FONT_H - (*pnYpos % FONT_H) + Playfield.uShiftLevelYpix + 2
                                        );
                    SDL_RenderDrawLine(pRenderer,
                                       uX * FONT_W - (*pnXpos % FONT_W) + Playfield.uShiftLevelXpix,
                                       uY * FONT_H - (*pnYpos % FONT_H) + Playfield.uShiftLevelYpix + FONT_H - 3,
                                       uX * FONT_W - (*pnXpos % FONT_W) + Playfield.uShiftLevelXpix + FONT_W,
                                       uY * FONT_H - (*pnYpos % FONT_H) + Playfield.uShiftLevelYpix + FONT_H - 3
                                        );
                    // Vertikale Linien zeichnen
                    SDL_SetRenderDrawColor(pRenderer,
                                           ge_uBeamColors[uVerticalBeamColor * 3 + 0],        // rot
                                           ge_uBeamColors[uVerticalBeamColor * 3 + 1],        // grün
                                           ge_uBeamColors[uVerticalBeamColor * 3 + 2],        // blau
                                           SDL_ALPHA_OPAQUE); // Farbe für Line setzen
                    SDL_RenderDrawLine(pRenderer,
                                       uX * FONT_W - (*pnXpos % FONT_W) + Playfield.uShiftLevelXpix + 2,
                                       uY * FONT_H - (*pnYpos % FONT_H) + Playfield.uShiftLevelYpix,
                                       uX * FONT_W - (*pnXpos % FONT_W) + Playfield.uShiftLevelXpix + 2,
                                       uY * FONT_H - (*pnYpos % FONT_H) + Playfield.uShiftLevelYpix + FONT_H
                                        );
                    SDL_RenderDrawLine(pRenderer,
                                       uX * FONT_W - (*pnXpos % FONT_W) + Playfield.uShiftLevelXpix + FONT_W - 3,
                                       uY * FONT_H - (*pnYpos % FONT_H) + Playfield.uShiftLevelYpix,
                                       uX * FONT_W - (*pnXpos % FONT_W) + Playfield.uShiftLevelXpix + FONT_W - 3,
                                       uY * FONT_H - (*pnYpos % FONT_H) + Playfield.uShiftLevelYpix + FONT_H
                                        );
                    SDL_SetRenderDrawColor(pRenderer,0,0,0, SDL_ALPHA_OPAQUE); // Zum Schluss wieder schwarz setzen
                    break;
                case (EMERALD_LIGHTBARRIER_RED_SWITCH):
                    if (Playfield.bLightBarrierRedOn) {
                        if (nAnimationCount <= 7) {
                            uTextureIndex = 384;
                        } else {
                            uTextureIndex = 385;
                        }
                    } else {
                        uTextureIndex = 383;
                    }
                    break;
                case (EMERALD_LIGHTBARRIER_GREEN_SWITCH):
                    if (Playfield.bLightBarrierGreenOn) {
                        if (nAnimationCount <= 7) {
                            uTextureIndex = 390;
                        } else {
                            uTextureIndex = 391;
                        }
                    } else {
                        uTextureIndex = 389;
                    }
                    break;
                case (EMERALD_LIGHTBARRIER_BLUE_SWITCH):
                    if (Playfield.bLightBarrierBlueOn) {
                        if (nAnimationCount <= 7) {
                            uTextureIndex = 393;
                        } else {
                            uTextureIndex = 394;
                        }
                    } else {
                        uTextureIndex = 392;
                    }
                    break;
                case (EMERALD_LIGHTBARRIER_YELLOW_SWITCH):
                    if (Playfield.bLightBarrierYellowOn) {
                        if (nAnimationCount <= 7) {
                            uTextureIndex = 387;
                        } else {
                            uTextureIndex = 388;
                        }
                    } else {
                        uTextureIndex = 386;
                    }
                    break;
                case (EMERALD_YAM):
                    uTextureIndex = 357;
                    if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN1) {
                        nXoffs = 15 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN2) {
                        nXoffs = 7 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = 0.5 + nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (uAnimStatus == EMERALD_ANIM_STAND) {
                        if (nAnimationCount <= 9) {
                            uTextureIndex = 357 + (nAnimationCount / 2);
                        } else if (nAnimationCount <= 11) { // 10,11
                            uTextureIndex = 360;
                        } else if (nAnimationCount <= 13) { // 12,13
                            uTextureIndex = 359;
                        } else { // 14,15
                            uTextureIndex = 358;
                        }
                    } else {
                        Y = Playfield.uFrameCounter % 11;       // Y von 0 bis 10
                        if (Y <= 5) {                           // 0,1,2,3,4,5
                            uTextureIndex = 362 + Y;            // 362 - 367
                        } else {                                // 6,7,8,9,10
                            uTextureIndex = 367 + 5 - Y;        // 366 - 362
                        }
                        if (uAnimStatus == EMERALD_ANIM_LEFT) {
                            nXoffs = -nAnimationCount * 2;
                        } else if (uAnimStatus == EMERALD_ANIM_RIGHT) {
                            nXoffs = nAnimationCount * 2;
                        } else if (uAnimStatus == EMERALD_ANIM_DOWN) {
                            nYoffs = nAnimationCount * 2;
                        } else if (uAnimStatus == EMERALD_ANIM_UP) {
                            nYoffs = -nAnimationCount * 2;
                        }
                    }
                    break;
                case (EMERALD_MESSAGE_1):
                case (EMERALD_MESSAGE_2):
                case (EMERALD_MESSAGE_3):
                case (EMERALD_MESSAGE_4):
                case (EMERALD_MESSAGE_5):
                case (EMERALD_MESSAGE_6):
                case (EMERALD_MESSAGE_7):
                case (EMERALD_MESSAGE_8):
                    uTextureIndex = 356;
                    if (uSelfStatus == EMERALD_ANIM_MESSAGE_SHRINK) { // Man mit Richtung und Fire
                        nXoffs = nAnimationCount;
                        nYoffs = nAnimationCount;
                        fScaleW = 1 - nAnimationCount * 0.06;
                        fScaleH = fScaleW;
                        fAngleOffs = nAnimationCount * 10;
                    }
                    break;
                case (EMERALD_STEEL_MARKER_LEFT_UP):
                    uTextureIndex = 348;
                    break;
                case (EMERALD_STEEL_MARKER_UP):
                    uTextureIndex = 349;
                    break;
                case (EMERALD_STEEL_MARKER_RIGHT_UP):
                    uTextureIndex = 350;
                    break;
                case (EMERALD_STEEL_MARKER_LEFT):
                    uTextureIndex = 351;
                    break;
                case (EMERALD_STEEL_MARKER_RIGHT):
                    uTextureIndex = 352;
                    break;
                case (EMERALD_STEEL_MARKER_LEFT_BOTTOM):
                    uTextureIndex = 353;
                    break;
                case (EMERALD_STEEL_MARKER_BOTTOM):
                    uTextureIndex = 354;
                    break;
                case (EMERALD_STEEL_MARKER_RIGHT_BOTTOM):
                    uTextureIndex = 355;
                    break;
                case (EMERALD_STEEL_HEART):
                    uTextureIndex = 474;
                    break;
                case (EMERALD_STEEL_PLAYERHEAD):
                    uTextureIndex = 475;
                    break;
                case (EMERALD_STEEL_NO_ENTRY):
                    uTextureIndex = 476;
                    break;
                case (EMERALD_STEEL_GIVE_WAY):
                    uTextureIndex = 477;
                    break;
                case (EMERALD_STEEL_YING):
                    uTextureIndex = 478;
                    break;
                case (EMERALD_STEEL_WHEELCHAIR):
                    uTextureIndex = 479;
                    break;
                case (EMERALD_STEEL_ARROW_DOWN):
                    uTextureIndex = 480;
                    break;
                case (EMERALD_STEEL_ARROW_UP):
                    uTextureIndex = 481;
                    break;
                case (EMERALD_STEEL_ARROW_LEFT):
                    uTextureIndex = 482;
                    break;
                case (EMERALD_STEEL_ARROW_RIGHT):
                    uTextureIndex = 483;
                    break;
                case (EMERALD_GREEN_DROP_COMES):
                    uTextureIndex = 346;
                    nXoffs = 16 - nAnimationCount;
                    nYoffs = nXoffs;
                    fScaleW = nAnimationCount * 0.06;
                    fScaleH = fScaleW;
                    break;
                case (EMERALD_GREEN_DROP):
                    uTextureIndex = 346;
                    if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN1) {
                        nXoffs = 15 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN2) {
                        nXoffs = 7 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = 0.5 + nAnimationCount * 0.031;
                        fScaleH = fScaleW;

                    } else {
                        if (nAnimationCount <= 7) { // 0 ... 7
                            fScaleW = 1 - nAnimationCount * 0.02;   // 1 ..... 0.86
                            nXoffs = nAnimationCount / 2;           // max. 7 / 2 = 3 Pixel zur Mitte
                        } else {                    // 8 ... 15
                            fScaleW = 0.85 + (nAnimationCount - 8) * 0.02;
                            nXoffs = 3 + 4 - nAnimationCount / 2;
                        }
                        if (uSelfStatus == EMERALD_ANIM_GREEN_DROP_1) {
                            nYoffs = nAnimationCount;
                        } else if (uSelfStatus == EMERALD_ANIM_GREEN_DROP_2) {
                            nYoffs = (FONT_H / 2) + nAnimationCount;
                        } else {
                            SDL_Log("%s: green drop, something wrong!",__FUNCTION__);
                        }
                    }
                    fScaleH = fScaleW;
                    break;
                case (EMERALD_GREEN_CHEESE):
                    uTextureIndex = 343 + I % 3;    // Es gibt 3 (0, 1 und 2) verschiedene Käsestückchen-Grafiken
                    break;
                case (EMERALD_STEEL_FORBIDDEN):
                    uTextureIndex = 334;
                    break;
                case (EMERALD_STEEL_EXIT):
                    uTextureIndex = 332;
                    break;
                case (EMERALD_STEEL_RADIOACTIVE):
                    uTextureIndex = 337;
                    break;
                case (EMERALD_STEEL_EXPLOSION):
                    uTextureIndex = 333;
                    break;
                case (EMERALD_STEEL_ACID):
                    uTextureIndex = 329;
                    break;
                case (EMERALD_STEEL_NOT_ROUND):
                    uTextureIndex = 335;
                    break;
                case (EMERALD_WALL_NOT_ROUND):
                    uTextureIndex = 342;
                    break;
                case (EMERALD_STEEL_PARKING):
                    uTextureIndex = 336;
                    break;
                case (EMERALD_STEEL_STOP):
                    uTextureIndex = 340;
                    break;
                case (EMERALD_STEEL_DEADEND):
                    uTextureIndex = 331;
                    break;
                case (EMERALD_STEEL_BIOHAZARD):
                    uTextureIndex = 330;
                    break;
                case (EMERALD_STEEL_WARNING):
                    uTextureIndex = 341;
                    break;
                case (EMERALD_STEEL_ROUND):
                    uTextureIndex = 338;
                    break;
                case (EMERALD_STEEL_ROUND_PIKE):
                    uTextureIndex = 339;
                    break;
                case (EMERALD_WALL_ROUND_PIKE):
                    uTextureIndex = 328;
                    break;
                case (EMERALD_DOOR_END_NOT_READY):
                    uTextureIndex = 287;
                    if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN1) {
                        nXoffs = 15 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN2) {
                        nXoffs = 7 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = 0.5 + nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    }
                    break;
                case (EMERALD_DOOR_END_READY):
                    uTextureIndex = 319 + ((Playfield.uFrameCounter & 0xFFFFFFF8) >> 3) % 8; // Endtür, blinkend
                    if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN1) {
                        nXoffs = 15 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN2) {
                        nXoffs = 7 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = 0.5 + nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    }
                    break;
                case (EMERALD_DOOR_END_NOT_READY_STEEL):
                    uTextureIndex = 509;
                    break;
                case (EMERALD_DOOR_END_READY_STEEL):
                    uTextureIndex = 510 + ((Playfield.uFrameCounter & 0xFFFFFFF8) >> 3) % 8; // Endtür, blinkend
                    break;
                case (EMERALD_EXPLOSION_PHASE_1):
                    if (nAnimationCount < 14) {
                        uTextureIndex = 279 + nAnimationCount / 2;
                    } else {
                        uTextureIndex = 279 + 5;
                    }
                    break;
                case (EMERALD_EXPLOSION_PHASE_2):
                    switch (nAnimationCount) {
                        case (0):
                        case (1):
                            uTextureIndex = 279 + 4;
                            break;
                        case (2):
                        case (3):
                            uTextureIndex = 279 + 3;
                            break;
                        case (4):
                        case (5):
                            uTextureIndex = 279 + 2;
                            break;
                        case (6):
                        case (7):
                            uTextureIndex = 279 + 1;
                            break;
                        case (8):
                        case (9):
                            uTextureIndex = 279 + 0;
                        break;
                        default:
                            uTextureIndex = 0;  // Space
                            break;
                    }
                    break;
                case (EMERALD_MAGIC_WALL):
                    if (uSelfStatus == EMERALD_ANIM_SAG_OUT_MAGIC_WALL) {
                        bExtendedElement = true;
                        uNewMagicElement = Playfield.pInvalidElement[I];
                        // SDL_Log("%s: element %x comes from blue wall",__FUNCTION__,uNewMagicElement);
                        nYoffs_0 = nAnimationCount * 2;
                        switch (uNewMagicElement) {
                            case(EMERALD_EMERALD):      // Stein -> Emerald
                                uTextureIndex_0 = 226 + nAnimationCount / 2;     // Emerald, fallend
                                break;
                            case(EMERALD_SAPPHIRE):     // Emerald -> Saphir
                                uTextureIndex_0 = 248 + ((Playfield.uFrameCounter & 0xFFFFFFFE) >> 1) % 9; // Saphir fallend
                                break;
                            case(EMERALD_STONE):        // Saphir -> Stein
                                uTextureIndex_0 = 71;
                                break;
                            case(EMERALD_BOMB):        // Rubin -> Bombe (noch klein)
                                uTextureIndex_0 = 271 + nAnimationCount % 8;
                                break;
                            case(EMERALD_CRYSTAL):     // Kristall -> Kristall
                                uTextureIndex_0 = 309;
                                break;
                            default:
                                SDL_Log("%s: warning: new unhandled magic element: %x at %d",__FUNCTION__,uNewMagicElement,I);
                                break;
                        }
                    }
                    // Magic Wall muss Element beim Austritt (teilweise) übermalen
                    nYoffs = 0; //- FONT_H;
                    if (Playfield.bMagicWallRunning) {
                        uTextureIndex = 293 + nAnimationCount / 2;
                    } else {
                        uTextureIndex = 293;
                    }
                    break;
                case (EMERALD_MAGIC_WALL_STEEL):
                    if (uSelfStatus == EMERALD_ANIM_SAG_OUT_MAGIC_WALL) {
                        bExtendedElement = true;
                        uNewMagicElement = Playfield.pInvalidElement[I];
                        // SDL_Log("%s: element %x comes from blue wall",__FUNCTION__,uNewMagicElement);
                        nYoffs_0 = nAnimationCount * 2;
                        switch (uNewMagicElement) {
                            case(EMERALD_EMERALD):      // Stein -> Emerald
                                uTextureIndex_0 = 226 + nAnimationCount / 2;     // Emerald, fallend
                                break;
                            case(EMERALD_SAPPHIRE):     // Emerald -> Saphir
                                uTextureIndex_0 = 248 + ((Playfield.uFrameCounter & 0xFFFFFFFE) >> 1) % 9; // Saphir fallend
                                break;
                            case(EMERALD_STONE):        // Saphir -> Stein
                                uTextureIndex_0 = 71;
                                break;
                            case(EMERALD_BOMB):        // Rubin -> Bombe (noch klein)
                                uTextureIndex_0 = 271 + nAnimationCount % 8;
                                break;
                            case(EMERALD_CRYSTAL):     // Kristall -> Kristall
                                uTextureIndex_0 = 309;
                                break;
                            default:
                                SDL_Log("%s: warning: new unhandled magic element: %x at %d",__FUNCTION__,uNewMagicElement,I);
                                break;
                        }
                    }
                    // Magic Wall muss Element beim Austritt (teilweise) übermalen
                    nYoffs = 0; //- FONT_H;
                    if (Playfield.bMagicWallRunning) {
                        uTextureIndex = 466 + nAnimationCount / 2;
                    } else {
                        uTextureIndex = 466;
                    }
                    break;
                case(EMERALD_STONE_SINK):
                    uTextureIndex = 71;
                    if (uSelfStatus == EMERALD_ANIM_STONE_SWAMP1) {
                        nYoffs = nAnimationCount;
                    } else if (uSelfStatus == EMERALD_ANIM_STONE_SWAMP2){
                        nYoffs = (FONT_H / 2) + nAnimationCount;
                    }
                    break;
                case(EMERALD_STONE_SAG):
                    bExtendedElement = true;
                    // Zu übermalendes Element
                    uTextureIndex_0 = 71;   // Stein
                    nXoffs_0 = 0;
                    fScaleW_0 = 1;
                    fScaleH_0 = 1;
                    if (uSelfStatus == EMERALD_ANIM_STONE_SWAMP1) {
                        nYoffs_0 = nAnimationCount - FONT_H;
                    } else if (uSelfStatus == EMERALD_ANIM_STONE_SWAMP2){
                        nYoffs_0 = (FONT_H / 2) + nAnimationCount - FONT_H;
                    }
                    // Überdeckendes Element
                    nYoffs = -FONT_H;
                    uTextureIndex = 266;    // Sumpf
                    break;
                case (EMERALD_SWAMP):
                case (EMERALD_SWAMP_STONE):
                    uTextureIndex = 266;
                    break;
                case (EMERALD_ACIDPOOL_DESTROY):
                    // Splash rechts
                    Playfield.pPostAnimation[uPostAnimationIndex].uTextureIndex = 368;  // AcidSplash 1/15
                    Playfield.pPostAnimation[uPostAnimationIndex].uX = uX + 1;
                    Playfield.pPostAnimation[uPostAnimationIndex].uY = uY;
                    Playfield.pPostAnimation[uPostAnimationIndex].uFormulaIndex = 0;
                    Playfield.pPostAnimation[uPostAnimationIndex].bFlipHorizontal = false;
                    Playfield.pPostAnimation[uPostAnimationIndex].bFlipVertical = false;
                    uPostAnimationIndex++;
                    // Splash links
                    Playfield.pPostAnimation[uPostAnimationIndex].uTextureIndex = 368;  // AcidSplash 1/15
                    Playfield.pPostAnimation[uPostAnimationIndex].uX = uX - 1;
                    Playfield.pPostAnimation[uPostAnimationIndex].uY = uY;
                    Playfield.pPostAnimation[uPostAnimationIndex].uFormulaIndex = 0;
                    Playfield.pPostAnimation[uPostAnimationIndex].bFlipHorizontal = true;
                    Playfield.pPostAnimation[uPostAnimationIndex].bFlipVertical = false;
                    uPostAnimationIndex++;
                    uTextureIndex = GetTextureIndexByElementForAcidPool(Playfield.pInvalidElement[I],nAnimationCount,&fAngle);
                    nYoffs = nAnimationCount * 2;
                    break;
                case (EMERALD_ACIDPOOL_TOP_LEFT):
                    uTextureIndex = 257;        // Säurebecken, oben links
                    break;
                case (EMERALD_ACIDPOOL_TOP_MID):
                    uTextureIndex = 262 + (Playfield.uFrameCounter / 13) % 4;        // Säurebecken, oben mitte (aktives Feld), verlangsamte Animation (13 Frames für eine Animations-Phase)
                    break;
                case (EMERALD_ACIDPOOL_TOP_RIGHT):
                    uTextureIndex = 259;        // Säurebecken, oben rechts
                    break;
                case (EMERALD_ACIDPOOL_BOTTOM_LEFT):
                    uTextureIndex = 258;        // Säurebecken, unten links
                    break;
                case (EMERALD_ACIDPOOL_BOTTOM_MID):
                    uTextureIndex = 261;        // Säurebecken, unten mitte
                    break;
                case (EMERALD_ACIDPOOL_BOTTOM_RIGHT):
                    uTextureIndex = 260;        // Säurebecken, unten rechts
                    break;
                case (EMERALD_REPLICATOR_RED_SWITCH):
                    if (Playfield.bReplicatorRedOn) {
                        uTextureIndex = 155;     // Replikator-Schalter, rot, an
                    } else {
                        uTextureIndex = 154;     // Replikator-Schalter, rot, aus
                    }
                    break;
                case (EMERALD_REPLICATOR_YELLOW_SWITCH):
                    if (Playfield.bReplicatorYellowOn) {
                        uTextureIndex = 207;     // Replikator-Schalter, gelb, an
                    } else {
                        uTextureIndex = 206;     // Replikator-Schalter, gelb, aus
                    }
                    break;
                case (EMERALD_REPLICATOR_GREEN_SWITCH):
                    if (Playfield.bReplicatorGreenOn) {
                        uTextureIndex = 189;     // Replikator-Schalter, grün, an
                    } else {
                        uTextureIndex = 188;     // Replikator-Schalter, grün, aus
                    }
                    break;
                case (EMERALD_REPLICATOR_BLUE_SWITCH):
                    if (Playfield.bReplicatorBlueOn) {
                        uTextureIndex = 225;     // Replikator-Schalter, blau, an
                    } else {
                        uTextureIndex = 224;     // Replikator-Schalter, blau, aus
                    }
                    break;
                case (EMERALD_REPLICATOR_RED_TOP_LEFT):
                    if (Playfield.bReplicatorRedOn) {
                        if ((uReplicatorAnimation >= 0) && (uReplicatorAnimation <= 3)) {
                            uTextureIndex = 143;     // Replikator, rot, an, oben links, Animationsschritt 1
                        } else if ((uReplicatorAnimation >= 4) && (uReplicatorAnimation <= 7)) {
                            uTextureIndex = 146;     // Replikator, rot, an, oben links, Animationsschritt 2
                        } else {
                            uTextureIndex = 149;     // Replikator, rot, an, oben links, Animationsschritt 3
                        }
                    } else {
                        uTextureIndex = 138;     // Replikator, rot, aus, oben links
                    }
                    break;
                case (EMERALD_REPLICATOR_RED_TOP_MID):
                    if (Playfield.bReplicatorRedOn) {
                        if ((uReplicatorAnimation >= 0) && (uReplicatorAnimation <= 3)) {
                            uTextureIndex = 144;     // Replikator, rot, an, mitte links, Animationsschritt 1
                        } else if ((uReplicatorAnimation >= 4) && (uReplicatorAnimation <= 7)) {
                            uTextureIndex = 147;     // Replikator, rot, an, mitte links, Animationsschritt 2
                        } else {
                            uTextureIndex = 150;     // Replikator, rot, an, mitte links, Animationsschritt 3
                        }
                    } else {
                        uTextureIndex = 139;     // Replikator, rot, aus, oben mitte
                    }
                    break;
                case (EMERALD_REPLICATOR_RED_TOP_RIGHT):
                    if (Playfield.bReplicatorRedOn) {
                        if ((uReplicatorAnimation >= 0) && (uReplicatorAnimation <= 3)) {
                            uTextureIndex = 145;     // Replikator, rot, an, oben rechts, Animationsschritt 1
                        } else if ((uReplicatorAnimation >= 4) && (uReplicatorAnimation <= 7)) {
                            uTextureIndex = 148;     // Replikator, rot, an, oben rechts, Animationsschritt 2
                        } else {
                            uTextureIndex = 151;     // Replikator, rot, an, oben rechts, Animationsschritt 3
                        }
                    } else {
                        uTextureIndex = 140;     // Replikator, rot, aus, oben rechts
                    }
                    break;
                case (EMERALD_REPLICATOR_RED_BOTTOM_LEFT):
                    if (Playfield.bReplicatorRedOn) {
                        uTextureIndex = 152;     // Replikator, rot, an, unten links, Animationsschritt 1,2 u. 3
                    } else {
                        uTextureIndex = 141;     // Replikator, rot, aus, unten links
                    }
                    break;
                case (EMERALD_REPLICATOR_RED_BOTTOM_RIGHT):
                    if (Playfield.bReplicatorRedOn) {
                        uTextureIndex = 153;     // Replikator, rot, an, unten rechts, Animationsschritt 1,2 u. 3
                    } else {
                        uTextureIndex = 142;     // Replikator, rot, aus, unten rechts
                    }
                    break;
                case (EMERALD_REPLICATOR_GREEN_TOP_LEFT):
                    if (Playfield.bReplicatorGreenOn) {
                        if ((uReplicatorAnimation >= 0) && (uReplicatorAnimation <= 3)) {
                            uTextureIndex = 177;     // Replikator, grün, an, oben links, Animationsschritt 1
                        } else if ((uReplicatorAnimation >= 4) && (uReplicatorAnimation <= 7)) {
                            uTextureIndex = 180;     // Replikator, grün, an, oben links, Animationsschritt 2
                        } else {
                            uTextureIndex = 183;     // Replikator, grün, an, oben links, Animationsschritt 3
                        }
                    } else {
                        uTextureIndex = 172;     // Replikator, grün, aus, oben links
                    }
                    break;
                case (EMERALD_REPLICATOR_GREEN_TOP_MID):
                    if (Playfield.bReplicatorGreenOn) {
                        if ((uReplicatorAnimation >= 0) && (uReplicatorAnimation <= 3)) {
                            uTextureIndex = 178;     // Replikator, grün, an, mitte links, Animationsschritt 1
                        } else if ((uReplicatorAnimation >= 4) && (uReplicatorAnimation <= 7)) {
                            uTextureIndex = 181;     // Replikator, grün, an, mitte links, Animationsschritt 2
                        } else {
                            uTextureIndex = 184;     // Replikator, grün, an, mitte links, Animationsschritt 3
                        }
                    } else {
                        uTextureIndex = 173;     // Replikator, grün, aus, oben mitte
                    }
                    break;
                case (EMERALD_REPLICATOR_GREEN_TOP_RIGHT):
                    if (Playfield.bReplicatorGreenOn) {
                        if ((uReplicatorAnimation >= 0) && (uReplicatorAnimation <= 3)) {
                            uTextureIndex = 179;     // Replikator, grün, an, oben rechts, Animationsschritt 1
                        } else if ((uReplicatorAnimation >= 4) && (uReplicatorAnimation <= 7)) {
                            uTextureIndex = 182;     // Replikator, grün, an, oben rechts, Animationsschritt 2
                        } else {
                            uTextureIndex = 185;     // Replikator, grün, an, oben rechts, Animationsschritt 3
                        }
                    } else {
                        uTextureIndex = 174;     // Replikator, grün, aus, oben rechts
                    }
                    break;
                case (EMERALD_REPLICATOR_GREEN_BOTTOM_LEFT):
                    if (Playfield.bReplicatorGreenOn) {
                        uTextureIndex = 186;     // Replikator, grün, an, unten links, Animationsschritt 1,2 u. 3
                    } else {
                        uTextureIndex = 175;     // Replikator, grün, aus, unten links
                    }
                    break;
                case (EMERALD_REPLICATOR_GREEN_BOTTOM_RIGHT):
                    if (Playfield.bReplicatorGreenOn) {
                        uTextureIndex = 187;     // Replikator, grün, an, unten rechts, Animationsschritt 1,2 u. 3
                    } else {
                        uTextureIndex = 176;     // Replikator, grün, aus, unten rechts
                    }
                    break;
                case (EMERALD_REPLICATOR_BLUE_TOP_LEFT):
                    if (Playfield.bReplicatorBlueOn) {
                        if ((uReplicatorAnimation >= 0) && (uReplicatorAnimation <= 3)) {
                            uTextureIndex = 213;     // Replikator, blau, an, oben links, Animationsschritt 1
                        } else if ((uReplicatorAnimation >= 4) && (uReplicatorAnimation <= 7)) {
                            uTextureIndex = 216;     // Replikator, blau, an, oben links, Animationsschritt 2
                        } else {
                            uTextureIndex = 219;     // Replikator, blau, an, oben links, Animationsschritt 3
                        }
                    } else {
                        uTextureIndex = 208;     // Replikator, blau, aus, oben links
                    }
                    break;
                case (EMERALD_REPLICATOR_BLUE_TOP_MID):
                    if (Playfield.bReplicatorBlueOn) {
                        if ((uReplicatorAnimation >= 0) && (uReplicatorAnimation <= 3)) {
                            uTextureIndex = 214;     // Replikator, blau, an, mitte links, Animationsschritt 1
                        } else if ((uReplicatorAnimation >= 4) && (uReplicatorAnimation <= 7)) {
                            uTextureIndex = 217;     // Replikator, blau, an, mitte links, Animationsschritt 2
                        } else {
                            uTextureIndex = 220;     // Replikator, blau, an, mitte links, Animationsschritt 3
                        }
                    } else {
                        uTextureIndex = 209;     // Replikator, blau, aus, oben mitte
                    }
                    break;
                case (EMERALD_REPLICATOR_BLUE_TOP_RIGHT):
                    if (Playfield.bReplicatorBlueOn) {
                        if ((uReplicatorAnimation >= 0) && (uReplicatorAnimation <= 3)) {
                            uTextureIndex = 215;     // Replikator, blau, an, oben rechts, Animationsschritt 1
                        } else if ((uReplicatorAnimation >= 4) && (uReplicatorAnimation <= 7)) {
                            uTextureIndex = 218;     // Replikator, blau, an, oben rechts, Animationsschritt 2
                        } else {
                            uTextureIndex = 221;     // Replikator, blau, an, oben rechts, Animationsschritt 3
                        }
                    } else {
                        uTextureIndex = 210;     // Replikator, blau, aus, oben rechts
                    }
                    break;
                case (EMERALD_REPLICATOR_BLUE_BOTTOM_LEFT):
                    if (Playfield.bReplicatorBlueOn) {
                        uTextureIndex = 222;     // Replikator, blau, an, unten links, Animationsschritt 1,2 u. 3
                    } else {
                        uTextureIndex = 211;     // Replikator, blau, aus, unten links
                    }
                    break;
                case (EMERALD_REPLICATOR_BLUE_BOTTOM_RIGHT):
                    if (Playfield.bReplicatorBlueOn) {
                        uTextureIndex = 223;     // Replikator, blau, an, unten rechts, Animationsschritt 1,2 u. 3
                    } else {
                        uTextureIndex = 212;     // Replikator, blau, aus, unten rechts
                    }
                    break;
                case (EMERALD_REPLICATOR_YELLOW_TOP_LEFT):
                    if (Playfield.bReplicatorYellowOn) {
                        if ((uReplicatorAnimation >= 0) && (uReplicatorAnimation <= 3)) {
                            uTextureIndex = 195;     // Replikator, gelb, an, oben links, Animationsschritt 1
                        } else if ((uReplicatorAnimation >= 4) && (uReplicatorAnimation <= 7)) {
                            uTextureIndex = 198;     // Replikator, gelb, an, oben links, Animationsschritt 2
                        } else {
                            uTextureIndex = 201;     // Replikator, gelb, an, oben links, Animationsschritt 3
                        }
                    } else {
                        uTextureIndex = 190;     // Replikator, gelb, aus, oben links
                    }
                    break;
                case (EMERALD_REPLICATOR_YELLOW_TOP_MID):
                    if (Playfield.bReplicatorYellowOn) {
                        if ((uReplicatorAnimation >= 0) && (uReplicatorAnimation <= 3)) {
                            uTextureIndex = 196;     // Replikator, gelb, an, mitte links, Animationsschritt 1
                        } else if ((uReplicatorAnimation >= 4) && (uReplicatorAnimation <= 7)) {
                            uTextureIndex = 199;     // Replikator, gelb, an, mitte links, Animationsschritt 2
                        } else {
                            uTextureIndex = 202;     // Replikator, gelb, an, mitte links, Animationsschritt 3
                        }
                    } else {
                        uTextureIndex = 191;     // Replikator, gelb, aus, oben mitte
                    }
                    break;
                case (EMERALD_REPLICATOR_YELLOW_TOP_RIGHT):
                    if (Playfield.bReplicatorYellowOn) {
                        if ((uReplicatorAnimation >= 0) && (uReplicatorAnimation <= 3)) {
                            uTextureIndex = 197;     // Replikator, gelb, an, oben rechts, Animationsschritt 1
                        } else if ((uReplicatorAnimation >= 4) && (uReplicatorAnimation <= 7)) {
                            uTextureIndex = 200;     // Replikator, gelb, an, oben rechts, Animationsschritt 2
                        } else {
                            uTextureIndex = 203;     // Replikator, gelb, an, oben rechts, Animationsschritt 3
                        }
                    } else {
                        uTextureIndex = 192;     // Replikator, gelb, aus, oben rechts
                    }
                    break;
                case (EMERALD_REPLICATOR_YELLOW_BOTTOM_LEFT):
                    if (Playfield.bReplicatorYellowOn) {
                        uTextureIndex = 204;     // Replikator, gelb, an, unten links, Animationsschritt 1,2 u. 3
                    } else {
                        uTextureIndex = 193;     // Replikator, gelb, aus, unten links
                    }
                    break;
                case (EMERALD_REPLICATOR_YELLOW_BOTTOM_RIGHT):
                    if (Playfield.bReplicatorYellowOn) {
                        uTextureIndex = 205;     // Replikator, gelb, an, unten rechts, Animationsschritt 1,2 u. 3
                    } else {
                        uTextureIndex = 194;     // Replikator, gelb, aus, unten rechts
                    }
                    break;
                case (EMERALD_KEY_WHITE):
                    uTextureIndex = 519;
                    if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN1) {
                        nXoffs = 15 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN2) {
                        nXoffs = 7 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = 0.5 + nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (uSelfStatus == EMERALD_ANIM_KEY_WHITE_SHRINK) { // Man mit Richtung und Fire
                        nXoffs = nAnimationCount;
                        nYoffs = nAnimationCount;
                        fScaleW = 1 - nAnimationCount * 0.06;
                        fScaleH = fScaleW;
                        fAngleOffs = nAnimationCount * 10;
                    }
                    break;
                case (EMERALD_KEY_GENERAL):
                    uTextureIndex = 520;
                    if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN1) {
                        nXoffs = 15 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN2) {
                        nXoffs = 7 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = 0.5 + nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (uSelfStatus == EMERALD_ANIM_KEY_GENERAL_SHRINK) { // Man mit Richtung und Fire
                        nXoffs = nAnimationCount;
                        nYoffs = nAnimationCount;
                        fScaleW = 1 - nAnimationCount * 0.06;
                        fScaleH = fScaleW;
                        fAngleOffs = nAnimationCount * 10;
                    }
                    break;
                case (EMERALD_KEY_RED):
                    uTextureIndex = 98;     // roter Schlüssel
                    if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN1) {
                        nXoffs = 15 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN2) {
                        nXoffs = 7 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = 0.5 + nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (uSelfStatus == EMERALD_ANIM_KEY_RED_SHRINK) { // Man mit Richtung und Fire
                        nXoffs = nAnimationCount;
                        nYoffs = nAnimationCount;
                        fScaleW = 1 - nAnimationCount * 0.06;
                        fScaleH = fScaleW;
                        fAngleOffs = nAnimationCount * 10;
                    }
                    break;
                case (EMERALD_KEY_YELLOW):
                    uTextureIndex = 101;    // gelber Schlüssel
                    if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN1) {
                        nXoffs = 15 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN2) {
                        nXoffs = 7 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = 0.5 + nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (uSelfStatus == EMERALD_ANIM_KEY_YELLOW_SHRINK) { // Man mit Richtung und Fire
                        nXoffs = nAnimationCount;
                        nYoffs = nAnimationCount;
                        fScaleW = 1 - nAnimationCount * 0.06;
                        fScaleH = fScaleW;
                        fAngleOffs = nAnimationCount * 10;
                    }
                    break;
                case (EMERALD_KEY_BLUE):
                    uTextureIndex = 100;    // blauer Schlüssel
                    if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN1) {
                        nXoffs = 15 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN2) {
                        nXoffs = 7 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = 0.5 + nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (uSelfStatus == EMERALD_ANIM_KEY_BLUE_SHRINK) { // Man mit Richtung und Fire
                        nXoffs = nAnimationCount;
                        nYoffs = nAnimationCount;
                        fScaleW = 1 - nAnimationCount * 0.06;
                        fScaleH = fScaleW;
                        fAngleOffs = nAnimationCount * 10;
                    }
                    break;
                case (EMERALD_KEY_GREEN):
                    uTextureIndex = 99;     // grüner Schlüssel
                    if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN1) {
                        nXoffs = 15 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN2) {
                        nXoffs = 7 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = 0.5 + nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (uSelfStatus == EMERALD_ANIM_KEY_GREEN_SHRINK) { // Man mit Richtung und Fire
                        nXoffs = nAnimationCount;
                        nYoffs = nAnimationCount;
                        fScaleW = 1 - nAnimationCount * 0.06;
                        fScaleH = fScaleW;
                        fAngleOffs = nAnimationCount * 10;
                    }
                    break;
                case (EMERALD_DOOR_WHITE):
                    uTextureIndex = 521;
                    break;
                case (EMERALD_DOOR_WHITE_WOOD):
                    uTextureIndex = 522;
                    break;
                case (EMERALD_DOOR_RED_WOOD):     // rote Holztür
                    uTextureIndex = 462;
                    break;
                case (EMERALD_DOOR_YELLOW_WOOD):  // gelbe Holztür
                    uTextureIndex = 465;
                    break;
                case (EMERALD_DOOR_BLUE_WOOD):    // blaue Holztür
                    uTextureIndex = 464;
                    break;
                case (EMERALD_DOOR_GREEN_WOOD):   // grüne Holztür
                    uTextureIndex = 463;
                    break;
                case (EMERALD_DOOR_RED):
                    uTextureIndex = 94;     // rote Tür
                    break;
                case (EMERALD_DOOR_YELLOW):
                    uTextureIndex = 97;     // gelbe Tür
                    break;
                case (EMERALD_DOOR_BLUE):
                    uTextureIndex = 96;     // blaue Tür
                    break;
                case (EMERALD_DOOR_GREEN):
                    uTextureIndex = 95;     // grüne Tür
                    break;
                case (EMERALD_DOOR_GREY_RED):
                case (EMERALD_DOOR_GREY_GREEN):
                case (EMERALD_DOOR_GREY_BLUE):
                case (EMERALD_DOOR_GREY_YELLOW):
                case (EMERALD_DOOR_GREY_WHITE):
                    uTextureIndex = 288;     // Geheimtür
                    break;
                case (EMERALD_MAN):
                    // Nimmt Man Gegenstand auf, d.h. läuft er in das Element hinein?
                    if ( (uSelfStatus == EMERALD_ANIM_KEY_RED_SHRINK) ||
                         (uSelfStatus == EMERALD_ANIM_KEY_GREEN_SHRINK) ||
                         (uSelfStatus == EMERALD_ANIM_KEY_BLUE_SHRINK) ||
                         (uSelfStatus == EMERALD_ANIM_KEY_YELLOW_SHRINK) ||
                         (uSelfStatus == EMERALD_ANIM_KEY_WHITE_SHRINK) ||
                         (uSelfStatus == EMERALD_ANIM_KEY_GENERAL_SHRINK) ||
                         (uSelfStatus == EMERALD_ANIM_EMERALD_SHRINK) ||
                         (uSelfStatus == EMERALD_ANIM_RUBY_SHRINK) ||
                         (uSelfStatus == EMERALD_ANIM_PERL_SHRINK) ||
                         (uSelfStatus == EMERALD_ANIM_CRYSTAL_SHRINK) ||
                         (uSelfStatus == EMERALD_ANIM_TIME_COIN_SHRINK) ||
                         (uSelfStatus == EMERALD_ANIM_HAMMER_SHRINK) ||
                         (uSelfStatus == EMERALD_ANIM_DYNAMITE_SHRINK) ||
                         (uSelfStatus == EMERALD_ANIM_MESSAGE_SHRINK) ||
                         (uSelfStatus == EMERALD_ANIM_SAPPHIRE_SHRINK) ||
                         (uSelfStatus == EMERALD_ANIM_SAND_SHRINK) ) {
                            bExtendedElement = true;
                            uTextureIndex_0 = GetTextureIndexByShrink(uSelfStatus);
                            nXoffs_0 = nAnimationCount;
                            nYoffs_0 = nAnimationCount;
                            fScaleW_0 = 1 - nAnimationCount * 0.06;
                            fScaleH_0 = fScaleW_0;
                    }
                    if (uAnimStatus == EMERALD_ANIM_LEFT) {
                        nXoffs = FONT_W - nAnimationCount * 2;         // Man steht bereits auf neuer Position, daher + FONT_W
                        if (uSelfStatus == EMERALD_ANIM_MAN_PUSH_LEFT) {
                            Flip = SDL_FLIP_HORIZONTAL;
                            if (nAnimationCount <= 9) {
                                uTextureIndex = 243 + nAnimationCount / 2;     // Man links schiebend
                            } else {
                                uTextureIndex = 246 + 5 - nAnimationCount / 2;
                            }
                        } else {
                            uTextureIndex = 103 + nAnimationCount % 8;     // Man links
                        }
                    } else if (uAnimStatus == EMERALD_ANIM_RIGHT) {
                        nXoffs = nAnimationCount * 2 - FONT_W;      // Man steht bereits auf neuer Position, daher - FONT_W
                        if (uSelfStatus == EMERALD_ANIM_MAN_PUSH_RIGHT) {
                            if (nAnimationCount <= 9) {
                                uTextureIndex = 243 + nAnimationCount / 2;     // Man rechts schiebend
                            } else {
                                uTextureIndex = 246 + 5 - nAnimationCount / 2;
                            }
                        } else {
                            uTextureIndex = 103 + nAnimationCount % 8; // Man rechts
                            Flip = SDL_FLIP_HORIZONTAL;
                        }
                    } else if (uAnimStatus == EMERALD_ANIM_UP) {
                        uTextureIndex = 111 + nAnimationCount % 8;     // Man hoch
                        nYoffs = -nAnimationCount * 2 + FONT_H;        // Man steht bereits auf neuer Position, daher + FONT_H
                    } else if (uAnimStatus == EMERALD_ANIM_DOWN) {
                        uTextureIndex = 119 + nAnimationCount % 8;     // Man runter
                        nYoffs = nAnimationCount * 2 - FONT_H;         // Man steht bereits auf neuer Position, daher - FONT_H
                    } else if (uAnimStatus == EMERALD_ANIM_LEFT_DOUBLESPEED) {
                        uTextureIndex = 103 + nAnimationCount % 8;     // Man links
                        nXoffs = -nAnimationCount * 4 + FONT_W * 2;    // Man steht bereits auf neuer Position, daher + FONT_W * 2 (DoubleSpeed)
                    } else if (uAnimStatus == EMERALD_ANIM_RIGHT_DOUBLESPEED) {
                        uTextureIndex = 103 + nAnimationCount % 8;     // Man rechts
                        nXoffs = nAnimationCount * 4 - FONT_W * 2;     // Man steht bereits auf neuer Position, daher - FONT_W * 2 (DoubleSpeed)
                        Flip = SDL_FLIP_HORIZONTAL;
                    } else if (uAnimStatus == EMERALD_ANIM_UP_DOUBLESPEED) {
                        uTextureIndex = 111 + nAnimationCount % 8;     // Man hoch
                        nYoffs = -nAnimationCount * 4 + FONT_H * 2;    // Man steht bereits auf neuer Position, daher + FONT_H * 2 (DoubleSpeed)
                    } else if (uAnimStatus == EMERALD_ANIM_DOWN_DOUBLESPEED) {
                        uTextureIndex = 119 + nAnimationCount % 8;     // Man runter
                        nYoffs = nAnimationCount * 4 - FONT_H * 2;    // Man steht bereits auf neuer Position, daher - FONT_H * 2 (DoubleSpeed)
                    } else {
                        uTextureIndex = 102;                            // Man stehend
                        if (uSelfStatus == EMERALD_ANIM_MAN_LEFT_ARM) {
                            uTextureIndex = 240;                        // Man stehend, linker Arm
                        } else if (uSelfStatus == EMERALD_ANIM_MAN_RIGHT_ARM) {
                            uTextureIndex = 241;                        // Man stehend, rechter Arm
                        } else if (uSelfStatus == EMERALD_ANIM_MAN_UP_ARM) {
                            uTextureIndex = 242;                        // Man stehend, Arm hoch
                        } else if (uSelfStatus == EMERALD_ANIM_MAN_DOWN_ARM) {
                            uTextureIndex = 239;                        // Man stehend, Arm runter
                        } else if (uSelfStatus == EMERALD_ANIM_MAN_PUSH_RIGHT) {
                            if (nAnimationCount <= 9) {
                                uTextureIndex = 243 + nAnimationCount / 2;     // Man rechts schiebend
                            } else {
                                uTextureIndex = 246 + 5 - nAnimationCount / 2;
                            }
                        } else if (uSelfStatus == EMERALD_ANIM_MAN_PUSH_LEFT) {
                            Flip = SDL_FLIP_HORIZONTAL;
                            if (nAnimationCount <= 9) {
                                uTextureIndex = 243 + nAnimationCount / 2;     // Man rechts schiebend
                            } else {
                                uTextureIndex = 246 + 5 - nAnimationCount / 2;
                            }
                        }
                    }
                    break;
                case (EMERALD_BOMB):
                    uTextureIndex = 271;     // Bomb, liegend
                    if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN1) {
                        nXoffs = 15 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN2) {
                        nXoffs = 7 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = 0.5 + nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (uAnimStatus == EMERALD_ANIM_LEFT) {
                        uTextureIndex = 271 + nAnimationCount % 8;
                        if (uSelfStatus == EMERALD_ANIM_MAN_PUSH_LEFT2) {
                            nXoffs = -nAnimationCount * 2 + FONT_W;
                        } else {
                            nXoffs = -nAnimationCount * 2;
                        }
                    } else if (uAnimStatus == EMERALD_ANIM_RIGHT) {
                        uTextureIndex = 271 + nAnimationCount % 8;
                        if (uSelfStatus == EMERALD_ANIM_MAN_PUSH_RIGHT2) {
                            nXoffs = nAnimationCount * 2 - FONT_W;
                        } else {
                            nXoffs = nAnimationCount * 2;
                        }
                    } else if (uAnimStatus == EMERALD_ANIM_DOWN) {
                        nYoffs = nAnimationCount * 2;
                        uTextureIndex = 271 + nAnimationCount % 8;
                    }
                    break;
                case (EMERALD_MEGABOMB):
                    uTextureIndex = 524;     // Mega-Bomb, liegend
                    if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN1) {
                        nXoffs = 15 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN2) {
                        nXoffs = 7 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = 0.5 + nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (uAnimStatus == EMERALD_ANIM_LEFT) {
                        fAngleOffs = -nAnimationCount * 5.5;
                        if (uSelfStatus == EMERALD_ANIM_MAN_PUSH_LEFT2) {
                            nXoffs = -nAnimationCount * 2 + FONT_W;
                        } else {
                            nXoffs = -nAnimationCount * 2;
                        }
                    } else if (uAnimStatus == EMERALD_ANIM_RIGHT) {
                        fAngleOffs = nAnimationCount * 5.5;
                        if (uSelfStatus == EMERALD_ANIM_MAN_PUSH_RIGHT2) {
                            nXoffs = nAnimationCount * 2 - FONT_W;
                        } else {
                            nXoffs = nAnimationCount * 2;
                        }
                    } else if (uAnimStatus == EMERALD_ANIM_DOWN) {
                        uTextureIndex = 524 + ((Playfield.uFrameCounter & 0xFFFFFFFC) >> 2) % 5;
                        nYoffs = nAnimationCount * 2;
                    }
                    break;
                case (EMERALD_PERL):
                    uTextureIndex = 436;     // Perle, liegend
                    if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN1) {
                        nXoffs = 15 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN2) {
                        nXoffs = 7 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = 0.5 + nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (uSelfStatus == EMERALD_ANIM_PERL_SHRINK) { // Man mit Richtung und Fire
                            nXoffs = nAnimationCount;
                            nYoffs = nAnimationCount;
                            fScaleW = 1 - nAnimationCount * 0.06;
                            fScaleH = fScaleW;
                            fAngleOffs = nAnimationCount * 10;
                    } else if (uAnimStatus == EMERALD_ANIM_LEFT) {
                        uTextureIndex = 436 + nAnimationCount % 8;
                        nXoffs = -nAnimationCount * 2;
                    } else if (uAnimStatus == EMERALD_ANIM_RIGHT) {
                        uTextureIndex = 436 + nAnimationCount % 8;
                        nXoffs = nAnimationCount * 2;
                    } else if ((uAnimStatus == EMERALD_ANIM_DOWN) || (uSelfStatus == EMERALD_ANIM_SINK_IN_MAGIC_WALL)) {
                        nYoffs = nAnimationCount * 2;
                        uTextureIndex = 436 + nAnimationCount % 8;
                    } else if (uSelfStatus == EMERALD_ANIM_PERL_BREAK) {
                        if (nAnimationCount <= 2) {   // 0, 1, 2
                            uTextureIndex = 444;
                        } else if (nAnimationCount <= 5) { // 3, 4, 5
                            uTextureIndex = 445;
                        } else if (nAnimationCount <= 8) { // 6, 7, 8
                            uTextureIndex = 446;
                        } else {
                            uTextureIndex = 0;  // Space
                        }
                    }
                    break;
                case (EMERALD_STONE):
                    uTextureIndex = 71;     // Stein, liegend
                    if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN1) {
                        nXoffs = 15 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN2) {
                        nXoffs = 7 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = 0.5 + nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (uAnimStatus == EMERALD_ANIM_LEFT) {
                        fAngleOffs = -nAnimationCount * 5.5;
                        if (uSelfStatus == EMERALD_ANIM_MAN_PUSH_LEFT2) {
                            nXoffs = -nAnimationCount * 2 + FONT_W;
                        } else {
                            nXoffs = -nAnimationCount * 2;
                        }
                    } else if (uAnimStatus == EMERALD_ANIM_RIGHT) {
                        fAngleOffs = nAnimationCount * 5.5;
                        if (uSelfStatus == EMERALD_ANIM_MAN_PUSH_RIGHT2) {
                            nXoffs = nAnimationCount * 2 - FONT_W;
                        } else {
                            nXoffs = nAnimationCount * 2;
                        }
                    } else if (uAnimStatus == EMERALD_ANIM_DOWN) {
                        nYoffs = nAnimationCount * 2;
                        if (uSelfStatus == EMERALD_ANIM_SAPPHIRE_SQUEAK) {
                            nYoffs = nAnimationCount * 2 - FONT_H;
                            bExtendedElement = true;
                            uTextureIndex_0 = 248; // Saphir, liegend
                            nXoffs_0 = 0;
                            nYoffs_0 = nAnimationCount * 2;
                            fScaleW_0 = 1;
                            fScaleH_0 = 1 - nAnimationCount * 0.06;
                        }
                    } else if (uSelfStatus == EMERALD_ANIM_SINK_IN_MAGIC_WALL) {
                        nYoffs = nAnimationCount * 2;
                    }
                    break;
                case (EMERALD_NUT):
                    uTextureIndex = 234;     // Nut, liegend
                    if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN1) {
                        nXoffs = 15 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN2) {
                        nXoffs = 7 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = 0.5 + nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_NUT_CRACK2) {
                        uTextureIndex = 235 + nAnimationCount / 4;     // Nut, liegend
                    } else if (uAnimStatus == EMERALD_ANIM_LEFT) {
                        fAngleOffs = -nAnimationCount * 5.5;
                        if (uSelfStatus == EMERALD_ANIM_MAN_PUSH_LEFT2) {
                            nXoffs = -nAnimationCount * 2 + FONT_W;
                        } else {
                            nXoffs = -nAnimationCount * 2;
                        }
                    } else if (uAnimStatus == EMERALD_ANIM_RIGHT) {
                        fAngleOffs = nAnimationCount * 5.5;
                        if (uSelfStatus == EMERALD_ANIM_MAN_PUSH_RIGHT2) {
                            nXoffs = nAnimationCount * 2 - FONT_W;
                        } else {
                            nXoffs = nAnimationCount * 2;
                        }
                    } else if (uAnimStatus == EMERALD_ANIM_DOWN) {
                        nYoffs = nAnimationCount * 2;
                    }
                    break;
                case (EMERALD_RUBY):
                    // Soll Rubin blitzen ?
                    if ((nAnimationCount == 0) && (Playfield.pStatusAnimation[I] == EMERALD_ANIM_STAND)) {
                        if ((uint8_t)randf(0,100) < 10) {
                            Playfield.pStatusAnimation[I] = EMERALD_ANIM_BLITZ;
                        }
                    }
                    if (Playfield.pStatusAnimation[I] != EMERALD_ANIM_BLITZ) {
                        uTextureIndex = 301;     // Rubin, liegend
                        if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN1) {
                            nXoffs = 15 - nAnimationCount / 2;
                            nYoffs = nXoffs;
                            fScaleW = nAnimationCount * 0.031;
                            fScaleH = fScaleW;
                        } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN2) {
                            nXoffs = 7 - nAnimationCount / 2;
                            nYoffs = nXoffs;
                            fScaleW = 0.5 + nAnimationCount * 0.031;
                            fScaleH = fScaleW;
                        } else if (uSelfStatus == EMERALD_ANIM_RUBY_SHRINK) { // Man mit Richtung und Fire
                            nXoffs = nAnimationCount;
                            nYoffs = nAnimationCount;
                            fScaleW = 1 - nAnimationCount * 0.06;
                            fScaleH = fScaleW;
                            fAngleOffs = nAnimationCount * 10;
                        } else if (uAnimStatus == EMERALD_ANIM_LEFT) {
                            uTextureIndex = 301 + nAnimationCount / 2;     // Rubin, liegend
                            nXoffs = -nAnimationCount * 2;
                            fAngleOffs = -nAnimationCount * 5.5;
                        } else if (uAnimStatus == EMERALD_ANIM_RIGHT) {
                            uTextureIndex = 301 + nAnimationCount / 2;     // Rubin, liegend
                            nXoffs = nAnimationCount * 2;
                            fAngleOffs = nAnimationCount * 5.5;
                        } else if ((uAnimStatus == EMERALD_ANIM_DOWN) || (uSelfStatus == EMERALD_ANIM_SINK_IN_MAGIC_WALL)) {
                            nYoffs = nAnimationCount * 2;
                            uTextureIndex = 301 + nAnimationCount / 2;     // Rubin, liegend
                        }
                    } else {
                        bExtendedElement = true;
                        fAngle_0 = 0;
                        uTextureIndex_0 = 301;     // Rubin, liegend
                        if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN1) {
                            nXoffs_0 = 15 - nAnimationCount / 2;
                            nYoffs_0 = nXoffs_0;
                            fScaleW_0 = nAnimationCount * 0.031;
                            fScaleH_0 = fScaleW_0;
                        } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN2) {
                            nXoffs_0 = 7 - nAnimationCount / 2;
                            nYoffs_0 = nXoffs_0;
                            fScaleW_0 = 0.5 + nAnimationCount * 0.031;
                            fScaleH_0 = fScaleW_0;
                        } else if (uSelfStatus == EMERALD_ANIM_RUBY_SHRINK) { // Man mit Richtung und Fire
                            nXoffs_0 = nAnimationCount;
                            nYoffs_0 = nAnimationCount;
                            fScaleW_0 = 1 - nAnimationCount * 0.06;
                            fScaleH_0 = fScaleW_0;
                            fAngleOffs_0 = nAnimationCount * 10;
                        } else if (uAnimStatus == EMERALD_ANIM_LEFT) {
                            uTextureIndex_0 = 301 + nAnimationCount / 2;     // Rubin, liegend
                            nXoffs_0 = -nAnimationCount * 2;
                            fAngleOffs_0 = -nAnimationCount * 5.5;
                        } else if (uAnimStatus == EMERALD_ANIM_RIGHT) {
                            uTextureIndex_0 = 301 + nAnimationCount / 2;     // Rubin, liegend
                            nXoffs_0 = nAnimationCount * 2;
                            fAngleOffs_0 = nAnimationCount * 5.5;
                        } else if (uAnimStatus == EMERALD_ANIM_DOWN) {
                            nYoffs_0 = nAnimationCount * 2;
                            uTextureIndex_0 = 301 + nAnimationCount / 2;     // Rubin, liegend
                        }
                        // Blitz-Animation über Rubin
                        nXoffs = 0;
                        nYoffs = 0;
                        if (nAnimationCount <= 7) {
                            uTextureIndex = 267 + nAnimationCount / 2;
                        } else {
                            uTextureIndex = 270 - (nAnimationCount - 8) / 2;
                        }
                    }
                    break;
                case (EMERALD_CRYSTAL):
                    // Soll Kristall blitzen ?
                    uTextureIndex = 309;     // Kristall, liegend
                    uTextureIndex_0 = 309;     // Kristall, liegend
                    if ((nAnimationCount == 0) && (Playfield.pStatusAnimation[I] == EMERALD_ANIM_STAND)) {
                        if ((uint8_t)randf(0,100) < 10) {
                            Playfield.pStatusAnimation[I] = EMERALD_ANIM_BLITZ;
                        }
                    }
                    if (Playfield.pStatusAnimation[I] != EMERALD_ANIM_BLITZ) {
                        if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN1) {
                            nXoffs = 15 - nAnimationCount / 2;
                            nYoffs = nXoffs;
                            fScaleW = nAnimationCount * 0.031;
                            fScaleH = fScaleW;
                        } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN2) {
                            nXoffs = 7 - nAnimationCount / 2;
                            nYoffs = nXoffs;
                            fScaleW = 0.5 + nAnimationCount * 0.031;
                            fScaleH = fScaleW;
                        } else if (uSelfStatus == EMERALD_ANIM_CRYSTAL_SHRINK) { // Man mit Richtung und Fire
                            nXoffs = nAnimationCount;
                            nYoffs = nAnimationCount;
                            fScaleW = 1 - nAnimationCount * 0.06;
                            fScaleH = fScaleW;
                            fAngleOffs = nAnimationCount * 10;
                        } else if (uAnimStatus == EMERALD_ANIM_LEFT) {
                            nXoffs = -nAnimationCount * 2;
                            fAngleOffs = -nAnimationCount * 22.5;
                        } else if (uAnimStatus == EMERALD_ANIM_RIGHT) {
                            nXoffs = nAnimationCount * 2;
                            fAngleOffs = nAnimationCount * 22.5;
                        } else if ((uAnimStatus == EMERALD_ANIM_DOWN) || (uSelfStatus == EMERALD_ANIM_SINK_IN_MAGIC_WALL)) {
                            nYoffs = nAnimationCount * 2;
                        }
                    } else {
                        bExtendedElement = true;
                        fAngle_0 = 0;
                        if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN1) {
                            nXoffs_0 = 15 - nAnimationCount / 2;
                            nYoffs_0 = nXoffs_0;
                            fScaleW_0 = nAnimationCount * 0.031;
                            fScaleH_0 = fScaleW_0;
                        } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN2) {
                            nXoffs_0 = 7 - nAnimationCount / 2;
                            nYoffs_0 = nXoffs_0;
                            fScaleW_0 = 0.5 + nAnimationCount * 0.031;
                            fScaleH_0 = fScaleW_0;
                        } else if (uSelfStatus == EMERALD_ANIM_CRYSTAL_SHRINK) { // Man mit Richtung und Fire
                            nXoffs_0 = nAnimationCount;
                            nYoffs_0 = nAnimationCount;
                            fScaleW_0 = 1 - nAnimationCount * 0.06;
                            fScaleH_0 = fScaleW_0;
                            fAngleOffs_0 = nAnimationCount * 10;
                        } else if (uAnimStatus == EMERALD_ANIM_LEFT) {
                            nXoffs_0 = -nAnimationCount * 2;
                            fAngleOffs_0 = -nAnimationCount * 5.5;
                        } else if (uAnimStatus == EMERALD_ANIM_RIGHT) {
                            nXoffs_0 = nAnimationCount * 2;
                            fAngleOffs_0 = nAnimationCount * 5.5;
                        } else if (uAnimStatus == EMERALD_ANIM_DOWN) {
                            nYoffs_0 = nAnimationCount * 2;
                        }
                        // Blitz-Animation über Kristall
                        nXoffs = 0;
                        nYoffs = 0;
                        if (nAnimationCount <= 7) {
                            uTextureIndex = 267 + nAnimationCount / 2;
                        } else {
                            uTextureIndex = 270 - (nAnimationCount - 8) / 2;
                        }
                    }
                    break;
                case (EMERALD_EMERALD):
                    // Soll Emerald blitzen ?
                    if ((nAnimationCount == 0) && (Playfield.pStatusAnimation[I] == EMERALD_ANIM_STAND)) {
                        if ((uint8_t)randf(0,100) < 10) {
                            Playfield.pStatusAnimation[I] = EMERALD_ANIM_BLITZ;
                        }
                    }
                    if (Playfield.pStatusAnimation[I] != EMERALD_ANIM_BLITZ) {
                        uTextureIndex = 226;     // Emerald, liegend
                        if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN1) {
                            nXoffs = 15 - nAnimationCount / 2;
                            nYoffs = nXoffs;
                            fScaleW = nAnimationCount * 0.031;
                            fScaleH = fScaleW;
                        } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN2) {
                            nXoffs = 7 - nAnimationCount / 2;
                            nYoffs = nXoffs;
                            fScaleW = 0.5 + nAnimationCount * 0.031;
                            fScaleH = fScaleW;
                        } else if (uSelfStatus == EMERALD_ANIM_EMERALD_SHRINK) { // Man mit Richtung und Fire
                            nXoffs = nAnimationCount;
                            nYoffs = nAnimationCount;
                            fScaleW = 1 - nAnimationCount * 0.06;
                            fScaleH = fScaleW;
                            fAngleOffs = nAnimationCount * 10;
                        } else if (uAnimStatus == EMERALD_ANIM_LEFT) {
                            uTextureIndex = 226 + nAnimationCount / 2;     // Emerald, liegend
                            nXoffs = -nAnimationCount * 2;
                            fAngleOffs = -nAnimationCount * 5.5;
                        } else if (uAnimStatus == EMERALD_ANIM_RIGHT) {
                            uTextureIndex = 226 + nAnimationCount / 2;     // Emerald, liegend
                            nXoffs = nAnimationCount * 2;
                            fAngleOffs = nAnimationCount * 5.5;
                        } else if ((uAnimStatus == EMERALD_ANIM_DOWN) || (uSelfStatus == EMERALD_ANIM_SINK_IN_MAGIC_WALL)) {
                            nYoffs = nAnimationCount * 2;
                            uTextureIndex = 226 + nAnimationCount / 2;     // Emerald, liegend
                        }
                    } else {
                        bExtendedElement = true;
                        fAngle_0 = 0;
                        uTextureIndex_0 = 226;     // Emerald, liegend
                        if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN1) {
                            nXoffs_0 = 15 - nAnimationCount / 2;
                            nYoffs_0 = nXoffs_0;
                            fScaleW_0 = nAnimationCount * 0.031;
                            fScaleH_0 = fScaleW_0;
                        } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN2) {
                            nXoffs_0 = 7 - nAnimationCount / 2;
                            nYoffs_0 = nXoffs_0;
                            fScaleW_0 = 0.5 + nAnimationCount * 0.031;
                            fScaleH_0 = fScaleW_0;
                        } else if (uSelfStatus == EMERALD_ANIM_EMERALD_SHRINK) { // Man mit Richtung und Fire
                            nXoffs_0 = nAnimationCount;
                            nYoffs_0 = nAnimationCount;
                            fScaleW_0 = 1 - nAnimationCount * 0.06;
                            fScaleH_0 = fScaleW_0;
                            fAngleOffs_0 = nAnimationCount * 10;
                        } else if (uAnimStatus == EMERALD_ANIM_LEFT) {
                            uTextureIndex_0 = 226 + nAnimationCount / 2;     // Emerald, liegend
                            nXoffs_0 = -nAnimationCount * 2;
                            fAngleOffs_0 = -nAnimationCount * 5.5;
                        } else if (uAnimStatus == EMERALD_ANIM_RIGHT) {
                            uTextureIndex_0 = 226 + nAnimationCount / 2;     // Emerald, liegend
                            nXoffs_0 = nAnimationCount * 2;
                            fAngleOffs_0 = nAnimationCount * 5.5;
                        } else if (uAnimStatus == EMERALD_ANIM_DOWN) {
                            nYoffs_0 = nAnimationCount * 2;
                            uTextureIndex_0 = 226 + nAnimationCount / 2;     // Emerald, liegend
                        }
                        // Blitz-Animation über Emerald
                        nXoffs = 0;
                        nYoffs = 0;
                        if (nAnimationCount <= 7) {
                            uTextureIndex = 267 + nAnimationCount / 2;
                        } else {
                            uTextureIndex = 270 - (nAnimationCount - 8) / 2;
                        }
                    }
                    break;
                case (EMERALD_SAPPHIRE):
                    // Soll Emerald blitzen ?
                    if ((nAnimationCount == 0) && (Playfield.pStatusAnimation[I] == EMERALD_ANIM_STAND)) {
                        if ((uint8_t)randf(0,100) < 10) {
                            Playfield.pStatusAnimation[I] = EMERALD_ANIM_BLITZ;
                        }
                    }
                    if (Playfield.pStatusAnimation[I] != EMERALD_ANIM_BLITZ) {
                        uTextureIndex = 248;
                        if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN1) {
                            nXoffs = 15 - nAnimationCount / 2;
                            nYoffs = nXoffs;
                            fScaleW = nAnimationCount * 0.031;
                            fScaleH = fScaleW;
                        } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN2) {
                            nXoffs = 7 - nAnimationCount / 2;
                            nYoffs = nXoffs;
                            fScaleW = 0.5 + nAnimationCount * 0.031;
                            fScaleH = fScaleW;
                        } else if (uSelfStatus == EMERALD_ANIM_SAPPHIRE_SHRINK) { // Man mit Richtung und Fire
                            nXoffs = nAnimationCount;
                            nYoffs = nAnimationCount;
                            fScaleW = 1 - nAnimationCount * 0.06;
                            fScaleH = fScaleW;
                            fAngleOffs = nAnimationCount * 10;
                        } else if (uAnimStatus == EMERALD_ANIM_LEFT) {
                            nXoffs = -nAnimationCount * 2;
                            fAngleOffs = -nAnimationCount * 5.5;
                        } else if (uAnimStatus == EMERALD_ANIM_RIGHT) {
                            nXoffs = nAnimationCount * 2;
                            fAngleOffs = nAnimationCount * 5.5;
                        } else if ((uAnimStatus == EMERALD_ANIM_DOWN) || (uSelfStatus == EMERALD_ANIM_SINK_IN_MAGIC_WALL)) {
                            nYoffs = nAnimationCount * 2;
                            uTextureIndex = 248 + ((Playfield.uFrameCounter & 0xFFFFFFFE) >> 1) % 9; // Saphir fallend
                        }
                    } else {
                        bExtendedElement = true;
                        fAngle_0 = 0;
                        uTextureIndex_0 = 248;     // Saphir, liegend
                        if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN1) {
                            nXoffs_0 = 15 - nAnimationCount / 2;
                            nYoffs_0 = nXoffs_0;
                            fScaleW_0 = nAnimationCount * 0.031;
                            fScaleH_0 = fScaleW_0;
                        } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN2) {
                            nXoffs_0 = 7 - nAnimationCount / 2;
                            nYoffs_0 = nXoffs_0;
                            fScaleW_0 = 0.5 + nAnimationCount * 0.031;
                            fScaleH_0 = fScaleW_0;
                        } else if (uSelfStatus == EMERALD_ANIM_SAPPHIRE_SHRINK) { // Man mit Richtung und Fire
                            nXoffs_0 = nAnimationCount;
                            nYoffs_0 = nAnimationCount;
                            fScaleW_0 = 1 - nAnimationCount * 0.06;
                            fScaleH_0 = fScaleW_0;
                            fAngleOffs_0 = nAnimationCount * 10;
                        } else if (uAnimStatus == EMERALD_ANIM_LEFT) {
                            nXoffs_0 = -nAnimationCount * 2;
                            fAngleOffs_0 = -nAnimationCount * 5.5;
                        } else if (uAnimStatus == EMERALD_ANIM_RIGHT) {
                            nXoffs_0 = nAnimationCount * 2;
                            fAngleOffs_0 = nAnimationCount * 5.5;
                        } else if (uAnimStatus == EMERALD_ANIM_DOWN) {
                            nYoffs_0 = nAnimationCount * 2;
                            uTextureIndex_0 = 248 + ((Playfield.uFrameCounter & 0xFFFFFFFE) >> 1) % 9; // Saphir fallend
                        }
                        // Blitz-Animation über Saphir
                        nXoffs = 0;
                        nYoffs = 0;
                        if (nAnimationCount <= 7) {
                            uTextureIndex = 267 + nAnimationCount / 2;
                        } else {
                            uTextureIndex = 270 - (nAnimationCount - 8) / 2;
                        }
                    }
                    break;
                case (EMERALD_TIME_COIN):
                    uTextureIndex = 310 + ((Playfield.uFrameCounter & 0xFFFFFFF8) >> 3) % 6; // Zeitmünze, drehend
                    if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN1) {
                        nXoffs = 15 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN2) {
                        nXoffs = 7 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = 0.5 + nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (uSelfStatus == EMERALD_ANIM_TIME_COIN_SHRINK) { // Man mit Richtung und Fire
                        nXoffs = nAnimationCount;
                        nYoffs = nAnimationCount;
                        fScaleW = 1 - nAnimationCount * 0.06;
                        fScaleH = fScaleW;
                        fAngleOffs = nAnimationCount * 10;
                    }
                    break;
                case (EMERALD_HAMMER):
                    uTextureIndex = 318;                             // Hammer
                    if (uSelfStatus == EMERALD_ANIM_HAMMER_SHRINK) { // Man mit Richtung und Fire
                        nXoffs = nAnimationCount;
                        nYoffs = nAnimationCount;
                        fScaleW = 1 - nAnimationCount * 0.06;
                        fScaleH = fScaleW;
                        fAngleOffs = nAnimationCount * 10;
                    } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN1) {
                        nXoffs = 15 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN2) {
                        nXoffs = 7 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = 0.5 + nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    }
                    break;
                case (EMERALD_DYNAMITE_OFF):
                    uTextureIndex = 286;                                // Dynamit
                    if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN1) {
                        nXoffs = 15 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN2) {
                        nXoffs = 7 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = 0.5 + nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (uSelfStatus == EMERALD_ANIM_DYNAMITE_SHRINK) {  // Man mit Richtung und Fire
                        nXoffs = nAnimationCount;
                        nYoffs = nAnimationCount;
                        fScaleW = 1 - nAnimationCount * 0.06;
                        fScaleH = fScaleW;
                        fAngleOffs = nAnimationCount * 10;
                    }
                    break;
                case (EMERALD_ALIEN):
                    if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN1) {
                        uTextureIndex = 127 + nAnimationCount / 2;     // Alien stehend
                        nXoffs = 15 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN2) {
                        uTextureIndex = 127 + nAnimationCount / 2;     // Alien stehend
                        nXoffs = 7 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = 0.5 + nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (uAnimStatus == EMERALD_ANIM_STAND) {
                        uTextureIndex = 127 + nAnimationCount / 2;     // Alien stehend
                    } else {
                        if ((nAnimationCount >= 4) && (nAnimationCount <= 11)) {
                            uTextureIndex = 135;                        // Alien geht 2, Flügel voll ausgebreitet
                        } else {
                            uTextureIndex = 136;                        // Alien geht 1
                        }
                        if (uAnimStatus == EMERALD_ANIM_LEFT) {
                            nXoffs = -nAnimationCount * 2;
                        } else if (uAnimStatus == EMERALD_ANIM_RIGHT) {
                            nXoffs = nAnimationCount * 2;
                        } else if (uAnimStatus == EMERALD_ANIM_UP) {
                            nYoffs = -nAnimationCount * 2;
                        } else if (uAnimStatus == EMERALD_ANIM_DOWN) {
                            nYoffs = nAnimationCount * 2;
                        }
                    }
                    break;
                case (EMERALD_WHEEL):
                    uTextureIndex = 137;     // Alien-Rad
                    if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_WHEEL_RUN) {
                        fAngleOffs = nAnimationCount * 11;
                    } else {
                        fAngleOffs = 0;
                    }
                    break;
                case (EMERALD_STEEL):
                    uTextureIndex = 72;     // Mauer hart
                    break;
                case (EMERALD_WALL_CORNERED):
                    uTextureIndex = 316;     // Mauer eckig
                    break;
                case (EMERALD_WALL_ROUND):
                    uTextureIndex = 317;     // Mauer rund
                    break;
                case (EMERALD_SAND):
                    if (uSelfStatus == EMERALD_ANIM_SAND_SHRINK) { // Man mit Richtung und Fire
                        uTextureIndex = 156; // kompl. freier Sand
                        nXoffs = nAnimationCount;
                        nYoffs = nAnimationCount;
                        fScaleW = 1 - nAnimationCount * 0.06;
                        fScaleH = fScaleW;
                        fAngleOffs = nAnimationCount * 5.5;
                    } else {
                        uTextureIndex = 156 + (Playfield.pStatusAnimation[I] & 0xFF);     // Sand mit richtigem Rand aussuchen
                    }
                    break;
                case (EMERALD_SPACE):
                    uTextureIndex = 0;      // Space
                    break;
                case (EMERALD_BEETLE_UP):   // Käfer hoch
                    uTextureIndex = 75 + nAnimationCount % 8;     // Käfer links
                    fAngle = 90;
                    if (uAnimStatus == EMERALD_ANIM_UP) {
                        nYoffs = -nAnimationCount * 2;
                    } else if (uAnimStatus == EMERALD_ANIM_SPIN_UP_TO_RIGHT) {
                        fAngleOffs = nAnimationCount * 5.5;
                    } else if (uAnimStatus == EMERALD_ANIM_SPIN_UP_TO_LEFT) {
                        fAngleOffs = -nAnimationCount * 5.5;
                    }
                    break;
                case (EMERALD_BEETLE_RIGHT):// Käfer rechts
                    uTextureIndex = 75 + nAnimationCount % 8;     // Käfer links
                    fAngle = 180;
                    if (uAnimStatus == EMERALD_ANIM_RIGHT) {
                        nXoffs = nAnimationCount * 2;
                    } else if (uAnimStatus == EMERALD_ANIM_SPIN_RIGHT_TO_DOWN) {
                        fAngleOffs = nAnimationCount * 5.5;
                    } else if (uAnimStatus == EMERALD_ANIM_SPIN_RIGHT_TO_UP) {
                        fAngleOffs = -nAnimationCount * 5.5;
                    }
                    break;
                case (EMERALD_BEETLE_DOWN): // Käfer runter
                    uTextureIndex = 75 + nAnimationCount % 8;     // Käfer links
                    fAngle = 270;
                    if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN1) {
                        nXoffs = 15 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN2) {
                        nXoffs = 7 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = 0.5 + nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (uAnimStatus == EMERALD_ANIM_DOWN) {
                        nYoffs = nAnimationCount * 2;
                    } else if (uAnimStatus == EMERALD_ANIM_SPIN_DOWN_TO_LEFT) {
                        fAngleOffs = nAnimationCount * 5.5;
                    } else if (uAnimStatus == EMERALD_ANIM_SPIN_DOWN_TO_RIGHT) {
                        fAngleOffs = -nAnimationCount * 5.5;
                    }
                    break;
                case (EMERALD_BEETLE_LEFT): // Käfer left
                    uTextureIndex = 75 + nAnimationCount % 8;     // Käfer links
                    fAngle = 0;
                    if (uAnimStatus == EMERALD_ANIM_LEFT) {
                        nXoffs = -nAnimationCount * 2;
                    } else if (uAnimStatus == EMERALD_ANIM_SPIN_LEFT_TO_DOWN) {
                        fAngleOffs = -nAnimationCount * 5.5;
                    } else if (uAnimStatus == EMERALD_ANIM_SPIN_LEFT_TO_UP) {
                        fAngleOffs = nAnimationCount * 5.5; // OK
                    }
                    break;
                case (EMERALD_MOLE_UP):     // Mole hoch
                    fAngle = 90;
                     uTextureIndex = 450 + Playfield.uFrameCounter % 11;     // Mole links
                    if (uAnimStatus == EMERALD_ANIM_UP) {
                        nYoffs = FONT_H - nAnimationCount * 2;
                    } else if (uAnimStatus == EMERALD_ANIM_MOLE_STAND) {
                        nYoffs = 0;         // läuft auf der Stelle
                    } else if (uAnimStatus == EMERALD_ANIM_SPIN_UP_TO_RIGHT) {
                        fAngleOffs = nAnimationCount * 5.5;
                    } else if (uAnimStatus == EMERALD_ANIM_SPIN_UP_TO_LEFT) {
                        fAngleOffs = -nAnimationCount * 5.5;
                    }
                    break;
                case (EMERALD_MOLE_RIGHT):  // Mole rechts
                    fAngle = 180;
                     uTextureIndex = 450 + Playfield.uFrameCounter % 11;     // Mole links
                    if (uAnimStatus == EMERALD_ANIM_RIGHT) {
                        nXoffs = -FONT_W + nAnimationCount * 2;
                    } else if (uAnimStatus == EMERALD_ANIM_MOLE_STAND) {
                        nXoffs = 0;         // läuft auf der Stelle
                    } else if (uAnimStatus == EMERALD_ANIM_SPIN_RIGHT_TO_DOWN) {
                        fAngleOffs = nAnimationCount * 5.5;
                    } else if (uAnimStatus == EMERALD_ANIM_SPIN_RIGHT_TO_UP) {
                        fAngleOffs = -nAnimationCount * 5.5;
                    }
                     break;
                case (EMERALD_MOLE_DOWN):   // Mole runter (kann über 2 Steuerungsphasen geboren werden)
                    fAngle = 270;
                    uTextureIndex = 450 + Playfield.uFrameCounter % 11;     // Mole links
                    if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN1) {
                        nXoffs = 15 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN2) {
                        nXoffs = 7 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = 0.5 + nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (uAnimStatus == EMERALD_ANIM_DOWN) {
                        nYoffs = -FONT_H + nAnimationCount * 2;
                    } else if (uAnimStatus == EMERALD_ANIM_MOLE_STAND) {
                        nYoffs = 0;         // läuft auf der Stelle
                    } else if (uAnimStatus == EMERALD_ANIM_SPIN_DOWN_TO_LEFT) {
                        fAngleOffs = nAnimationCount * 5.5;
                    } else if (uAnimStatus == EMERALD_ANIM_SPIN_DOWN_TO_RIGHT) {
                        fAngleOffs = -nAnimationCount * 5.5;
                    }
                    break;
                case (EMERALD_MOLE_LEFT):
                    fAngle = 0;
                     uTextureIndex = 450 + Playfield.uFrameCounter % 11;     // Mole links
                    if (uAnimStatus == EMERALD_ANIM_LEFT) {
                        nXoffs = FONT_W - nAnimationCount * 2;
                    } else if (uAnimStatus == EMERALD_ANIM_MOLE_STAND) {
                        nXoffs = 0;         // läuft auf der Stelle
                    } else if (uAnimStatus == EMERALD_ANIM_SPIN_LEFT_TO_DOWN) {
                        fAngleOffs = -nAnimationCount * 5.5;
                    } else if (uAnimStatus == EMERALD_ANIM_SPIN_LEFT_TO_UP) {
                        fAngleOffs = nAnimationCount * 5.5; // OK
                    }
                    break;
                case (EMERALD_MINE_UP):     // Mine hoch
                    if ( ((nAnimationCount >= 0) && (nAnimationCount <= 3)) || ((nAnimationCount >= 8) && (nAnimationCount <=11)) ) {
                        uTextureIndex = 73;     // Mine links
                    } else {
                        uTextureIndex = 74;     // Mine links an
                    }
                    fAngle = 90;
                    if (uAnimStatus == EMERALD_ANIM_UP) {
                        nYoffs = -nAnimationCount * 2;
                    } else if (uAnimStatus == EMERALD_ANIM_SPIN_UP_TO_RIGHT) {
                        fAngleOffs = nAnimationCount * 5.5;
                    } else if (uAnimStatus == EMERALD_ANIM_SPIN_UP_TO_LEFT) {
                        fAngleOffs = -nAnimationCount * 5.5;
                    }
                    break;
                case (EMERALD_MINE_RIGHT):  // Mine rechts
                    if ( ((nAnimationCount >= 0) && (nAnimationCount <= 3)) || ((nAnimationCount >= 8) && (nAnimationCount <=11)) ) {
                        uTextureIndex = 73;     // Mine links
                    } else {
                        uTextureIndex = 74;     // Mine links an
                    }
                    fAngle = 180;
                    if (uAnimStatus == EMERALD_ANIM_RIGHT) {
                        nXoffs = nAnimationCount * 2;
                    } else if (uAnimStatus == EMERALD_ANIM_SPIN_RIGHT_TO_DOWN) {
                        fAngleOffs = nAnimationCount * 5.5;
                    } else if (uAnimStatus == EMERALD_ANIM_SPIN_RIGHT_TO_UP) {
                        fAngleOffs = -nAnimationCount * 5.5;
                    }
                    break;
                case (EMERALD_MINE_DOWN):   // Mine runter (kann über 2 Steuerungsphasen geboren werden)
                    if ( ((nAnimationCount >= 0) && (nAnimationCount <= 3)) || ((nAnimationCount >= 8) && (nAnimationCount <=11)) ) {
                        uTextureIndex = 73;     // Mine links
                    } else {
                        uTextureIndex = 74;     // Mine links an
                    }
                    fAngle = 270;
                    if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN1) {
                        nXoffs = 15 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN2) {
                        nXoffs = 7 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = 0.5 + nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (uAnimStatus == EMERALD_ANIM_DOWN) {
                        nYoffs = nAnimationCount * 2;
                    } else if (uAnimStatus == EMERALD_ANIM_SPIN_DOWN_TO_LEFT) {
                        fAngleOffs = nAnimationCount * 5.5;
                    } else if (uAnimStatus == EMERALD_ANIM_SPIN_DOWN_TO_RIGHT) {
                        fAngleOffs = -nAnimationCount * 5.5;
                    }
                    break;
                case (EMERALD_MINE_LEFT):   // Mine links
                    if ( ((nAnimationCount >= 0) && (nAnimationCount <= 3)) || ((nAnimationCount >= 8) && (nAnimationCount <=11)) ) {
                        uTextureIndex = 73;     // Mine links
                    } else {
                        uTextureIndex = 74;     // Mine links an
                    }
                    fAngle = 0;
                    if (uAnimStatus == EMERALD_ANIM_LEFT) {
                        nXoffs = -nAnimationCount * 2;
                    } else if (uAnimStatus == EMERALD_ANIM_SPIN_LEFT_TO_DOWN) {
                        fAngleOffs = -nAnimationCount * 5.5;
                    } else if (uAnimStatus == EMERALD_ANIM_SPIN_LEFT_TO_UP) {
                        fAngleOffs = nAnimationCount * 5.5; // OK
                    }
                    break;
                case (EMERALD_FONT_EXCLAMATION):
                    uTextureIndex = 1;
                    break;
                case (EMERALD_FONT_ARROW_RIGHT):
                    uTextureIndex = 4;
                    break;
                case (EMERALD_FONT_ARROW_UP):
                    uTextureIndex = 5;
                    break;
                case (EMERALD_FONT_ARROW_DOWN):
                    uTextureIndex = 6;
                    break;
                case (EMERALD_FONT_APOSTROPHE):
                    uTextureIndex = 7;
                    break;
                case (EMERALD_FONT_BRACE_OPEN):
                    uTextureIndex = 8;
                    break;
                case (EMERALD_FONT_BRACE_CLOSE):
                    uTextureIndex = 9;
                    break;
                case (EMERALD_FONT_COPYRIGHT):
                    uTextureIndex = 10;
                    break;
                case (EMERALD_FONT_PLUS):
                    uTextureIndex = 11;
                    break;
                case (EMERALD_FONT_COMMA):
                    uTextureIndex = 12;
                    break;
                case (EMERALD_FONT_MINUS):
                    uTextureIndex = 13;
                    break;
                case (EMERALD_FONT_POINT):
                    uTextureIndex = 14;
                    break;
                case (EMERALD_FONT_SLASH):
                    uTextureIndex = 15;
                    break;
                case (EMERALD_FONT_0):
                    uTextureIndex = 16;
                    break;
                case (EMERALD_FONT_1):
                    uTextureIndex = 17;
                    break;
                case (EMERALD_FONT_2):
                    uTextureIndex = 18;
                    break;
                case (EMERALD_FONT_3):
                    uTextureIndex = 19;
                    break;
                case (EMERALD_FONT_4):
                    uTextureIndex = 20;
                    break;
                case (EMERALD_FONT_5):
                    uTextureIndex = 21;
                    break;
                case (EMERALD_FONT_6):
                    uTextureIndex = 22;
                    break;
                case (EMERALD_FONT_7):
                    uTextureIndex = 23;
                    break;
                case (EMERALD_FONT_8):
                    uTextureIndex = 24;
                    break;
                case (EMERALD_FONT_9):
                    uTextureIndex = 25;
                    break;
                case (EMERALD_FONT_DOUBLE_POINT):
                    uTextureIndex = 26;
                    break;
                case (EMERALD_FONT_PLATE):
                    uTextureIndex = 27;
                    break;
                case (EMERALD_FONT_ARROW_LEFT):
                    uTextureIndex = 28;
                    break;
                case (EMERALD_FONT_PAFF):
                    uTextureIndex = 29;
                    break;
                case (EMERALD_FONT_QUESTION_MARK):
                    uTextureIndex = 31;
                    break;
                case (EMERALD_FONT_A):
                    uTextureIndex = 33;
                    break;
                case (EMERALD_FONT_B):
                    uTextureIndex = 34;
                    break;
                case (EMERALD_FONT_C):
                    uTextureIndex = 35;
                    break;
                case (EMERALD_FONT_D):
                    uTextureIndex = 36;
                    break;
                case (EMERALD_FONT_E):
                    uTextureIndex = 37;
                    break;
                case (EMERALD_FONT_F):
                    uTextureIndex = 38;
                    break;
                case (EMERALD_FONT_G):
                    uTextureIndex = 39;
                    break;
                case (EMERALD_FONT_H):
                    uTextureIndex = 40;
                    break;
                case (EMERALD_FONT_I):
                    uTextureIndex = 41;
                    break;
                case (EMERALD_FONT_J):
                    uTextureIndex = 42;
                    break;
                case (EMERALD_FONT_K):
                    uTextureIndex = 43;
                    break;
                case (EMERALD_FONT_L):
                    uTextureIndex = 44;
                    break;
                case (EMERALD_FONT_M):
                    uTextureIndex = 45;
                    break;
                case (EMERALD_FONT_N):
                    uTextureIndex = 46;
                    break;
                case (EMERALD_FONT_O):
                    uTextureIndex = 47;
                    break;
                case (EMERALD_FONT_P):
                    uTextureIndex = 48;
                    break;
                case (EMERALD_FONT_Q):
                    uTextureIndex = 49;
                    break;
                case (EMERALD_FONT_R):
                    uTextureIndex = 50;
                    break;
                case (EMERALD_FONT_S):
                    uTextureIndex = 51;
                    break;
                case (EMERALD_FONT_T):
                    uTextureIndex = 52;
                    break;
                case (EMERALD_FONT_U):
                    uTextureIndex = 53;
                    break;
                case (EMERALD_FONT_V):
                    uTextureIndex = 54;
                    break;
                case (EMERALD_FONT_W):
                    uTextureIndex = 55;
                    break;
                case (EMERALD_FONT_X):
                    uTextureIndex = 56;
                    break;
                case (EMERALD_FONT_Y):
                    uTextureIndex = 57;
                    break;
                case (EMERALD_FONT_Z):
                    uTextureIndex = 58;
                    break;
                case (EMERALD_FONT_AE):
                    uTextureIndex = 65;
                    break;
                case (EMERALD_FONT_OE):
                    uTextureIndex = 66;
                    break;
                case (EMERALD_FONT_UE):
                    uTextureIndex = 67;
                    break;
                case (EMERALD_INVALID):     // invalides Element gefunden
                    // Es muss sich nur um Elemente gekümmert werden, die sich am Rand des sichtbaren Bereichs befinden
                    // und sich in den sichtbaren Bereich >hinein< bewegen
                    if (Playfield.pInvalidElement[I] != EMERALD_NONE) {   // Bei grünen Tropfen in Phase 1
                        uTextureIndex = GetTextureIndexByElement(Playfield.pInvalidElement[I],nAnimationCount,&fAngle);
                        if ((uY == 0) && ((Playfield.pStatusAnimation[I] & 0x00FF0000) == EMERALD_ANIM_CLEAN_UP)) {
                            // 1. Element befindet sich am oberen Rand und will nach unten in den sichtbaren Bereich
                            //SDL_Log("%s: invalid element at top found    Anim:%x   T:%u",__FUNCTION__,Playfield.pStatusAnimation[I],SDL_GetTicks());
                            nYoffs = -FONT_H + nAnimationCount * 2;
                        } else if ((uX == 0) && ((Playfield.pStatusAnimation[I] & 0x00FF0000) == EMERALD_ANIM_CLEAN_LEFT)) {
                            // 2. Element befindet sich am linken Rand und will nach rechts in den sichtbaren Bereich
                            //SDL_Log("%s: invalid element at left found    Anim:%x   T:%u",__FUNCTION__,Playfield.pStatusAnimation[I],SDL_GetTicks());
                            nXoffs = -FONT_H + nAnimationCount * 2;
                        } else if ((uX == (WINDOW_W / FONT_W)) && ((Playfield.pStatusAnimation[I] & 0x00FF0000) == EMERALD_ANIM_CLEAN_RIGHT)) {
                            // 3. Element befindet sich am rechten Rand und will nach links in den sichtbaren Bereich
                            //SDL_Log("%s: invalid element at right found    Anim:%x   T:%u",__FUNCTION__,Playfield.pStatusAnimation[I],SDL_GetTicks());
                            nXoffs = FONT_H - nAnimationCount * 2;
                        } else if ( (uY == ((WINDOW_H - PANEL_H) / FONT_H)) && ((Playfield.pStatusAnimation[I] & 0x00FF0000) == EMERALD_ANIM_CLEAN_DOWN)) {
                            // 4. Element befindet sich am unteren Rand und will nach oben in den sichtbaren Bereich
                            //SDL_Log("%s: invalid element at bottom found    Anim:%x   T:%u",__FUNCTION__,Playfield.pStatusAnimation[I],SDL_GetTicks());
                            nYoffs = FONT_H - nAnimationCount * 2;
                        } else {
                            uTextureIndex = 0;      // Space
                            fAngle = 0;
                        }
                    } else {
                        uTextureIndex = 0;      // Space
                        fAngle = 0;
                    }
                    break;
                default:
                    fAngle = 0;
                    uTextureIndex = 0;      // Space
                    SDL_Log("%s: default: unknown element[%d] at adress %u found",__FUNCTION__,uLevelElement,I);
                    break;
            }
            // Erweitertes Element
            if (bExtendedElement) {
                // Position innerhalb des Renderers
                DestR.x = uX * FONT_W - (*pnXpos % FONT_W) + nXoffs_0 + Playfield.uShiftLevelXpix;
                DestR.y = uY * FONT_H - (*pnYpos % FONT_H) + nYoffs_0 + Playfield.uShiftLevelYpix;
                DestR.w = FONT_W * fScaleW_0;
                DestR.h = FONT_H * fScaleH_0;
                if (SDL_RenderCopyEx(pRenderer,GetTextureByIndex(uTextureIndex_0),NULL,&DestR,fAngle_0 + fAngleOffs_0,NULL, Flip_0) != 0) {
                    SDL_Log("%s: SDL_RenderCopyEx(extended) failed: %s",__FUNCTION__,SDL_GetError());
                    nErrorCode = -1;
                }
            }
            // Standardelement
            if ((uLevelElement != EMERALD_SPACE) && (uLevelElement != EMERALD_BEAM_CROSS) && (nErrorCode == 0)) {   // Space muss nicht gerendert werden, da nach jedem Bild sowieso Alles gelöscht wird
                // Position innerhalb des Renderers
                DestR.x = uX * FONT_W - (*pnXpos % FONT_W) + nXoffs + Playfield.uShiftLevelXpix;
                DestR.y = uY * FONT_H - (*pnYpos % FONT_H) + nYoffs + Playfield.uShiftLevelYpix;
                DestR.w = FONT_W * fScaleW;
                DestR.h = FONT_H * fScaleH;
                if (SDL_RenderCopyEx(pRenderer,GetTextureByIndex(uTextureIndex),NULL,&DestR,fAngle + fAngleOffs,NULL, Flip) != 0) {
                    SDL_Log("%s: SDL_RenderCopyEx(standard) failed: %s",__FUNCTION__,SDL_GetError());
                    nErrorCode = -1;
                }
            }
        } // for (uX = 0; (uX <= (WINDOW_W / FONT_W)) && (uX < Playfield.uLevel_X_Dimension) && (nErrorCode == 0); uX++)
    } // for (uY = 0; (uY <= ((WINDOW_H - PANEL_H) / FONT_H)) && (uY < Playfield.uLevel_Y_Dimension) && (nErrorCode == 0); uY++)
    // Ab hier Post-Animationen. Diese sind keinem bestimmten Element zugeordnet.
    for (Y = 0; (Y < uPostAnimationIndex) & (nErrorCode == 0); Y++) {
        uTextureIndex = Playfield.pPostAnimation[Y].uTextureIndex;
        uX = Playfield.pPostAnimation[Y].uX;
        uY = Playfield.pPostAnimation[Y].uY;
        // Es gibt z.Z. noch keine unterschiedlichen Animationsformeln
        if (nAnimationCount < 15) {
            uTextureIndex = uTextureIndex + nAnimationCount;    // Animationsformel 0
        } else {
            uTextureIndex = 0;
        }
        nXoffs = 0;
        nYoffs = 0;
        fAngle = 0;
        fAngleOffs = 0;
        fScaleW = 1;
        fScaleH = 1;
        Flip = SDL_FLIP_NONE;
        if (Playfield.pPostAnimation[Y].bFlipHorizontal) {
            Flip = (SDL_RendererFlip)(Flip | SDL_FLIP_HORIZONTAL);
        }
        if (Playfield.pPostAnimation[Y].bFlipVertical) {
            Flip = (SDL_RendererFlip)(Flip | SDL_FLIP_VERTICAL);
        }
        // Position innerhalb des Renderers
        DestR.x = uX * FONT_W - (*pnXpos % FONT_W) + nXoffs + Playfield.uShiftLevelXpix;
        DestR.y = uY * FONT_H - (*pnYpos % FONT_H) + nYoffs + Playfield.uShiftLevelYpix;
        DestR.w = FONT_W * fScaleW;
        DestR.h = FONT_H * fScaleH;
        if (SDL_RenderCopyEx(pRenderer,GetTextureByIndex(uTextureIndex),NULL,&DestR,fAngle + fAngleOffs,NULL, Flip) != 0) {
            SDL_Log("%s: SDL_RenderCopyEx(post animation) failed: %s, TextureIndex: %u",__FUNCTION__,SDL_GetError(),uTextureIndex);
            nErrorCode = -1;
        }
    }
    if (nErrorCode == 0) {
        if (ShowPanel(pRenderer) != 0) {
            SDL_Log("%s: ShowPanel() failed",__FUNCTION__);
        }
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           GetTextureIndexByElement
------------------------------------------------------------------------------
Beschreibung: Holt den entsprechenden Texture-Index anhand eines Elements und des
              Animations-Schrittes
Parameter
      Eingang: uElement, uint16_t, Element, z.B. EMERALD_MINE_RIGHT
               nAnimationCount, int, Animationsschritt
      Ausgang: pfAngle, float *, Winkel
Rückgabewert:  uint32_t , Texture, wenn keine Texture ermittelt werden kann, wird
                SPACE (EMERALD_SPACE) zurückgegeben.
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
uint32_t GetTextureIndexByElement(uint16_t uElement,int nAnimationCount,float *pfAngle) {
    uint32_t uTextureIndex;
    uint32_t K;

    *pfAngle = 0;
    switch (uElement) {
        case (EMERALD_FONT_EXCLAMATION):
            uTextureIndex = 1;
            break;
        case (EMERALD_FONT_ARROW_RIGHT):
            uTextureIndex = 4;
            break;
        case (EMERALD_FONT_ARROW_UP):
            uTextureIndex = 5;
            break;
        case (EMERALD_FONT_ARROW_DOWN):
            uTextureIndex = 6;
            break;
        case (EMERALD_FONT_APOSTROPHE):
            uTextureIndex = 7;
            break;
        case (EMERALD_FONT_BRACE_OPEN):
            uTextureIndex = 8;
            break;
        case (EMERALD_FONT_BRACE_CLOSE):
            uTextureIndex = 9;
            break;
        case (EMERALD_FONT_COPYRIGHT):
            uTextureIndex = 10;
            break;
        case (EMERALD_FONT_PLUS):
            uTextureIndex = 11;
            break;
        case (EMERALD_FONT_COMMA):
            uTextureIndex = 12;
            break;
        case (EMERALD_FONT_MINUS):
            uTextureIndex = 13;
            break;
        case (EMERALD_FONT_POINT):
            uTextureIndex = 14;
            break;
        case (EMERALD_FONT_SLASH):
            uTextureIndex = 15;
            break;
        case (EMERALD_FONT_0):
            uTextureIndex = 16;
            break;
        case (EMERALD_FONT_1):
            uTextureIndex = 17;
            break;
        case (EMERALD_FONT_2):
            uTextureIndex = 18;
            break;
        case (EMERALD_FONT_3):
            uTextureIndex = 19;
            break;
        case (EMERALD_FONT_4):
            uTextureIndex = 20;
            break;
        case (EMERALD_FONT_5):
            uTextureIndex = 21;
            break;
        case (EMERALD_FONT_6):
            uTextureIndex = 22;
            break;
        case (EMERALD_FONT_7):
            uTextureIndex = 23;
            break;
        case (EMERALD_FONT_8):
            uTextureIndex = 24;
            break;
        case (EMERALD_FONT_9):
            uTextureIndex = 25;
            break;
        case (EMERALD_FONT_DOUBLE_POINT):
            uTextureIndex = 26;
            break;
        case (EMERALD_FONT_PLATE):
            uTextureIndex = 27;
            break;
        case (EMERALD_FONT_ARROW_LEFT):
            uTextureIndex = 28;
            break;
        case (EMERALD_FONT_PAFF):
            uTextureIndex = 29;
            break;
        case (EMERALD_FONT_QUESTION_MARK):
            uTextureIndex = 31;
            break;
        case (EMERALD_FONT_A):
            uTextureIndex = 33;
            break;
        case (EMERALD_FONT_B):
            uTextureIndex = 34;
            break;
        case (EMERALD_FONT_C):
            uTextureIndex = 35;
            break;
        case (EMERALD_FONT_D):
            uTextureIndex = 36;
            break;
        case (EMERALD_FONT_E):
            uTextureIndex = 37;
            break;
        case (EMERALD_FONT_F):
            uTextureIndex = 38;
            break;
        case (EMERALD_FONT_G):
            uTextureIndex = 39;
            break;
        case (EMERALD_FONT_H):
            uTextureIndex = 40;
            break;
        case (EMERALD_FONT_I):
            uTextureIndex = 41;
            break;
        case (EMERALD_FONT_J):
            uTextureIndex = 42;
            break;
        case (EMERALD_FONT_K):
            uTextureIndex = 43;
            break;
        case (EMERALD_FONT_L):
            uTextureIndex = 44;
            break;
        case (EMERALD_FONT_M):
            uTextureIndex = 45;
            break;
        case (EMERALD_FONT_N):
            uTextureIndex = 46;
            break;
        case (EMERALD_FONT_O):
            uTextureIndex = 47;
            break;
        case (EMERALD_FONT_P):
            uTextureIndex = 48;
            break;
        case (EMERALD_FONT_Q):
            uTextureIndex = 49;
            break;
        case (EMERALD_FONT_R):
            uTextureIndex = 50;
            break;
        case (EMERALD_FONT_S):
            uTextureIndex = 51;
            break;
        case (EMERALD_FONT_T):
            uTextureIndex = 52;
            break;
        case (EMERALD_FONT_U):
            uTextureIndex = 53;
            break;
        case (EMERALD_FONT_V):
            uTextureIndex = 54;
            break;
        case (EMERALD_FONT_W):
            uTextureIndex = 55;
            break;
        case (EMERALD_FONT_X):
            uTextureIndex = 56;
            break;
        case (EMERALD_FONT_Y):
            uTextureIndex = 57;
            break;
        case (EMERALD_FONT_Z):
            uTextureIndex = 58;
            break;
        case (EMERALD_FONT_AE):
            uTextureIndex = 65;
            break;
        case (EMERALD_FONT_OE):
            uTextureIndex = 66;
            break;
        case (EMERALD_FONT_UE):
            uTextureIndex = 67;
            break;
        case (EMERALD_DYNAMITE_ON):
            uTextureIndex = 555 + nAnimationCount / 4;
            break;
        case (EMERALD_SWAMP_STONE):
            uTextureIndex = 554;                // Für Editor
            break;
        case (EMERALD_WALL_WITH_CRYSTAL):
            uTextureIndex = 533;
            break;
        case (EMERALD_WALL_WITH_KEY_RED):
            uTextureIndex = 534;
            break;
        case (EMERALD_WALL_WITH_KEY_GREEN):
            uTextureIndex = 535;
            break;
        case (EMERALD_WALL_WITH_KEY_BLUE):
            uTextureIndex = 536;
            break;
        case (EMERALD_WALL_WITH_KEY_YELLOW):
            uTextureIndex = 537;
            break;
        case (EMERALD_WALL_WITH_KEY_WHITE):
            uTextureIndex = 538;
            break;
        case (EMERALD_WALL_WITH_KEY_GENERAL):
            uTextureIndex = 539;
            break;
        case (EMERALD_WALL_WITH_BOMB):
            uTextureIndex = 540;
            break;
        case (EMERALD_WALL_WITH_MEGABOMB):
            uTextureIndex = 541;
            break;
        case (EMERALD_WALL_WITH_STONE):
            uTextureIndex = 542;
            break;
        case (EMERALD_WALL_WITH_NUT):
            uTextureIndex = 543;
            break;
        case (EMERALD_WALL_WITH_WHEEL):
            uTextureIndex = 544;
            break;
        case (EMERALD_WALL_WITH_DYNAMITE):
            uTextureIndex = 545;
            break;
        case (EMERALD_WALL_WITH_ENDDOOR):
            uTextureIndex = 546;
            break;
        case (EMERALD_WALL_WITH_ENDDOOR_READY):
            uTextureIndex = 547;
            break;
        case (EMERALD_WALL_WITH_MINE_UP):
            uTextureIndex = 548;
            break;
        case (EMERALD_WALL_WITH_BEETLE_UP):
            uTextureIndex = 549;
            break;
        case (EMERALD_WALL_WITH_YAM):
            uTextureIndex = 550;
            break;
        case (EMERALD_WALL_WITH_ALIEN):
            uTextureIndex = 551;
            break;
        case (EMERALD_WALL_WITH_MOLE):
            uTextureIndex = 552;
            break;
        case (EMERALD_WALL_WITH_GREEN_CHEESE):
            uTextureIndex = 553;
            break;
        case (EMERALD_WALL_WITH_EMERALD):
            uTextureIndex = 529;
            break;
        case (EMERALD_WALL_WITH_RUBY):
            uTextureIndex = 530;
            break;
        case (EMERALD_WALL_WITH_SAPPHIRE):
            uTextureIndex = 531;
            break;
        case (EMERALD_WALL_WITH_PERL):
            uTextureIndex = 532;
            break;
        case (EMERALD_DOOR_WHITE):
            uTextureIndex = 521;
            break;
        case (EMERALD_DOOR_WHITE_WOOD):
            uTextureIndex = 522;
            break;
        case (EMERALD_DOOR_GREY_WHITE):
            uTextureIndex = 523;
            break;
        case (EMERALD_KEY_WHITE):
            uTextureIndex = 519;
            break;
        case (EMERALD_KEY_GENERAL):
            uTextureIndex = 520;
            break;
        case (EMERALD_SWITCHDOOR_OPEN):
            uTextureIndex = 506;
            break;
        case (EMERALD_SWITCHDOOR_CLOSED):
            uTextureIndex = 502;
            break;
        case (EMERALD_SWITCH_SWITCHDOOR):
            uTextureIndex = 507;
            break;
        case (EMERALD_DOOR_TIME):
            uTextureIndex = 501;
            break;
        case (EMERALD_WHEEL_TIMEDOOR):
            uTextureIndex = 493 + nAnimationCount / 4;
            break;
        case (EMERALD_DOOR_EMERALD):
            uTextureIndex = 487;
            break;
        case (EMERALD_DOOR_MULTICOLOR):
            uTextureIndex = 488;
            break;
        case (EMERALD_DOOR_ONLY_UP):
            uTextureIndex = 489;
            break;
        case (EMERALD_DOOR_ONLY_DOWN):
            uTextureIndex = 490;
            break;
        case (EMERALD_DOOR_ONLY_LEFT):
            uTextureIndex = 491;
            break;
        case (EMERALD_DOOR_ONLY_RIGHT):
            uTextureIndex = 492;
            break;
        case (EMERALD_STEEL_INVISIBLE):
            uTextureIndex = 484;
            break;
        case (EMERALD_WALL_INVISIBLE):
            uTextureIndex = 485;
            break;
        case (EMERALD_LIGHT_SWITCH):
            uTextureIndex = 486;
            break;
        case (EMERALD_STEEL_HEART):
            uTextureIndex = 474;
            break;
        case (EMERALD_STEEL_PLAYERHEAD):
            uTextureIndex = 475;
            break;
        case (EMERALD_STEEL_NO_ENTRY):
            uTextureIndex = 476;
            break;
        case (EMERALD_STEEL_GIVE_WAY):
            uTextureIndex = 477;
            break;
        case (EMERALD_STEEL_YING):
            uTextureIndex = 478;
            break;
        case (EMERALD_STEEL_WHEELCHAIR):
            uTextureIndex = 479;
            break;
        case (EMERALD_STEEL_ARROW_DOWN):
            uTextureIndex = 480;
            break;
        case (EMERALD_STEEL_ARROW_UP):
            uTextureIndex = 481;
            break;
        case (EMERALD_STEEL_ARROW_LEFT):
            uTextureIndex = 482;
            break;
        case (EMERALD_STEEL_ARROW_RIGHT):
            uTextureIndex = 483;
            break;
        case (EMERALD_STANDMINE):
            if ((((Playfield.uFrameCounter & 0xFFFFFFFC) >> 2) % 32) == 0) {
                uTextureIndex = 448;
            } else if ((((Playfield.uFrameCounter & 0xFFFFFFFC) >> 2) % 16) == 0) {
                uTextureIndex = 449;
            } else {
                uTextureIndex = 447;
            }
            break;
        case (EMERALD_MAGIC_WALL_SWITCH):
            uTextureIndex = 435;
            break;
        case (EMERALD_LIGHTBARRIER_RED_UP):
            if (Playfield.bLightBarrierRedOn) {
                uTextureIndex = 423;
            } else {
                uTextureIndex = 419;
            }
            break;
        case (EMERALD_LIGHTBARRIER_RED_DOWN):
            if (Playfield.bLightBarrierRedOn) {
                uTextureIndex = 424;
            } else {
                uTextureIndex = 420;
            }
            break;
        case (EMERALD_LIGHTBARRIER_RED_LEFT):
            if (Playfield.bLightBarrierRedOn) {
                uTextureIndex = 425;
            } else {
                uTextureIndex = 421;
            }
            break;
        case (EMERALD_LIGHTBARRIER_RED_RIGHT):
            if (Playfield.bLightBarrierRedOn) {
                uTextureIndex = 426;
            } else {
                uTextureIndex = 422;
            }
            break;
        case (EMERALD_LIGHTBARRIER_GREEN_UP):
            if (Playfield.bLightBarrierGreenOn) {
                uTextureIndex = 399;
            } else {
                uTextureIndex = 395;
            }
            break;
        case (EMERALD_LIGHTBARRIER_GREEN_DOWN):
            if (Playfield.bLightBarrierGreenOn) {
                uTextureIndex = 400;
            } else {
                uTextureIndex = 396;
            }
            break;
        case (EMERALD_LIGHTBARRIER_GREEN_LEFT):
            if (Playfield.bLightBarrierGreenOn) {
                uTextureIndex = 401;
            } else {
                uTextureIndex = 397;
            }
            break;
        case (EMERALD_LIGHTBARRIER_GREEN_RIGHT):
            if (Playfield.bLightBarrierGreenOn) {
                uTextureIndex = 402;
            } else {
                uTextureIndex = 398;
            }
            break;
        case (EMERALD_LIGHTBARRIER_BLUE_UP):
            if (Playfield.bLightBarrierBlueOn) {
                uTextureIndex = 407;
            } else {
                uTextureIndex = 403;
            }
            break;
        case (EMERALD_LIGHTBARRIER_BLUE_DOWN):
            if (Playfield.bLightBarrierBlueOn) {
                uTextureIndex = 408;
            } else {
                uTextureIndex = 404;
            }
            break;
        case (EMERALD_LIGHTBARRIER_BLUE_LEFT):
            if (Playfield.bLightBarrierBlueOn) {
                uTextureIndex = 409;
            } else {
                uTextureIndex = 405;
            }
            break;
        case (EMERALD_LIGHTBARRIER_BLUE_RIGHT):
            if (Playfield.bLightBarrierBlueOn) {
                uTextureIndex = 410;
            } else {
                uTextureIndex = 406;
            }
            break;
        case (EMERALD_LIGHTBARRIER_YELLOW_UP):
            if (Playfield.bLightBarrierYellowOn) {
                uTextureIndex = 415;
            } else {
                uTextureIndex = 411;
            }
            break;
        case (EMERALD_LIGHTBARRIER_YELLOW_DOWN):
            if (Playfield.bLightBarrierYellowOn) {
                uTextureIndex = 416;
            } else {
                uTextureIndex = 412;
            }
            break;
        case (EMERALD_LIGHTBARRIER_YELLOW_LEFT):
            if (Playfield.bLightBarrierYellowOn) {
                uTextureIndex = 417;
            } else {
                uTextureIndex = 413;
            }
            break;
        case (EMERALD_LIGHTBARRIER_YELLOW_RIGHT):
            if (Playfield.bLightBarrierYellowOn) {
                uTextureIndex = 418;
            } else {
                uTextureIndex = 414;
            }
            break;
        case (EMERALD_BEAM_RED_VERTICAL):
            uTextureIndex = 433;
            break;
        case (EMERALD_BEAM_RED_HORIZONTAL):
            uTextureIndex = 434;
            break;
        case (EMERALD_BEAM_GREEN_VERTICAL):
            uTextureIndex = 427;
            break;
        case (EMERALD_BEAM_GREEN_HORIZONTAL):
            uTextureIndex = 428;
            break;
        case (EMERALD_BEAM_BLUE_VERTICAL):
            uTextureIndex = 429;
            break;
        case (EMERALD_BEAM_BLUE_HORIZONTAL):
            uTextureIndex = 430;
            break;
        case (EMERALD_BEAM_YELLOW_VERTICAL):
            uTextureIndex = 431;
            break;
        case (EMERALD_BEAM_YELLOW_HORIZONTAL):
            uTextureIndex = 432;
            break;
        case (EMERALD_BEAM_CROSS):
            uTextureIndex = 0;
            break;
        case (EMERALD_LIGHTBARRIER_RED_SWITCH):
            uTextureIndex = 383;
            break;
        case (EMERALD_LIGHTBARRIER_GREEN_SWITCH):
            uTextureIndex = 389;
            break;
        case (EMERALD_LIGHTBARRIER_BLUE_SWITCH):
            uTextureIndex = 392;
            break;
        case (EMERALD_LIGHTBARRIER_YELLOW_SWITCH):
            uTextureIndex = 386;
            break;
        case (EMERALD_YAM):
            K = Playfield.uFrameCounter % 11;       // Y von 0 bis 10
            if (K <= 5) {                           // 0,1,2,3,4,5
                uTextureIndex = 362 + K;            // 362 - 367
            } else {                                // 6,7,8,9,10
                uTextureIndex = 367 + 5 - K;        // 366 - 362
            }
            break;
        case (EMERALD_MESSAGE_1):
        case (EMERALD_MESSAGE_2):
        case (EMERALD_MESSAGE_3):
        case (EMERALD_MESSAGE_4):
        case (EMERALD_MESSAGE_5):
        case (EMERALD_MESSAGE_6):
        case (EMERALD_MESSAGE_7):
        case (EMERALD_MESSAGE_8):
            uTextureIndex = 356;
            break;
        case (EMERALD_STEEL_MARKER_LEFT_UP):
            uTextureIndex = 348;
            break;
        case (EMERALD_STEEL_MARKER_UP):
            uTextureIndex = 349;
            break;
        case (EMERALD_STEEL_MARKER_RIGHT_UP):
            uTextureIndex = 350;
            break;
        case (EMERALD_STEEL_MARKER_LEFT):
            uTextureIndex = 351;
            break;
        case (EMERALD_STEEL_MARKER_RIGHT):
            uTextureIndex = 352;
            break;
        case (EMERALD_STEEL_MARKER_LEFT_BOTTOM):
            uTextureIndex = 353;
            break;
        case (EMERALD_STEEL_MARKER_BOTTOM):
            uTextureIndex = 354;
            break;
        case (EMERALD_STEEL_MARKER_RIGHT_BOTTOM):
            uTextureIndex = 355;
            break;
        case (EMERALD_GREEN_DROP):
            uTextureIndex = 346;
            break;
        case (EMERALD_GREEN_CHEESE):
            uTextureIndex = 343;
            break;
        case (EMERALD_STEEL_FORBIDDEN):
            uTextureIndex = 334;
            break;
        case (EMERALD_STEEL_EXIT):
            uTextureIndex = 332;
            break;
        case (EMERALD_STEEL_RADIOACTIVE):
            uTextureIndex = 337;
            break;
        case (EMERALD_STEEL_EXPLOSION):
            uTextureIndex = 333;
            break;
        case (EMERALD_STEEL_ACID):
            uTextureIndex = 329;
            break;
        case (EMERALD_STEEL_NOT_ROUND):
            uTextureIndex = 335;
            break;
        case (EMERALD_WALL_NOT_ROUND):
            uTextureIndex = 342;
            break;
        case (EMERALD_STEEL_PARKING):
            uTextureIndex = 336;
            break;
        case (EMERALD_STEEL_STOP):
            uTextureIndex = 340;
            break;
        case (EMERALD_STEEL_DEADEND):
            uTextureIndex = 331;
            break;
        case (EMERALD_STEEL_BIOHAZARD):
            uTextureIndex = 330;
            break;
        case (EMERALD_STEEL_WARNING):
            uTextureIndex = 341;
            break;
        case (EMERALD_STEEL_ROUND):
            uTextureIndex = 338;
            break;
        case (EMERALD_STEEL_ROUND_PIKE):
            uTextureIndex = 339;
            break;
        case (EMERALD_EMERALD):
            uTextureIndex = 226 + nAnimationCount / 2;     // Emerald, liegend
            break;
        case (EMERALD_RUBY):
            uTextureIndex = 301 + nAnimationCount / 2;     // Rubin, liegend
            break;
        case (EMERALD_CRYSTAL):
            uTextureIndex = 309;
            break;
        case (EMERALD_SAPPHIRE):
            uTextureIndex = 248 + ((Playfield.uFrameCounter & 0xFFFFFFFE) >> 1) % 9; // Saphir fallend
            break;
        case (EMERALD_TIME_COIN):
            uTextureIndex = 310 + ((Playfield.uFrameCounter & 0xFFFFFFF8) >> 3) % 6; // Zeitmünze, drehend
            break;
        case (EMERALD_HAMMER):
            uTextureIndex = 318;                            // Hammer
            break;
        case (EMERALD_DYNAMITE_OFF):
            uTextureIndex = 286;                            // Dynamit, aus
            break;
        case (EMERALD_NUT):
            uTextureIndex = 234;
            break;
        case (EMERALD_STONE):
            uTextureIndex = 71;
            break;
        case (EMERALD_ALIEN):
            if ((nAnimationCount >= 4) && (nAnimationCount <= 11)) {
                uTextureIndex = 135;                        // Alien geht 2, Flügel voll ausgebreitet
            } else {
                uTextureIndex = 136;                        // Alien geht 1
            }
            break;
        case (EMERALD_MOLE_UP):
            *pfAngle = 90;
             uTextureIndex = 450 + Playfield.uFrameCounter % 11;     // Mole links
            break;
        case (EMERALD_MOLE_RIGHT):
            *pfAngle = 180;
             uTextureIndex = 450 + Playfield.uFrameCounter % 11;     // Mole links
             break;
        case (EMERALD_MOLE_DOWN):
            *pfAngle = 270;
            uTextureIndex = 450 + Playfield.uFrameCounter % 11;     // Mole links
            break;
        case( EMERALD_MOLE_LEFT):
            *pfAngle = 0;
             uTextureIndex = 450 + Playfield.uFrameCounter % 11;     // Mole links
            break;
        case (EMERALD_MINE_UP):
            *pfAngle = 90;
            if ( ((nAnimationCount >= 0) && (nAnimationCount <= 3)) || ((nAnimationCount >= 8) && (nAnimationCount <=11)) ) {
                uTextureIndex = 73;     // Mine links
            } else {
                uTextureIndex = 74;     // Mine links an
            }
            break;
        case (EMERALD_MINE_RIGHT):
            *pfAngle = 180;
            if ( ((nAnimationCount >= 0) && (nAnimationCount <= 3)) || ((nAnimationCount >= 8) && (nAnimationCount <=11)) ) {
                uTextureIndex = 73;     // Mine links
            } else {
                uTextureIndex = 74;     // Mine links an
            }
            break;
        case (EMERALD_MINE_DOWN):
            *pfAngle = 270;
            if ( ((nAnimationCount >= 0) && (nAnimationCount <= 3)) || ((nAnimationCount >= 8) && (nAnimationCount <=11)) ) {
                uTextureIndex = 73;     // Mine links
            } else {
                uTextureIndex = 74;     // Mine links an
            }
            break;
        case( EMERALD_MINE_LEFT):
            *pfAngle = 0;
            if ( ((nAnimationCount >= 0) && (nAnimationCount <= 3)) || ((nAnimationCount >= 8) && (nAnimationCount <=11)) ) {
                uTextureIndex = 73;     // Mine links
            } else {
                uTextureIndex = 74;     // Mine links an
            }
            break;
        case (EMERALD_BEETLE_UP):
            *pfAngle = 90;
            uTextureIndex = 75 + nAnimationCount % 8;     // Käfer links
            break;
        case (EMERALD_BEETLE_RIGHT):
            *pfAngle = 180;
            uTextureIndex = 75 + nAnimationCount % 8;     // Käfer links
            break;
        case (EMERALD_BEETLE_DOWN):
            *pfAngle = 270;
            uTextureIndex = 75 + nAnimationCount % 8;     // Käfer links
            break;
        case (EMERALD_BEETLE_LEFT):
            *pfAngle = 0;
            uTextureIndex = 75 + nAnimationCount % 8;     // Käfer links
            break;
        case (EMERALD_SPACE):
            uTextureIndex = 0;      // Space
            break;
        case (EMERALD_STEEL):
            uTextureIndex = 72;     // Mauer hart
            break;
        case (EMERALD_KEY_RED):
            uTextureIndex = 98;     // roter Schlüssel
            break;
        case (EMERALD_KEY_YELLOW):
            uTextureIndex = 101;    // gelber Schlüssel
            break;
        case (EMERALD_KEY_BLUE):
            uTextureIndex = 100;    // blauer Schlüssel
            break;
        case (EMERALD_KEY_GREEN):
            uTextureIndex = 99;     // grüner Schlüssel
            break;
        case (EMERALD_DOOR_RED):
            uTextureIndex = 94;     // rote Tür
            break;
        case (EMERALD_DOOR_YELLOW):
            uTextureIndex = 97;     // gelbe Tür
            break;
        case (EMERALD_DOOR_BLUE):
            uTextureIndex = 96;     // blaue Tür
            break;
        case (EMERALD_DOOR_GREEN):
            uTextureIndex = 95;     // grüne Tür
            break;
        case (EMERALD_DOOR_RED_WOOD):     // rote Holztür
            uTextureIndex = 462;
            break;
        case (EMERALD_DOOR_YELLOW_WOOD):  // gelbe Holztür
            uTextureIndex = 465;
            break;
        case (EMERALD_DOOR_BLUE_WOOD):    // blaue Holztür
            uTextureIndex = 464;
            break;
        case (EMERALD_DOOR_GREEN_WOOD):   // grüne Holztür
            uTextureIndex = 463;
            break;
        case (EMERALD_MAN):
            uTextureIndex = 102;    // Man
            //uTextureIndex = 119 + nAnimationCount % 8;     // Man runter
            break;
        case (EMERALD_REPLICATOR_RED_TOP_LEFT):
            uTextureIndex = 138;
            break;
        case (EMERALD_REPLICATOR_RED_TOP_MID):
            uTextureIndex = 139;
            break;
        case (EMERALD_REPLICATOR_RED_TOP_RIGHT):
            uTextureIndex = 140;
            break;
        case (EMERALD_REPLICATOR_RED_BOTTOM_LEFT):
            uTextureIndex = 141;
            break;
        case (EMERALD_REPLICATOR_RED_BOTTOM_RIGHT):
            uTextureIndex = 142;
            break;
        case (EMERALD_REPLICATOR_RED_SWITCH):
            uTextureIndex = 154;
            break;
        case (EMERALD_REPLICATOR_YELLOW_TOP_LEFT):
            uTextureIndex = 190;
            break;
        case (EMERALD_REPLICATOR_YELLOW_TOP_MID):
            uTextureIndex = 191;
            break;
        case (EMERALD_REPLICATOR_YELLOW_TOP_RIGHT):
            uTextureIndex = 192;
            break;
        case (EMERALD_REPLICATOR_YELLOW_BOTTOM_LEFT):
            uTextureIndex = 193;
            break;
        case (EMERALD_REPLICATOR_YELLOW_BOTTOM_RIGHT):
            uTextureIndex = 194;
            break;
        case (EMERALD_REPLICATOR_YELLOW_SWITCH):
            uTextureIndex = 206;
            break;
        case (EMERALD_REPLICATOR_GREEN_TOP_LEFT):
            uTextureIndex = 172;
            break;
        case (EMERALD_REPLICATOR_GREEN_TOP_MID):
            uTextureIndex = 173;
            break;
        case (EMERALD_REPLICATOR_GREEN_TOP_RIGHT):
            uTextureIndex = 174;
            break;
        case (EMERALD_REPLICATOR_GREEN_BOTTOM_LEFT):
            uTextureIndex = 175;
            break;
        case (EMERALD_REPLICATOR_GREEN_BOTTOM_RIGHT):
            uTextureIndex = 176;
            break;
        case (EMERALD_REPLICATOR_GREEN_SWITCH):
            uTextureIndex = 188;
            break;
        case (EMERALD_REPLICATOR_BLUE_TOP_LEFT):
            uTextureIndex = 208;
            break;
        case (EMERALD_REPLICATOR_BLUE_TOP_MID):
            uTextureIndex = 209;
            break;
        case (EMERALD_REPLICATOR_BLUE_TOP_RIGHT):
            uTextureIndex = 210;
            break;
        case (EMERALD_REPLICATOR_BLUE_BOTTOM_LEFT):
            uTextureIndex = 211;
            break;
        case (EMERALD_REPLICATOR_BLUE_BOTTOM_RIGHT):
            uTextureIndex = 212;
            break;
        case (EMERALD_REPLICATOR_BLUE_SWITCH):
            uTextureIndex = 224;
            break;
        case (EMERALD_ACIDPOOL_TOP_LEFT):
            uTextureIndex = 257;
            break;
        case (EMERALD_ACIDPOOL_TOP_MID):
            uTextureIndex = 262 + (Playfield.uFrameCounter / 13) % 4;  // Säurebecken, oben mitte (aktives Feld), verlangsamte Animation (13 Frames für eine Animations-Phase)
            break;
        case (EMERALD_ACIDPOOL_TOP_RIGHT):
            uTextureIndex = 259;
            break;
        case (EMERALD_ACIDPOOL_BOTTOM_LEFT):
            uTextureIndex = 258;
            break;
        case (EMERALD_ACIDPOOL_BOTTOM_MID):
            uTextureIndex = 261;
            break;
        case (EMERALD_ACIDPOOL_BOTTOM_RIGHT):
            uTextureIndex = 260;
            break;
        case (EMERALD_SAND):
            uTextureIndex = 171;
            break;
        case (EMERALD_SWAMP):
            uTextureIndex = 266;
            break;
        case (EMERALD_PERL):
            uTextureIndex = 436;
            break;
        case (EMERALD_MEGABOMB):
            uTextureIndex = 524 + ((Playfield.uFrameCounter & 0xFFFFFFFC) >> 2) % 5;
            break;
        case (EMERALD_BOMB):
            uTextureIndex = 271;
            break;
        case (EMERALD_WHEEL):
            uTextureIndex = 137;
            *pfAngle = nAnimationCount * 11;
            break;
        case (EMERALD_DOOR_GREY_RED):            // Für Editor
            uTextureIndex = 289;
            break;
        case (EMERALD_DOOR_GREY_GREEN):         // Für Editor
            uTextureIndex = 290;
            break;
        case (EMERALD_DOOR_GREY_BLUE):         // Für Editor
            uTextureIndex = 291;
            break;
        case (EMERALD_DOOR_GREY_YELLOW):         // Für Editor
            uTextureIndex = 292;
            break;
        case (EMERALD_MAGIC_WALL_STEEL):
            uTextureIndex = 466 + nAnimationCount / 2;
            break;
        case (EMERALD_MAGIC_WALL):
            uTextureIndex = 293 + nAnimationCount / 2;
            break;
        case (EMERALD_WALL_CORNERED):
            uTextureIndex = 316;
            break;
        case (EMERALD_WALL_ROUND):
            uTextureIndex = 317;
            break;
        case (EMERALD_DOOR_END_NOT_READY):
            uTextureIndex = 287;
            break;
        case (EMERALD_DOOR_END_READY):
            uTextureIndex = 319 + ((Playfield.uFrameCounter & 0xFFFFFFF8) >> 3) % 8; // Endtür, blinkend
            break;
        case (EMERALD_DOOR_END_NOT_READY_STEEL):
            uTextureIndex = 509;
            break;
        case (EMERALD_DOOR_END_READY_STEEL):
            uTextureIndex = 510 + ((Playfield.uFrameCounter & 0xFFFFFFF8) >> 3) % 8; // Endtür, blinkend
            break;
        case (EMERALD_WALL_ROUND_PIKE):
            uTextureIndex = 328;
            break;
        default:
            SDL_Log("%s: unknown element: %x",__FUNCTION__,uElement);
            uTextureIndex = 0;     // Space
            break;
    }
    return uTextureIndex;
}


/*----------------------------------------------------------------------------
Name:           GetTextureIndexByElementForAcidPool
------------------------------------------------------------------------------
Beschreibung: Holt den entsprechenden Texture-Index anhand eines Elements und des
              Animations-Schrittes. Die Funktion ist speziell für Elemente, die
              ins Säurebecken fallen können.
Parameter
      Eingang: uElement, uint16_t, Element, z.B. EMERALD_MINE_RIGHT
               nAnimationCount, int, Animationsschritt
      Ausgang: pfAngle, float *, Winkel für Texture;
Rückgabewert:  uint32_t , Texture, wenn keine Texture ermittelt werden kann, wird
                SPACE (EMERALD_SPACE) zurückgegeben.
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
uint32_t GetTextureIndexByElementForAcidPool(uint16_t uElement,int nAnimationCount, float *pfAngle) {
    uint32_t uTextureIndex;
    uint32_t K;
    float fAngle = 0;

    switch (uElement) {
        case (EMERALD_GREEN_DROP):
            uTextureIndex = 346;
            break;
        case (EMERALD_EMERALD):
            uTextureIndex = 226 + nAnimationCount / 2;     // Emerald, liegend
            break;
        case (EMERALD_RUBY):
            uTextureIndex = 301 + nAnimationCount / 2;     // Rubin, liegend
            break;
        case (EMERALD_CRYSTAL):
            uTextureIndex = 309;                            // Kristall, liegend
            break;
        case (EMERALD_SAPPHIRE):
            uTextureIndex = 248 + ((Playfield.uFrameCounter & 0xFFFFFFFE) >> 1) % 9; // Saphir fallend
            break;
        case (EMERALD_NUT):
            uTextureIndex = 234;
            break;
        case (EMERALD_STONE):
            uTextureIndex = 71;
            break;
        case (EMERALD_YAM):
            K = Playfield.uFrameCounter % 11;       // Y von 0 bis 10
            if (K <= 5) {                           // 0,1,2,3,4,5
                uTextureIndex = 362 + K;            // 362 - 367
            } else {                                // 6,7,8,9,10
                uTextureIndex = 367 + 5 - K;        // 366 - 362
            }
            break;
        case (EMERALD_ALIEN):
            if ((nAnimationCount >= 4) && (nAnimationCount <= 11)) {
                uTextureIndex = 135;                        // Alien geht 2, Flügel voll ausgebreitet
            } else {
                uTextureIndex = 136;                        // Alien geht 1
            }
            break;
        case (EMERALD_MOLE_UP):
        case (EMERALD_MOLE_RIGHT):
        case (EMERALD_MOLE_DOWN):
        case( EMERALD_MOLE_LEFT):
            fAngle = 270;
            uTextureIndex = 450 + Playfield.uFrameCounter % 11;     // Mole links
            break;
        case (EMERALD_MINE_UP):
        case (EMERALD_MINE_RIGHT):
        case (EMERALD_MINE_DOWN):
        case( EMERALD_MINE_LEFT):
            fAngle = 270;
            if ( ((nAnimationCount >= 0) && (nAnimationCount <= 3)) || ((nAnimationCount >= 8) && (nAnimationCount <=11)) ) {
                uTextureIndex = 73;     // Mine links
            } else {
                uTextureIndex = 74;     // Mine links an
            }
            break;
        case (EMERALD_BEETLE_UP):
        case (EMERALD_BEETLE_RIGHT):
        case (EMERALD_BEETLE_DOWN):
        case (EMERALD_BEETLE_LEFT):
            uTextureIndex = 75 + nAnimationCount % 8;     // Käfer links
            fAngle = 270;
            break;
        case (EMERALD_MAN):
            uTextureIndex = 119 + nAnimationCount % 8;     // Man runter
            break;
        case (EMERALD_BOMB):
            uTextureIndex = 271 + nAnimationCount % 8;
            break;
        case (EMERALD_PERL):
            uTextureIndex = 436 + nAnimationCount % 8;
            break;
        default:
            SDL_Log("%s: unknown element: %x",__FUNCTION__,uElement);
            uTextureIndex = 0;     // Space
            break;
    }
    *pfAngle = fAngle;
    return uTextureIndex;
}


/*----------------------------------------------------------------------------
Name:           InitRollUnderground
------------------------------------------------------------------------------
Beschreibung: Initialisiert das Array Playfield.uRollUnderground[256]. Hier ist für
              maximal 65536 Elemente (Element-Index, z.B. EMERALD_SAPPHIRE) abgelegt,
              ob Elemente von einem anderen Element herunterrollen.
              Bit 0 = Emerald, Bit 1 = Saphir, Bit 2 = Stone, Bit 3 = Nut, Bit 4 = Bomb, Bit 5 = Rubin, Bit 6 = Kristall, Bit 7 = Perle, Bit 8 = Megabombe
              Beispiel: Playfield.uRollUnderground[EMERALD_KEY_GREEN] = 0x03.
              -> Emerald (Bit 1) und Saphir (Bit 2) rollt von grünem Schlüssel
              Funktion wird 1-malig zu Programmbeginn aufgerufen.
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void InitRollUnderground(void) {
    memset(Playfield.uRollUnderground,0,sizeof(Playfield.uRollUnderground));
    Playfield.uRollUnderground[EMERALD_STONE] = 0x1FF;
    Playfield.uRollUnderground[EMERALD_SAPPHIRE] = 0x1FF;
    Playfield.uRollUnderground[EMERALD_BOMB] = 0x1FF;
    Playfield.uRollUnderground[EMERALD_MEGABOMB] = 0x1FF;
    Playfield.uRollUnderground[EMERALD_EMERALD] = 0x1FF;
    Playfield.uRollUnderground[EMERALD_RUBY] = 0x1FF;
    Playfield.uRollUnderground[EMERALD_PERL] = 0x1FF;
    Playfield.uRollUnderground[EMERALD_CRYSTAL] = 0x1FF;
    Playfield.uRollUnderground[EMERALD_NUT] = 0x1FF;                    // Nut rollt bei DC3 nicht von Schlüssel
    Playfield.uRollUnderground[EMERALD_WALL_ROUND] = 0x1FF;
    Playfield.uRollUnderground[EMERALD_WALL_ROUND_PIKE] = 0x1FF;
    Playfield.uRollUnderground[EMERALD_STEEL_ROUND_PIKE] = 0x1FF;
    Playfield.uRollUnderground[EMERALD_STEEL] = 0xEB;                   // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_WALL_CORNERED] = 0xEB;           // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_KEY_RED] = 0x1FF;                // nicht bei DC3
    Playfield.uRollUnderground[EMERALD_KEY_YELLOW] = 0x1FF;             // nicht bei DC3
    Playfield.uRollUnderground[EMERALD_KEY_BLUE] = 0x1FF;               // nicht bei DC3
    Playfield.uRollUnderground[EMERALD_KEY_GREEN] = 0x1FF;              // nicht bei DC3
    Playfield.uRollUnderground[EMERALD_WHEEL] = 0xFF;
    Playfield.uRollUnderground[EMERALD_ACIDPOOL_TOP_LEFT] = 0x1FF;      // nicht bei DC3
    Playfield.uRollUnderground[EMERALD_ACIDPOOL_TOP_RIGHT] = 0x1FF;     // nicht bei DC3
    Playfield.uRollUnderground[EMERALD_DOOR_RED] = 0x1FF;
    Playfield.uRollUnderground[EMERALD_DOOR_YELLOW] = 0x1FF;
    Playfield.uRollUnderground[EMERALD_DOOR_BLUE] = 0x1FF;
    Playfield.uRollUnderground[EMERALD_DOOR_GREEN] = 0x1FF;
    Playfield.uRollUnderground[EMERALD_STEEL_ROUND] = 0x1FF;
    Playfield.uRollUnderground[EMERALD_STEEL_WARNING] = 0xEB;           // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_STEEL_BIOHAZARD] = 0xEB;         // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_STEEL_DEADEND] = 0xEB;           // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_STEEL_STOP] = 0xEB;              // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_STEEL_PARKING] = 0xEB;           // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_STEEL_FORBIDDEN] = 0xEB;         // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_STEEL_EXIT] = 0xEB;              // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_STEEL_RADIOACTIVE] = 0xEB;       // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_STEEL_EXPLOSION] = 0xEB;         // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_STEEL_ACID] = 0xEB;              // Nur Steine und Bomben rollen hier nicht herunter
}


/*----------------------------------------------------------------------------
Name:           GetFreeRollDirections
------------------------------------------------------------------------------
Beschreibung: Prüft freie Roll-Richtungen
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
Rückgabewert:  uint8_t, 0 = kann nicht rollen, 1 = kann links rollen, 2 = kann rechts rollen, 3 = kann links und rechts rollen
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
uint8_t GetFreeRollDirections(uint32_t I) {
    uint8_t uFreeDirs = 0;

    // Kann Element links rollen?
    if ( (Playfield.pLevel[I - 1] == EMERALD_SPACE) &&
         ((Playfield.pLevel[I + Playfield.uLevel_X_Dimension - 1] == EMERALD_SPACE) || (Playfield.pLevel[I + Playfield.uLevel_X_Dimension - 1] == EMERALD_ACIDPOOL)) ) {
        uFreeDirs = uFreeDirs | 0x01;
    }
    // Kann Element rechts rollen?
    if ( (Playfield.pLevel[I + 1] == EMERALD_SPACE) &&
        ((Playfield.pLevel[I + Playfield.uLevel_X_Dimension + 1] == EMERALD_SPACE) || (Playfield.pLevel[I + Playfield.uLevel_X_Dimension + 1] == EMERALD_ACIDPOOL)) ) {
        uFreeDirs = uFreeDirs | 0x02;
    }
    return uFreeDirs;
}


/*----------------------------------------------------------------------------
Name:           GetTextureIndexByShrink
------------------------------------------------------------------------------
Beschreibung: Ermittelt anhand einer "Shrink-Animation" den Texture-Index
Parameter
      Eingang: uShrinkAnimation, uint32_t, Shrink-Animation, z.B.: EMERALD_ANIM_KEY_RED_SHRINK
Rückgabewert:  uint32_t, TextureIndex, 0 = Space = nicht verfügbar
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
uint32_t GetTextureIndexByShrink(uint32_t uShrinkAnimation) {
    uint32_t uTextureIndex;

    switch (uShrinkAnimation) {
        case (EMERALD_ANIM_MESSAGE_SHRINK):
            uTextureIndex = 356;
            break;
        case (EMERALD_ANIM_KEY_RED_SHRINK):
            uTextureIndex = 98;
            break;
        case (EMERALD_ANIM_KEY_GREEN_SHRINK):
            uTextureIndex = 99;
            break;
        case (EMERALD_ANIM_KEY_BLUE_SHRINK):
            uTextureIndex = 100;
            break;
        case (EMERALD_ANIM_KEY_YELLOW_SHRINK):
            uTextureIndex = 101;
            break;
        case (EMERALD_ANIM_KEY_WHITE_SHRINK):
            uTextureIndex = 519;
            break;
        case (EMERALD_ANIM_KEY_GENERAL_SHRINK):
            uTextureIndex = 520;
            break;
        case (EMERALD_ANIM_SAND_SHRINK):
            uTextureIndex = 156;
            break;
        case (EMERALD_ANIM_EMERALD_SHRINK):
            uTextureIndex = 226;
            break;
        case (EMERALD_ANIM_RUBY_SHRINK):
            uTextureIndex = 301;
            break;
        case (EMERALD_ANIM_PERL_SHRINK):
            uTextureIndex = 436;
            break;
        case (EMERALD_ANIM_CRYSTAL_SHRINK):
            uTextureIndex = 309;
            break;
        case (EMERALD_ANIM_SAPPHIRE_SHRINK):
            uTextureIndex = 248;
            break;
        case (EMERALD_ANIM_TIME_COIN_SHRINK):
            uTextureIndex = 310;
            break;
        case (EMERALD_ANIM_HAMMER_SHRINK):
            uTextureIndex = 318;
            break;
        case (EMERALD_ANIM_DYNAMITE_SHRINK):
            uTextureIndex = 286;
            break;
        default:
            uTextureIndex = 0; // Space
            SDL_Log("%s: Animation (%d) not found",__FUNCTION__,uShrinkAnimation);
            break;
    }
    return uTextureIndex;
}


/*----------------------------------------------------------------------------
Name:           ControlPreElements
------------------------------------------------------------------------------
Beschreibung: Erzeugt ggf. Spaces für Elemente, die sich auflösen.
              Molen-Sand wird in normalen Sand gewandelt.
              Schaltet Schalttüren ggf. um.
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlPreElements(void) {
    uint32_t I;

    for (I = 0; I < Playfield.uLevel_X_Dimension * Playfield.uLevel_Y_Dimension; I++) {
        if ( (Playfield.pLevel[I] == EMERALD_GREEN_CHEESE_GOES) ||
             ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_SINK_IN_MAGIC_WALL) ||   // Für Elemente, die im Magic Wall eintauchen
             (Playfield.pStatusAnimation[I] == EMERALD_ANIM_PERL_BREAK) ) {                         // Zerbrechende Perle
           Playfield.pLevel[I] = EMERALD_SPACE;           // Enstehenden Tropfen in Tropfen wandeln.
           Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
        } else if (Playfield.pLevel[I] == EMERALD_SAND_MOLE) {
            Playfield.pLevel[I] = EMERALD_SAND;     // Molen-Sand in normalen Sand wandeln
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
        } else if ((Playfield.pLevel[I] == EMERALD_SWITCHDOOR_CLOSED) && (Playfield.pStatusAnimation[I] == EMERALD_ANIM_DOOR_OPEN)) {
            Playfield.pLevel[I] = EMERALD_SWITCHDOOR_OPEN;
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
        } else if ((Playfield.pLevel[I] == EMERALD_SWITCHDOOR_OPEN) && (Playfield.pStatusAnimation[I] == EMERALD_ANIM_DOOR_CLOSE)) {
            Playfield.pLevel[I] = EMERALD_SWITCHDOOR_CLOSED;
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
        } else if ((Playfield.pLevel[I] == EMERALD_EXPLOSION_PHASE_1) || (Playfield.pLevel[I] == EMERALD_EXPLOSION_PHASE_2)) {
            ControlExplosion(I);
        }
    }
}


/*----------------------------------------------------------------------------
Name:           ControlTimeDoor
------------------------------------------------------------------------------
Beschreibung: Steuert die Zeit-Tür.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlTimeDoor(uint32_t I) {
    bool bActDoorOpenState;     // false = geschlossen, true = geöffnet

    Playfield.pStatusAnimation[I] &= 0x00FFFFFF;    // Selbststeuernden Animationsstatus zurücksetzen

    bActDoorOpenState = ((Playfield.pStatusAnimation[I] & 0xFF00) == EMERALD_STATUS_DOOR_OPEN);
    if (Playfield.bTimeDoorOpen != bActDoorOpenState) {
        if (Playfield.bTimeDoorOpen) {
            SDL_Log("%s: Time door opens",__FUNCTION__);
            Playfield.pStatusAnimation[I] = EMERALD_STATUS_DOOR_OPEN | EMERALD_ANIM_DOOR_OPEN;
            PreparePlaySound(SOUND_DOOR_OPEN_CLOSE,I);
        } else {
            SDL_Log("%s: Time door closes",__FUNCTION__);
            Playfield.pStatusAnimation[I] = EMERALD_STATUS_DOOR_CLOSE |EMERALD_ANIM_DOOR_CLOSE;
            PreparePlaySound(SOUND_DOOR_OPEN_CLOSE,I);
        }
    }
}


/*----------------------------------------------------------------------------
Name:           PostControlSwitchDoors
------------------------------------------------------------------------------
Beschreibung: Steuert die Schalttüren unmittelbar nach der Haupt-Steuerungsrunde, wenn
              der Man einen Schalt-Impuls gegeben hat.
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void PostControlSwitchDoors(void) {
    uint32_t I;

    if (Playfield.bSwitchDoorImpluse) {
        for (I = 0; I < Playfield.uLevel_X_Dimension * Playfield.uLevel_Y_Dimension; I++) {
            if (Playfield.pLevel[I] == EMERALD_SWITCHDOOR_CLOSED) {
                Playfield.pStatusAnimation[I] = EMERALD_ANIM_DOOR_OPEN;     // Öffnen der Schalttür einleiten
                PreparePlaySound(SOUND_DOOR_OPEN_CLOSE,I);
            } else if (Playfield.pLevel[I] == EMERALD_SWITCHDOOR_OPEN) {
                Playfield.pStatusAnimation[I] = EMERALD_ANIM_DOOR_CLOSE;    // Schließen der Schalttür einleiten
                PreparePlaySound(SOUND_DOOR_OPEN_CLOSE,I);
            }
        }
        Playfield.bSwitchDoorImpluse = false;
    }
}

/*----------------------------------------------------------------------------
Name:           IsSteel
------------------------------------------------------------------------------
Beschreibung: Prüft, ob ein Element Stahleigenschaften hat.
Parameter
      Eingang: uElement, uint16_t, Element, das geprüft wird
      Ausgang: -
Rückgabewert:  bool, true = Element gehört zum Stahl.
Seiteneffekte: -
------------------------------------------------------------------------------*/
bool IsSteel(uint16_t uElement) {

    return ((uElement == EMERALD_STEEL) ||
    (uElement == EMERALD_STEEL_ROUND_PIKE) ||
    (uElement == EMERALD_STEEL_ROUND) ||
    (uElement == EMERALD_STEEL_WARNING) ||
    (uElement == EMERALD_STEEL_BIOHAZARD) ||
    (uElement == EMERALD_STEEL_DEADEND) ||
    (uElement == EMERALD_STEEL_STOP) ||
    (uElement == EMERALD_STEEL_PARKING) ||
    (uElement == EMERALD_STEEL_FORBIDDEN) ||
    (uElement == EMERALD_STEEL_EXIT) ||
    (uElement == EMERALD_STEEL_RADIOACTIVE) ||
    (uElement == EMERALD_STEEL_EXPLOSION) ||
    (uElement == EMERALD_STEEL_ACID) ||
    (uElement == EMERALD_STEEL_NOT_ROUND) ||
    (uElement == EMERALD_STEEL_MARKER_LEFT_UP) ||
    (uElement == EMERALD_STEEL_MARKER_UP) ||
    (uElement == EMERALD_STEEL_MARKER_RIGHT_UP) ||
    (uElement == EMERALD_STEEL_MARKER_LEFT) ||
    (uElement == EMERALD_STEEL_MARKER_RIGHT) ||
    (uElement == EMERALD_STEEL_MARKER_LEFT_BOTTOM) ||
    (uElement == EMERALD_STEEL_MARKER_BOTTOM) ||
    (uElement == EMERALD_STEEL_MARKER_RIGHT_BOTTOM) ||
    (uElement == EMERALD_STEEL_HEART) ||
    (uElement == EMERALD_STEEL_PLAYERHEAD) ||
    (uElement == EMERALD_STEEL_NO_ENTRY) ||
    (uElement == EMERALD_STEEL_GIVE_WAY) ||
    (uElement == EMERALD_STEEL_YING) ||
    (uElement == EMERALD_STEEL_WHEELCHAIR) ||
    (uElement == EMERALD_STEEL_ARROW_DOWN) ||
    (uElement == EMERALD_STEEL_ARROW_UP) ||
    (uElement == EMERALD_STEEL_ARROW_LEFT) ||
    (uElement == EMERALD_STEEL_ARROW_RIGHT) ||
    (uElement == EMERALD_STEEL_INVISIBLE));
}
