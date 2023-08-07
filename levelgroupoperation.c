#include "buttons_checkboxes.h"
#include "editor.h"
#include "EmeraldMineMainMenu.h"
#include "KeyboardMouse.h"
#include "levelconverter.h"
#include "levelgroupoperation.h"
#include "loadlevel.h"
#include "md5.h"
#include "modplay.h"

extern CLIPBOARD Clipboard;
extern LEVELGROUP SelectedLevelgroup;
extern unsigned char ge_new_levelgroup[];
extern ED Ed;
extern INPUTSTATES InputStates;
extern MAINMENU MainMenu;
extern LEVELFILESLIST Dc3LevelFileList[EMERALD_MAX_IMPORTFILES];


/*----------------------------------------------------------------------------
Name:           GetLevelFromLevelgroup
------------------------------------------------------------------------------
Beschreibung: Gibt einen Level aus einer Levelgruppe zurück. Im Erfolgsfall
              alloziert die Funktion Speicher, der außerhalb dieser Funktion
              wieder freigegeben werden muss. Der level ist \0 terminiert und
              somit als String verwendbar.
Parameter
      Eingang: pszLevelgroup, uint8_t *, XML-Daten einer Levelgruppe
               uLevelNumber, uint32_t, Levelnummer
      Ausgang: -
Rückgabewert:  uint8_t *, NULL = Fehler, sonst XML-Leveldaten
Seiteneffekte: -
------------------------------------------------------------------------------*/
uint8_t *GetLevelFromLevelgroup(uint8_t *pszLevelgroup, uint32_t uLevelNumber) {
    uint8_t *pszLevel = NULL;
    char szLevelStartTag[32];      // "<level000>"
    char szLevelEndTag[32];      // "</level000>"
    uint32_t uLevelSize;
    uint8_t *pStart;
    uint8_t *pEnd;

    if ((pszLevelgroup != NULL) && (uLevelNumber < EMERALD_MAX_LEVELCOUNT)) {
        sprintf(szLevelStartTag,"<level%03u>\r\n",uLevelNumber);
        sprintf(szLevelEndTag,"</level%03u>\r\n",uLevelNumber);
        if ((strstr((char*)pszLevelgroup,"<levelgroup>") != NULL) && (strstr((char*)pszLevelgroup,"</levelgroup>") != NULL)) {
            pStart = (uint8_t*)strstr((char*)pszLevelgroup,szLevelStartTag);
            pEnd = (uint8_t*)strstr((char*)pszLevelgroup,szLevelEndTag);
            if ((pStart != NULL) && (pEnd != NULL)) {
                uLevelSize = pEnd - pStart + strlen(szLevelEndTag);
                pszLevel = malloc(uLevelSize + 1);
                if (pszLevel != NULL) {
                    memset(pszLevel,0,uLevelSize + 1);
                    memcpy(pszLevel,pStart,uLevelSize);
                }
            }
        }
    }
    return pszLevel;
}


/*----------------------------------------------------------------------------
Name:           GetLevelTitleFromLevel
------------------------------------------------------------------------------
Beschreibung: Holt den level-Titel aus einer Leveldatei.
Parameter
      Eingang: pszLevel, uint8_t *, XML-Daten eines Levels
      Ausgang: pszLevelTitle, uint8_t *, Zeiger auf Speicher für Leveltitel, mindestens
               EMERALD_TITLE_LEN + 1 Bytes Speicher
Rückgabewert:  int, 0 = Alles OK, sonst Fehler
Seiteneffekte: -
------------------------------------------------------------------------------*/
int GetLevelTitleFromLevel(uint8_t *pszLevel, uint8_t *pszLevelTitle) {
    int nErrorCode = -1;
    uint8_t *pStart;
    uint8_t *pEnd;
    int nSize;

    if ((pszLevel != NULL) && (pszLevelTitle != NULL)) {
        pStart = (uint8_t*)strstr((char*)pszLevel,"<title>");
        pEnd = (uint8_t*)strstr((char*)pszLevel,"</title>\r\n");
        if ((pStart != NULL) && (pEnd != NULL) && (pEnd > pStart)) {
            pStart = pStart + strlen("<title>");
            memset(pszLevelTitle,0,EMERALD_TITLE_LEN + 1);
            nSize = pEnd - pStart;
            if ((nSize > 0) && (nSize <= EMERALD_TITLE_LEN)) {
                memcpy(pszLevelTitle,pStart,nSize);
            }
            nErrorCode = 0;
        }
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           LevelgroupOperaton_Edit
------------------------------------------------------------------------------
Beschreibung: Ersetzt einen Level innerhalb einer Levelgruppe durch neue Leveldaten.
Parameter
      Eingang: uLevelNumber, uint32_t, Levelnummer
               NewLevel, DYNSTRING *, Zeiger auf Struktur für neue XML-Leveldaten
      Ausgang: -
Rückgabewert:  0 = Alles OK, sonst Fehler
Seiteneffekte: SelectedLevelgroup.x
------------------------------------------------------------------------------*/
int LevelgroupOperaton_Edit(uint32_t uLevelNumber, DYNSTRING *NewLevel) {
    int nErrorCode = -1;
    uint32_t uXmlLen;
    uint8_t *pLevelgroupXml = NULL;
    uint8_t *puLastTag;
    uint8_t *puEndPart1;
    uint8_t *puPart1 = NULL;
    uint8_t *puRestPartStart;
    uint32_t uPart1Size = 0;
    char szLevelNumberTag[32];      // "level000"
    uint8_t uLevelgroupHash[16];
    DYNSTRING *S;

    nErrorCode = -1;
    S = DynStringInit();
    if ((uLevelNumber < SelectedLevelgroup.uLevelCount) && (NewLevel != NULL) && (SelectedLevelgroup.bOK)  && (S != NULL)) {
        if ((NewLevel->pszString != NULL) && (NewLevel->nLen > 0)) {
            pLevelgroupXml = ReadFile(SelectedLevelgroup.szFilename,&uXmlLen);     // Levelgruppen-Datei einlesen
            if (pLevelgroupXml != NULL) {
                puLastTag = (uint8_t*)strstr((char*)pLevelgroupXml,"</levelgroup>");     // "Höchster" Pointer
                if (puLastTag != NULL) {
                    if (UpdateCreateTimestamp(pLevelgroupXml) == 0) {
                        sprintf(szLevelNumberTag,"<level%03u>\r\n",uLevelNumber);
                        puEndPart1 = (uint8_t*)strstr((char*)pLevelgroupXml,szLevelNumberTag);     // Hier ist Teil 1 zu Ende
                        if ((puEndPart1 != NULL) && (puEndPart1 > pLevelgroupXml) && (puEndPart1 < puLastTag)) {
                            uPart1Size = puEndPart1 - pLevelgroupXml;
                            puPart1 = malloc(uPart1Size + 1); // + 1 für String-Terminierung
                            if (puPart1 != NULL) {
                                memset(puPart1,0,uPart1Size + 1);
                                memcpy(puPart1,pLevelgroupXml,uPart1Size);
                                DynStringAdd(S,(char*)puPart1);
                                DynStringAdd(S,szLevelNumberTag);
                                DynStringAdd(S,NewLevel->pszString);
                                sprintf(szLevelNumberTag,"</level%03u>\r\n",uLevelNumber);
                                DynStringAdd(S,szLevelNumberTag);
                                puRestPartStart = (uint8_t*)strstr((char*)pLevelgroupXml,szLevelNumberTag);
                                if (puRestPartStart != NULL) {
                                    puRestPartStart = puRestPartStart + strlen(szLevelNumberTag);
                                    if (puRestPartStart < puLastTag) {
                                        DynStringAdd(S,(char*)puRestPartStart);
                                        if (UpdateLevelgroupHash((uint8_t*)S->pszString,uLevelgroupHash) == 0) {
                                            // Levelgruppe aktualisieren
                                            if (WriteFile(SelectedLevelgroup.szFilename,(uint8_t*)S->pszString,S->nLen,false) == 0) {
                                                if (GetLevelgroupFiles() == 0) {    // Wenn das nicht funktioniert, kann nicht weitergemacht werden!
                                                    if (SelectAlternativeLevelgroup(uLevelgroupHash,false) == 0) {
                                                        InitLists();
                                                        nErrorCode = 0;
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
    DynStringFree(S);
    SAFE_FREE(puPart1);
    SAFE_FREE(pLevelgroupXml);
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           LevelgroupOperaton_Copy
------------------------------------------------------------------------------
Beschreibung: Kopiert einen Level innerhalb einer Levelgruppe. Wird zum Beispiel
              Level 2 kopiert, so wird ein neues Level zu 3 und die nachfolgenden
              Level verschieben sich.
Parameter
      Eingang: uLevelNumber, uint32_t, Levelnummer
      Ausgang: -
Rückgabewert:  0 = Alles OK, sonst Fehler
Seiteneffekte: SelectedLevelgroup.x
------------------------------------------------------------------------------*/
int LevelgroupOperaton_Copy(uint32_t uLevelNumber) {
    int nErrorCode = -1;
    int nNewLevelCount = 0;
    uint32_t uXmlLen;
    uint8_t *pLevelgroupXml = NULL;
    uint8_t *puLastTag;
    uint8_t *puEndPart1;            // beinhaltet alle Level einschließlich uLevelNumber
    uint8_t *puPart1 = NULL;
    uint8_t *puRestPartStart;
    uint32_t uPart1Size = 0;
    // Für Levelcount
    char *puStartLevelCountTag;
    char *puEndLevelCountTag;
    uint8_t *puPart2 = NULL;
    char szLevelCount[16];
    char szLevelNumberTag[32];      // "level000"
    DYNSTRING *S;
    DYNSTRING *S2;
    uint8_t *pszlevel = NULL;
    char szLevelTitle[EMERALD_TITLE_LEN + 1];
    char szNewLevelTitle[EMERALD_TITLE_LEN + 10 + 1]; // "COPY OF " + 8
    uint8_t uLevelgroupHash[16];

    S = DynStringInit();
    S2 = DynStringInit();
    if ((uLevelNumber < SelectedLevelgroup.uLevelCount) && (SelectedLevelgroup.bOK)  && (S != NULL) && (S2 != NULL)) {
        if ((pLevelgroupXml = ReadFile(SelectedLevelgroup.szFilename,&uXmlLen)) != NULL) {     // Levelgruppen-Datei einlesen
            if ((puLastTag = (uint8_t*)strstr((char*)pLevelgroupXml,"</levelgroup>")) != NULL) {     // "Höchster" Pointer
                if (UpdateCreateTimestamp(pLevelgroupXml) == 0) {
                    sprintf(szLevelNumberTag,"</level%03u>\r\n",uLevelNumber);
                    puEndPart1 = (uint8_t*)strstr((char*)pLevelgroupXml,szLevelNumberTag);     // Hier ist Teil 1 zu Ende
                    if ((puEndPart1 != NULL) && (puEndPart1 > pLevelgroupXml) && (puEndPart1 < puLastTag)) {
                        uPart1Size = puEndPart1 - pLevelgroupXml + strlen(szLevelNumberTag);
                        puPart1 = malloc(uPart1Size + 1); // + 1 für String-Terminierung
                        if (puPart1 != NULL) {
                            memset(puPart1,0,uPart1Size + 1);
                            memcpy(puPart1,pLevelgroupXml,uPart1Size);
                            DynStringAdd(S,(char*)puPart1);         // Es befinden sich nun alle Level bis einschließlich Level X im String.
                            pszlevel = GetLevelFromLevelgroup(pLevelgroupXml,uLevelNumber); // Kopie von Level X holen
                            if (pszlevel != NULL) {
                                if (GetLevelTitleFromLevel(pszlevel,(uint8_t*)szLevelTitle) == 0) {
                                    sprintf(szLevelNumberTag,"<level%03u>\r\n",uLevelNumber);
                                    DynStringAdd(S,szLevelNumberTag);       // Start der Kopie
                                    DynStringAdd(S,"  <title>");
                                    strcpy(szNewLevelTitle,"COPY OF "); // Len = 8
                                    strcat(szNewLevelTitle,szLevelTitle);
                                    szNewLevelTitle[EMERALD_TITLE_LEN] = 0;  // Länge begrenzen
                                    DynStringAdd(S,szNewLevelTitle);
                                    DynStringAdd(S,"</title>\r\n");
                                    puRestPartStart = (uint8_t*)strstr((char*)pszlevel,"<author>");     // Start vom Rest des levels
                                    if (puRestPartStart != NULL) {
                                        DynStringAdd(S,(char*)puRestPartStart);
                                        sprintf(szLevelNumberTag,"</level%03u>\r\n",uLevelNumber);
                                        puEndPart1 = puEndPart1 + strlen(szLevelNumberTag);
                                        if (puEndPart1 < puLastTag) {
                                            DynStringAdd(S,(char*)puEndPart1);        // Den Rest der Levelgruppe anhängen
                                            if ((nNewLevelCount = RenumLevelgroup((uint8_t *)S->pszString)) > 0) {
                                                // Levelgruppe ist fast fertig, jetzt Anpassung von <levelcount>x</levelcount>
                                                puStartLevelCountTag = strstr(S->pszString,"<levelcount>");
                                                puEndLevelCountTag = strstr(S->pszString,"</levelcount>\r\n");
                                                if ((puStartLevelCountTag != NULL) && (puStartLevelCountTag > S->pszString) && (puEndLevelCountTag != NULL)) {
                                                    uPart1Size = puStartLevelCountTag - S->pszString;
                                                    puPart2 = malloc(uPart1Size + 1);
                                                    if (puPart2 != NULL) {
                                                        memset(puPart2,0,uPart1Size + 1);
                                                        memcpy(puPart2,S->pszString,uPart1Size);
                                                        DynStringAdd(S2,(char*)puPart2);
                                                        DynStringAdd(S2,"<levelcount>");
                                                        sprintf(szLevelCount,"%d",nNewLevelCount);
                                                        DynStringAdd(S2,szLevelCount);
                                                        DynStringAdd(S2,puEndLevelCountTag);
                                                        if (UpdateLevelgroupHash((uint8_t*)S2->pszString,uLevelgroupHash) == 0) {
                                                            // Levelgruppe aktualisieren
                                                            if (WriteFile(SelectedLevelgroup.szFilename,(uint8_t*)S2->pszString,S2->nLen,false) == 0) {
                                                                if (GetLevelgroupFiles() == 0) {    // Wenn das nicht funktioniert, kann nicht weitergemacht werden!
                                                                    if (SelectAlternativeLevelgroup(uLevelgroupHash,false) == 0) {
                                                                        InitLists();
                                                                        nErrorCode = 0;
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
    DynStringFree(S);
    DynStringFree(S2);
    SAFE_FREE(pszlevel);
    SAFE_FREE(puPart1);
    SAFE_FREE(puPart2);
    SAFE_FREE(pLevelgroupXml);
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           LevelgroupOperaton_CopyClipboard
------------------------------------------------------------------------------
Beschreibung: Kopiert einen Level aus dem Clipboard hinter das ausgewählte Level
              innerhalb einer Levelgruppe.
Parameter
      Eingang: uLevelNumber, uint32_t, Levelnummer
      Ausgang: -
Rückgabewert:  0 = Alles OK, sonst Fehler
Seiteneffekte: SelectedLevelgroup.x, Clipboard.x
------------------------------------------------------------------------------*/
int LevelgroupOperaton_CopyClipboard(uint32_t uLevelNumber) {
    int nErrorCode = -1;
    int nNewLevelCount = 0;
    uint32_t uXmlLen;
    uint8_t *pLevelgroupXml = NULL;
    uint8_t *puLastTag;
    uint8_t *puEndPart1;            // beinhaltet alle Level einschließlich uLevelNumber
    uint8_t *puPart1 = NULL;
    uint32_t uPart1Size = 0;
    // Für Levelcount
    char *puStartLevelCountTag;
    char *puEndLevelCountTag;
    uint8_t *puPart2 = NULL;
    char szLevelCount[16];
    char szLevelNumberTag[32];      // "level000"
    DYNSTRING *S;
    DYNSTRING *S2;
    uint8_t uLevelgroupHash[16];

    S = DynStringInit();
    S2 = DynStringInit();
    if ((uLevelNumber < SelectedLevelgroup.uLevelCount) && (SelectedLevelgroup.bOK)  && (S != NULL) && (S2 != NULL) && (Clipboard.pLevelXml != NULL)) {
        if ((pLevelgroupXml = ReadFile(SelectedLevelgroup.szFilename,&uXmlLen)) != NULL) {     // Levelgruppen-Datei einlesen
            if ((puLastTag = (uint8_t*)strstr((char*)pLevelgroupXml,"</levelgroup>")) != NULL) {     // "Höchster" Pointer
                if (UpdateCreateTimestamp(pLevelgroupXml) == 0) {
                    sprintf(szLevelNumberTag,"</level%03u>\r\n",uLevelNumber);
                    puEndPart1 = (uint8_t*)strstr((char*)pLevelgroupXml,szLevelNumberTag);     // Hier ist Teil 1 zu Ende
                    if ((puEndPart1 != NULL) && (puEndPart1 > pLevelgroupXml) && (puEndPart1 < puLastTag)) {
                        uPart1Size = puEndPart1 - pLevelgroupXml + strlen(szLevelNumberTag);
                        puPart1 = malloc(uPart1Size + 1); // + 1 für String-Terminierung
                        if (puPart1 != NULL) {
                            memset(puPart1,0,uPart1Size + 1);
                            memcpy(puPart1,pLevelgroupXml,uPart1Size);
                            DynStringAdd(S,(char*)puPart1);         // Es befinden sich nun alle Level bis einschließlich Level X im String.
                            DynStringAdd(S,(char*)Clipboard.pLevelXml); // Clipboard-Level dazu kopieren.
                            puEndPart1 = puEndPart1 + strlen(szLevelNumberTag);
                            if (puEndPart1 < puLastTag) {
                                DynStringAdd(S,(char*)puEndPart1);        // Den Rest der Levelgruppe anhängen
                                if ((nNewLevelCount = RenumLevelgroup((uint8_t *)S->pszString)) > 0) {
                                    // Levelgruppe ist fast fertig, jetzt Anpassung von <levelcount>x</levelcount>
                                    puStartLevelCountTag = strstr(S->pszString,"<levelcount>");
                                    puEndLevelCountTag = strstr(S->pszString,"</levelcount>\r\n");
                                    if ((puStartLevelCountTag != NULL) && (puStartLevelCountTag > S->pszString) && (puEndLevelCountTag != NULL)) {
                                        uPart1Size = puStartLevelCountTag - S->pszString;
                                        puPart2 = malloc(uPart1Size + 1);
                                        if (puPart2 != NULL) {
                                            memset(puPart2,0,uPart1Size + 1);
                                            memcpy(puPart2,S->pszString,uPart1Size);
                                            DynStringAdd(S2,(char*)puPart2);
                                            DynStringAdd(S2,"<levelcount>");
                                            sprintf(szLevelCount,"%d",nNewLevelCount);
                                            DynStringAdd(S2,szLevelCount);
                                            DynStringAdd(S2,puEndLevelCountTag);
                                            if (UpdateLevelgroupHash((uint8_t*)S2->pszString,uLevelgroupHash) == 0) {
                                                // Levelgruppe aktualisieren
                                                if (WriteFile(SelectedLevelgroup.szFilename,(uint8_t*)S2->pszString,S2->nLen,false) == 0) {
                                                    if (GetLevelgroupFiles() == 0) {    // Wenn das nicht funktioniert, kann nicht weitergemacht werden!
                                                        if (SelectAlternativeLevelgroup(uLevelgroupHash,false) == 0) {
                                                            InitLists();
                                                            nErrorCode = 0;
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
    DynStringFree(S);
    DynStringFree(S2);
    SAFE_FREE(puPart1);
    SAFE_FREE(puPart2);
    SAFE_FREE(pLevelgroupXml);
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           LevelgroupOperaton_Delete
------------------------------------------------------------------------------
Beschreibung: Löscht einen Level innerhalb einer Levelgruppe. Wird zum Beispiel
              Level 2 gelöscht, so rücken die nachfolgenden Level ab 3 weiter nach
              vorne.
Parameter
      Eingang: uLevelNumber, uint32_t, Levelnummer -> Level, dass gelöscht werden soll
      Ausgang: -
Rückgabewert:  0 = Alles OK, sonst Fehler
Seiteneffekte: SelectedLevelgroup.x
------------------------------------------------------------------------------*/
int LevelgroupOperaton_Delete(uint32_t uLevelNumber) {
    int nErrorCode = -1;
    int nNewLevelCount = 0;
    uint32_t uXmlLen;
    uint8_t *pLevelgroupXml = NULL;
    uint8_t *puLastTag;
    uint8_t *puEndPart1;            // beinhaltet alle Level einschließlich uLevelNumber
    uint8_t *puPart1 = NULL;
    uint8_t *puRestPartStart;
    uint32_t uPart1Size = 0;
    // Für Levelcount
    char *puStartLevelCountTag;
    char *puEndLevelCountTag;
    uint8_t *puPart2 = NULL;
    char szLevelCount[16];
    char szLevelNumberTag[32];      // "level000"
    DYNSTRING *S;
    DYNSTRING *S2;
    uint8_t uLevelgroupHash[16];

    S = DynStringInit();
    S2 = DynStringInit();
    if ((uLevelNumber < SelectedLevelgroup.uLevelCount) && (SelectedLevelgroup.bOK)  && (S != NULL) && (S2 != NULL)) {
        if ((pLevelgroupXml = ReadFile(SelectedLevelgroup.szFilename,&uXmlLen)) != NULL) {     // Levelgruppen-Datei einlesen
            if ((puLastTag = (uint8_t*)strstr((char*)pLevelgroupXml,"</levelgroup>")) != NULL) {     // "Höchster" Pointer
                if (UpdateCreateTimestamp(pLevelgroupXml) == 0) {
                    sprintf(szLevelNumberTag,"<level%03u>\r\n",uLevelNumber);   // Start des zu löschenden Levels
                    puEndPart1 = (uint8_t*)strstr((char*)pLevelgroupXml,szLevelNumberTag);     // Hier ist Teil 1 zu Ende
                    if ((puEndPart1 != NULL) && (puEndPart1 > pLevelgroupXml) && (puEndPart1 < puLastTag)) {
                        uPart1Size = puEndPart1 - pLevelgroupXml;
                        puPart1 = malloc(uPart1Size + 1); // + 1 für String-Terminierung
                        if (puPart1 != NULL) {
                            memset(puPart1,0,uPart1Size + 1);
                            memcpy(puPart1,pLevelgroupXml,uPart1Size);
                            DynStringAdd(S,(char*)puPart1);         // Es befinden sich nun alle Level bis einschließlich Level X im String.
                            sprintf(szLevelNumberTag,"</level%03u>\r\n",uLevelNumber);   // Ende des zu löschenden Levels
                            if ((puRestPartStart = (uint8_t*)strstr((char*)pLevelgroupXml,szLevelNumberTag)) != NULL) {
                                puRestPartStart = puRestPartStart + strlen(szLevelNumberTag);   // Start vom Rest der Levelgruppe
                                if (puRestPartStart < puLastTag) {
                                    DynStringAdd(S,(char*)puRestPartStart);         // Den Rest anhängen
                                    if ((nNewLevelCount = RenumLevelgroup((uint8_t *)S->pszString)) > 0) {
                                        // Levelgruppe ist fast fertig, jetzt Anpassung von <levelcount>x</levelcount>
                                        puStartLevelCountTag = strstr(S->pszString,"<levelcount>");
                                        puEndLevelCountTag = strstr(S->pszString,"</levelcount>\r\n");
                                        if ((puStartLevelCountTag != NULL) && (puStartLevelCountTag > S->pszString) && (puEndLevelCountTag != NULL)) {
                                            uPart1Size = puStartLevelCountTag - S->pszString;
                                            puPart2 = malloc(uPart1Size + 1);
                                            if (puPart2 != NULL) {
                                                memset(puPart2,0,uPart1Size + 1);
                                                memcpy(puPart2,S->pszString,uPart1Size);
                                                DynStringAdd(S2,(char*)puPart2);
                                                DynStringAdd(S2,"<levelcount>");
                                                sprintf(szLevelCount,"%d",nNewLevelCount);
                                                DynStringAdd(S2,szLevelCount);
                                                DynStringAdd(S2,puEndLevelCountTag);
                                                if (UpdateLevelgroupHash((uint8_t*)S2->pszString,uLevelgroupHash) == 0) {
                                                    // Levelgruppe aktualisieren
                                                    if (WriteFile(SelectedLevelgroup.szFilename,(uint8_t*)S2->pszString,S2->nLen,false) == 0) {
                                                        if (GetLevelgroupFiles() == 0) {    // Wenn das nicht funktioniert, kann nicht weitergemacht werden!
                                                            if (SelectAlternativeLevelgroup(uLevelgroupHash,false) == 0) {
                                                                InitLists();
                                                                nErrorCode = 0;
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
    DynStringFree(S);
    DynStringFree(S2);
    SAFE_FREE(puPart1);
    SAFE_FREE(puPart2);
    SAFE_FREE(pLevelgroupXml);
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           LevelgroupOperaton_Move
------------------------------------------------------------------------------
Beschreibung: Verschiebt einen Level innerhalb einer Levelgruppe an eine andere
              Stelle.
Parameter
      Eingang: uSrcLevelNumber, uint32_t, Levelnummer -> Level, dass verschoben werden soll
               uDestLevelNumber, uint32_t, Levelnummer -> an diese Position wird verschoben
      Ausgang: -
Rückgabewert:  0 = Alles OK, sonst Fehler
Seiteneffekte: SelectedLevelgroup.x
------------------------------------------------------------------------------*/
int LevelgroupOperaton_Move(uint32_t uSrcLevelNumber,uint32_t uDestLevelNumber) {
    uint32_t I;
    bool bOK;
    int nLevels[EMERALD_MAX_LEVELCOUNT];    // Dieses Array enthält die neue Levelsortierung
    int nErrorCode = -1;
    int nNewLevelCount = 0;
    uint32_t uXmlLen;
    uint8_t *pLevelgroupXml = NULL;
    uint8_t *pszlevel = NULL;
    uint8_t *puLastTag;
    uint8_t *puEndPart1;            // beinhaltet alle Level einschließlich uLevelNumber
    uint8_t *puPart1 = NULL;
    uint8_t *puRestPartStart;
    uint32_t uPart1Size = 0;
    char szLevelNumberTag[32];      // "level000"
    DYNSTRING *S;
    uint8_t uLevelgroupHash[16];

    if (uSrcLevelNumber == uDestLevelNumber) {
        return 0;
    }
    for (I = 0; I < SelectedLevelgroup.uLevelCount; I++) {
        nLevels[I] = I;
    }
    if (uSrcLevelNumber < uDestLevelNumber) {           // Level nach "oben" ziehen
        for (I = uSrcLevelNumber; I < uDestLevelNumber; I++) {
            nLevels[I] = nLevels[I + 1];
        }
        nLevels[uDestLevelNumber] = uSrcLevelNumber;
    } else {                                            // Level nach "unten" ziehen
        for (I = uSrcLevelNumber; I > uDestLevelNumber; I--) {
            nLevels[I] = nLevels[I - 1];
        }
        nLevels[uDestLevelNumber] = uSrcLevelNumber;
    }
    S = DynStringInit();
    if ((uSrcLevelNumber < SelectedLevelgroup.uLevelCount) && (uDestLevelNumber < SelectedLevelgroup.uLevelCount) && (SelectedLevelgroup.bOK)  && (S != NULL)) {
        if ((pLevelgroupXml = ReadFile(SelectedLevelgroup.szFilename,&uXmlLen)) != NULL) {     // Levelgruppen-Datei einlesen
            if ((puLastTag = (uint8_t*)strstr((char*)pLevelgroupXml,"</levelgroup>")) != NULL) {     // "Höchster" Pointer
                sprintf(szLevelNumberTag,"<level000>\r\n");   // Ende des ersten Teils, bzw. Start von Level 000
                puEndPart1 = (uint8_t*)strstr((char*)pLevelgroupXml,szLevelNumberTag);     // Hier ist Teil 1 zu Ende
                if ((puEndPart1 != NULL) && (puEndPart1 > pLevelgroupXml) && (puEndPart1 < puLastTag)) {
                    uPart1Size = puEndPart1 - pLevelgroupXml;
                    puPart1 = malloc(uPart1Size + 1); // + 1 für String-Terminierung
                    if (puPart1 != NULL) {
                        memset(puPart1,0,uPart1Size + 1);
                        memcpy(puPart1,pLevelgroupXml,uPart1Size);
                        DynStringAdd(S,(char*)puPart1);         // Alle Daten vor level000 im String.
                        bOK = true;
                        for (I = 0; (I < SelectedLevelgroup.uLevelCount) && bOK; I++) {
                            pszlevel = GetLevelFromLevelgroup(pLevelgroupXml,nLevels[I]); // Level X holen
                            if (pszlevel != NULL) {
                                if (DynStringAdd(S,(char*)pszlevel) != 0) {
                                    bOK =  false;
                                }
                                SAFE_FREE(pszlevel);
                            } else {
                                bOK = false;
                            }
                        }
                        if (bOK) {
                            if (((puRestPartStart = (uint8_t*)strstr((char*)pLevelgroupXml,"<password_md5_hash>")) != NULL) && (puRestPartStart < puLastTag)) {
                                DynStringAdd(S,(char*)puRestPartStart);
                                if ((nNewLevelCount = RenumLevelgroup((uint8_t *)S->pszString)) > 0) {
                                    if (nNewLevelCount == SelectedLevelgroup.uLevelCount) { // Hier darf sich nichts geändert haben
                                        if (UpdateLevelgroupHash((uint8_t*)S->pszString,uLevelgroupHash) == 0) {
                                            // Levelgruppe aktualisieren
                                            if (WriteFile(SelectedLevelgroup.szFilename,(uint8_t*)S->pszString,S->nLen,false) == 0) {
                                                if (GetLevelgroupFiles() == 0) {    // Wenn das nicht funktioniert, kann nicht weitergemacht werden!
                                                    if (SelectAlternativeLevelgroup(uLevelgroupHash,false) == 0) {
                                                        InitLists();
                                                        nErrorCode = 0;
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
    DynStringFree(S);
    SAFE_FREE(puPart1);
    SAFE_FREE(pLevelgroupXml);
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           LevelgroupOperaton_NewGroup
------------------------------------------------------------------------------
Beschreibung: Erzeugt eine neue Levelgruppe mit einem Level.
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  0 = Alles OK, sonst Fehler
Seiteneffekte: SelectedLevelgroup.x, ge_new_levelgroup[]
------------------------------------------------------------------------------*/
int LevelgroupOperaton_NewGroup(void) {
    int nErrorCode = -1;
    uint8_t *pszlevel = NULL;
    uint8_t uLevelgroupHash[16];
    uint8_t *upGroupnameStartTag = NULL;
    uint8_t *upGroupnameEndTag = NULL;
    char szTimestamp[16];    // Format: YYYYMMDD_HHMMSS
    char szLevelgroupname[EMERALD_GROUPNAME_LEN + 1];
    char szFilename[EMERALD_MAX_FILENAME_LEN];

    pszlevel = malloc(strlen((char*)ge_new_levelgroup) + 1);
    if (pszlevel != NULL) {
        strcpy((char*)pszlevel,(char*)ge_new_levelgroup);
        strcpy(szLevelgroupname,"NEW LG "); // <groupname>NEW LG 20230411_222417</groupname>
        GetActualTimestamp(szTimestamp);
        strcat(szLevelgroupname,szTimestamp);
        sprintf(szFilename,"%s/LG_%s.xml",EMERALD_LEVELGROUPS_DIRECTORYNAME,szTimestamp);
        upGroupnameStartTag = (uint8_t*)strstr((char*)pszlevel,"<groupname>");
        upGroupnameEndTag = (uint8_t*)strstr((char*)pszlevel,"</groupname>");
        if ((upGroupnameStartTag != NULL) && (upGroupnameEndTag != NULL)) {
            if ((upGroupnameEndTag - upGroupnameStartTag) == 33) {
                memcpy(upGroupnameStartTag + strlen("<groupname>"),szLevelgroupname,strlen(szLevelgroupname));
                if (UpdateCreateTimestamp(pszlevel) == 0) {
                    if (UpdateLevelgroupHash((uint8_t*)pszlevel,uLevelgroupHash) == 0) {
                        if (WriteFile(szFilename,pszlevel,strlen((char*)pszlevel),false) == 0) {
                            if (GetLevelgroupFiles() == 0) {    // Wenn das nicht funktioniert, kann nicht weitergemacht werden!
                                if (SelectAlternativeLevelgroup(uLevelgroupHash,false) == 0) {
                                    InitLists();
                                    nErrorCode = 0;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    SAFE_FREE(pszlevel);
    return nErrorCode;
}




/*----------------------------------------------------------------------------
Name:           LevelgroupOperaton_RenameGroupname
------------------------------------------------------------------------------
Beschreibung: Benennt den Levelgruppennamen um und bietet hierfür eine Eingabe
              an.
Parameter
      Eingang: SDL_Renderer *, pRenderer, Zeiger auf Renderer
      Ausgang: -
Rückgabewert:  0 = Alles OK, sonst Fehler
Seiteneffekte: SelectedLevelgroup.x, Inputstates.x, MainMenu.x
------------------------------------------------------------------------------*/
int LevelgroupOperaton_RenameGroupname(SDL_Renderer *pRenderer) {
    int nErrorCode = 0;
    int nColorDimm;
    uint32_t uModVolume;
    bool bExit = false;
    bool bPrepareExit = false;
    bool bChangeLevelgroupname = false;
    uint32_t uKey;
    uint32_t uCursorPos;
    uint32_t I;
    uint32_t uXmlLen;
    char szTempName[EMERALD_GROUPNAME_LEN + 1];
    uint8_t *pLevelgroupXml;
    uint8_t *puPart2;
    DYNSTRING *S;
    uint8_t uLevelgroupHash[16];

    memset(szTempName,0,sizeof(szTempName));
    strcpy(szTempName,SelectedLevelgroup.szLevelgroupname);
    uCursorPos = strlen(szTempName);
    nColorDimm = 0;
    uModVolume = 0;
    SetMenuBorderAndClear();
    SetMenuText(MainMenu.uMenuScreen,"CHANGE LEVELGROUPNAME",10,4,EMERALD_FONT_BLUE);
    // Mauer-Eingabefeld
    for (I = 0; I < EMERALD_GROUPNAME_LEN + 3; I++) {
        MainMenu.uMenuScreen[6 * MainMenu.uXdim + I + 6] = EMERALD_WALL_CORNERED;
        MainMenu.uMenuScreen[8 * MainMenu.uXdim + I + 6] = EMERALD_WALL_CORNERED;
    }
    MainMenu.uMenuScreen[7 * MainMenu.uXdim + 6] = EMERALD_WALL_CORNERED;
    MainMenu.uMenuScreen[7 * MainMenu.uXdim + 6 + EMERALD_GROUPNAME_LEN + 2] = EMERALD_WALL_CORNERED;
    while ((nErrorCode == 0) && (!bExit)) {
        UpdateInputStates();
        // Eingabe eines Zeichens für den Levelgruppennamen
        nErrorCode = SDL_SetTextureColorMod(GetTextureByIndex(71),MainMenu.uFlashBrightness[MainMenu.uFlashIndex],MainMenu.uFlashBrightness[MainMenu.uFlashIndex],MainMenu.uFlashBrightness[MainMenu.uFlashIndex]);
        MainMenu.uFlashIndex++;
        if (MainMenu.uFlashIndex > MainMenu.uMaxFlashIndex) {
            MainMenu.uFlashIndex = 0;
        }
        SetMenuText(MainMenu.uMenuScreen,"                         ",7,7,EMERALD_FONT_GREEN);
        SetMenuText(MainMenu.uMenuScreen,szTempName,7,7,EMERALD_FONT_GREEN);
        if (!bPrepareExit) {
            MainMenu.uMenuScreen[7 * MainMenu.uXdim + uCursorPos + 7] = EMERALD_STONE;
        }
        uKey = FilterBigFontKey(GetKey());
        if (uKey != 0) {
           if (uCursorPos < EMERALD_GROUPNAME_LEN) {
                szTempName[uCursorPos] = uKey;
                MainMenu.uMenuScreen[7 * MainMenu.uXdim + uCursorPos + 7] = EMERALD_SPACE;  // alten Cursor löschen
                uCursorPos++;
           }
        } else  if (InputStates.pKeyboardArray[SDL_SCANCODE_BACKSPACE]) {
             if (uCursorPos > 0) {
                szTempName[uCursorPos - 1] = 0;
                MainMenu.uMenuScreen[7 * MainMenu.uXdim + uCursorPos + 7] = EMERALD_SPACE;  // alten Cursor löschen
                uCursorPos--;
             }
             do {
                UpdateInputStates();
             } while (InputStates.pKeyboardArray[SDL_SCANCODE_BACKSPACE]);
        }
        else if (InputStates.pKeyboardArray[SDL_SCANCODE_ESCAPE]) {
            bPrepareExit = true;
        }
        else if (InputStates.pKeyboardArray[SDL_SCANCODE_RETURN]) {
            bPrepareExit = true;
            bChangeLevelgroupname = true;
        }
        if (nErrorCode == 0) {
            nErrorCode = RenderMenuElements(pRenderer);
        }
        SDL_RenderPresent(pRenderer);   // Renderer anzeigen, lässt Hauptschleife mit ~ 60 Hz (Bild-Wiederholfrequenz) laufen
        SDL_RenderClear(pRenderer);     // Renderer für nächstes Frame löschen
        if ((!bPrepareExit) && (nColorDimm < 100)) {
            nColorDimm = nColorDimm + 4;
            SetAllTextureColors(nColorDimm);
            uModVolume = uModVolume + 4;
            SetModVolume(uModVolume);
        } else if (bPrepareExit) {
            MainMenu.uMenuScreen[7 * MainMenu.uXdim + uCursorPos + 7] = EMERALD_SPACE;  // alten Cursor löschen
            if (nColorDimm > 0) {
                nColorDimm = nColorDimm - 4;
                SetAllTextureColors(nColorDimm);
                uModVolume = uModVolume -4;
                SetModVolume(uModVolume);
            } else {
                bExit = true;
            }
        }
        PlayMusic(false);
    }
    WaitNoKey();
    if (nErrorCode == 0) {
        nErrorCode = SDL_SetTextureColorMod(GetTextureByIndex(71),100,100,100); // Cursor-Stein auf volle Helligkeit stellen
    }
    if ((bChangeLevelgroupname) && (strcmp(szTempName,SelectedLevelgroup.szLevelgroupname) != 0)) {
        nErrorCode = -1;
        S = DynStringInit();
        if (S != NULL) {
            if ((pLevelgroupXml = ReadFile(SelectedLevelgroup.szFilename,&uXmlLen)) != NULL) {     // Levelgruppen-Datei einlesen
                if ((puPart2 = (uint8_t*)strstr((char*)pLevelgroupXml,"</groupname>")) != NULL) {     // "Höchster" Pointer
                    DynStringAdd(S,"<?xml version=\"1.0\"?>\r\n");
                    DynStringAdd(S,"<levelgroup>\r\n");
                    DynStringAdd(S,"<groupname>");
                    DynStringAdd(S,szTempName);
                    DynStringAdd(S,(char*)puPart2);
                    if (UpdateCreateTimestamp((uint8_t*)S->pszString) == 0) {
                        if (UpdateLevelgroupHash((uint8_t*)S->pszString,uLevelgroupHash) == 0) {
                            if (WriteFile(SelectedLevelgroup.szFilename,(uint8_t*)S->pszString,S->nLen,false) == 0) {
                                if (GetLevelgroupFiles() == 0) {    // Wenn das nicht funktioniert, kann nicht weitergemacht werden!
                                    if (SelectAlternativeLevelgroup(uLevelgroupHash,false) == 0) {
                                        InitLists();
                                        nErrorCode = 0;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        DynStringFree(S);
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           LevelgroupOperaton_Password
------------------------------------------------------------------------------
Beschreibung: Setzt oder lösct ein Levelgruppenpasswort. Falls ein Passwort
              gesetzt werden muss, wird hierfür eine Eingabe angeboten.
Parameter
      Eingang: SDL_Renderer *, pRenderer, Zeiger auf Renderer
      Ausgang: -
Rückgabewert:  0 = Alles OK, sonst Fehler
Seiteneffekte: SelectedLevelgroup.x, Inputstates.x, MainMenu.x
------------------------------------------------------------------------------*/
int LevelgroupOperaton_Password(SDL_Renderer *pRenderer) {
    int nErrorCode = 0;
    int nColorDimm;
    uint32_t I;
    uint32_t uModVolume;
    uint32_t uCursorPos;
    uint32_t uKey;
    uint32_t uXmlLen;
    uint32_t uPart1Size;
    char szPassword[EMERALD_GROUP_PASSWORD_LEN + 1];
    bool bPrepareExit = false;
    bool bExit = false;
    bool bChangeLevelgroupPassword = false;
    bool bClearLevelgroupPassword = false;
    char szLevelgroupPasswordHash[32 + 1];
    MD5Context MD5Password;
    uint8_t *pLevelgroupXml = NULL;
    uint8_t *puPart1End = NULL;
    uint8_t *puPart1 = NULL;;
    DYNSTRING *S = NULL;
    uint8_t uLevelgroupHash[16];

    if (strlen(SelectedLevelgroup.szPasswordHash) == 0) {
        // Levelgruppe hat zur Zeit keinen Passwort-Hash -> neues Passwort vergeben
        memset(szPassword,0,sizeof(szPassword));
        uCursorPos = 0;
        nColorDimm = 0;
        uModVolume = 0;
        SetMenuBorderAndClear();
        SetMenuText(MainMenu.uMenuScreen,"SET LEVELGROUP PASSWORD",8,4,EMERALD_FONT_BLUE);
        // Mauer-Eingabefeld
        for (I = 0; I < EMERALD_GROUP_PASSWORD_LEN + 3; I++) {
            MainMenu.uMenuScreen[6 * MainMenu.uXdim + I + 6] = EMERALD_WALL_CORNERED;
            MainMenu.uMenuScreen[8 * MainMenu.uXdim + I + 6] = EMERALD_WALL_CORNERED;
        }
        MainMenu.uMenuScreen[7 * MainMenu.uXdim + 6] = EMERALD_WALL_CORNERED;
        MainMenu.uMenuScreen[7 * MainMenu.uXdim + 6 + EMERALD_GROUP_PASSWORD_LEN + 2] = EMERALD_WALL_CORNERED;
        while ((nErrorCode == 0) && (!bExit)) {
            UpdateInputStates();
            // Eingabe eines Zeichens für den Levelgruppennamen
            nErrorCode = SDL_SetTextureColorMod(GetTextureByIndex(71),MainMenu.uFlashBrightness[MainMenu.uFlashIndex],MainMenu.uFlashBrightness[MainMenu.uFlashIndex],MainMenu.uFlashBrightness[MainMenu.uFlashIndex]);
            MainMenu.uFlashIndex++;
            if (MainMenu.uFlashIndex > MainMenu.uMaxFlashIndex) {
                MainMenu.uFlashIndex = 0;
            }
            SetMenuText(MainMenu.uMenuScreen,"                         ",7,7,EMERALD_FONT_GREEN);
            SetMenuText(MainMenu.uMenuScreen,szPassword,7,7,EMERALD_FONT_GREEN);
            if (!bPrepareExit) {
                MainMenu.uMenuScreen[7 * MainMenu.uXdim + uCursorPos + 7] = EMERALD_STONE;
            }
            uKey = FilterBigFontKey(GetKey());
            if (uKey != 0) {
               if (uCursorPos < EMERALD_GROUP_PASSWORD_LEN) {
                    szPassword[uCursorPos] = uKey;
                    MainMenu.uMenuScreen[7 * MainMenu.uXdim + uCursorPos + 7] = EMERALD_SPACE;  // alten Cursor löschen
                    uCursorPos++;
               }
            } else  if (InputStates.pKeyboardArray[SDL_SCANCODE_BACKSPACE]) {
                 if (uCursorPos > 0) {
                    szPassword[uCursorPos - 1] = 0;
                    MainMenu.uMenuScreen[7 * MainMenu.uXdim + uCursorPos + 7] = EMERALD_SPACE;  // alten Cursor löschen
                    uCursorPos--;
                 }
                 do {
                    UpdateInputStates();
                 } while (InputStates.pKeyboardArray[SDL_SCANCODE_BACKSPACE]);
            }
            else if (InputStates.pKeyboardArray[SDL_SCANCODE_ESCAPE]) {
                bPrepareExit = true;
            }
            else if (InputStates.pKeyboardArray[SDL_SCANCODE_RETURN]) {
                bPrepareExit = true;
                bChangeLevelgroupPassword = (strlen(szPassword) > 0);
            }
            if (nErrorCode == 0) {
                nErrorCode = RenderMenuElements(pRenderer);
            }
            SDL_RenderPresent(pRenderer);   // Renderer anzeigen, lässt Hauptschleife mit ~ 60 Hz (Bild-Wiederholfrequenz) laufen
            SDL_RenderClear(pRenderer);     // Renderer für nächstes Frame löschen
            if ((!bPrepareExit) && (nColorDimm < 100)) {
                nColorDimm = nColorDimm + 4;
                SetAllTextureColors(nColorDimm);
                uModVolume = uModVolume + 4;
                SetModVolume(uModVolume);
            } else if (bPrepareExit) {
                MainMenu.uMenuScreen[7 * MainMenu.uXdim + uCursorPos + 7] = EMERALD_SPACE;  // alten Cursor löschen
                if (nColorDimm > 0) {
                    nColorDimm = nColorDimm - 4;
                    SetAllTextureColors(nColorDimm);
                    uModVolume = uModVolume -4;
                    SetModVolume(uModVolume);
                } else {
                    bExit = true;
                }
            }
            PlayMusic(false);
        }
        WaitNoKey();
        if (nErrorCode == 0) {
            nErrorCode = SDL_SetTextureColorMod(GetTextureByIndex(71),100,100,100); // Cursor-Stein auf volle Helligkeit stellen
        }
        if (bChangeLevelgroupPassword) {
            // Passwort-Hash bilden
            md5Init(&MD5Password);
            md5Update(&MD5Password,(uint8_t*)szPassword,strlen(szPassword));
            md5Finalize(&MD5Password);
            GetMd5String(MD5Password.digest,szLevelgroupPasswordHash);
        }
    } else {
        // Levelgruppe hat bereits einen Passwort-Hash -> Passwort entfernen
        bClearLevelgroupPassword = true;
    }
    if ((bChangeLevelgroupPassword) || (bClearLevelgroupPassword)) {
        nErrorCode = -1;
        S = DynStringInit();
        if (S != NULL) {
            if ((pLevelgroupXml = ReadFile(SelectedLevelgroup.szFilename,&uXmlLen)) != NULL) {     // Levelgruppen-Datei einlesen
                if ((puPart1End = (uint8_t*)strstr((char*)pLevelgroupXml,"<password_md5_hash>")) != NULL) {     // Ende Part1
                    uPart1Size = puPart1End - pLevelgroupXml;
                    puPart1 = malloc(uPart1Size + 1);
                    if (puPart1 != NULL) {
                        memset(puPart1,0,uPart1Size + 1);
                        memcpy(puPart1,pLevelgroupXml,uPart1Size);
                        DynStringAdd(S,(char*)puPart1);
                        DynStringAdd(S,"<password_md5_hash>");
                        if (bChangeLevelgroupPassword) {
                            DynStringAdd(S,szLevelgroupPasswordHash);
                        }
                        DynStringAdd(S,"</password_md5_hash>\r\n");
                        DynStringAdd(S,"<levelgroup_md5_hash>00000000000000000000000000000000</levelgroup_md5_hash>\r\n");
                        DynStringAdd(S,"</levelgroup>");
                        if (UpdateCreateTimestamp((uint8_t*)S->pszString) == 0) {
                            if (UpdateLevelgroupHash((uint8_t*)S->pszString,uLevelgroupHash) == 0) {
                                if (WriteFile(SelectedLevelgroup.szFilename,(uint8_t*)S->pszString,S->nLen,false) == 0) {
                                    if (GetLevelgroupFiles() == 0) {    // Wenn das nicht funktioniert, kann nicht weitergemacht werden!
                                        if (SelectAlternativeLevelgroup(uLevelgroupHash,false) == 0) {
                                            InitLists();
                                            nErrorCode = 0;
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
    DynStringFree(S);
    SAFE_FREE(puPart1);
    SAFE_FREE(pLevelgroupXml);
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           LevelgroupOperaton_AskPassword
------------------------------------------------------------------------------
Beschreibung: Falls eine Levelgruppe passwortgeschützt ist, kommt die folgende
              Abfrage.
Parameter
      Eingang: SDL_Renderer *, pRenderer, Zeiger auf Renderer
      Ausgang: -
Rückgabewert:  0 = Alles OK
               -1 = Fehler
               -2 = Kennwort falsch
Seiteneffekte: SelectedLevelgroup.x, Inputstates.x, MainMenu.x
------------------------------------------------------------------------------*/
int LevelgroupOperaton_AskPassword(SDL_Renderer *pRenderer) {
    int nErrorCode = 0;
    int nColorDimm;
    uint32_t I;
    uint32_t uCursorPos;
    uint32_t uModVolume;
    uint32_t uKey;
    uint8_t uLevelgroupPasswordHash[16];
    char szLevelgroupPasswordHash[32 + 1];
    MD5Context MD5Password;
    char szPassword[EMERALD_GROUP_PASSWORD_LEN + 1];
    bool bPrepareExit = false;
    bool bExit = false;

    if (strlen(SelectedLevelgroup.szPasswordHash) > 0) {
        GetMd5HashFromString(SelectedLevelgroup.szPasswordHash,uLevelgroupPasswordHash);
        memset(szPassword,0,sizeof(szPassword));
        uCursorPos = 0;
        nColorDimm = 0;
        uModVolume = 0;
        SetMenuBorderAndClear();
        SetMenuText(MainMenu.uMenuScreen,"LEVELGROUP IS PASSWORD PROTECTED",4,4,EMERALD_FONT_BLUE);
        // Mauer-Eingabefeld
        for (I = 0; I < EMERALD_GROUP_PASSWORD_LEN + 3; I++) {
            MainMenu.uMenuScreen[6 * MainMenu.uXdim + I + 6] = EMERALD_WALL_CORNERED;
            MainMenu.uMenuScreen[8 * MainMenu.uXdim + I + 6] = EMERALD_WALL_CORNERED;
        }
        MainMenu.uMenuScreen[7 * MainMenu.uXdim + 6] = EMERALD_WALL_CORNERED;
        MainMenu.uMenuScreen[7 * MainMenu.uXdim + 6 + EMERALD_GROUP_PASSWORD_LEN + 2] = EMERALD_WALL_CORNERED;
        while ((nErrorCode == 0) && (!bExit)) {
            UpdateInputStates();
            // Eingabe eines Zeichens für den Levelgruppennamen
            nErrorCode = SDL_SetTextureColorMod(GetTextureByIndex(71),MainMenu.uFlashBrightness[MainMenu.uFlashIndex],MainMenu.uFlashBrightness[MainMenu.uFlashIndex],MainMenu.uFlashBrightness[MainMenu.uFlashIndex]);
            MainMenu.uFlashIndex++;
            if (MainMenu.uFlashIndex > MainMenu.uMaxFlashIndex) {
                MainMenu.uFlashIndex = 0;
            }
            SetMenuText(MainMenu.uMenuScreen,"                         ",7,7,EMERALD_FONT_GREEN);
            SetMenuText(MainMenu.uMenuScreen,szPassword,7,7,EMERALD_FONT_GREEN);
            if (!bPrepareExit) {
                MainMenu.uMenuScreen[7 * MainMenu.uXdim + uCursorPos + 7] = EMERALD_STONE;
            }
            uKey = FilterBigFontKey(GetKey());
            if (uKey != 0) {
               if (uCursorPos < EMERALD_GROUP_PASSWORD_LEN) {
                    szPassword[uCursorPos] = uKey;
                    MainMenu.uMenuScreen[7 * MainMenu.uXdim + uCursorPos + 7] = EMERALD_SPACE;  // alten Cursor löschen
                    uCursorPos++;
               }
            } else  if (InputStates.pKeyboardArray[SDL_SCANCODE_BACKSPACE]) {
                 if (uCursorPos > 0) {
                    szPassword[uCursorPos - 1] = 0;
                    MainMenu.uMenuScreen[7 * MainMenu.uXdim + uCursorPos + 7] = EMERALD_SPACE;  // alten Cursor löschen
                    uCursorPos--;
                 }
                 do {
                    UpdateInputStates();
                 } while (InputStates.pKeyboardArray[SDL_SCANCODE_BACKSPACE]);
            }
            else if ((InputStates.pKeyboardArray[SDL_SCANCODE_ESCAPE]) || (InputStates.pKeyboardArray[SDL_SCANCODE_RETURN])) {
                bPrepareExit = true;
            }
            if (nErrorCode == 0) {
                nErrorCode = RenderMenuElements(pRenderer);
            }
            SDL_RenderPresent(pRenderer);   // Renderer anzeigen, lässt Hauptschleife mit ~ 60 Hz (Bild-Wiederholfrequenz) laufen
            SDL_RenderClear(pRenderer);     // Renderer für nächstes Frame löschen
            if ((!bPrepareExit) && (nColorDimm < 100)) {
                nColorDimm = nColorDimm + 4;
                SetAllTextureColors(nColorDimm);
                uModVolume = uModVolume + 4;
                SetModVolume(uModVolume);
            } else if (bPrepareExit) {
                MainMenu.uMenuScreen[7 * MainMenu.uXdim + uCursorPos + 7] = EMERALD_SPACE;  // alten Cursor löschen
                if (nColorDimm > 0) {
                    nColorDimm = nColorDimm - 4;
                    SetAllTextureColors(nColorDimm);
                    uModVolume = uModVolume -4;
                    SetModVolume(uModVolume);
                } else {
                    bExit = true;
                }
            }
            PlayMusic(false);
        }
        WaitNoKey();
        if (nErrorCode == 0) {
            nErrorCode = SDL_SetTextureColorMod(GetTextureByIndex(71),100,100,100); // Cursor-Stein auf volle Helligkeit stellen
        }
        if (nErrorCode == 0) {
            if (strlen(szPassword) > 0) {
                // Passwort-Hash bilden
                md5Init(&MD5Password);
                md5Update(&MD5Password,(uint8_t*)szPassword,strlen(szPassword));
                md5Finalize(&MD5Password);
                GetMd5String(MD5Password.digest,szLevelgroupPasswordHash);
                if (strcmp(SelectedLevelgroup.szPasswordHash,szLevelgroupPasswordHash) == 0) {
                    nErrorCode = 0;
                } else {
                    nErrorCode = -2;
                }
            } else {
                nErrorCode = -2;    // Nichts eingegeben
            }
        }
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           LevelgroupOperaton_ImportDC3
------------------------------------------------------------------------------
Beschreibung: Bietet eine Dateiauswahl für den DC3-Levelimport an.
Parameter
      Eingang: SDL_Renderer *, pRenderer, Zeiger auf Renderer
      Ausgang: -
Rückgabewert:  0 = Alles OK, sonst Fehler
Seiteneffekte: Inputstates.x, MainMenu.x, Dc3LevelFileList[].x, Ed.x
------------------------------------------------------------------------------*/
int LevelgroupOperaton_ImportDC3(SDL_Renderer *pRenderer) {
    int nErrorCode = 0;
    int nColorDimm;
    int nNewLevelCount = 0;
    uint32_t uBeamPosition;
    uint32_t uModVolume;
    uint32_t I;
    bool bExit = false;
    bool bPrepareExit = false;
    uint32_t uXmlLen;
    uint8_t *pLevelgroupXml = NULL;
    uint8_t *puEndPart1 = NULL;
    uint32_t uPart1Size;
    uint8_t *puLastTag = NULL;
    uint8_t *puPart1 = NULL;
    uint8_t *puPart2 = NULL;
    uint8_t *puRestPartStart = NULL;
    char *puStartLevelCountTag = NULL;
    char *puEndLevelCountTag = NULL;
    DYNSTRING *XML = NULL;
    DYNSTRING *S = NULL;
    DYNSTRING *S2 = NULL;
    char szFullFilename[EMERALD_MAX_FILENAME_LEN * 2];  // Mit Directory
    char szLevelNumberTag[32];      // "level000"
    char szLevelCount[16];
    uint8_t uLevelgroupHash[16];

    S = DynStringInit();
    S2 = DynStringInit();
    if ((S == NULL) || (S2 == NULL)) {
        return -1;
    }
    nColorDimm = 0;
    uModVolume = 0;
    SetMenuBorderAndClear();
    SetMenuText(MainMenu.uMenuScreen,"SELECT BITMAP TO IMPORT",-1,0,EMERALD_FONT_STEEL_BLUE);
    while ((nErrorCode == 0) && (!bExit)) {
        UpdateInputStates();

        if (nErrorCode == 0) {
            nErrorCode = RenderMenuElements(pRenderer);
        }
        // Import-Dateien auflisten
        for (I = 0; I < EMERALD_MAX_MAXIMPORTFILES_IN_LIST; I++) {
            if (MainMenu.uImportFileListDc3[I] != 0xFFFF) {
                PrintLittleFont(pRenderer,40,37 + I * 20,0,Dc3LevelFileList[MainMenu.uImportFileListDc3[I]].szShowFilename,K_RELATIVE);
            }
        }
        nErrorCode = ImportMenuSelectFile(pRenderer,&uBeamPosition);
        if (uBeamPosition != 0xFFFFFFFF) {
            strcpy(szFullFilename,EMERALD_IMPORTDC3_DIRECTORYNAME);
            strcat(szFullFilename,"/");             // Funktioniert auch unter Windows
            strcat(szFullFilename,Dc3LevelFileList[MainMenu.uImportFileListDc3[uBeamPosition]].szFilename);
            if (LevelConverterFromBitap(szFullFilename) == 0) {
                // Ab hier befindet sich das konvertierte Bitmap-Level in Ed.x
                if (SetLevelBorder(Ed.pLevel,false,false) == 0) { // 1. false = Level nicht auf EMERALD_SPACE setzen, 2. false = Nur auf EMERALD_STEEL setzen, wenn noch kein kompatibles Stahl-Element
                    InitYamExplosions(Ed.YamExplosions);
                    Ed.uReplicatorRedObject = EMERALD_SPACE;
                    Ed.uReplicatorGreenObject = EMERALD_SPACE;
                    Ed.uReplicatorBlueObject = EMERALD_SPACE;
                    Ed.uReplicatorYellowObject = EMERALD_SPACE;
                    XML = GetLevelXmlFromEditor();
                    if (XML != NULL) {
                        if ((pLevelgroupXml = ReadFile(SelectedLevelgroup.szFilename,&uXmlLen)) != NULL) {     // Levelgruppen-Datei einlesen
                            if ((puLastTag = (uint8_t*)strstr((char*)pLevelgroupXml,"</levelgroup>")) != NULL) {     // "Höchster" Pointer
                                if (UpdateCreateTimestamp(pLevelgroupXml) == 0) {
                                    sprintf(szLevelNumberTag,"</level%03u>\r\n",SelectedLevelgroup.uLevelCount - 1);    // Ende-Tag des letzten Levels
                                    puEndPart1 = (uint8_t*)strstr((char*)pLevelgroupXml,szLevelNumberTag);     // Hier ist Teil 1 zu Ende
                                    if ((puEndPart1 != NULL) && (puEndPart1 > pLevelgroupXml) && (puEndPart1 < puLastTag)) {
                                        uPart1Size = puEndPart1 - pLevelgroupXml + strlen(szLevelNumberTag);
                                        puPart1 = malloc(uPart1Size + 1); // + 1 für String-Terminierung
                                        if (puPart1 != NULL) {
                                            memset(puPart1,0,uPart1Size + 1);
                                            memcpy(puPart1,pLevelgroupXml,uPart1Size);
                                            DynStringAdd(S,(char*)puPart1);         // Es befinden sich nun alle Level im String S.
                                            sprintf(szLevelNumberTag,"<level%03u>\r\n",SelectedLevelgroup.uLevelCount);    // Start-Tag des neuen importierten Levels
                                            DynStringAdd(S,szLevelNumberTag);       // Start-Tag hinzufügen
                                            DynStringAdd(S,XML->pszString);         // Das importierte Level hinzufügen
                                            sprintf(szLevelNumberTag,"</level%03u>\r\n",SelectedLevelgroup.uLevelCount);    // End-Tag des neuen importierten Levels
                                            DynStringAdd(S,szLevelNumberTag);       // End-Tag hinzufügen
                                            if ((puRestPartStart = (uint8_t*)strstr((char*)pLevelgroupXml,"<password_md5_hash>")) != NULL) {
                                                if (puRestPartStart < puLastTag) {
                                                    DynStringAdd(S,(char*)puRestPartStart);    // Den Rest hinzufügen
                                                    if ((nNewLevelCount = RenumLevelgroup((uint8_t *)S->pszString)) > 0) {
                                                        // Levelgruppe ist fast fertig, jetzt Anpassung von <levelcount>x</levelcount>
                                                        puStartLevelCountTag = strstr(S->pszString,"<levelcount>");
                                                        puEndLevelCountTag = strstr(S->pszString,"</levelcount>\r\n");
                                                        if ((puStartLevelCountTag != NULL) && (puStartLevelCountTag > S->pszString) && (puEndLevelCountTag != NULL)) {
                                                            uPart1Size = puStartLevelCountTag - S->pszString;
                                                            puPart2 = malloc(uPart1Size + 1);
                                                            if (puPart2 != NULL) {
                                                                memset(puPart2,0,uPart1Size + 1);
                                                                memcpy(puPart2,S->pszString,uPart1Size);
                                                                DynStringAdd(S2,(char*)puPart2);
                                                                DynStringAdd(S2,"<levelcount>");
                                                                sprintf(szLevelCount,"%d",nNewLevelCount);
                                                                DynStringAdd(S2,szLevelCount);
                                                                DynStringAdd(S2,puEndLevelCountTag);
                                                                if (UpdateLevelgroupHash((uint8_t*)S2->pszString,uLevelgroupHash) == 0) {
                                                                    // Levelgruppe aktualisieren
                                                                    if (WriteFile(SelectedLevelgroup.szFilename,(uint8_t*)S2->pszString,S2->nLen,false) == 0) {
                                                                        if (GetLevelgroupFiles() == 0) {    // Wenn das nicht funktioniert, kann nicht weitergemacht werden!
                                                                            if (SelectAlternativeLevelgroup(uLevelgroupHash,false) == 0) {
                                                                                InitLists();
                                                                                nErrorCode = 0;
                                                                                bPrepareExit = true;
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
            WaitNoKey();
        }
        ShowButtons(pRenderer);
        if ( (InputStates.pKeyboardArray[SDL_SCANCODE_ESCAPE]) || (IsButtonPressed(BUTTONLABEL_EXIT_HIGHSCORES))) {
            bPrepareExit = true;
        }

        SDL_RenderPresent(pRenderer);   // Renderer anzeigen, lässt Hauptschleife mit ~ 60 Hz (Bild-Wiederholfrequenz) laufen
        SDL_RenderClear(pRenderer);     // Renderer für nächstes Frame löschen


        if ((!bPrepareExit) && (nColorDimm < 100)) {
            nColorDimm = nColorDimm + 4;
            SetAllTextureColors(nColorDimm);
            uModVolume = uModVolume + 4;
            SetModVolume(uModVolume);
        } else if (bPrepareExit) {
            if (nColorDimm > 0) {
                nColorDimm = nColorDimm - 4;
                SetAllTextureColors(nColorDimm);
                uModVolume = uModVolume -4;
                SetModVolume(uModVolume);
            } else {
                bExit = true;
            }
        }
        PlayMusic(false);
    }
    DynStringFree(XML);
    DynStringFree(S);
    DynStringFree(S2);
    SAFE_FREE(Ed.pLevel);
    SAFE_FREE(pLevelgroupXml);
    SAFE_FREE(puPart1);
    SAFE_FREE(puPart2);
    WaitNoKey();
    return nErrorCode;
}
