#include <SDL2/SDL.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include "config.h"
#include "editor.h"
#include "EmeraldMine.h"
#include "EmeraldMineMainMenu.h"
#include "ezxml.h"
#include "highscores.h"
#include "loadlevel.h"
#include "miniz.h"
#include "md5.h"
#include "mySDL.h"
#include "mystd.h"
#include "teleporter.h"



extern PLAYFIELD Playfield;
extern SDL_DisplayMode ge_DisplayMode;
extern CONFIG Config;

uint32_t g_LevelgroupFilesCount;
LEVELGROUPFILE LevelgroupFiles[EMERALD_MAX_LEVELGROUPFILES];
LEVELGROUP SelectedLevelgroup;

NAMES Names;
ACTUALPLAYER Actualplayer;


/*----------------------------------------------------------------------------
Name:           GetLeveldimensionFromXml
------------------------------------------------------------------------------
Beschreibung: Ermittelt die X- und Y-Dimension aus der XML-Leveldatei.
Parameter
      Eingang: xml, ezxml_t, gültiges XML-Handle
      Ausgang: puX, uint32_t *, X-Level-Dimension
               puY, uint32_t *, Y-Level-Dimension
Rückgabewert:  int , 0 = OK, sonst Fehler
Seiteneffekte: -
------------------------------------------------------------------------------*/
int GetLeveldimensionFromXml(ezxml_t xml,uint32_t *puX,uint32_t *puY) {
    int nErrorCode;
    long int nX;
    long int nY;
    ezxml_t leveldimension,x,y;

    nErrorCode = -1;
    if ((xml != NULL) && (puX != NULL) && (puY != NULL)) {
        leveldimension = ezxml_child(xml,"leveldimension");
        if (leveldimension != NULL) {
            x = ezxml_child(leveldimension,"x");
            y = ezxml_child(leveldimension,"y");
            if ((x != NULL) && (y != NULL)) {
                nX = strtol(x->txt,NULL,10);
                nY = strtol(y->txt,NULL,10);
                if ((nX >= MIN_LEVEL_W) && (nX <= MAX_LEVEL_W) && (nY >= MIN_LEVEL_H) && (nY <= MAX_LEVEL_H)) {
                    *puX = (uint32_t)nX;
                    *puY = (uint32_t)nY;
                    nErrorCode = 0;
                } else {
                    SDL_Log("%s: bad level dimension, x = %d, y = %d",__FUNCTION__,(int)nX,(int)nY);
                }
            } else {
                SDL_Log("%s: error in xml file, 'x' or 'y' not found",__FUNCTION__);
            }
        } else {
            SDL_Log("%s: error in xml file, 'leveldimension' not found",__FUNCTION__);
        }
    } else {
        SDL_Log("%s: bad xml handle, null pointer",__FUNCTION__);
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           GetMemoryForPlayfield
------------------------------------------------------------------------------
Beschreibung: Erzeugt die Speicherbereiche für das Spielfeld.
              Hinweis: Die Level-Dimension (X/Y) muss bereits ermittelt worden sein.
              Speicherbedarf S = X * Y * 2 * 2 * 4 * 8 = X * Y * 64
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  int , 0 = OK, sonst Fehler
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
int GetMemoryForPlayfield(void) {
    int nErrorCode;

    Playfield.pLevel = (uint16_t*)malloc(Playfield.uLevel_X_Dimension * Playfield.uLevel_Y_Dimension * sizeof(uint16_t));
    Playfield.pInvalidElement = (uint16_t*)malloc(Playfield.uLevel_X_Dimension * Playfield.uLevel_Y_Dimension * sizeof(uint16_t));
    Playfield.pStatusAnimation = (uint32_t*)malloc(Playfield.uLevel_X_Dimension * Playfield.uLevel_Y_Dimension * sizeof(uint32_t));
    Playfield.pLastStatusAnimation = (uint32_t*)malloc(Playfield.uLevel_X_Dimension * Playfield.uLevel_Y_Dimension * sizeof(uint32_t));
    Playfield.pPostAnimation = (POSTANIMATION*)malloc(Playfield.uLevel_X_Dimension * Playfield.uLevel_Y_Dimension * sizeof(POSTANIMATION));
    Playfield.pLastYamDirection = (uint8_t*)malloc(Playfield.uLevel_X_Dimension * Playfield.uLevel_Y_Dimension);
    if ((Playfield.pLevel != NULL) && (Playfield.pStatusAnimation != NULL) && (Playfield.pLastStatusAnimation != NULL) && (Playfield.pPostAnimation != NULL) && (Playfield.pInvalidElement != NULL) && (Playfield.pLastYamDirection != NULL)) {
        memset(Playfield.pLevel,0,Playfield.uLevel_X_Dimension * Playfield.uLevel_Y_Dimension * sizeof(uint16_t));
        memset(Playfield.pInvalidElement,0,Playfield.uLevel_X_Dimension * Playfield.uLevel_Y_Dimension * sizeof(uint16_t));
        memset(Playfield.pStatusAnimation,0,Playfield.uLevel_X_Dimension * Playfield.uLevel_Y_Dimension * sizeof(uint32_t));
        memset(Playfield.pLastStatusAnimation,0,Playfield.uLevel_X_Dimension * Playfield.uLevel_Y_Dimension * sizeof(uint32_t));
        memset(Playfield.pPostAnimation,0,Playfield.uLevel_X_Dimension * Playfield.uLevel_Y_Dimension * sizeof(POSTANIMATION));
        memset(Playfield.pLastYamDirection,0,Playfield.uLevel_X_Dimension * Playfield.uLevel_Y_Dimension);
        nErrorCode = 0;
    } else {
        nErrorCode = -1;
        SDL_Log("%s: can not allocate memory for playfield.",__FUNCTION__);
        SAFE_FREE(Playfield.pLevel);
        SAFE_FREE(Playfield.pInvalidElement);
        SAFE_FREE(Playfield.pStatusAnimation);
        SAFE_FREE(Playfield.pLastStatusAnimation);
        SAFE_FREE(Playfield.pPostAnimation);
        SAFE_FREE(Playfield.pLastYamDirection);
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           GetTitleAuthorVersionHashFromXml
------------------------------------------------------------------------------
Beschreibung: Ermittelt den Level-Titel, Level-Autor, die Level-Version und den
              MD5-Hash und legt diese Informationen in der Struktur Playfield.x ab.
Parameter
      Eingang: xml, ezxml_t, gültiges XML-Handle
      Ausgang: -
               -
Rückgabewert:  int , 0 = OK, sonst Fehler
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
int GetTitleAuthorVersionHashFromXml(ezxml_t xml) {
    int nErrorCode;
    uint32_t uLen;
    ezxml_t node;
    char *pAttr;

    nErrorCode = -1;
    if (xml != NULL) {
        node = ezxml_child(xml,"title");
        if (node != NULL) {
            pAttr = node->txt;
            uLen = (uint32_t)strlen(pAttr);
            if (uLen == 0) {
                strcpy(Playfield.szLevelTitle,"UNKNOWN TITLE");
            } else if (uLen > EMERALD_TITLE_LEN) {
                memcpy(Playfield.szLevelTitle,pAttr,EMERALD_TITLE_LEN);
            } else {
                strcpy(Playfield.szLevelTitle,pAttr);
            }
            node = ezxml_child(xml,"author");
            if (node != NULL) {
                pAttr = node->txt;
                uLen = (uint32_t)strlen(pAttr);
                if (uLen == 0) {
                    strcpy(Playfield.szLevelAuthor,"UNKNOWN AUTHOR");
                } else if (uLen > EMERALD_AUTHOR_LEN) {
                    memcpy(Playfield.szLevelAuthor,pAttr,EMERALD_AUTHOR_LEN);
                } else {
                    strcpy(Playfield.szLevelAuthor,pAttr);
                }
                node = ezxml_child(xml,"version");
                if (node != NULL) {
                    pAttr = node->txt;
                    uLen = (uint32_t)strlen(pAttr);
                    if ((uLen == 0) || (uLen > EMERALD_VERSION_LEN)) {
                        strcpy(Playfield.szVersion,"00.00");
                    } else {
                        strcpy(Playfield.szVersion,pAttr);
                    }
                    node = ezxml_child(xml,"leveldata_md5_hash");
                    if (node != NULL) {
                        pAttr = node->txt;
                        uLen = (uint32_t)strlen(pAttr);
                        if (uLen == 32) {   // MD5-Hash muss immer 32 Zeichen lang sein
                            strcpy(Playfield.szMd5String,pAttr);
                            nErrorCode = 0;
                        } else {
                            SDL_Log("%s: error in xml file, invalid md5 length: %d",__FUNCTION__,uLen);
                        }
                    } else {
                        SDL_Log("%s: error in xml file, 'leveldata_md5_hash' not found",__FUNCTION__);
                    }
                } else {
                    SDL_Log("%s: error in xml file, 'version' not found",__FUNCTION__);
                }
            } else {
                SDL_Log("%s: error in xml file, 'author' not found",__FUNCTION__);
            }
        } else {
            SDL_Log("%s: error in xml file, 'title' not found",__FUNCTION__);
        }
    } else {
        SDL_Log("%s: bad xml handle, null pointer",__FUNCTION__);
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           GetLevelScoresFromXml
------------------------------------------------------------------------------
Beschreibung: Ermittelt alle Level-Scores und trägt diese in die Struktur Playfield.x ein.
Parameter
      Eingang: xml, ezxml_t, gültiges XML-Handle
      Ausgang: -
               -
Rückgabewert:  int , 0 = OK, sonst Fehler
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
int GetLevelScoresFromXml(ezxml_t xml) {
    int nErrorCode;
    ezxml_t scores,node;
    long int nNum;
    char *pAttr;

    nErrorCode = -1;
    if (xml != NULL) {
        scores = ezxml_child(xml,"scores");
        if (scores != NULL) {
            node = ezxml_child(scores,"emerald");
            if (node != NULL) {
                pAttr = node->txt;
                nNum = strtol(pAttr,NULL,10);
                Playfield.uScoreEmerald = (uint32_t)nNum;
                node = ezxml_child(scores,"ruby");
                if (node != NULL) {
                    pAttr = node->txt;
                    nNum = strtol(pAttr,NULL,10);
                    Playfield.uScoreRuby = (uint32_t)nNum;
                    node = ezxml_child(scores,"sapphire");
                    if (node != NULL) {
                        pAttr = node->txt;
                        nNum = strtol(pAttr,NULL,10);
                        Playfield.uScoreSaphir = (uint32_t)nNum;
                        node = ezxml_child(scores,"perl");
                        if (node != NULL) {
                            pAttr = node->txt;
                            nNum = strtol(pAttr,NULL,10);
                            Playfield.uScorePerl = (uint32_t)nNum;
                            node = ezxml_child(scores,"crystal");
                            if (node != NULL) {
                                pAttr = node->txt;
                                nNum = strtol(pAttr,NULL,10);
                                Playfield.uScoreCrystal = (uint32_t)nNum;
                                node = ezxml_child(scores,"letter");
                                if (node != NULL) {
                                    pAttr = node->txt;
                                    nNum = strtol(pAttr,NULL,10);
                                    Playfield.uScoreMessage = (uint32_t)nNum;
                                    node = ezxml_child(scores,"key");
                                    if (node != NULL) {
                                        pAttr = node->txt;
                                        nNum = strtol(pAttr,NULL,10);
                                        Playfield.uScoreKey = (uint32_t)nNum;
                                        node = ezxml_child(scores,"dynamite");
                                        if (node != NULL) {
                                            pAttr = node->txt;
                                            nNum = strtol(pAttr,NULL,10);
                                            Playfield.uScoreDynamite = (uint32_t)nNum;
                                            node = ezxml_child(scores,"hammer");
                                            if (node != NULL) {
                                                pAttr = node->txt;
                                                nNum = strtol(pAttr,NULL,10);
                                                Playfield.uScoreHammer = (uint32_t)nNum;
                                                node = ezxml_child(scores,"nutcracking");
                                                if (node != NULL) {
                                                    pAttr = node->txt;
                                                    nNum = strtol(pAttr,NULL,10);
                                                    Playfield.uScoreNutCracking = (uint32_t)nNum;
                                                    node = ezxml_child(scores,"stoning_beetle");
                                                    if (node != NULL) {
                                                        pAttr = node->txt;
                                                        nNum = strtol(pAttr,NULL,10);
                                                        Playfield.uScoreStoningBeetle = (uint32_t)nNum;
                                                        node = ezxml_child(scores,"stoning_mine");
                                                        if (node != NULL) {
                                                            pAttr = node->txt;
                                                            nNum = strtol(pAttr,NULL,10);
                                                            Playfield.uScoreStoningMine = (uint32_t)nNum;
                                                            node = ezxml_child(scores,"stoning_alien");
                                                            if (node != NULL) {
                                                                pAttr = node->txt;
                                                                nNum = strtol(pAttr,NULL,10);
                                                                Playfield.uScoreStoningAlien = (uint32_t)nNum;
                                                                node = ezxml_child(scores,"stoning_yam");
                                                                if (node != NULL) {
                                                                    pAttr = node->txt;
                                                                    nNum = strtol(pAttr,NULL,10);
                                                                    Playfield.uScoreStoningYam = (uint32_t)nNum;
                                                                    node = ezxml_child(scores,"timecoin");
                                                                    if (node != NULL) {
                                                                        pAttr = node->txt;
                                                                        nNum = strtol(pAttr,NULL,10);
                                                                        Playfield.uScoreTimeCoin = (uint32_t)nNum;
                                                                        nErrorCode = 0;
                                                                    } else {
                                                                        SDL_Log("%s: error in xml file, 'scores->timecoin' not found",__FUNCTION__);
                                                                    }
                                                                } else {
                                                                    SDL_Log("%s: error in xml file, 'scores->stoning_yam' not found",__FUNCTION__);
                                                                }
                                                            } else {
                                                                SDL_Log("%s: error in xml file, 'scores->stoning_alien' not found",__FUNCTION__);
                                                            }
                                                        } else {
                                                            SDL_Log("%s: error in xml file, 'scores->stoning_mine' not found",__FUNCTION__);
                                                        }
                                                    } else {
                                                        SDL_Log("%s: error in xml file, 'scores->stoning_beetle' not found",__FUNCTION__);
                                                    }
                                                } else {
                                                    SDL_Log("%s: error in xml file, 'scores->nutcracking' not found",__FUNCTION__);
                                                }
                                            } else {
                                                SDL_Log("%s: error in xml file, 'scores->hammer' not found",__FUNCTION__);
                                            }
                                        } else {
                                            SDL_Log("%s: error in xml file, 'scores->dynamite' not found",__FUNCTION__);
                                        }
                                    } else {
                                        SDL_Log("%s: error in xml file, 'scores->key' not found",__FUNCTION__);
                                    }
                                } else {
                                    SDL_Log("%s: error in xml file, 'scores->letter' not found",__FUNCTION__);
                                }
                            } else {
                                SDL_Log("%s: error in xml file, 'scores->crystal' not found",__FUNCTION__);
                            }
                        } else {
                            SDL_Log("%s: error in xml file, 'scores->perl' not found",__FUNCTION__);
                        }
                    } else {
                        SDL_Log("%s: error in xml file, 'scores->sapphire' not found",__FUNCTION__);
                    }
                } else {
                    SDL_Log("%s: error in xml file, 'scores->ruby' not found",__FUNCTION__);
                }
            } else {
                SDL_Log("%s: error in xml file, 'scores->emerald' not found",__FUNCTION__);
            }
        } else {
            SDL_Log("%s: error in xml file, 'scores' not found",__FUNCTION__);
        }
    } else {
        SDL_Log("%s: bad xml handle, null pointer",__FUNCTION__);
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           GetOtherLevelValuesFromXml
------------------------------------------------------------------------------
Beschreibung: Ermittelt folgende 'Values' aus der Level-XML-Datei  und trägt
              diese in die Struktur Playfield.x ein.
              * Emerald, die gesammelt werden müssen, um Spiel zu lösen
              * Time / Score - Faktor
              * Ausbreitungsgeschwindigkeit für grünen Käse
Parameter
      Eingang: xml, ezxml_t, gültiges XML-Handle
      Ausgang: -
               -
Rückgabewert:  int , 0 = OK, sonst Fehler
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
int GetOtherLevelValuesFromXml(ezxml_t xml) {
    int nErrorCode;
    ezxml_t values,node;
    long int nNum;
    char *pAttr;

    nErrorCode = -1;
    if (xml != NULL) {
        values = ezxml_child(xml,"values");
        if (values != NULL) {
            node = ezxml_child(values,"emeralds_to_collect");
            if (node != NULL) {
                pAttr = node->txt;
                nNum = strtol(pAttr,NULL,10);
                Playfield.uEmeraldsToCollect = (uint32_t)nNum;
                node = ezxml_child(values,"score_time_factor");
                if (node != NULL) {
                    pAttr = node->txt;
                    nNum = strtol(pAttr,NULL,10);
                    Playfield.uTimeScoreFactor = (uint32_t)nNum;
                    node = ezxml_child(values,"speed_cheese_spread");
                    if (node != NULL) {
                        pAttr = node->txt;
                        nNum = strtol(pAttr,NULL,10);
                        Playfield.uCheeseSpreadSpeed = (uint32_t)nNum;
                        node = ezxml_child(values,"speed_grass_spread");
                        if (node != NULL) {
                            pAttr = node->txt;
                            nNum = strtol(pAttr,NULL,10);
                            Playfield.uGrassSpreadSpeed = (uint32_t)nNum;
                            nErrorCode = 0;
                        } else {
                            SDL_Log("%s: error in xml file, 'values->speed_grass_spread' not found",__FUNCTION__);
                        }
                    } else {
                        SDL_Log("%s: error in xml file, 'values->speed_cheese_spread' not found",__FUNCTION__);
                    }
                } else {
                    SDL_Log("%s: error in xml file, 'values->score_time_factor' not found",__FUNCTION__);
                }
            } else {
                SDL_Log("%s: error in xml file, 'values->emeralds_to_collect' not found",__FUNCTION__);
            }
        } else {
            SDL_Log("%s: error in xml file, 'values' not found",__FUNCTION__);
        }
    } else {
        SDL_Log("%s: bad xml handle, null pointer",__FUNCTION__);
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           GetLevelTimesFromXml
------------------------------------------------------------------------------
Beschreibung: Ermittelt alle Level-Zeiten aus der Level-XML-Datei und trägt
              diese in die Struktur Playfield.x ein.
Parameter
      Eingang: xml, ezxml_t, gültiges XML-Handle
      Ausgang: -
               -
Rückgabewert:  int , 0 = OK, sonst Fehler
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
int GetLevelTimesFromXml(ezxml_t xml) {
    int nErrorCode;
    ezxml_t times,node;
    long int nNum;
    char *pAttr;

    nErrorCode = -1;
    if (xml != NULL) {
        times = ezxml_child(xml,"times");
        if (times != NULL) {
            node = ezxml_child(times,"to_play");
            if (node != NULL) {
                pAttr = node->txt;
                nNum = strtol(pAttr,NULL,10);
                Playfield.uTimeToPlay = (uint32_t)nNum * ge_DisplayMode.refresh_rate;
                node = ezxml_child(times,"wheel_rotation");
                if (node != NULL) {
                    pAttr = node->txt;
                    nNum = strtol(pAttr,NULL,10);
                    Playfield.uTimeWheelRotation = (uint32_t)nNum * ge_DisplayMode.refresh_rate;
                    node = ezxml_child(times,"magic_wall");
                    if (node != NULL) {
                        pAttr = node->txt;
                        nNum = strtol(pAttr,NULL,10);
                        Playfield.uTimeMagicWall = (uint32_t)nNum * ge_DisplayMode.refresh_rate;
                        node = ezxml_child(times,"light");
                        if (node != NULL) {
                            pAttr = node->txt;
                            nNum = strtol(pAttr,NULL,10);
                            Playfield.uTimeLight = (uint32_t)nNum * ge_DisplayMode.refresh_rate;
                            node = ezxml_child(times,"timedoor");
                            if (node != NULL) {
                                pAttr = node->txt;
                                nNum = strtol(pAttr,NULL,10);
                                Playfield.uTimeDoorTime = (uint32_t)nNum * ge_DisplayMode.refresh_rate;
                                node = ezxml_child(times,"timecoin");
                                if (node != NULL) {
                                    pAttr = node->txt;
                                    nNum = strtol(pAttr,NULL,10);
                                    Playfield.uAdditonalTimeCoinTime = (uint32_t)nNum * ge_DisplayMode.refresh_rate;
                                    nErrorCode = 0;
                                } else {
                                    SDL_Log("%s: error in xml file, 'times->timecoin' not found",__FUNCTION__);
                                }
                            } else {
                                SDL_Log("%s: error in xml file, 'times->timedoor' not found",__FUNCTION__);
                            }
                        } else {
                            SDL_Log("%s: error in xml file, 'times->light' not found",__FUNCTION__);
                        }
                    } else {
                        SDL_Log("%s: error in xml file, 'times->magic_wall' not found",__FUNCTION__);
                    }
                } else {
                    SDL_Log("%s: error in xml file, 'times->wheel_rotation' not found",__FUNCTION__);
                }
            } else {
                SDL_Log("%s: error in xml file, 'times->to_play' not found",__FUNCTION__);
            }
        } else {
            SDL_Log("%s: error in xml file, 'times' not found",__FUNCTION__);
        }
    } else {
        SDL_Log("%s: bad xml handle, null pointer",__FUNCTION__);
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           GetLevelInventoryFromXml
------------------------------------------------------------------------------
Beschreibung: Ermittelt alle Gegenstände, die die Spielfigur bei Levelstart
              erhält und trägt diese in die Struktur Playfield.x ein.
Parameter
      Eingang: xml, ezxml_t, gültiges XML-Handle
      Ausgang: -
               -
Rückgabewert:  int , 0 = OK, sonst Fehler
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
int GetLevelInventoryFromXml(ezxml_t xml) {
    int nErrorCode;
    ezxml_t inventory,node;
    long int nNum;
    char *pAttr;

    nErrorCode = -1;
    if (xml != NULL) {
        inventory = ezxml_child(xml,"inventory");
        if (inventory != NULL) {
            node = ezxml_child(inventory,"dynamite");
            if (node != NULL) {
                pAttr = node->txt;
                nNum = strtol(pAttr,NULL,10);
                Playfield.uDynamiteCount = (uint32_t)nNum;
                node = ezxml_child(inventory,"hammer");
                if (node != NULL) {
                    pAttr = node->txt;
                    nNum = strtol(pAttr,NULL,10);
                    Playfield.uHammerCount = (uint32_t)nNum;
                    node = ezxml_child(inventory,"white_key");
                    if (node != NULL) {
                        pAttr = node->txt;
                        nNum = strtol(pAttr,NULL,10);
                        Playfield.uWhiteKeyCount = (uint32_t)nNum;
                        nErrorCode = 0;
                    } else {
                        SDL_Log("%s: error in xml file, 'inventory->white_key' not found",__FUNCTION__);
                    }
                } else {
                    SDL_Log("%s: error in xml file, 'inventory->hammer' not found",__FUNCTION__);
                }
            } else {
                SDL_Log("%s: error in xml file, 'inventory->dynamite' not found",__FUNCTION__);
            }
        } else {
            SDL_Log("%s: error in xml file, 'inventory' not found",__FUNCTION__);
        }
    } else {
        SDL_Log("%s: bad xml handle, null pointer",__FUNCTION__);
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           GetConveyorBeltSettingsFromXml
------------------------------------------------------------------------------
Beschreibung: Ermittelt für alle Laufbänder die Einstellungen
                und trägt diese in die Struktur Playfield.x ein.
Parameter
      Eingang: xml, ezxml_t, gültiges XML-Handle
      Ausgang: -
               -
Rückgabewert:  int , 0 = OK, sonst Fehler
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
int GetConveyorBeltSettingsFromXml(ezxml_t xml) {
    int nErrorCode;
    ezxml_t conveyorbelt,color,node,node2;
    char *pAttr;

    nErrorCode = -1;
    conveyorbelt = ezxml_child(xml,"conveyorbelts");
    if (conveyorbelt != NULL) {
        color = ezxml_child(conveyorbelt,"red");
        if (color != NULL) {
            node = ezxml_child(color,"direction_at_start");
            node2 = ezxml_child(color,"next_direction");
            if ((node != NULL) && (node2 != NULL)) {
                pAttr = node->txt;      // "off", "left" oder "right"
                if (strcmp(pAttr,"left") == 0) {
                    Playfield.uConveybeltRedState = EMERALD_CONVEYBELT_LEFT;
                    Playfield.uConveybeltRedDirection = EMERALD_CONVEYBELT_TO_RIGHT;
                } else if (strcmp(pAttr,"right") == 0) {
                    Playfield.uConveybeltRedState = EMERALD_CONVEYBELT_RIGHT;
                    Playfield.uConveybeltRedDirection = EMERALD_CONVEYBELT_TO_LEFT;
                } else {
                    Playfield.uConveybeltRedState = EMERALD_CONVEYBELT_OFF;
                    pAttr = node2->txt; // "left" oder "right"
                    if (strcmp(pAttr,"left") == 0) {
                        Playfield.uConveybeltRedDirection = EMERALD_CONVEYBELT_TO_LEFT;
                    } else {
                        Playfield.uConveybeltRedDirection = EMERALD_CONVEYBELT_TO_RIGHT;
                    }
                }
                color = ezxml_child(conveyorbelt,"green");
                if (color != NULL) {
                    node = ezxml_child(color,"direction_at_start");
                    node2 = ezxml_child(color,"next_direction");
                    if ((node != NULL) && (node2 != NULL)) {
                        pAttr = node->txt;      // "off", "left" oder "right"
                        if (strcmp(pAttr,"left") == 0) {
                            Playfield.uConveybeltGreenState = EMERALD_CONVEYBELT_LEFT;
                            Playfield.uConveybeltGreenDirection = EMERALD_CONVEYBELT_TO_RIGHT;
                        } else if (strcmp(pAttr,"right") == 0) {
                            Playfield.uConveybeltGreenState = EMERALD_CONVEYBELT_RIGHT;
                            Playfield.uConveybeltGreenDirection = EMERALD_CONVEYBELT_TO_LEFT;
                        } else {
                            Playfield.uConveybeltGreenState = EMERALD_CONVEYBELT_OFF;
                            pAttr = node2->txt; // "left" oder "right"
                            if (strcmp(pAttr,"left") == 0) {
                                Playfield.uConveybeltGreenDirection = EMERALD_CONVEYBELT_TO_LEFT;
                            } else {
                                Playfield.uConveybeltGreenDirection = EMERALD_CONVEYBELT_TO_RIGHT;
                            }
                        }
                        color = ezxml_child(conveyorbelt,"blue");
                        if (color != NULL) {
                            node = ezxml_child(color,"direction_at_start");
                            node2 = ezxml_child(color,"next_direction");
                            if ((node != NULL) && (node2 != NULL)) {
                                pAttr = node->txt;      // "off", "left" oder "right"
                                if (strcmp(pAttr,"left") == 0) {
                                    Playfield.uConveybeltBlueState = EMERALD_CONVEYBELT_LEFT;
                                    Playfield.uConveybeltBlueDirection = EMERALD_CONVEYBELT_TO_RIGHT;
                                } else if (strcmp(pAttr,"right") == 0) {
                                    Playfield.uConveybeltBlueState = EMERALD_CONVEYBELT_RIGHT;
                                    Playfield.uConveybeltBlueDirection = EMERALD_CONVEYBELT_TO_LEFT;
                                } else {
                                    Playfield.uConveybeltBlueState = EMERALD_CONVEYBELT_OFF;
                                    pAttr = node2->txt; // "left" oder "right"
                                    if (strcmp(pAttr,"left") == 0) {
                                        Playfield.uConveybeltBlueDirection = EMERALD_CONVEYBELT_TO_LEFT;
                                    } else {
                                        Playfield.uConveybeltBlueDirection = EMERALD_CONVEYBELT_TO_RIGHT;
                                    }
                                }
                                color = ezxml_child(conveyorbelt,"yellow");
                                if (color != NULL) {
                                    node = ezxml_child(color,"direction_at_start");
                                    node2 = ezxml_child(color,"next_direction");
                                    if ((node != NULL) && (node2 != NULL)) {
                                        pAttr = node->txt;      // "off", "left" oder "right"
                                        if (strcmp(pAttr,"left") == 0) {
                                            Playfield.uConveybeltYellowState = EMERALD_CONVEYBELT_LEFT;
                                            Playfield.uConveybeltYellowDirection = EMERALD_CONVEYBELT_TO_RIGHT;
                                        } else if (strcmp(pAttr,"right") == 0) {
                                            Playfield.uConveybeltYellowState = EMERALD_CONVEYBELT_RIGHT;
                                            Playfield.uConveybeltYellowDirection = EMERALD_CONVEYBELT_TO_LEFT;
                                        } else {
                                            Playfield.uConveybeltYellowState = EMERALD_CONVEYBELT_OFF;
                                            pAttr = node2->txt; // "left" oder "right"
                                            if (strcmp(pAttr,"left") == 0) {
                                                Playfield.uConveybeltYellowDirection = EMERALD_CONVEYBELT_TO_LEFT;
                                            } else {
                                                Playfield.uConveybeltYellowDirection = EMERALD_CONVEYBELT_TO_RIGHT;
                                            }
                                        }
                                        nErrorCode = 0;
                                    } else {
                                        SDL_Log("%s: error in xml file, 'conveyorbelts->yellow->direction_at_start' or 'conveyorbelts->yellow->next_direction' not found",__FUNCTION__);
                                    }
                                } else {
                                    SDL_Log("%s: error in xml file, 'conveyorbelts->yellow' not found",__FUNCTION__);
                                }
                            } else {
                                SDL_Log("%s: error in xml file, 'conveyorbelts->blue->direction_at_start' or 'conveyorbelts->blue->next_direction' not found",__FUNCTION__);
                            }
                        } else {
                            SDL_Log("%s: error in xml file, 'conveyorbelts->blue' not found",__FUNCTION__);
                        }
                    } else {
                        SDL_Log("%s: error in xml file, 'conveyorbelts->green->direction_at_start' or 'conveyorbelts->green->next_direction' not found",__FUNCTION__);
                    }
                } else {
                    SDL_Log("%s: error in xml file, 'conveyorbelts->green' not found",__FUNCTION__);
                }
            } else {
                SDL_Log("%s: error in xml file, 'conveyorbelts->red->direction_at_start' or 'conveyorbelts->red->next_direction' not found",__FUNCTION__);
            }
        } else {
            SDL_Log("%s: error in xml file, 'conveyorbelts->red' not found",__FUNCTION__);
        }
    } else {
        SDL_Log("%s: error in xml file, 'conveyorbelts' not found",__FUNCTION__);
    }
    if (nErrorCode != 0) {
        Playfield.uConveybeltRedState = EMERALD_CONVEYBELT_OFF;
        Playfield.uConveybeltRedDirection = EMERALD_CONVEYBELT_TO_LEFT;
        Playfield.uConveybeltGreenState = EMERALD_CONVEYBELT_OFF;
        Playfield.uConveybeltGreenDirection = EMERALD_CONVEYBELT_TO_LEFT;
        Playfield.uConveybeltBlueState = EMERALD_CONVEYBELT_OFF;
        Playfield.uConveybeltBlueDirection = EMERALD_CONVEYBELT_TO_LEFT;
        Playfield.uConveybeltYellowState = EMERALD_CONVEYBELT_OFF;
        Playfield.uConveybeltYellowDirection = EMERALD_CONVEYBELT_TO_LEFT;
    } else {
        SDL_Log("Alles bestens:");
        SDL_Log("Playfield.uConveybeltRedState: %d",Playfield.uConveybeltRedState);
        SDL_Log("Playfield.uConveybeltRedDirection :%d",Playfield.uConveybeltRedDirection);
        SDL_Log("Playfield.uConveybeltGreenState: %d",Playfield.uConveybeltGreenState);
        SDL_Log("Playfield.uConveybeltGreenDirection :%d",Playfield.uConveybeltGreenDirection);
        SDL_Log("Playfield.uConveybeltBlueState: %d",Playfield.uConveybeltBlueState);
        SDL_Log("Playfield.uConveybeltBlueDirection :%d",Playfield.uConveybeltBlueDirection);
        SDL_Log("Playfield.uConveybeltYellowState: %d",Playfield.uConveybeltYellowState);
        SDL_Log("Playfield.uConveybeltYellowDirection :%d",Playfield.uConveybeltYellowDirection);
    }
    return nErrorCode;
}




/*----------------------------------------------------------------------------
Name:           GetReplicatorLighbarrierSettingsFromXml
------------------------------------------------------------------------------
Beschreibung: Ermittelt für alle Replikatoren und Lichtschranken die Einstellungen
                und trägt diese in die Struktur Playfield.x ein.
Parameter
      Eingang: xml, ezxml_t, gültiges XML-Handle
      Ausgang: -
               -
Rückgabewert:  int , 0 = OK, sonst Fehler
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
int GetReplicatorLighbarrierSettingsFromXml(ezxml_t xml) {
    int nErrorCode;
    ezxml_t lightbarrier,replicator,color,node;
    long int nNum;
    char *pAttr;

    nErrorCode = -1;
    if (xml != NULL) {
        replicator = ezxml_child(xml,"replicators");
        if (replicator != NULL) {
            color = ezxml_child(replicator,"red");
            if (color != NULL) {
                node = ezxml_child(color,"active");
                if (node != NULL) {
                    pAttr = node->txt;
                    nNum = strtol(pAttr,NULL,10);
                    Playfield.bReplicatorRedOn = (nNum == 1);
                    node = ezxml_child(color,"element");
                    if (node != NULL) {
                        pAttr = node->txt;
                        nNum = strtol(pAttr,NULL,10);
                        Playfield.uReplicatorRedObject = (uint32_t)nNum;
                        nErrorCode = 0;
                        color = ezxml_child(replicator,"green");
                        if (color != NULL) {
                            node = ezxml_child(color,"active");
                            if (node != NULL) {
                                pAttr = node->txt;
                                nNum = strtol(pAttr,NULL,10);
                                Playfield.bReplicatorGreenOn = (nNum == 1);
                                node = ezxml_child(color,"element");
                                if (node != NULL) {
                                    pAttr = node->txt;
                                    nNum = strtol(pAttr,NULL,10);
                                    Playfield.uReplicatorGreenObject = (uint32_t)nNum;
                                    color = ezxml_child(replicator,"blue");
                                    if (color != NULL) {
                                        node = ezxml_child(color,"active");
                                        if (node != NULL) {
                                            pAttr = node->txt;
                                            nNum = strtol(pAttr,NULL,10);
                                            Playfield.bReplicatorBlueOn = (nNum == 1);
                                            node = ezxml_child(color,"element");
                                            if (node != NULL) {
                                                pAttr = node->txt;
                                                nNum = strtol(pAttr,NULL,10);
                                                Playfield.uReplicatorBlueObject = (uint32_t)nNum;
                                                color = ezxml_child(replicator,"yellow");
                                                if (color != NULL) {
                                                    node = ezxml_child(color,"active");
                                                    if (node != NULL) {
                                                        pAttr = node->txt;
                                                        nNum = strtol(pAttr,NULL,10);
                                                        Playfield.bReplicatorYellowOn = (nNum == 1);
                                                        node = ezxml_child(color,"element");
                                                        if (node != NULL) {
                                                            pAttr = node->txt;
                                                            nNum = strtol(pAttr,NULL,10);
                                                            Playfield.uReplicatorYellowObject = (uint32_t)nNum;
                                                            nErrorCode = 0;
                                                        } else {
                                                            SDL_Log("%s: error in xml file, 'replicators->yellow->element' not found",__FUNCTION__);
                                                        }
                                                    } else {
                                                        SDL_Log("%s: error in xml file, 'replicators->yellow->active' not found",__FUNCTION__);
                                                    }
                                                } else {
                                                    SDL_Log("%s: error in xml file, 'replicators->yellow' not found",__FUNCTION__);
                                                }
                                            } else {
                                                SDL_Log("%s: error in xml file, 'replicators->blue->element' not found",__FUNCTION__);
                                            }
                                        } else {
                                            SDL_Log("%s: error in xml file, 'replicators->blue->active' not found",__FUNCTION__);
                                        }
                                    } else {
                                        SDL_Log("%s: error in xml file, 'replicators->blue' not found",__FUNCTION__);
                                    }
                                } else {
                                    SDL_Log("%s: error in xml file, 'replicators->green->element' not found",__FUNCTION__);
                                }
                            } else {
                                SDL_Log("%s: error in xml file, 'replicators->green->active' not found",__FUNCTION__);
                            }
                        } else {
                            SDL_Log("%s: error in xml file, 'replicators->green' not found",__FUNCTION__);
                        }
                    } else {
                        SDL_Log("%s: error in xml file, 'replicators->red->element' not found",__FUNCTION__);
                    }
                } else {
                    SDL_Log("%s: error in xml file, 'replicators->red->active' not found",__FUNCTION__);
                }
            } else {
                SDL_Log("%s: error in xml file, 'replicators->red' not found",__FUNCTION__);
            }
        } else {
            SDL_Log("%s: error in xml file, 'replicators' not found",__FUNCTION__);
        }
    } else {
        SDL_Log("%s: bad xml handle, null pointer",__FUNCTION__);
    }
    if (nErrorCode == 0) {
        nErrorCode = -1;
        lightbarrier = ezxml_child(xml,"lightbarriers");
        if (lightbarrier != NULL) {
            color = ezxml_child(lightbarrier,"red");
            if (color != NULL) {
                node = ezxml_child(color,"active");
                if (node != NULL) {
                    pAttr = node->txt;
                    nNum = strtol(pAttr,NULL,10);
                    Playfield.bLightBarrierRedOn = (nNum == 1);
                    color = ezxml_child(lightbarrier,"green");
                    if (color != NULL) {
                        node = ezxml_child(color,"active");
                        if (node != NULL) {
                            pAttr = node->txt;
                            nNum = strtol(pAttr,NULL,10);
                            Playfield.bLightBarrierGreenOn = (nNum == 1);
                            color = ezxml_child(lightbarrier,"blue");
                            if (color != NULL) {
                                node = ezxml_child(color,"active");
                                if (node != NULL) {
                                    pAttr = node->txt;
                                    nNum = strtol(pAttr,NULL,10);
                                    Playfield.bLightBarrierBlueOn = (nNum == 1);
                                    color = ezxml_child(lightbarrier,"yellow");
                                    if (color != NULL) {
                                        node = ezxml_child(color,"active");
                                        if (node != NULL) {
                                            pAttr = node->txt;
                                            nNum = strtol(pAttr,NULL,10);
                                            Playfield.bLightBarrierYellowOn = (nNum == 1);
                                            nErrorCode = 0;
                                        } else {
                                            SDL_Log("%s: error in xml file, 'lightbarriers->yellow->active' not found",__FUNCTION__);
                                        }
                                    } else {
                                        SDL_Log("%s: error in xml file, 'lightbarriers->yellow' not found",__FUNCTION__);
                                    }
                                } else {
                                    SDL_Log("%s: error in xml file, 'lightbarriers->blue->active' not found",__FUNCTION__);
                                }
                            } else {
                                SDL_Log("%s: error in xml file, 'lightbarriers->blue' not found",__FUNCTION__);
                            }
                        } else {
                            SDL_Log("%s: error in xml file, 'lightbarriers->green->active' not found",__FUNCTION__);
                        }
                    } else {
                        SDL_Log("%s: error in xml file, 'lightbarriers->green' not found",__FUNCTION__);
                    }
                } else {
                    SDL_Log("%s: error in xml file, 'lightbarriers->red->active' not found",__FUNCTION__);
                }
            } else {
                SDL_Log("%s: error in xml file, 'lightbarriers->red' not found",__FUNCTION__);
            }
        } else {
            SDL_Log("%s: error in xml file, 'lightbarriers' not found",__FUNCTION__);
        }
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           GetLetterMessagesFromXml
------------------------------------------------------------------------------
Beschreibung: Ermittelt alle Nachrichten für das Element 'Briefumschlag', EMERALD_MESSAGE_1 bis EMERALD_MESSAGE_8
               und trägt diese in die Struktur Playfield.x ein.
Parameter
      Eingang: xml, ezxml_t, gültiges XML-Handle
      Ausgang: -
               -
Rückgabewert:  int , 0 = OK, sonst Fehler
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
int GetLetterMessagesFromXml(ezxml_t xml) {
    int nErrorCode;
    uint32_t I;
    ezxml_t messages,messagenr;
    char *pAttr;
    char szM[4];    // 'm1' bis 'm8'
    uint32_t uBinaryLen;

    nErrorCode = -1;
    szM[0] = 'm';
    szM[1] = 'x';
    szM[2] = 0;
    if (xml != NULL) {
        messages = ezxml_child(xml,"messages");
        if (messages != NULL) {
            for (I = 1; (I <= EMERALD_MAX_MESSAGES) && (nErrorCode == -1); I++) {
               szM[1] = I + 0x30;
               messagenr = ezxml_child(messages,szM);
               if (messagenr != NULL) {
                    pAttr = messagenr->txt;
                    if (strlen(pAttr) > 0) {
                        Playfield.pMessage[I - 1] = malloc(strlen(pAttr) + 1);  // Zielspeicher wird auf jeden Fall kleiner
                        if (Playfield.pMessage[I - 1] != NULL) {
                            memset(Playfield.pMessage[I - 1],0,strlen(pAttr) + 1); // Für Stringende sorgen
                            if (Base64ToBin((uint8_t*)Playfield.pMessage[I - 1],(uint8_t*)pAttr,strlen(pAttr),&uBinaryLen) != 0) {
                                SDL_Log("%s: Base64ToBin() failed for Message Nr. %d.",__FUNCTION__,I + 1);
                                nErrorCode = -2;
                            }
                        } else {
                            SDL_Log("%s: can not allocate memory for Message Nr. %d.",__FUNCTION__,I + 1);
                            nErrorCode = -2;
                        }
                    }
               } else {
                    SDL_Log("%s: error in xml file, 'messages->%s' not found",__FUNCTION__,szM);
                    nErrorCode = -2;
               }
            }
            nErrorCode = 0;
        } else {
            SDL_Log("%s: error in xml file, 'messages' not found",__FUNCTION__);
        }
    } else {
        SDL_Log("%s: bad xml handle, null pointer",__FUNCTION__);
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           GetYamExplosionsFromXml
------------------------------------------------------------------------------
Beschreibung: Ermittelt YAM-Explosionen und trägt diese in die Struktur Playfield.x ein.
Parameter
      Eingang: xml, ezxml_t, gültiges XML-Handle
      Ausgang: -
               -
Rückgabewert:  int , 0 = OK, sonst Fehler
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
int GetYamExplosionsFromXml(ezxml_t xml) {
    int nErrorCode;
    long int nNum;
    uint32_t I;
    uint32_t E;
    ezxml_t yams,explosion,element;
    char *pAttr;
    char szTag[32];

    nErrorCode = -1;
    yams = ezxml_child(xml,"max_yam_explosions");
    if (yams != NULL) {
        pAttr = yams->txt;
        nNum = strtol(pAttr,NULL,10);
        if ((nNum > 0) && (nNum <= EMERALD_MAX_YAM_EXPLOSIONS)) {
            Playfield.uMaxYamExplosionIndex = (uint32_t)nNum - 1;
            yams = ezxml_child(xml,"yam_explosions");
            if (yams != NULL) {
                nErrorCode = 0;
                for (I = 0; (I <= Playfield.uMaxYamExplosionIndex) && (nErrorCode == 0); I++) {
                    sprintf(szTag,"explosion%02d",I + 1);
                    explosion = ezxml_child(yams,szTag);
                    if (explosion != NULL) {
                        for (E = 0; (E < 9) && (nErrorCode == 0); E++) {
                            sprintf(szTag,"element%02d",E + 1);
                            element = ezxml_child(explosion,szTag);
                            if (element != NULL) {
                                pAttr = element->txt;
                                nNum = strtol(pAttr,NULL,10);
                                Playfield.YamExplosions[I].uElement[E] = (uint16_t)nNum;
                            } else {
                                SDL_Log("%s: error in xml file, tag <%s> not found",__FUNCTION__,szTag);
                                nErrorCode = -1;
                            }
                        }
                    } else {
                        SDL_Log("%s: error in xml file, tag <%s> not found",__FUNCTION__,szTag);
                        nErrorCode = -1;
                    }
                }
            } else {
                SDL_Log("%s: error in xml file, 'yam_explosions' not found",__FUNCTION__);
            }
        } else {
            SDL_Log("%s: error in xml file, bad yam explosions: %u",__FUNCTION__,(uint32_t)nNum);
        }
    } else {
        SDL_Log("%s: error in xml file, 'max_yam_explosions' not found",__FUNCTION__);
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           GetLeveldataFromXml
------------------------------------------------------------------------------
Beschreibung:  Holt die eigentlichen Labyrinth-Leveldaten aus der Level-XML-Datei
               und kopiert diese in die Struktur Playfield.pLevel. Der Speicher
               muss dort bereits alloziert worden sein.
Parameter
      Eingang: xml, ezxml_t, gültiges XML-Handle
      Ausgang: -
               -
Rückgabewert:  int , 0 = OK, sonst Fehler
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
int GetLeveldataFromXml(ezxml_t xml) {
    int nErrorCode;
    ezxml_t leveldata;
    char *pAttr;
    uint32_t uBase64Len;
    uint32_t uBinLen;
    char szMD5String[32 + 1];   // + 1 für Stringende
    MD5Context MD5Leveldata;
    uint8_t *pCompressed;
    uint32_t uUnCompressLen;
    int nMiniz;

    pCompressed = NULL;
    nErrorCode = -1;
    if (xml != NULL) {
        leveldata = ezxml_child(xml,"leveldata");
        if (leveldata != NULL) {
            pAttr = leveldata->txt;
            uBase64Len = strlen(pAttr);
            if (uBase64Len > 0) {
                md5Init(&MD5Leveldata);
                md5Update(&MD5Leveldata,(uint8_t*)pAttr,strlen(pAttr));
                md5Finalize(&MD5Leveldata);
                GetMd5String(MD5Leveldata.digest,szMD5String);
                if (strcasecmp(Playfield.szMd5String,szMD5String) == 0) {
                    // Genaue Länge der Binärdaten ermitteln und Probelauf mit Base64-Daten
                    if (Base64ToBin(NULL,(uint8_t*)pAttr, strlen(pAttr),&uBinLen) != 0) {
                        uBinLen = -1;
                    }
                    if (uBinLen > 0) {
                        pCompressed = malloc(uBinLen);
                        if (pCompressed != NULL) {
                            Base64ToBin(pCompressed,(uint8_t*)pAttr,strlen(pAttr),&uBinLen);
                            uUnCompressLen = Playfield.uLevel_X_Dimension * Playfield.uLevel_Y_Dimension * sizeof(uint16_t);
                            nMiniz = mz_uncompress((uint8_t*)Playfield.pLevel,(mz_ulong*)&uUnCompressLen,pCompressed,(mz_ulong)uBinLen);
                            if (nMiniz == MZ_OK) {
                                if (uUnCompressLen == (Playfield.uLevel_X_Dimension * Playfield.uLevel_Y_Dimension * sizeof(uint16_t))) {
                                    nErrorCode = 0;
                                } else {
                                    SDL_Log("%s: decompress unexpected len: %u     expected: %u",__FUNCTION__,(uint32_t)uUnCompressLen,(uint32_t)(Playfield.uLevel_X_Dimension * Playfield.uLevel_Y_Dimension * sizeof(uint16_t)));
                                }
                            } else {
                                SDL_Log("%s: mz_uncompress ERROR: %d",__FUNCTION__,nMiniz);
                            }
                        } else {
                            SDL_Log("%s: malloc() failed for compressed leveldata",__FUNCTION__);
                        }
                    } else {
                        SDL_Log("%s: Base64ToBin() failed",__FUNCTION__);
                    }
                } else {
                    SDL_Log("%s: md5 hash not valid",__FUNCTION__);
                }
            } else {
                SDL_Log("%s: error in xml file, empty level",__FUNCTION__);
            }
        } else {
            SDL_Log("%s: error in xml file, 'leveldata' not found",__FUNCTION__);
        }
    } else {
        SDL_Log("%s: bad xml handle, null pointer",__FUNCTION__);
    }
    SAFE_FREE(pCompressed);
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           GetLevelTag
------------------------------------------------------------------------------
Beschreibung: Ermittelt das Leveltag innerhalb einer geöffneten Levelgruppe.
Parameter
      Eingang: xml, ezxml_t, Zeiger auf geöffnete XML-Datei (Levelgruppe).
               uLevelNumber, uint32_t, Level innerhalb der Levelgruppe
      Ausgang: -
Rückgabewert:  ezxml_t, NULL = Fehler, sonst Tag auf Level
Seiteneffekte: -
------------------------------------------------------------------------------*/
ezxml_t GetLevelTag(ezxml_t xml,uint32_t uLevelNumber) {
    char szLevelNumberTag[16];              // level000

    sprintf(szLevelNumberTag,"level%03u",uLevelNumber);
    return ezxml_child(xml,szLevelNumberTag);
}

/*----------------------------------------------------------------------------
Name:           InitialisePlayfield
------------------------------------------------------------------------------
Beschreibung: Lädt die Leveldaten und initialisiert das Spielfeld. Das Ergebnis
              wird in der Struktur Playfield.x abgelegt.
              Diese Funktion alloziert Speicher, der später wieder freigegebem
              werden muss: Playfield.pLevel und Playfield.pStatusAnimation.

              Vor Aufruf dieser Funktion muss eine Levelgruppe mit SelectLevelgroup() ausgewählt worden sein.
Parameter
      Eingang: uLevelgroupIndex, uint32_t, Auswahl der Levelgruppe aus LevelgroupFiles[].x über Levelgruppen-Index
               uLevelNumber, uint32_t, Level innerhalb der Levelgruppe
      Ausgang: -
Rückgabewert:  int , 0 = OK, sonst Fehler
Seiteneffekte: Playfield.x, ge_DisplayMode, SelectedLevelgroup.x, Config.x
------------------------------------------------------------------------------*/
int InitialisePlayfield(uint32_t uLevelNumber) {
    ezxml_t xml;
    ezxml_t level;
    int nErrorCode;
    uint32_t I;
    uint32_t uXmlLen;
    uint8_t *pXml;

    nErrorCode = -1;
    memset(&Playfield,0,sizeof(Playfield));
    InitYamExplosions(Playfield.YamExplosions);
    Playfield.uReplicatorRedObject = EMERALD_SPACE;
    Playfield.uReplicatorGreenObject = EMERALD_SPACE;
    Playfield.uReplicatorBlueObject = EMERALD_SPACE;
    Playfield.uReplicatorYellowObject = EMERALD_SPACE;
    Playfield.uConveybeltRedState = EMERALD_CONVEYBELT_OFF;
    Playfield.uConveybeltGreenState = EMERALD_CONVEYBELT_OFF;
    Playfield.uConveybeltBlueState = EMERALD_CONVEYBELT_OFF;
    Playfield.uConveybeltYellowState = EMERALD_CONVEYBELT_OFF;
    Playfield.uConveybeltRedDirection = EMERALD_CONVEYBELT_TO_LEFT;
    Playfield.uConveybeltGreenDirection = EMERALD_CONVEYBELT_TO_LEFT;
    Playfield.uConveybeltBlueDirection = EMERALD_CONVEYBELT_TO_LEFT;
    Playfield.uConveybeltYellowDirection = EMERALD_CONVEYBELT_TO_LEFT;
    Playfield.bInitOK = false;
    for (I = 0; I < EMERALD_MAX_MESSAGES; I++) {
        Playfield.pMessage[I] = NULL;
    }
    if ((SelectedLevelgroup.bOK) && (uLevelNumber < SelectedLevelgroup.uLevelCount)) {
        pXml = ReadFile(SelectedLevelgroup.szFilename,&uXmlLen);     // Levelgruppen-Datei einlesen
        if (pXml != NULL) {
            SDL_Log("%s: using levelgroup %s, filesize: %u",__FUNCTION__,SelectedLevelgroup.szFilename,uXmlLen);
        } else {
            SDL_Log("%s: can not read levelgroup file %s",__FUNCTION__,SelectedLevelgroup.szFilename);
            return -1;
       }
        xml = ezxml_parse_str((char*)pXml,strlen((char*)pXml));
        if (xml != NULL) {
            level = GetLevelTag(xml,uLevelNumber);
            if (level != NULL) {
                // Zunächst X- und Y-Dimension ermitteln. Diese Werte werden benötigt, um die Größe des Levelspeichers zu bestimmen
                if (GetLeveldimensionFromXml(level,&Playfield.uLevel_X_Dimension,&Playfield.uLevel_Y_Dimension) == 0) {
                    if (GetMemoryForPlayfield() == 0) {
                        if (GetTitleAuthorVersionHashFromXml(level) == 0) {
                            if (GetLevelScoresFromXml(level) == 0) {
                                if (GetOtherLevelValuesFromXml(level) == 0) {
                                    if (GetLevelTimesFromXml(level) == 0) {
                                        if (GetLevelInventoryFromXml(level) == 0) {
                                            if (GetReplicatorLighbarrierSettingsFromXml(level) == 0) {
                                                if (GetConveyorBeltSettingsFromXml(level) == 0) {
                                                    if (GetLetterMessagesFromXml(level) == 0) {
                                                        if (GetYamExplosionsFromXml(level) == 0) {
                                                            if (GetLeveldataFromXml(level) == 0) {
                                                                if (GetManCoordinates(Playfield.pLevel,Playfield.uLevel_X_Dimension,Playfield.uLevel_Y_Dimension,&Playfield.uManXpos,&Playfield.uManYpos) == 0) {
                                                                    if (CheckReplicators(Playfield.pLevel,Playfield.uLevel_X_Dimension,Playfield.uLevel_Y_Dimension) == 0) {
                                                                        if (CheckAcidPools(Playfield.pLevel,Playfield.uLevel_X_Dimension,Playfield.uLevel_Y_Dimension) == 0) {
                                                                            if (CheckLevelBorder() == 0) {
                                                                                CloseAllDoors();
                                                                                SetActiveDynamiteP1();
                                                                                nErrorCode = SearchTeleporter();
                                                                                Playfield.bInitOK = (nErrorCode == 0);
                                                                            }
                                                                        }
                                                                    }
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        } else {
            SDL_Log("%s: ezxml_parse_str() failed",__FUNCTION__);
        }
        ezxml_free(xml);    // Prüft selbst, ob Pointer NULL ist
        SAFE_FREE(pXml);
    }
    if (Playfield.bInitOK) {
        // Sichtbaren Bereich berechnen
        Playfield.uVisibleY = ((Config.uResY - PANEL_H) / FONT_H);
        Playfield.uVisibleX = Config.uResX / FONT_W;
        Playfield.uVisibleCenterY = Playfield.uVisibleY / 2;
        Playfield.uVisibleCenterX = Playfield.uVisibleX / 2;
        // Y-Zentrierung des Levels im Anzeigebereich
        if (Playfield.uVisibleY >= Playfield.uLevel_Y_Dimension) {    // Level ist in Y-Richtung kleiner bzw. gleich der Anzeigehöhe (in Y-Richtung wird nicht gescrollt)
            Playfield.uShiftLevelYpix = ((Playfield.uVisibleY - Playfield.uLevel_Y_Dimension) / 2) * FONT_H;    // Level im Anzeigebereich zentrieren
        } else {
            // Level ist in Y-Richtung größer als Anzeigebereich, daher oben anfangen (es muss ggf. in Y-Richtung gescrollt werden)
            Playfield.uShiftLevelYpix = 0;
        }
        // X-Zentrierung des Levels im Anzeigebereich
        if (Playfield.uVisibleX >= Playfield.uLevel_X_Dimension) {    // Level ist in X-Richtung kleiner bzw. gleich der Anzeigebreite (in X-Richtung wird nicht gescrollt)
            Playfield.uShiftLevelXpix = ((Playfield.uVisibleX - Playfield.uLevel_X_Dimension) / 2) * FONT_H;    // Level im Anzeigebereich zentrieren
        } else {
            // Level ist in X-Richtung größer als Anzeigebereich, daher oben anfangen (es muss ggf. in Y-Richtung gescrollt werden)
            Playfield.uShiftLevelXpix = 0;
        }
        // Positionsüberläufe abfangen
        Playfield.nMaxXpos = (Playfield.uLevel_X_Dimension * FONT_W) - Config.uResX;
        if (Playfield.nMaxXpos < 0) {
            Playfield.nMaxXpos = 0;
        }
        Playfield.nMaxYpos = (Playfield.uLevel_Y_Dimension * FONT_H) - Config.uResY + PANEL_H;
        if (Playfield.nMaxYpos < 0) {
            Playfield.nMaxYpos = 0;
        }
        // Startposition im Level
        if (Playfield.uManXpos >= Playfield.uVisibleCenterX) {
            Playfield.nTopLeftXpos = (Playfield.uManXpos - Playfield.uVisibleCenterX) * FONT_W;
        } else {
            Playfield.nTopLeftXpos = 0;
        }
        if (Playfield.uManYpos >= Playfield.uVisibleCenterY) {
            Playfield.nTopLeftYpos = (Playfield.uManYpos - Playfield.uVisibleCenterY) * FONT_H;
        } else {
            Playfield.nTopLeftYpos = 0;
        }
        Playfield.uTotalScore = 0;
        Playfield.uFrameCounter = 0;
        Playfield.bMagicWallRunning = false;
        Playfield.bMagicWallWasOn = false;
        Playfield.uTimeMagicWallLeft = 0;
        Playfield.uTimeLightLeft = 0;
        Playfield.bWheelRunning = false;
        Playfield.uTimeWheelRotationLeft = 0;
        Playfield.uWheelRunningXpos = 0;      // nur gültig, wenn bWheelRunning = true
        Playfield.uWheelRunningYpos = 0;      // nur gültig, wenn bWheelRunning = true
        Playfield.bHasRedKey = false;
        Playfield.bHasGreenKey = false;
        Playfield.bHasBlueKey = false;
        Playfield.bHasYellowKey = false;
        Playfield.bHasGeneralKey = false;
        Playfield.bManDead = false;
        Playfield.bWellDone = false;
        Playfield.bLightOn = false;
        Playfield.uTimeDoorTimeLeft = 0;
        Playfield.bSwitchDoorState = false;
        Playfield.bSwitchDoorImpluse = false;
        Playfield.bSwitchRemoteBombLeft = false;
        Playfield.bSwitchRemoteBombRight = false;
        Playfield.bSwitchRemoteBombDown = false;
        Playfield.bSwitchRemoteBombUp = false;
        Playfield.bSwitchRemoteBombIgnition = false;
        Playfield.bRemoteBombMoved = false;
        Playfield.uShowMessageNo = 0;
        Playfield.uYamExplosion = 0;          // Aktuelle YAM-Explosion
        Playfield.uDynamitePos = 0xFFFFFFFF;  // lineare Koordinate des manuell gezündeten Dynamits durch den Man, 0xFFFFFFFF = keine Zündung
        Playfield.uDynamiteStatusAnim = EMERALD_ANIM_STAND; // Status/Animation für manuell gezündetes Dynamit
        InitRollUnderground();
        PrintPlayfieldValues();
        SetCentralExplosionCoordinates();
        SetCentralMegaExplosionCoordinates();
        Playfield.uPlayTimeStart = 0;
        Playfield.uPlayTimeEnd = 0;
        Playfield.bReadyToGo = false;
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           SetCentralExplosionCoordinates
------------------------------------------------------------------------------
Beschreibung: Hinterlegt die 3x3-Koordinaten für eine zentrale Explosion ab Mittelpunkt.
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void SetCentralExplosionCoordinates() {
    Playfield.nCentralExplosionCoordinates[0] = -(int)Playfield.uLevel_X_Dimension - 1;         // oben links
    Playfield.nCentralExplosionCoordinates[1] = -(int)Playfield.uLevel_X_Dimension;             // oben mitte
    Playfield.nCentralExplosionCoordinates[2] = -(int)Playfield.uLevel_X_Dimension + 1;         // oben rechts
    Playfield.nCentralExplosionCoordinates[3] = -1;                                             // mitte links
    Playfield.nCentralExplosionCoordinates[4] = 1;                                              // mitte rechts
    Playfield.nCentralExplosionCoordinates[5] = (int)Playfield.uLevel_X_Dimension - 1;          // unten links
    Playfield.nCentralExplosionCoordinates[6] = (int)Playfield.uLevel_X_Dimension;              // unten mitte
    Playfield.nCentralExplosionCoordinates[7] = (int)Playfield.uLevel_X_Dimension + 1;          // unten rechts
    // Für Yam-Explosion mit Replikator (obere Hälfte)
    Playfield.nCheckReplicatorForYamExplosionTop[0] = -(int)Playfield.uLevel_X_Dimension - 1;   // R. oben links
    Playfield.nCheckReplicatorForYamExplosionTop[1] = -(int)Playfield.uLevel_X_Dimension;       // R. oben mitte
    Playfield.nCheckReplicatorForYamExplosionTop[2] = -(int)Playfield.uLevel_X_Dimension + 1;   // R. oben rechts
    Playfield.nCheckReplicatorForYamExplosionTop[3] = -1;                                       // R. mitte links
    Playfield.nCheckReplicatorForYamExplosionTop[4] = 1;                                        // R. mitte rechts
    // Für Yam-Explosion mit Replikator (untere Hälfte)
    Playfield.nCheckReplicatorForYamExplosionButtom[0] = -1;                                    // R. oben links
    Playfield.nCheckReplicatorForYamExplosionButtom[1] = 0;                                     // R. oben mitte
    Playfield.nCheckReplicatorForYamExplosionButtom[2] = 1;                                     // R. oben rechts
    Playfield.nCheckReplicatorForYamExplosionButtom[3] = (int)Playfield.uLevel_X_Dimension - 1; // R. mitte links;
    Playfield.nCheckReplicatorForYamExplosionButtom[4] = (int)Playfield.uLevel_X_Dimension + 1; // R. mitte rechts;
    // Für Yam-Explosion mit Säurebecken (obere Hälfte)
    Playfield.nCheckAcidPoolForYamExplosionTop[0] = -(int)Playfield.uLevel_X_Dimension - 1;     // S. oben links
    Playfield.nCheckAcidPoolForYamExplosionTop[1] = -(int)Playfield.uLevel_X_Dimension;         // S. oben mitte
    Playfield.nCheckAcidPoolForYamExplosionTop[2] = -(int)Playfield.uLevel_X_Dimension + 1;     // S. oben rechts
    Playfield.nCheckAcidPoolForYamExplosionTop[3] = - 1;                                        // S. mitte links
    Playfield.nCheckAcidPoolForYamExplosionTop[4] = 0;                                          // S. mitte
    Playfield.nCheckAcidPoolForYamExplosionTop[5] = 1;                                          // S. mitte rechts
    // Für Yam-Explosion mit Säurebecken (untere Hälfte)
    Playfield.nCheckAcidPoolForYamExplosionButtom[0] = -1;                                      // S. oben links
    Playfield.nCheckAcidPoolForYamExplosionButtom[1] = 0;                                       // S. oben mitte
    Playfield.nCheckAcidPoolForYamExplosionButtom[2] = 1;                                       // S. oben rechts
    Playfield.nCheckAcidPoolForYamExplosionButtom[3] = (int)Playfield.uLevel_X_Dimension - 1;   // S. mitte links
    Playfield.nCheckAcidPoolForYamExplosionButtom[4] = (int)Playfield.uLevel_X_Dimension;       // S. mitte
    Playfield.nCheckAcidPoolForYamExplosionButtom[5] = (int)Playfield.uLevel_X_Dimension + 1;   // S. mitte rechts
}


/*----------------------------------------------------------------------------
Name:           SetCentralMegaExplosionCoordinates
------------------------------------------------------------------------------
Beschreibung: Hinterlegt die Koordinaten für eine zentrale Mega-Explosion ab Mittelpunkt.
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void SetCentralMegaExplosionCoordinates(void) {
    Playfield.nCentralMegaExplosionCoordinates[0] = -(int)Playfield.uLevel_X_Dimension * 2 - 1; // Ganz oben links
    Playfield.nCentralMegaExplosionCoordinates[1] = -(int)Playfield.uLevel_X_Dimension * 2;     // Ganz oben mitte
    Playfield.nCentralMegaExplosionCoordinates[2] = -(int)Playfield.uLevel_X_Dimension * 2 + 1; // Ganz oben rechts
    Playfield.nCentralMegaExplosionCoordinates[3] = -(int)Playfield.uLevel_X_Dimension - 2;     // oben links, 2.Reihe
    Playfield.nCentralMegaExplosionCoordinates[4] = -(int)Playfield.uLevel_X_Dimension - 1;     // oben links, 2.Reihe
    Playfield.nCentralMegaExplosionCoordinates[5] = -(int)Playfield.uLevel_X_Dimension;         // oben mitte, 2.Reihe
    Playfield.nCentralMegaExplosionCoordinates[6] = -(int)Playfield.uLevel_X_Dimension + 1;     // oben rechts, 2.Reihe
    Playfield.nCentralMegaExplosionCoordinates[7] = -(int)Playfield.uLevel_X_Dimension + 2;     // oben rechts, 2.Reihe
    Playfield.nCentralMegaExplosionCoordinates[8] = -2;                                         // mitte, ganz links
    Playfield.nCentralMegaExplosionCoordinates[9] = -1;                                         // mitte, links
    Playfield.nCentralMegaExplosionCoordinates[10] = 1;                                         // mitte, rechts
    Playfield.nCentralMegaExplosionCoordinates[11] = 2;                                         // mitte, ganz rechts
    Playfield.nCentralMegaExplosionCoordinates[12] = (int)Playfield.uLevel_X_Dimension - 2;     // unten links, 4.Reihe
    Playfield.nCentralMegaExplosionCoordinates[13] = (int)Playfield.uLevel_X_Dimension - 1;     // unten links, 4.Reihe
    Playfield.nCentralMegaExplosionCoordinates[14] = (int)Playfield.uLevel_X_Dimension;         // unten mitte, 4.Reihe
    Playfield.nCentralMegaExplosionCoordinates[15] = (int)Playfield.uLevel_X_Dimension + 1;     // unten rechts, 4.Reihe
    Playfield.nCentralMegaExplosionCoordinates[16] = (int)Playfield.uLevel_X_Dimension + 2;     // unten rechts, 4.Reihe
    Playfield.nCentralMegaExplosionCoordinates[17] = (int)Playfield.uLevel_X_Dimension * 2 - 1; // Ganz unten links
    Playfield.nCentralMegaExplosionCoordinates[18] = (int)Playfield.uLevel_X_Dimension * 2;     // Ganz unten mitte
    Playfield.nCentralMegaExplosionCoordinates[19] = (int)Playfield.uLevel_X_Dimension * 2 + 1; // Ganz unten rechts
}


/*----------------------------------------------------------------------------
Name:           InitYamExplosions
------------------------------------------------------------------------------
Beschreibung: Stellt für alle Yam-Explosionen das Element auf EMERALD_SPACE.
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: -
------------------------------------------------------------------------------*/
void InitYamExplosions(YAMEXPLOSION *pYamExplosions) {
    uint32_t I;     // Yam-Explosion
    uint32_t E;     // Element einer Yam-Explosion

    for (I = 0; I < EMERALD_MAX_YAM_EXPLOSIONS; I++) {
        for (E = 0; E < 9; E++) {
            pYamExplosions[I].uElement[E] = EMERALD_SPACE;
        }
    }
}


/*----------------------------------------------------------------------------
Name:           CheckLevelBorder
------------------------------------------------------------------------------
Beschreibung: Prüft, ob die Level-Umrandung mit "Stahl" an jeder Stelle abgrenzt.
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  int , 0 = OK, sonst Fehler
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
int CheckLevelBorder(void) {
    uint32_t I;
    int nErrorCode;

    nErrorCode = 0;
    // Obere Zeile prüfen
    for (I = 0; (I < Playfield.uLevel_X_Dimension) && (nErrorCode == 0); I++) {
        if (!IsSteel(Playfield.pLevel[I])) {
            SDL_Log("%s[top line]  invalid element (%x) found at %d",__FUNCTION__,Playfield.pLevel[I],I);
            nErrorCode = -1;
        }
    }
    // Untere Zeile prüfen
    for (I = Playfield.uLevel_X_Dimension * (Playfield.uLevel_Y_Dimension - 1); (I < Playfield.uLevel_X_Dimension * Playfield.uLevel_Y_Dimension) && (nErrorCode == 0); I++) {
        if (!IsSteel(Playfield.pLevel[I])) {
            SDL_Log("%s[buttom line]  invalid element (%x) found at %d",__FUNCTION__,Playfield.pLevel[I],I);
            nErrorCode = -1;
        }
    }
    // Linke Spalte prüfen
    for (I = 0; (I < Playfield.uLevel_Y_Dimension) && (nErrorCode == 0); I++) {

        if (!IsSteel(Playfield.pLevel[I * Playfield.uLevel_X_Dimension])) {
            SDL_Log("%s[left column]  invalid element (%x) found at %d",__FUNCTION__,Playfield.pLevel[I],I);
            nErrorCode = -1;
        }
    }
    // Rechte Spalte prüfen
    for (I = 1; (I <= Playfield.uLevel_Y_Dimension) && (nErrorCode == 0); I++) {
        if (!IsSteel(Playfield.pLevel[I * Playfield.uLevel_X_Dimension - 1])) {
            SDL_Log("%s[right column]  invalid element (%x) found at %d",__FUNCTION__,Playfield.pLevel[I],I);
            nErrorCode = -1;
        }
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           CheckReplicators
------------------------------------------------------------------------------
Beschreibung: Prüft, ob im Level die Replikatoren korrekt zusammengebaut sind.
Parameter
      Eingang: pLevel, uint16_t *, Zeiger auf Leveldaten
               uXdim, uint32_t, X-Dimension des Levels
               uYdim, uint32_t, Y-Dimension des Levels
      Ausgang: -
Rückgabewert:  int , 0 = OK, sonst Fehler
Seiteneffekte: -
------------------------------------------------------------------------------*/
int CheckReplicators(uint16_t *pLevel, uint32_t uXdim, uint32_t uYdim) {
    uint32_t I;
    uint32_t uMaxIndex;
    int nErrorCode;
    uint16_t uElement;

    if (pLevel == NULL) {
        return -1;
    }
    nErrorCode = 0;
    uMaxIndex = (uXdim * uYdim) - 1;
    for (I = 0; (I <= uMaxIndex) && (nErrorCode == 0); I++) {
        uElement = pLevel[I];
        switch (uElement) {
            // Roter Replikator
            case (EMERALD_REPLICATOR_RED_TOP_LEFT):
                if ((I + 1) <= uMaxIndex) {
                    if (pLevel[I + 1] == EMERALD_REPLICATOR_RED_TOP_MID) {
                        if ((I + uXdim) <= uMaxIndex) {
                            if (pLevel[I + uXdim] != EMERALD_REPLICATOR_RED_BOTTOM_LEFT) {
                                nErrorCode = -1;
                            }
                        } else {
                            nErrorCode = -1;
                        }
                    } else {
                        nErrorCode = -1;
                    }
                } else {
                    nErrorCode = -1;
                }
                break;
            case (EMERALD_REPLICATOR_RED_TOP_MID):
                if ((I - 1) >= 0) {
                    if (pLevel[I - 1] == EMERALD_REPLICATOR_RED_TOP_LEFT) {
                        if ((I + 1) <= uMaxIndex) {
                            if (pLevel[I + 1] != EMERALD_REPLICATOR_RED_TOP_RIGHT) {
                                nErrorCode = -1;
                            }
                        } else {
                            nErrorCode = -1;
                        }
                    } else {
                        nErrorCode = -1;
                    }
                } else {
                    nErrorCode = -1;
                }
                break;
            case (EMERALD_REPLICATOR_RED_TOP_RIGHT):
                if ((I - 1) >= 0) {
                    if (pLevel[I - 1] == EMERALD_REPLICATOR_RED_TOP_MID) {
                        if ((I + uXdim) <= uMaxIndex) {
                            if (pLevel[I + uXdim] != EMERALD_REPLICATOR_RED_BOTTOM_RIGHT) {
                                nErrorCode = -1;
                            }
                        } else {
                            nErrorCode = -1;
                        }
                    } else {
                        nErrorCode = -1;
                    }
                } else {
                    nErrorCode = -1;
                }
                break;
            case (EMERALD_REPLICATOR_RED_BOTTOM_LEFT):
                if ((I - uXdim) >= 0) {
                    if (pLevel[I - uXdim] != EMERALD_REPLICATOR_RED_TOP_LEFT) {
                        nErrorCode = -1;
                    }
                } else {
                    nErrorCode = -1;
                }
                break;
            case (EMERALD_REPLICATOR_RED_BOTTOM_RIGHT):
                if ((I - uXdim) >= 0) {
                    if (pLevel[I - uXdim] != EMERALD_REPLICATOR_RED_TOP_RIGHT) {
                        nErrorCode = -1;
                    }
                } else {
                    nErrorCode = -1;
                }
                break;
            // Grüner Replikator
            case (EMERALD_REPLICATOR_GREEN_TOP_LEFT):
                if ((I + 1) <= uMaxIndex) {
                    if (pLevel[I + 1] == EMERALD_REPLICATOR_GREEN_TOP_MID) {
                        if ((I + uXdim) <= uMaxIndex) {
                            if (pLevel[I + uXdim] != EMERALD_REPLICATOR_GREEN_BOTTOM_LEFT) {
                                nErrorCode = -1;
                            }
                        } else {
                            nErrorCode = -1;
                        }
                    } else {
                        nErrorCode = -1;
                    }
                } else {
                    nErrorCode = -1;
                }
                break;
            case (EMERALD_REPLICATOR_GREEN_TOP_MID):
                if ((I - 1) >= 0) {
                    if (pLevel[I - 1] == EMERALD_REPLICATOR_GREEN_TOP_LEFT) {
                        if ((I + 1) <= uMaxIndex) {
                            if (pLevel[I + 1] != EMERALD_REPLICATOR_GREEN_TOP_RIGHT) {
                                nErrorCode = -1;
                            }
                        } else {
                            nErrorCode = -1;
                        }
                    } else {
                        nErrorCode = -1;
                    }
                } else {
                    nErrorCode = -1;
                }
                break;
            case (EMERALD_REPLICATOR_GREEN_TOP_RIGHT):
                if ((I - 1) >= 0) {
                    if (pLevel[I - 1] == EMERALD_REPLICATOR_GREEN_TOP_MID) {
                        if ((I + uXdim) <= uMaxIndex) {
                            if (pLevel[I + uXdim] != EMERALD_REPLICATOR_GREEN_BOTTOM_RIGHT) {
                                nErrorCode = -1;
                            }
                        } else {
                            nErrorCode = -1;
                        }
                    } else {
                        nErrorCode = -1;
                    }
                } else {
                    nErrorCode = -1;
                }
                break;
            case (EMERALD_REPLICATOR_GREEN_BOTTOM_LEFT):
                if ((I - uXdim) >= 0) {
                    if (pLevel[I - uXdim] != EMERALD_REPLICATOR_GREEN_TOP_LEFT) {
                        nErrorCode = -1;
                    }
                } else {
                    nErrorCode = -1;
                }
                break;
            case (EMERALD_REPLICATOR_GREEN_BOTTOM_RIGHT):
                if ((I - uXdim) >= 0) {
                    if (pLevel[I - uXdim] != EMERALD_REPLICATOR_GREEN_TOP_RIGHT) {
                        nErrorCode = -1;
                    }
                } else {
                    nErrorCode = -1;
                }
                break;
            // Blauer Replikator
            case (EMERALD_REPLICATOR_BLUE_TOP_LEFT):
                if ((I + 1) <= uMaxIndex) {
                    if (pLevel[I + 1] == EMERALD_REPLICATOR_BLUE_TOP_MID) {
                        if ((I + uXdim) <= uMaxIndex) {
                            if (pLevel[I + uXdim] != EMERALD_REPLICATOR_BLUE_BOTTOM_LEFT) {
                                nErrorCode = -1;
                            }
                        } else {
                            nErrorCode = -1;
                        }
                    } else {
                        nErrorCode = -1;
                    }
                } else {
                    nErrorCode = -1;
                }
                break;
            case (EMERALD_REPLICATOR_BLUE_TOP_MID):
                if ((I - 1) >= 0) {
                    if (pLevel[I - 1] == EMERALD_REPLICATOR_BLUE_TOP_LEFT) {
                        if ((I + 1) <= uMaxIndex) {
                            if (pLevel[I + 1] != EMERALD_REPLICATOR_BLUE_TOP_RIGHT) {
                                nErrorCode = -1;
                            }
                        } else {
                            nErrorCode = -1;
                        }
                    } else {
                        nErrorCode = -1;
                    }
                } else {
                    nErrorCode = -1;
                }
                break;
            case (EMERALD_REPLICATOR_BLUE_TOP_RIGHT):
                if ((I - 1) >= 0) {
                    if (pLevel[I - 1] == EMERALD_REPLICATOR_BLUE_TOP_MID) {
                        if ((I + uXdim) <= uMaxIndex) {
                            if (pLevel[I + uXdim] != EMERALD_REPLICATOR_BLUE_BOTTOM_RIGHT) {
                                nErrorCode = -1;
                            }
                        } else {
                            nErrorCode = -1;
                        }
                    } else {
                        nErrorCode = -1;
                    }
                } else {
                    nErrorCode = -1;
                }
                break;
            case (EMERALD_REPLICATOR_BLUE_BOTTOM_LEFT):
                if ((I - uXdim) >= 0) {
                    if (pLevel[I - uXdim] != EMERALD_REPLICATOR_BLUE_TOP_LEFT) {
                        nErrorCode = -1;
                    }
                } else {
                    nErrorCode = -1;
                }
                break;
            case (EMERALD_REPLICATOR_BLUE_BOTTOM_RIGHT):
                if ((I - uXdim) >= 0) {
                    if (pLevel[I - uXdim] != EMERALD_REPLICATOR_BLUE_TOP_RIGHT) {
                        nErrorCode = -1;
                    }
                } else {
                    nErrorCode = -1;
                }
                break;
            // Gelber Replikator
            case (EMERALD_REPLICATOR_YELLOW_TOP_LEFT):
                if ((I + 1) <= uMaxIndex) {
                    if (pLevel[I + 1] == EMERALD_REPLICATOR_YELLOW_TOP_MID) {
                        if ((I + uXdim) <= uMaxIndex) {
                            if (pLevel[I + uXdim] != EMERALD_REPLICATOR_YELLOW_BOTTOM_LEFT) {
                                nErrorCode = -1;
                            }
                        } else {
                            nErrorCode = -1;
                        }
                    } else {
                        nErrorCode = -1;
                    }
                } else {
                    nErrorCode = -1;
                }
                break;
            case (EMERALD_REPLICATOR_YELLOW_TOP_MID):
                if ((I - 1) >= 0) {
                    if (pLevel[I - 1] == EMERALD_REPLICATOR_YELLOW_TOP_LEFT) {
                        if ((I + 1) <= uMaxIndex) {
                            if (pLevel[I + 1] != EMERALD_REPLICATOR_YELLOW_TOP_RIGHT) {
                                nErrorCode = -1;
                            }
                        } else {
                            nErrorCode = -1;
                        }
                    } else {
                        nErrorCode = -1;
                    }
                } else {
                    nErrorCode = -1;
                }
                break;
            case (EMERALD_REPLICATOR_YELLOW_TOP_RIGHT):
                if ((I - 1) >= 0) {
                    if (pLevel[I - 1] == EMERALD_REPLICATOR_YELLOW_TOP_MID) {
                        if ((I + uXdim) <= uMaxIndex) {
                            if (pLevel[I + uXdim] != EMERALD_REPLICATOR_YELLOW_BOTTOM_RIGHT) {
                                nErrorCode = -1;
                            }
                        } else {
                            nErrorCode = -1;
                        }
                    } else {
                        nErrorCode = -1;
                    }
                } else {
                    nErrorCode = -1;
                }
                break;
            case (EMERALD_REPLICATOR_YELLOW_BOTTOM_LEFT):
                if ((I - uXdim) >= 0) {
                    if (pLevel[I - uXdim] != EMERALD_REPLICATOR_YELLOW_TOP_LEFT) {
                        nErrorCode = -1;
                    }
                } else {
                    nErrorCode = -1;
                }
                break;
            case (EMERALD_REPLICATOR_YELLOW_BOTTOM_RIGHT):
                if ((I - uXdim) >= 0) {
                    if (pLevel[I - uXdim] != EMERALD_REPLICATOR_YELLOW_TOP_RIGHT) {
                        nErrorCode = -1;
                    }
                } else {
                    nErrorCode = -1;
                }
                break;
            default:
                // kein Replikator-Element
                break;
        }
    }
    if (nErrorCode != 0) {
        SDL_Log("%s: bad replicator found",__FUNCTION__);
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           CheckAcidPools
------------------------------------------------------------------------------
Beschreibung: Prüft, ob im Level die Säurebecken korrekt zusammengebaut sind.
Parameter
      Eingang: pLevel, uint16_t *, Zeiger auf Leveldaten
               uXdim, uint32_t, X-Dimension des Levels
               uYdim, uint32_t, Y-Dimension des Levels
      Ausgang: -
Rückgabewert:  int , 0 = OK, sonst Fehler
Seiteneffekte: -
------------------------------------------------------------------------------*/
int CheckAcidPools(uint16_t *pLevel, uint32_t uXdim, uint32_t uYdim) {
    uint32_t I;
    uint32_t uMaxIndex;
    int nErrorCode;
    uint16_t uElement;

    if (pLevel == NULL) {
        return -1;
    }
    nErrorCode = 0;
    uMaxIndex = (uXdim * uYdim) - 1;
    for (I = 0; (I <= uMaxIndex) && (nErrorCode == 0); I++) {
        uElement = pLevel[I];
        switch (uElement) {
            // Roter Replikator
            case (EMERALD_ACIDPOOL_TOP_LEFT):
                if ((I + uXdim) <= uMaxIndex) {     // Schließt folgende Prüfung (I + 1) mit ein
                    if (pLevel[I + uXdim] == EMERALD_ACIDPOOL_BOTTOM_LEFT) {
                        if (pLevel[I + 1] != EMERALD_ACIDPOOL_TOP_MID) {
                            nErrorCode = -1;
                        }
                    } else {
                        nErrorCode = -1;
                    }
                } else {
                    nErrorCode = -1;
                }
                break;
            case (EMERALD_ACIDPOOL_TOP_MID):
                if ((I - 1) >= 0) {
                    if ((pLevel[I - 1] == EMERALD_ACIDPOOL_TOP_MID) || (pLevel[I - 1] == EMERALD_ACIDPOOL_TOP_LEFT)) {
                        if ((I + uXdim) <= uMaxIndex) {     // Schließt folgende Prüfung (I + 1) mit ein
                            if (pLevel[I + uXdim] == EMERALD_ACIDPOOL_BOTTOM_MID) {
                                if ((pLevel[I + 1] != EMERALD_ACIDPOOL_TOP_MID) && (pLevel[I + 1] != EMERALD_ACIDPOOL_TOP_RIGHT)) {
                                    nErrorCode = -1;
                                }
                            } else {
                                nErrorCode = -1;
                            }
                        } else {
                            nErrorCode = -1;
                        }
                    } else {
                        nErrorCode = -1;
                    }
                } else {
                    nErrorCode = -1;
                }
                break;
            case (EMERALD_ACIDPOOL_TOP_RIGHT):
                if ((I - 1) >= 0) {
                    if (pLevel[I - 1] == EMERALD_ACIDPOOL_TOP_MID) {
                        if ((I + uXdim) <= uMaxIndex) {     // Schließt folgende Prüfung (I + 1) mit ein
                            if (pLevel[I + uXdim] != EMERALD_ACIDPOOL_BOTTOM_RIGHT) {
                                nErrorCode = -1;
                            }
                        } else {
                            nErrorCode = -1;
                        }
                    } else {
                        nErrorCode = -1;
                    }
                } else {
                    nErrorCode = -1;
                }
                break;
            case (EMERALD_ACIDPOOL_BOTTOM_LEFT):
                if ((I - uXdim) >= 0) {
                    if (pLevel[I - uXdim] == EMERALD_ACIDPOOL_TOP_LEFT) {
                        if ((I + 1) <= uMaxIndex) {
                            if (pLevel[I + 1] != EMERALD_ACIDPOOL_BOTTOM_MID) {
                                nErrorCode = -1;
                            }
                        } else {
                            nErrorCode = -1;
                        }
                    } else {
                        nErrorCode = -1;
                    }
                } else {
                    nErrorCode = -1;
                }
                break;
            case (EMERALD_ACIDPOOL_BOTTOM_MID):
                if ((I - uXdim) >= 0) { // Schließt folgende Prüfung (I - 1) mit ein
                    if (pLevel[I - uXdim] == EMERALD_ACIDPOOL_TOP_MID) {
                        if ((pLevel[I - 1] == EMERALD_ACIDPOOL_BOTTOM_MID) || (pLevel[I - 1] == EMERALD_ACIDPOOL_BOTTOM_LEFT)) {
                            if ((I + 1) <= uMaxIndex) {
                                if ((pLevel[I + 1] != EMERALD_ACIDPOOL_BOTTOM_MID) && (pLevel[I + 1] != EMERALD_ACIDPOOL_BOTTOM_RIGHT)) {
                                    nErrorCode = -1;
                                }
                            } else {
                                nErrorCode = -1;
                            }
                        } else {
                            nErrorCode = -1;
                        }
                    } else {
                        nErrorCode = -1;
                    }
                } else {
                    nErrorCode = -1;
                }
                break;
            case (EMERALD_ACIDPOOL_BOTTOM_RIGHT):
                if ((I - uXdim) >= 0) { // Schließt folgende Prüfung (I - 1) mit ein
                    if (pLevel[I - uXdim] == EMERALD_ACIDPOOL_TOP_RIGHT) {
                        if (pLevel[I - 1] != EMERALD_ACIDPOOL_BOTTOM_MID) {
                            nErrorCode = -1;
                        }
                    } else {
                        nErrorCode = -1;
                    }
                } else {
                    nErrorCode = -1;
                }
                break;
        }
    }
    if (nErrorCode != 0) {
        SDL_Log("%s: bad acid pool found",__FUNCTION__);
    }
    return nErrorCode;
}

/*----------------------------------------------------------------------------
Name:           GetManCoordinates
------------------------------------------------------------------------------
Beschreibung: Ermittelt die X- und Y-Koordinate des Man im Level.

Parameter
      Eingang: pLevel, uint16_t *, Zeiger auf Leveldaten
               uXdim, uint32_t, X-Dimension des Levels
               uYdim, uint32_t, Y-Dimension des Levels
      Ausgang: puManXpos, uint32_t *, X-Koordinate des Man, darf NULL sein
               puManYpos, uint32_t *, Y-Koordinate des Man, darf NULL sein
Rückgabewert:  int , 0 = OK, sonst Fehler
Seiteneffekte: -
------------------------------------------------------------------------------*/
int GetManCoordinates(uint16_t *pLevel, uint32_t uXdim, uint32_t uYdim, uint32_t *puManXpos, uint32_t *puManYpos) {
    int nErrorcode;
    int nManCount;
    uint32_t I;                 // Index im Level
    uint32_t uLinCoordinate;    // Lineare Koordinate

    nManCount = 0;
    nErrorcode = -1;
    // Das Gröbste wurde bereits in InitialisePlayfield() geprüft
    if (pLevel != NULL) {
        // Prüfen, ob sich genau ein Man im Level befindet
        for (I = 0; I < (uXdim * uYdim); I++) {
            if (pLevel[I] == EMERALD_MAN) {
                uLinCoordinate = I;
                nManCount++;
            }
        }
        if (nManCount == 1) {
            // Lineare Koordinate in X- und Y-Koordinate zerlegen
            if (puManXpos != NULL) {
                *puManXpos = uLinCoordinate % uXdim;
            }
            if (puManYpos != NULL) {
                *puManYpos = uLinCoordinate / uXdim;
            }
            nErrorcode = 0;
        } else {
            SDL_Log("%s: invalid man count: %d",__FUNCTION__,nManCount);
        }

    }
    return nErrorcode;
}


/*----------------------------------------------------------------------------
Name:           CloseAllDoors
------------------------------------------------------------------------------
Beschreibung: Schließt alle Türen (Timer+Schalter) in einem Level.

Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void CloseAllDoors(void) {
    uint32_t I;                 // Index im Level

    for (I = 0; I < (Playfield.uLevel_X_Dimension * Playfield.uLevel_Y_Dimension); I++) {
        if (Playfield.pLevel[I] == EMERALD_DOOR_TIME) {
            Playfield.pStatusAnimation[I] = EMERALD_STATUS_DOOR_CLOSE;
        }
    }
}


/*----------------------------------------------------------------------------
Name:           SetActiveDynamiteP1
------------------------------------------------------------------------------
Beschreibung: Setzt alle aktiven Dynamits auf Phase 1.

Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void SetActiveDynamiteP1(void) {
    uint32_t I;                 // Index im Level

    for (I = 0; I < (Playfield.uLevel_X_Dimension * Playfield.uLevel_Y_Dimension); I++) {
        if (Playfield.pLevel[I] == EMERALD_DYNAMITE_ON) {
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_DYNAMITE_ON_P1;
        }
    }
}


/*----------------------------------------------------------------------------
Name:           PrintPlayfieldValues
------------------------------------------------------------------------------
Beschreibung: Zeigt die Werte der Struktur Playfield.x an.

Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void PrintPlayfieldValues() {
    if (Playfield.bInitOK) {
        printf("Playfield-Values\r\n");
        printf("================\r\n");
        printf("InitOK:                      %d\r\n",Playfield.bInitOK);
        printf("Welldone:                    %d\r\n",Playfield.bWellDone);
        printf("ManDead:                     %d\r\n",Playfield.bManDead);
        printf("Version:                     %s\r\n",Playfield.szVersion);       // z.B. "01.00"
        printf("Level title:                 %s\r\n",Playfield.szLevelTitle);
        printf("Level author:                %s\r\n",Playfield.szLevelAuthor);
        printf("Score Emerald:               %u\r\n",Playfield.uScoreEmerald);
        printf("Score Ruby:                  %u\r\n",Playfield.uScoreRuby);
        printf("Score Saphire:               %u\r\n",Playfield.uScoreSaphir);
        printf("Score Perl:                  %u\r\n",Playfield.uScorePerl);
        printf("Score crystal:               %u\r\n",Playfield.uScoreCrystal);
        printf("Score Letter:                %u\r\n",Playfield.uScoreMessage);
        printf("Score Key:                   %u\r\n",Playfield.uScoreKey);
        printf("Score Dynamite:              %u\r\n",Playfield.uScoreDynamite);
        printf("Score Hammer:                %u\r\n",Playfield.uScoreHammer);
        printf("Score Nut cracking:          %u\r\n",Playfield.uScoreNutCracking);
        printf("Score stoning beetle:        %u\r\n",Playfield.uScoreStoningBeetle);
        printf("Score stoning mine:          %u\r\n",Playfield.uScoreStoningMine);
        printf("Score stoning alien:         %u\r\n",Playfield.uScoreStoningAlien);
        printf("Score stoning yam:           %u\r\n",Playfield.uScoreStoningYam);
        printf("Score time coin:             %u\r\n",Playfield.uScoreTimeCoin);
        printf("Additional time for coin:    %u\r\n",Playfield.uAdditonalTimeCoinTime);
        printf("Emerald to collect:          %u\r\n",Playfield.uEmeraldsToCollect);
        printf("Dynamite Count:              %u\r\n",Playfield.uDynamiteCount);
        printf("Hammer Count:                %u\r\n",Playfield.uHammerCount);
        printf("White key Count:             %u\r\n",Playfield.uWhiteKeyCount);
        printf("Time Score Factor:           %u\r\n",Playfield.uTimeScoreFactor);
        printf("Cheese spread speed:         %u\r\n",Playfield.uCheeseSpreadSpeed);
        printf("Grass spread speed:          %u\r\n",Playfield.uCheeseSpreadSpeed);
        printf("Time to play:                %u\r\n",Playfield.uTimeToPlay);
        printf("Time Wheel Rotation:         %u\r\n",Playfield.uTimeWheelRotation);
        printf("Time Wheel Rotation Left:    %u\r\n",Playfield.uTimeWheelRotationLeft);
        printf("Time magic wall:             %u\r\n",Playfield.uTimeMagicWall);
        printf("Time light:                  %u\r\n",Playfield.uTimeLight);
        printf("Time TimeDoor:               %u\r\n",Playfield.uTimeDoorTime);
        printf("Replicator red, element:     %02X\r\n",Playfield.uReplicatorRedObject);
        printf("Replicator red, on:          %d\r\n",Playfield.bReplicatorRedOn);
        printf("Replicator green, element:   %02X\r\n",Playfield.uReplicatorGreenObject);
        printf("Replicator green, on:        %d\r\n",Playfield.bReplicatorGreenOn);
        printf("Replicator blue, element:    %02X\r\n",Playfield.uReplicatorBlueObject);
        printf("Replicator blue, on:         %d\r\n",Playfield.bReplicatorBlueOn);
        printf("Replicator yellow, element:  %02X\r\n",Playfield.uReplicatorYellowObject);
        printf("Replicator yellow, on:       %d\r\n",Playfield.bReplicatorYellowOn);
        printf("Lighbarrier red, on:         %d\r\n",Playfield.bLightBarrierRedOn);
        printf("Lighbarrier green, on:       %d\r\n",Playfield.bLightBarrierGreenOn);
        printf("Lighbarrier blue, on:        %d\r\n",Playfield.bLightBarrierBlueOn);
        printf("Lighbarrier yellow, on:      %d\r\n",Playfield.bLightBarrierYellowOn);
        printf("MaxYamExplosionIndex:        %d\r\n",Playfield.uMaxYamExplosionIndex);
        printf("Total Score:                 %u\r\n",Playfield.uTotalScore);            // Total-Score
        printf("Level Dimension:             X(%u) x Y(%u)\r\n",Playfield.uLevel_X_Dimension,Playfield.uLevel_Y_Dimension);
        printf("visible Dimension:           X(%u) x Y(%u)\r\n",Playfield.uVisibleX,Playfield.uVisibleY);   // Sichtbarer Bereich (nur von Fenstergröße abhängig)
        printf("visible Center:              X(%u) / Y(%u)\r\n",Playfield.uVisibleCenterX,Playfield.uVisibleCenterY);   // Zentrum des sichtbaren Bereichs (nur von Fenstergröße abhängig)
        printf("top left Pixel Position:     X(%d) / Y(%d)\r\n",Playfield.nTopLeftXpos,Playfield.nTopLeftYpos);           // aktuelle X/Y-Pixelposition, abhängig von Man position
        printf("Man Position:                X(%u) / Y(%u)\r\n",Playfield.uManXpos,Playfield.uManYpos);   // Man-X/Y-Element-Koordinate
        printf("FrameCounter:                %u\r\n",Playfield.uFrameCounter);
        printf("MD5 hash for leveldata:      %s\r\n",Playfield.szMd5String);
        // PrintTeleporters();
    } else {
        printf("Error in level data, can't show playfield values\r\n");
    }
}


/*----------------------------------------------------------------------------
Name:           CalculateLevelGroupMd5Hash
------------------------------------------------------------------------------
Beschreibung: Berechnet den MD5-Hash einer Levelgruppe.


Parameter
      Eingang: puLevelgroupXml, uint8_t *, Zeiger auf Levelgruppen-XML-Daten, muss 0-terminiert (String) sein
      Ausgang: puMd5Hash, uint8_t, Zeiger auf mindestens 16 Bytes für MD5-Hash, nur gültig, wenn Rückgabe = 0
Rückgabewert:  0 = Alles OK, sonst Fehler
Seiteneffekte: -
------------------------------------------------------------------------------*/
int CalculateLevelGroupMd5Hash(uint8_t *puLevelgroupXml,uint8_t *puMd5Hash) {
    int nErrorCode = -1;
    uint8_t *puLevelgroupMd5TagStart;   // An dieser Stelle startet das Tag <levelgroup_md5_hash>
    MD5Context MD5Leveldata;
    uint32_t uLevelgroupSize;

    if ((puLevelgroupXml != NULL) && (puMd5Hash != NULL)) {
        if (strlen((char*)puLevelgroupXml) > 0) {
            if ((strstr((char*)puLevelgroupXml,"<levelgroup>") != NULL) && (strstr((char*)puLevelgroupXml,"</levelgroup>") != NULL) ) {  // levelgroup tags gefunden?
                puLevelgroupMd5TagStart = (uint8_t*)strstr((char*)puLevelgroupXml,"<levelgroup_md5_hash>");
                if (puLevelgroupMd5TagStart != NULL) {
                    if (puLevelgroupMd5TagStart > puLevelgroupXml) {
                        uLevelgroupSize = puLevelgroupMd5TagStart - puLevelgroupXml;
                        md5Init(&MD5Leveldata);
                        md5Update(&MD5Leveldata,puLevelgroupXml,uLevelgroupSize);
                        md5Finalize(&MD5Leveldata);
                        memcpy(puMd5Hash,MD5Leveldata.digest,16);
                        nErrorCode = 0;
                    } else {
                        SDL_Log("%s: <levelgroup_md5_hash> pointer has wrong position",__FUNCTION__);
                    }
                } else {
                    SDL_Log("%s: <levelgroup_md5_hash> tag not found",__FUNCTION__);
                }
            } else {
                SDL_Log("%s: levelgroup tags not found",__FUNCTION__);
            }
        } else {
            SDL_Log("%s: empty file",__FUNCTION__);
        }
    } else {
        SDL_Log("%s: bad parameter",__FUNCTION__);
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           InitLevelgroups
------------------------------------------------------------------------------
Beschreibung: Initialisiert die Strukturen für die Levelgruppen.


Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: LevelgroupFiles[].x, g_LevelgroupFilesCount, SelectedLevelgroup.x
------------------------------------------------------------------------------*/
void InitLevelgroups(void) {
    memset(&LevelgroupFiles,0,sizeof(LevelgroupFiles));
    memset(&SelectedLevelgroup,0,sizeof(SelectedLevelgroup));
    g_LevelgroupFilesCount = 0;
}


/*----------------------------------------------------------------------------
Name:           GetLevelgroupFiles
------------------------------------------------------------------------------
Beschreibung: Ermittelt die Levelgruppen-Dateien im Arbeitsverzeichnis und stellt
              diese in der Struktur LevelgroupFiles[].x zur Verfügung.

              WICHTIG: Es muss sichergestellt werden, dass nach diesem Aufruf mindestens
              eine Levelgruppe zur Verfügung steht. Kann keine der bestehenden Levelgruppen
              verwendet werden, so wird eine Default-Gruppe angelegt.

Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  0 = alles OK, sonst Fehler
Seiteneffekte: LevelgroupFiles[].x, g_LevelgroupFilesCount
------------------------------------------------------------------------------*/
int GetLevelgroupFiles(void) {
    int nErrorCode;
    DIR *dir;
    struct dirent *entry;
    size_t FilenameLen;
    uint32_t uXmlLen;
    uint8_t *pXml = NULL;
    ezxml_t xml = NULL;
    ezxml_t levelgroupname,levelcount,levelgrouphash,passwordhash,createdate;
    uint8_t uCalculatedLevelgroupMd5Hash[16];       // berechnet
    uint8_t uLevelgroupMd5Hash[16];                 // gelesen
    char szFullFilename[EMERALD_MAX_FILENAME_LEN * 2];


    g_LevelgroupFilesCount = 0;
    nErrorCode = -1;
    if ((dir = opendir(EMERALD_LEVELGROUPS_DIRECTORYNAME)) == NULL) {
        SDL_Log("%s: can not open current directory, error: %s",__FUNCTION__,strerror(errno));
        return nErrorCode;
    } else {
        nErrorCode = 0;
        while (((entry = readdir(dir)) != NULL) && (g_LevelgroupFilesCount < EMERALD_MAX_LEVELGROUPFILES) ) {
            if (strlen(entry->d_name) < EMERALD_MAX_FILENAME_LEN)   {   // Kann gefahrlos Verzeichnisname und Filename addiert werden ?
                sprintf(szFullFilename,"%s/%s",EMERALD_LEVELGROUPS_DIRECTORYNAME,entry->d_name);
                FilenameLen = strlen(szFullFilename);
            } else {
                FilenameLen = 0;    // Fehler
            }
            if ((FilenameLen > 4) && (FilenameLen <= EMERALD_MAX_FILENAME_LEN)) {          // a.xml    muss es wenigstens sein
                // Nur XML-Dateien prüfen
                if ((memcmp(szFullFilename + FilenameLen - 4,".xml",4) == 0) || (memcmp(szFullFilename + FilenameLen - 4,".XML",4) == 0)) {
                    pXml = ReadFile(szFullFilename,&uXmlLen);
                    if (pXml != NULL) {
                        if ((strstr((char*)pXml,"<levelgroup>") != NULL) && (strstr((char*)pXml,"</levelgroup>") != NULL)) {  // levelgroup tags gefunden?
                            if (CalculateLevelGroupMd5Hash(pXml,uCalculatedLevelgroupMd5Hash) == 0) { // muss vor ezxml_parse_str() durchgeführt werden, da Library Original ändert
                                //GetMd5String2(uCalculatedLevelgroupMd5Hash);
                                //SDL_Log("LG: %s  --> Hash: %s",szFullFilename,p1);
                                xml = ezxml_parse_str((char*)pXml,strlen((char*)pXml));
                                if (xml != NULL) {
                                    levelgroupname = ezxml_child(xml,"groupname");
                                    levelcount = ezxml_child(xml,"levelcount");
                                    passwordhash = ezxml_child(xml,"password_md5_hash");
                                    createdate = ezxml_child(xml,"create_timestamp");
                                    if ( (levelgroupname != NULL) && (levelcount != NULL) && (passwordhash != NULL) && (createdate != NULL) && (ezxml_child(xml,"level000") != NULL) ) {
                                        levelgrouphash = ezxml_child(xml,"levelgroup_md5_hash");
                                        if (levelgrouphash != NULL) {
                                            // Stimmt der berechnete Hash mit dem Gelesenen?
                                            GetMd5HashFromString(levelgrouphash->txt,uLevelgroupMd5Hash);
                                            if (memcmp(uLevelgroupMd5Hash,uCalculatedLevelgroupMd5Hash,16) == 0) {
                                                strcpy(LevelgroupFiles[g_LevelgroupFilesCount].szFilename,szFullFilename); // Maximale Länge wurde bereits geprüft
                                                if ((strlen(levelgroupname->txt) > 0) && (strlen(levelgroupname->txt) <= EMERALD_GROUPNAME_LEN)) {
                                                    strcpy(LevelgroupFiles[g_LevelgroupFilesCount].szLevelgroupname,levelgroupname->txt);
                                                } else {
                                                    strcpy(LevelgroupFiles[g_LevelgroupFilesCount].szLevelgroupname,"NO GROUP NAME");
                                                }
                                                if (strlen(passwordhash->txt) == 32) {
                                                    strcpy(LevelgroupFiles[g_LevelgroupFilesCount].szPasswordHash,passwordhash->txt);
                                                } else {
                                                    strcpy(LevelgroupFiles[g_LevelgroupFilesCount].szPasswordHash,"\0");
                                                }
                                                if (strlen(createdate->txt) == 15) {
                                                    strcpy(LevelgroupFiles[g_LevelgroupFilesCount].szCreateTimestamp,createdate->txt);
                                                } else {
                                                    GetActualTimestamp(LevelgroupFiles[g_LevelgroupFilesCount].szCreateTimestamp);
                                                }
                                                LevelgroupFiles[g_LevelgroupFilesCount].uLevelCount = (uint32_t)strtol(levelcount->txt,NULL,10);
                                                memcpy(LevelgroupFiles[g_LevelgroupFilesCount].uMd5Hash,uCalculatedLevelgroupMd5Hash,16);
                                                g_LevelgroupFilesCount++;
                                            } else {
                                                GetMd5String2(uCalculatedLevelgroupMd5Hash);
                                                SDL_Log("%s: can not use levelgroup, bad hash: %s",__FUNCTION__,GetMd5String2(uCalculatedLevelgroupMd5Hash));
                                            }
                                        }
                                    }
                                    SAFE_FREE(xml);
                                }
                            }
                        }
                        SAFE_FREE(pXml);
                    }
                }
            }
        }
        closedir(dir);
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           GetLevelgroupIndexByHash
------------------------------------------------------------------------------
Beschreibung: Ermittelt den Levelgruppenindex anhand des Levelgruppen-MD5-Hashes.
              Vor Aufruf dieser Funktion muss mit GetLevelgroupFiles() eine Liste der Levelgruppen ermittelt worden sein.
Parameter
      Eingang: puLevelgroupMd5Hash, uint8_t *, Levelgruppen-MD5-Hash
      Ausgang: -
Rückgabewert:  uint32_t, Levelgruppenindex, Falls Hash nicht gefunden werden kann, wird -1 zurückgegeben
Seiteneffekte: LevelgroupFiles[].x, g_LevelgroupFilesCount
------------------------------------------------------------------------------*/
uint32_t GetLevelgroupIndexByHash(uint8_t *puLevelgroupMd5Hash) {
    uint32_t G;
    uint32_t uIndex;
    bool bFound = false;

    uIndex = -1;
    if (puLevelgroupMd5Hash != NULL) {
        for (G = 0; (G < g_LevelgroupFilesCount) && (!bFound); G++) {
            if (memcmp(puLevelgroupMd5Hash,LevelgroupFiles[G].uMd5Hash,16) == 0) {
                uIndex = G;
                bFound = true;
            }
        }
    }
    if (!bFound) {
        SDL_Log("%s: hash not found",__FUNCTION__);
    }
    return uIndex;
}


/*----------------------------------------------------------------------------
Name:           SelectLevelgroup
------------------------------------------------------------------------------
Beschreibung: Wählt eine Levelgruppe aus. Vor Aufruf dieser Funktion muss mit
              GetLevelgroupFiles() eine Liste der Levelgruppen ermittelt worden sein.
              Bei erfolgreicher Auswahl der Levelgruppe wird die Struktur
              SelectedLevelgroup.x befüllt.
Parameter
      Eingang: puLevelgroupMd5Hash, uint8_t *, Levelgruppen-MD5-Hash
               bReadWriteHighscores, bool, true = Highscores lesen/schreiben, false = Highscores nicht lesen/schreiben
      Ausgang: -
Rückgabewert:  0 = alles OK, sonst Fehler
Seiteneffekte: LevelgroupFiles[].x, g_LevelgroupFilesCount, SelectedLevelgroup.x.
               Config.x
------------------------------------------------------------------------------*/
int SelectLevelgroup(uint8_t *puLevelgroupMd5Hash, bool bReadWriteHighscores) {
    int nErrorCode;
    uint32_t uLevelNumber;
    uint32_t uXmlLen;
    uint32_t uLevelgroupIndex;
    uint8_t *pXml = NULL;
    char szLevelNumberTag[16];              // level000
    ezxml_t xml = NULL;
    ezxml_t level = NULL;
    ezxml_t leveltitle,levelauthor;

    nErrorCode = -1;
    memset(&SelectedLevelgroup,0,sizeof(SelectedLevelgroup));                   // setzt SelectedLevelgroup.bOK auch auf false
    uLevelgroupIndex = GetLevelgroupIndexByHash(puLevelgroupMd5Hash);
    if ((uLevelgroupIndex >= 0) && (uLevelgroupIndex < g_LevelgroupFilesCount)) {
        pXml = ReadFile(LevelgroupFiles[uLevelgroupIndex].szFilename,&uXmlLen);     // Levelgruppen-Datei einlesen
        if (pXml != NULL) {
            xml = ezxml_parse_str((char*)pXml,strlen((char*)pXml));
            if (xml != NULL) {
                nErrorCode = 0;
                for (uLevelNumber = 0; (uLevelNumber < LevelgroupFiles[uLevelgroupIndex].uLevelCount) && (nErrorCode == 0); uLevelNumber++) {
                    sprintf(szLevelNumberTag,"level%03u",uLevelNumber); // Levelnummern-Tag bauen
                    level = ezxml_child(xml,szLevelNumberTag);          // Level im xml auswählen
                    leveltitle = ezxml_child(level,"title");
                    levelauthor = ezxml_child(level,"author");
                    if ((leveltitle != NULL) && (levelauthor != NULL)) {
                        if ((strlen(leveltitle->txt) <= EMERALD_TITLE_LEN) && (strlen(levelauthor->txt) <= EMERALD_AUTHOR_LEN)) {
                            strcpy(SelectedLevelgroup.szLevelTitle[uLevelNumber],leveltitle->txt);
                            strcpy(SelectedLevelgroup.szLevelAuthor[uLevelNumber],levelauthor->txt);
                        } else {
                            SDL_Log("%s: invalid strings for title or author found, levelnumber: %u, file %s",__FUNCTION__,uLevelNumber,LevelgroupFiles[uLevelgroupIndex].szFilename);
                            nErrorCode = -1;
                        }
                    } else {
                        SDL_Log("%s: can not determine level title or level author, levelnumber: %u, file %s",__FUNCTION__,uLevelNumber,LevelgroupFiles[uLevelgroupIndex].szFilename);
                        nErrorCode = -1;
                    }
                }
                SAFE_FREE(xml);
            } else {
                SDL_Log("%s: can not parse xml file %s",__FUNCTION__,LevelgroupFiles[uLevelgroupIndex].szFilename);
            }
        } else {
            SDL_Log("%s: can not read level group file %s",__FUNCTION__,LevelgroupFiles[uLevelgroupIndex].szFilename);
        }
    } else {
        SDL_Log("%s: invalid level group index",__FUNCTION__);
    }
    SAFE_FREE(pXml);
    if (nErrorCode == 0) {
        strcpy(SelectedLevelgroup.szFilename,LevelgroupFiles[uLevelgroupIndex].szFilename);
        strcpy(SelectedLevelgroup.szLevelgroupname,LevelgroupFiles[uLevelgroupIndex].szLevelgroupname);
        SelectedLevelgroup.uLevelCount = LevelgroupFiles[uLevelgroupIndex].uLevelCount;
        SelectedLevelgroup.bOK = true;
        memcpy(SelectedLevelgroup.uMd5Hash,LevelgroupFiles[uLevelgroupIndex].uMd5Hash,16);
        strcpy(SelectedLevelgroup.szPasswordHash,LevelgroupFiles[uLevelgroupIndex].szPasswordHash);
        memcpy(Config.uLevelgroupMd5Hash,LevelgroupFiles[uLevelgroupIndex].uMd5Hash,16);
        nErrorCode = WriteConfigFile();
        if ((nErrorCode == 0) && (bReadWriteHighscores)) {
            nErrorCode = ReadHighScoreFile(SelectedLevelgroup.uMd5Hash);
        }
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           SelectAlternativeLevelgroup
------------------------------------------------------------------------------
Beschreibung: Wählt eine Levelgruppe aus. Vor Aufruf dieser Funktion muss mit
              GetLevelgroupFiles() eine Liste der Levelgruppen ermittelt worden sein.
              Bei erfolgreicher Auswahl der Levelgruppe wird die Struktur
              SelectedLevelgroup.x befüllt.

              Im Gegensatz zur Funktion SelectLevelgroup() wird ggf. eine andere
              Levelgruppe selektiert, wenn die Gewünschte nicht selektierbar ist.
Parameter
      Eingang: puLevelgroupMd5Hash, uint8_t *, Levelgruppen-MD5-Hash
               bReadWriteHighscores, bool, true = Highscores lesen/schreiben, false = Highscores nicht lesen/schreiben
      Ausgang: -
Rückgabewert:  0 = alles OK, sonst Fehler
Seiteneffekte: LevelgroupFiles[].x, g_LevelgroupFilesCount
------------------------------------------------------------------------------*/
int SelectAlternativeLevelgroup(uint8_t *puLevelgroupMd5Hash, bool bReadWriteHighscores) {
    int nErrorCode;
    uint32_t G;
    bool bSelected = false;

    // Zunächst versuchen die gewünschte Levelgruppe zu selektieren
    nErrorCode = SelectLevelgroup(puLevelgroupMd5Hash,bReadWriteHighscores);
    if (nErrorCode != 0) {
        SDL_Log("%s: searching for an alternative levelgroup ...",__FUNCTION__);
        // Falls das nicht klappt, die Nächstbeste selektieren
        for (G = 0; (G < g_LevelgroupFilesCount) && (!bSelected); G++) {
            if (SelectLevelgroup(LevelgroupFiles[G].uMd5Hash,bReadWriteHighscores) == 0) {
                bSelected = true;
                nErrorCode = 0;
                SDL_Log("%s: found an alternative levelgroup, OK",__FUNCTION__);
            }
        }
        if (!bSelected) {
            // Jetzt ist Holland in Not!
            SDL_Log("%s: can not select an alternative levelgroup!",__FUNCTION__);
        }
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           ShowSelectedLevelgroup
------------------------------------------------------------------------------
Beschreibung: Zeigt Informationen der ausgewählten Levelgruppe an.

Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: SelectedLevelgroup.x
------------------------------------------------------------------------------*/
void ShowSelectedLevelgroup(void) {
    uint32_t L;

    if (SelectedLevelgroup.bOK) {
        SDL_Log("==================================================");
        SDL_Log("Selected Levelgroup:  %s",SelectedLevelgroup.szLevelgroupname);
        SDL_Log("Filename:             %s",SelectedLevelgroup.szFilename);
        SDL_Log("Level count:          %u",SelectedLevelgroup.uLevelCount);
        SDL_Log("Password hash:        %s",SelectedLevelgroup.szPasswordHash);
        for (L = 0; L < SelectedLevelgroup.uLevelCount; L++) {
            SDL_Log("Level: %03u    Level title: %s    Level author: %s",L,SelectedLevelgroup.szLevelTitle[L],SelectedLevelgroup.szLevelAuthor[L]);
        }
        SDL_Log("==================================================");
    } else {
        SDL_Log("%s: no level group selected",__FUNCTION__);
    }
}


/*----------------------------------------------------------------------------
Name:           ShowAvailableLevelgroups
------------------------------------------------------------------------------
Beschreibung: Zeigt alle verfügbaren Levelgruppen an.

Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: LevelgroupFiles, g_LevelgroupFilesCount
------------------------------------------------------------------------------*/
void ShowAvailableLevelgroups(void) {
    char szMD5String[32 + 1];
    uint32_t G;

    if (g_LevelgroupFilesCount > 0) {
        SDL_Log("========= AVAILABLE LEVELGROUPS: %d  ============",g_LevelgroupFilesCount);
        for (G = 0; G < g_LevelgroupFilesCount; G++) {
            GetMd5String(LevelgroupFiles[G].uMd5Hash,szMD5String);
            SDL_Log("File: %s  MD5: %s  Group: %s   levels: %03u",LevelgroupFiles[G].szFilename,szMD5String,LevelgroupFiles[G].szLevelgroupname,LevelgroupFiles[G].uLevelCount);
        }
        SDL_Log("==================================================");
    } else {
        SDL_Log("No level groups available");
    }
}


/*----------------------------------------------------------------------------
Name:           WriteDefaultNamesFile
------------------------------------------------------------------------------
Beschreibung: Schreibt eine leere Namens-Datei (names.xml)

Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  0 = Alles OK, sonst Fehler
Seiteneffekte: Names.x
------------------------------------------------------------------------------*/
int WriteDefaultNamesFile(void) {
    SDL_Log("Writing default names file ...");
    memset(&Names,0,sizeof(Names));         // setzt alle Hashes auf "00000000000000000000000000000000" und alle Spielernamen auf \0, Spieleranzahl = 0
    return WriteNamesFile();
}


/*----------------------------------------------------------------------------
Name:           WriteNamesFile
------------------------------------------------------------------------------
Beschreibung: Schreibt den Inhalt der Struktur Names.x als Daten-File.

Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  0 = Alles OK, sonst Fehler
Seiteneffekte: Names.x
------------------------------------------------------------------------------*/
int WriteNamesFile(void) {
    MD5Context MD5Names;
    MD5Context MD5NameCount;
    uint8_t uResultHash[16];        // Ergebnis-Hash aus MD5Names und MD5nameCount
    uint32_t I;

    // Zunächst Hash der inneren Struktur Names.Name bilden
    md5Init(&MD5Names);
    md5Update(&MD5Names,(uint8_t*)&Names.Name,sizeof(Names.Name));
    md5Finalize(&MD5Names);
    // Dann Hash für Names.uNameCount bilden
    md5Init(&MD5NameCount);
    md5Update(&MD5Names,(uint8_t*)&Names.uNameCount,sizeof(uint32_t));
    md5Finalize(&MD5NameCount);
    for (I = 0; I < 16; I++) {
        uResultHash[I] = MD5Names.digest[I] ^ MD5NameCount.digest[I];
    }
    memcpy(Names.uSecurityHash,uResultHash,16);
    return WriteFile(EMERALD_NAMES_FILENAME,(uint8_t *)&Names,sizeof(Names),false);
}


/*----------------------------------------------------------------------------
Name:           ReadNamesFile
------------------------------------------------------------------------------
Beschreibung: Liest die Namensdatei in die Struktur Names.x ein.

Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  0 = Alles OK, sonst Fehler
Seiteneffekte: Names.x
------------------------------------------------------------------------------*/
int ReadNamesFile(void) {
    int nErrorCode;
    uint32_t I;
    uint32_t uDataLen;
    uint8_t *pData = NULL;
    bool bDataOK = false;
    MD5Context MD5Names;
    MD5Context MD5NameCount;
    uint8_t uResultHash[16];        // Ergebnis-Hash aus MD5Names und MD5nameCount

    nErrorCode = -1;
    memset(&Names,0,sizeof(Names));       // löscht auch letzten Spieler
    pData = ReadFile(EMERALD_NAMES_FILENAME,&uDataLen);     // Levelgruppen-Datei einlesen
    if (pData != NULL) {
        if (uDataLen == sizeof(Names)) {
            memcpy((uint8_t*)&Names,pData,uDataLen);
            // Wenn der Security-Hash auch noch OK ist, kann die Names-Struktur verwendet werden
            // Zunächst Hash der inneren Struktur Names.Name bilden
            md5Init(&MD5Names);
            md5Update(&MD5Names,(uint8_t*)&Names.Name,sizeof(Names.Name));
            md5Finalize(&MD5Names);
            // Dann Hash für Names.uNameCount bilden
            md5Init(&MD5NameCount);
            md5Update(&MD5Names,(uint8_t*)&Names.uNameCount,sizeof(uint32_t));
            md5Finalize(&MD5NameCount);
            for (I = 0; I < 16; I++) {
                uResultHash[I] = MD5Names.digest[I] ^ MD5NameCount.digest[I];
            }
            if (memcmp(uResultHash,Names.uSecurityHash,16) == 0) {
                bDataOK = true;
                nErrorCode = 0;
            } else {
                SDL_Log("%s: bad hash -> write default names file ...",__FUNCTION__);
            }
        }
    }
    if (!bDataOK) {
        nErrorCode = WriteDefaultNamesFile();
    }
    ShowNames();
    SAFE_FREE(pData);
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           ShowNames
------------------------------------------------------------------------------
Beschreibung: Zeigt alle hinterlegten Namen und deren Anzahl Gruppen-Hashes.

Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Names.x
------------------------------------------------------------------------------*/
void ShowNames(void) {
    uint32_t N;
    uint32_t G;
    uint32_t uHashCount;

    SDL_Log("================== NAMES: %d =================",Names.uNameCount);
    if (Names.uNameCount > 0) {

        for (N = 0; N < Names.uNameCount; N++) {
            uHashCount = 0;
            for (G = 0; G <EMERALD_MAX_LEVELGROUPFILES; G++) {
                if (memcmp(Names.Name[N].GroupHash[G].uHash,"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0",16) != 0) {
                    uHashCount++;
                }
            }
            SDL_Log("%02d: %s  Hashes: %u",N,Names.Name[N].szName,uHashCount);
        }
    } else {
        SDL_Log("No Names");
    }
    SDL_Log("=============================================");

}


/*----------------------------------------------------------------------------
Name:           CleanNameHashes
------------------------------------------------------------------------------
Beschreibung: Bereinigt die Struktur Names.x von nicht vorhandenen Levelgruppen-Hashes.
              Vor Aufruf dieser Funktion müssen folgende Funktionen mit Erfolg
              aufgerufen worden sein:
              * GetLevelgroupFiles():  Übersicht von vorhandenen Levelgroup-Hashes
              * ReadNamesFile(): Aktuelle Namen mit Hashes und Handicaps usw.

Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  int, 0 = Alles OK, sonst Fehler
Seiteneffekte: Names.x, LevelgroupFiles[].x, g_LevelgroupFilesCount
------------------------------------------------------------------------------*/
int CleanNameHashes(void) {
    uint32_t N;     // Name
    uint32_t G;     // Level-Gruppenhash aus Names.x
    uint32_t L;     // Level-Gruppenhash aus Levelgroup.x
    bool bHashFound;

    for (N = 0; N < Names.uNameCount; N++) {                    // Alle Namen prüfen
        for (G = 0; G <EMERALD_MAX_LEVELGROUPFILES; G++) {      // Alle Hashes dieses Namens prüfen
            if (memcmp(Names.Name[N].GroupHash[G].uHash,"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0",16) != 0) {
                bHashFound = false;
                for (L = 0; (L < g_LevelgroupFilesCount) && (!bHashFound); L++) {
                    bHashFound = (memcmp(Names.Name[N].GroupHash[G].uHash,LevelgroupFiles[L].uMd5Hash,16) == 0);
                }
                if (!bHashFound) {
                    memset(Names.Name[N].GroupHash[G].uHash,0,16);
                    Names.Name[N].GroupHash[G].uHandicap = 0;
                    Names.Name[N].GroupHash[G].uGamesPlayed = 0;
                    Names.Name[N].GroupHash[G].uGamesWon = 0;
                    Names.Name[N].GroupHash[G].uTotalScore = 0;
                    Names.Name[N].GroupHash[G].uPlayTimeS = 0;
                }
            }
        }
    }
    return WriteNamesFile();
}


/*----------------------------------------------------------------------------
Name:           ShowActualPlayer
------------------------------------------------------------------------------
Beschreibung: Zeigt den aktuell gewählten Spieler an.

Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Actualplayer.x
------------------------------------------------------------------------------*/
void ShowActualPlayer(void) {
    char szString[32 + 1];

    SDL_Log("===============ACTUAL PLAYER=================");
    if (Actualplayer.bValid) {
        GetMd5String(Actualplayer.uLevelgroupMd5Hash,szString);
        SDL_Log("Name:             %s",Actualplayer.szPlayername);      // Aktueller Spieler
        SDL_Log("Levelgroup Hash:  %s",szString);                       // aktuell gewählte Levelgruppe als MD5 Hash
        SDL_Log("Level:            %u",Actualplayer.uLevel);            // aktuell gewähltes Level (wird beim Init. auf Handicap gestellt)
        SDL_Log("Handicap:         %u",Actualplayer.uHandicap);         // aktuelles Handicap in der gewählten Levelgruppe
        SDL_Log("Games played:     %u",Actualplayer.uGamesPlayed);      // Anzahl gespielter Level in der gewählten Levelgruppe
        SDL_Log("Games won:        %u",Actualplayer.uGamesWon);         // Anzahl gewonnener Spiele in der gewählten Levelgruppe
        SDL_Log("Totalscore:       %u",Actualplayer.uTotalScore);       // Gesamtpunktezahl in der gewählten Levelgruppe
        SDL_Log("Playtime[secs]:   %u",Actualplayer.uPlayTimeS);        // Gesamtspielzeit in Sekunden in der gewählten Levelgruppe
    } else {
        SDL_Log("No actual player selected");
    }
    SDL_Log("=============================================");

}


/*----------------------------------------------------------------------------
Name:           InsertNewName
------------------------------------------------------------------------------
Beschreibung: Fügt einen neuen Namen in die Struktur Names.x hinzu. Falls es
              bereits diesen Spielernamen gibt, wird dieser neu angelegt.

Parameter
      Eingang: pszname, char*, Zeiger auf Spielernamen, der angelegt werden soll
      Ausgang: -
Rückgabewert:  0 = Alles OK, -1 = Fehler, -2 = kein Platz mehr
Seiteneffekte: Names.x
------------------------------------------------------------------------------*/
int InsertNewName(char *pszName) {
    int nErrorCode;
    uint32_t N;
    uint32_t uInsertIndex;
    bool bNameFound;

    uInsertIndex = 0;
    nErrorCode = -1;
    if (pszName != NULL) {
        if ((strlen(pszName) > 0) && (strlen(pszName) <= EMERALD_PLAYERNAME_LEN)) {
            if (Names.uNameCount < EMERALD_MAX_PLAYERNAMES) {
                // Zunächst schauen, ob es diesen Namen bereits gibt
                bNameFound =  false;
                uInsertIndex = Names.uNameCount;
                for (N = 0; (N < Names.uNameCount) && (!bNameFound); N++) {
                    if (strcmp(pszName,Names.Name[N].szName) == 0) {
                        SDL_Log("%s: Name %s already exist, clearing GroupHashes ...",__FUNCTION__,pszName);
                        bNameFound = true;
                        uInsertIndex = N;
                    }
                }
                // An uInsertIndex den (neuen) Namen eintragen und Gruppen-Hashes löschen
                strcpy(Names.Name[uInsertIndex].szName,pszName);
                memset(Names.Name[uInsertIndex].GroupHash,0,sizeof(GROUPHASH));
                if (!bNameFound) {
                    Names.uNameCount++; // Falls Name nicht überschrieben wurde, kommt ein neuer hinzu
                }
                nErrorCode = WriteNamesFile();
            } else {
                SDL_Log("%s: maximum amount of players reached!",__FUNCTION__);
                nErrorCode = -2;
            }
        } else {
            SDL_Log("%s: invalid playname length: %u, name: %s",__FUNCTION__,(uint32_t)strlen(pszName),pszName);
        }
    }
    return nErrorCode;
}





/*----------------------------------------------------------------------------
Name:           InsertGamesValuesIntoNamesFile
------------------------------------------------------------------------------
Beschreibung: Die Werte aus der Struktur Actualplayer.x werden in die Strukturen
              der Namensliste hinterlegt.

Parameter
      Eingang: pszName, char*, Zeiger auf Spielernamen, für den Werte gespeichert werden sollen
               puHash, uint8_t * , Zeiger auf Levelgruppen-Hash, für den die Werte gelten
      Ausgang: -
Rückgabewert:  0 = Alles OK, sonst Fehler
Seiteneffekte: Names.x, Actualplayer.x
------------------------------------------------------------------------------*/
int InsertGamesValuesIntoNamesFile(char *pszName, uint8_t *puHash) {
    int nErrorCode;
    bool bNameFound;
    bool bHashFound;
    uint32_t N;
    uint32_t H;
    uint32_t uFoundIndex;

    nErrorCode = -1;
    bNameFound =  false;
    uFoundIndex = 0;
    if ((pszName != NULL) && (puHash != NULL)) {
        if ((strlen(pszName) > 0) && (strlen(pszName) <= EMERALD_PLAYERNAME_LEN)) {
            // Zunächst schauen, ob es diesen Namen bereits gibt
            for (N = 0; (N < Names.uNameCount) && (!bNameFound); N++) {
                if (strcmp(pszName,Names.Name[N].szName) == 0) {
                    SDL_Log("%s: Name %s exist, selecting ...",__FUNCTION__,pszName);
                    bNameFound = true;
                    uFoundIndex = N;
                }
            }
            if (bNameFound) {
                // Wenn Name gefunden wurde, dann schauen, ob Hash schon vorhanden
                bHashFound = false;
                for (H = 0; (H < EMERALD_MAX_LEVELGROUPFILES) && (!bHashFound); H++) {
                    if (memcmp(Names.Name[uFoundIndex].GroupHash[H].uHash,puHash,16) == 0) {
                        SDL_Log("%s: level group Hash for Name %s found",__FUNCTION__,pszName);
                        bHashFound = true;
                        Names.Name[uFoundIndex].GroupHash[H].uHandicap = Actualplayer.uHandicap;
                        Names.Name[uFoundIndex].GroupHash[H].uGamesPlayed = Actualplayer.uGamesPlayed;
                        Names.Name[uFoundIndex].GroupHash[H].uGamesWon = Actualplayer.uGamesWon;
                        Names.Name[uFoundIndex].GroupHash[H].uTotalScore = Actualplayer.uTotalScore;
                        Names.Name[uFoundIndex].GroupHash[H].uPlayTimeS = Actualplayer.uPlayTimeS;
                        nErrorCode = 0;
                    }
                }
                if (!bHashFound) {
                    SDL_Log("%s: levelgroup hash for player: %s not found!",__FUNCTION__,pszName);
                }
            } else {
                SDL_Log("%s: Name %s not found.",__FUNCTION__,pszName);
            }
        } else {
            SDL_Log("%s: invalid stringlen: %u",__FUNCTION__,(uint32_t)strlen(pszName));
        }
    }
    if (nErrorCode == 0) {
        nErrorCode = WriteNamesFile();
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           SelectName
------------------------------------------------------------------------------
Beschreibung: Wählt einen Spielernamen aus.
              Die Funktion ReadNamesFile() muss bereits aufgerufen worden sein,
              d.h. die Struktur Names.x ist befüllt.

Parameter
      Eingang: pszName, char*, Zeiger auf Spielernamen, der angelegt werden soll
               puHash, uint8_t * , Zeiger auf Levelgruppen-Hash
      Ausgang: -
Rückgabewert:  0 = Alles OK, -1 = Fehler, -2 = name nicht gefunden
Seiteneffekte: Names.x, Actualplayer.x
------------------------------------------------------------------------------*/
int SelectName(char *pszName, uint8_t *puHash) {
    int nErrorCode;
    bool bNameFound;
    bool bHashFound;
    uint32_t N;
    uint32_t H;
    uint32_t uFoundIndex;

    nErrorCode = -1;
    bNameFound =  false;
    uFoundIndex = 0;
    memset(&Actualplayer,0,sizeof(Actualplayer));
    if ((pszName != NULL) && (puHash != NULL)) {
        if ((strlen(pszName) > 0) && (strlen(pszName) <= EMERALD_PLAYERNAME_LEN)) {
            // Zunächst schauen, ob es diesen Namen bereits gibt
            for (N = 0; (N < Names.uNameCount) && (!bNameFound); N++) {
                if (strcmp(pszName,Names.Name[N].szName) == 0) {
                    SDL_Log("%s: Name %s exist, selecting ...",__FUNCTION__,pszName);
                    bNameFound = true;
                    uFoundIndex = N;
                }
            }
            if (bNameFound) {
                // Wenn Name gefunden wurde, dann schauen, ob Hash schon vorhanden
                bHashFound = false;
                for (H = 0; (H < EMERALD_MAX_LEVELGROUPFILES) && (!bHashFound); H++) {
                    if (memcmp(Names.Name[uFoundIndex].GroupHash[H].uHash,puHash,16) == 0) {
                        SDL_Log("%s: level group Hash for Name %s found",__FUNCTION__,pszName);
                        bHashFound = true;
                        strcpy(Actualplayer.szPlayername,pszName);
                        memcpy(Actualplayer.uLevelgroupMd5Hash,Names.Name[uFoundIndex].GroupHash[H].uHash,16);
                        Actualplayer.uHandicap = Names.Name[uFoundIndex].GroupHash[H].uHandicap;
                        Actualplayer.uLevel = Actualplayer.uHandicap;
                        Actualplayer.uGamesPlayed = Names.Name[uFoundIndex].GroupHash[H].uGamesPlayed;
                        Actualplayer.uGamesWon = Names.Name[uFoundIndex].GroupHash[H].uGamesWon;
                        Actualplayer.uTotalScore = Names.Name[uFoundIndex].GroupHash[H].uTotalScore;
                        Actualplayer.uPlayTimeS = Names.Name[uFoundIndex].GroupHash[H].uPlayTimeS;
                        Actualplayer.bValid = true;
                        nErrorCode = 0;
                    }
                }
                // Wenn Levelgruppen-Hash nicht gefunden wurde, dann diesen hinzufügen
                if (!bHashFound) {
                    nErrorCode = InsertGroupHashForName(pszName,puHash);
                    if (nErrorCode == 0) {
                        strcpy(Actualplayer.szPlayername,pszName);
                        Actualplayer.uHandicap = 0;
                        Actualplayer.uLevel = Actualplayer.uHandicap;
                        Actualplayer.uGamesPlayed = 0;
                        Actualplayer.uGamesWon = 0;
                        Actualplayer.uTotalScore = 0;
                        Actualplayer.uPlayTimeS = 0;
                        Actualplayer.bValid = true;
                    } else {
                        SDL_Log("%s: InsertGroupHashForName() for name %s failed, Error: %d",__FUNCTION__,pszName,nErrorCode);
                    }
                }
            } else {
                SDL_Log("%s: Name %s not found.",__FUNCTION__,pszName);
                nErrorCode = -2;
            }
        } else {
            SDL_Log("%s: invalid stringlen: %u",__FUNCTION__,(uint32_t)strlen(pszName));
        }
    }
    if (nErrorCode == 0) {
        nErrorCode = WriteNamesFile();
        ShowActualPlayer();
    } else {
        memset(&Actualplayer,0,sizeof(Actualplayer));   // Bei Fehler die aktuelle Spielerstruktur auf invalid stellen
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           InsertGroupHashForName
------------------------------------------------------------------------------
Beschreibung: Fügt einen neuen Levlgruppen-Hash zu einem Namen hinzu.
              bereits diesen Spielernamen gibt, wird dieser neu angelegt.
Parameter
      Eingang: pszname, char*, Zeiger auf Spielernamen, bei dem der Levelgruppen-Hash hinzugefügt werden soll
               puHash, uint8_t * , Zeiger auf anzulegenden Levelgruppen-Hash
      Ausgang: -
Rückgabewert:  0 = Alles OK, -1 = Fehler,
                             -2 = Name nicht gefunden,
                             -3 = kein Platz mehr
                              0 = Hash gibt es schon
Seiteneffekte: Names.x
------------------------------------------------------------------------------*/
int InsertGroupHashForName(char *pszName, uint8_t *puHash) {
    int nErrorCode;
    bool bNameFound;
    bool bHashFound;
    uint32_t N;
    uint32_t H;
    uint32_t uInsertIndex;

    nErrorCode = -1;
    bNameFound =  false;
    uInsertIndex = 0;
    if ((pszName != NULL) && (puHash != NULL)) {
        if ((strlen(pszName) > 0) && (strlen(pszName) <= EMERALD_PLAYERNAME_LEN)) {
            // Zunächst schauen, ob es diesen Namen bereits gibt
            for (N = 0; (N < Names.uNameCount) && (!bNameFound); N++) {
                if (strcmp(pszName,Names.Name[N].szName) == 0) {
                    SDL_Log("%s: Name %s exist, clearing GroupHashes ...",__FUNCTION__,pszName);
                    bNameFound = true;
                    uInsertIndex = N;
                }
            }
            if (bNameFound) {
                // Wenn Name gefunden wurde, dann schauen, ob Hash schon vorhanden
                bHashFound = false;
                for (H = 0; (H < EMERALD_MAX_LEVELGROUPFILES) && (!bHashFound); H++) {
                    if (memcmp(Names.Name[uInsertIndex].GroupHash[H].uHash,puHash,16) == 0) {
                        SDL_Log("%s: Hash for Name %s already exist.",__FUNCTION__,pszName);
                        nErrorCode = 0;
                        bHashFound = true;
                    }
                }
                // Wenn Hash nicht gefunden wurde, dann Platz für neuen Hash suchen
                if (!bHashFound) {
                    for (H = 0; (H < EMERALD_MAX_LEVELGROUPFILES) && (!bHashFound); H++) {
                        if (memcmp(Names.Name[uInsertIndex].GroupHash[H].uHash,"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0",16) == 0) {
                            SDL_Log("%s: name: %s: empty place for new Hash found, inserting new hash ...",__FUNCTION__,pszName);
                            memcpy(Names.Name[uInsertIndex].GroupHash[H].uHash,puHash,16);
                            Names.Name[uInsertIndex].GroupHash[H].uHandicap = 0;
                            Names.Name[uInsertIndex].GroupHash[H].uGamesPlayed = 0;
                            Names.Name[uInsertIndex].GroupHash[H].uGamesWon = 0;
                            Names.Name[uInsertIndex].GroupHash[H].uTotalScore = 0;
                            Names.Name[uInsertIndex].GroupHash[H].uPlayTimeS = 0;
                            nErrorCode = WriteNamesFile();
                            bHashFound = true;
                        }
                    }
                    if (!bHashFound) {
                        SDL_Log("%s: name: %s: hash table full",__FUNCTION__,pszName);
                        nErrorCode = -3;
                    }
                }
            } else {
                SDL_Log("%s: Name %s not found.",__FUNCTION__,pszName);
                nErrorCode = -2;
            }
        }
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           DeleteName
------------------------------------------------------------------------------
Beschreibung: Löscht einen Namen aus der Struktur Names.x. Die zugehörigen
              Levelgruppen-Hashes werden mitgelöscht.
Parameter
      Eingang: pszname, char*, Zeiger auf Spielernamen, der gelöscht werden soll
      Ausgang: -
Rückgabewert:  0 = Alles OK, -1 = Fehler, -2 = Name nicht gefunden
Seiteneffekte: Names.x
------------------------------------------------------------------------------*/
int DeleteName(char *pszName) {
    int nErrorCode;
    bool bNameFound;
    uint32_t N;
    uint32_t uDeleteIndex;

    nErrorCode = -1;
    bNameFound =  false;
    uDeleteIndex = 0;
    if (pszName != NULL) {
        if ((strlen(pszName) > 0) && (strlen(pszName) <= EMERALD_PLAYERNAME_LEN)) {
            // Prüfen, ob es diesen Namen gibt
            for (N = 0; (N < Names.uNameCount) && (!bNameFound); N++) {
                if (strcmp(pszName,Names.Name[N].szName) == 0) {
                    SDL_Log("%s: Name %s exist, deleting name ...",__FUNCTION__,pszName);
                    bNameFound = true;
                    uDeleteIndex = N;
                }
            }
            if (bNameFound) {
                // Wenn Name gefunden wurde, dann muss ab dieser Position Alles nach vorne verschoben werden
                for (N = uDeleteIndex; N < (EMERALD_MAX_PLAYERNAMES - 1); N++) {
                    memcpy(&Names.Name[N],&Names.Name[N + 1],sizeof(NAME));
                }
                memset(&Names.Name[N],0,sizeof(NAME));   // Der letzte Platz muss mit 0 aufgefüllt werden
                Names.uNameCount--;
                nErrorCode = WriteNamesFile();
            } else {
                SDL_Log("%s: Name %s not found.",__FUNCTION__,pszName);
                nErrorCode = -2;
            }
        }
    }
    return nErrorCode;
}
