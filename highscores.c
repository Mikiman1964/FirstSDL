#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include "highscores.h"
#include "mystd.h"

HIGHSCOREFILE HighscoreFile;


/*----------------------------------------------------------------------------
Name:           CheckHighScoresDirectory
------------------------------------------------------------------------------
Beschreibung: Prüft das Highscores-Directory und legt dieses an, falls es nicht existiert.
Parameter
      Eingang: -
      Ausgang: -

Rückgabewert:  int, 0 = kein Fehler, sonst Fehler
Seiteneffekte: -
------------------------------------------------------------------------------*/
int CheckHighScoresDirectory(void) {
    return CheckAndCreateDirectory(EMERALD_HIGHSCORES_DIRECTORYNAME);
}


/*----------------------------------------------------------------------------
Name:           WriteHighScoreFile
------------------------------------------------------------------------------
Beschreibung: Erzeugt ein Highscore-File für eine levelgruppe.
Parameter
      Eingang: puLevelgroupHash, uint8_t *, Zeiger auf Levelgruppenhash (16 Bytes)
      Ausgang: -

Rückgabewert:  int, 0 = kein Fehler, sonst Fehler
Seiteneffekte: HighscoreFile.x
------------------------------------------------------------------------------*/
int WriteHighScoreFile(uint8_t *puLevelgroupHash) {
    int nErrorCode;
    char szFilename[256];
    char szHashString[32 + 1];

    nErrorCode = -1;
    if (puLevelgroupHash != NULL) {
        strcpy(szFilename,EMERALD_HIGHSCORES_DIRECTORYNAME);
        strcat(szFilename,"/");                                     // Funktioniert aich unter Windows
        strcat(szFilename,EMERALD_HIGHSCORES_FILENAME);             // "high_"
        GetMd5String(puLevelgroupHash,szHashString);
        strcat(szFilename,szHashString);
        strcat(szFilename,EMERALD_HIGHSCORES_FILENAME_EXTENSION);   // ".dat"
        nErrorCode = WriteFile(szFilename,(uint8_t*)&HighscoreFile,sizeof(HIGHSCOREFILE),false);
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           ReadHighScoreFile
------------------------------------------------------------------------------
Beschreibung: Liest die Highscore-Datei einer Levelgruppe. Falls ein Lesen nicht
              möglich ist, wird eine neue Leere angelegt.
Parameter
      Eingang: puLevelgroupHash, uint8_t *, Zeiger auf Levelgruppenhash (16 Bytes)
      Ausgang: -

Rückgabewert:  int, 0 = kein Fehler, sonst Fehler
Seiteneffekte: HighscoreFile.x
------------------------------------------------------------------------------*/
int ReadHighScoreFile(uint8_t *puLevelgroupHash) {
    int nErrorCode;
    char szFilename[256];
    char szHashString[32 + 1];
    uint8_t *pcFile;
    uint32_t uFileLen;

    nErrorCode = -1;
    if (puLevelgroupHash != NULL) {
        strcpy(szFilename,EMERALD_HIGHSCORES_DIRECTORYNAME);
        strcat(szFilename,"/");                                     // Funktioniert auch unter Windows
        strcat(szFilename,EMERALD_HIGHSCORES_FILENAME);             // "high_"
        GetMd5String(puLevelgroupHash,szHashString);
        strcat(szFilename,szHashString);
        strcat(szFilename,EMERALD_HIGHSCORES_FILENAME_EXTENSION);   // ".dat"
        pcFile = ReadFile(szFilename,&uFileLen);
        if ((pcFile != NULL) && (uFileLen == sizeof(HIGHSCOREFILE))) {
            memcpy(&HighscoreFile,pcFile,sizeof(HIGHSCOREFILE));
            SAFE_FREE(pcFile);
            nErrorCode = 0;
        } else {
            memset(&HighscoreFile,0,sizeof(HIGHSCOREFILE));
            nErrorCode = WriteFile(szFilename,(uint8_t*)&HighscoreFile,sizeof(HIGHSCOREFILE),false);
        }
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           InsertScore
------------------------------------------------------------------------------
Beschreibung: Fügt einen Score ggf. in die Highscoreliste ein, wenn dieser
              ausreichend hoch ist.

Parameter
      Eingang: pszName, char*, Zeiger auf Spielernamen
               uLevel, uint32_t, Levelnummer
               uScore, uint32_t, Score
      Ausgang: -

Rückgabewert:  int, >= 0 = Alles OK, Neuer Highscore auf Index X
                    -1 = Fehler
                    -2 = Alles OK, kein neuer Highscore
Seiteneffekte: HighscoreFile.x
------------------------------------------------------------------------------*/
int InsertScore(char *pszName, uint32_t uLevel, uint32_t uScore) {
    int nErrorCode;
    uint32_t I;
    uint32_t S;     // Index, wo neuer Score eingetragen werden kann
    bool bIndexFound;

    nErrorCode = -1;
    S = 0;
    if ((pszName != NULL) && (uLevel < EMERALD_MAX_LEVELCOUNT)) {
        if ((strlen(pszName) > 0) && (strlen(pszName) <= EMERALD_PLAYERNAME_LEN)) {
            // Die TopTwenty sind absteigend sortiert -> Prüfen, wo der aktuelle Score eingefügt werden kann
            bIndexFound =  false;
            for (I = 0; (I < EMERALD_HIGHSCORE_LISTLEN) && (!bIndexFound); I++) {
                if (uScore >= HighscoreFile.TopTwenty[uLevel].uHighScore[I]) {
                    S = I;
                    bIndexFound =  true;
                }
            }
            if (bIndexFound) {
                nErrorCode = (int)S;
                // Verschiebung durchführen
                for ( (I = EMERALD_HIGHSCORE_LISTLEN - 1); I > S; I--) {
                    HighscoreFile.TopTwenty[uLevel].uHighScore[I] = HighscoreFile.TopTwenty[uLevel].uHighScore[I - 1];
                    strcpy(HighscoreFile.TopTwenty[uLevel].szTopTwenty[I],HighscoreFile.TopTwenty[uLevel].szTopTwenty[I - 1]);
                }
                // Neuen Eintrag vornehmen
                HighscoreFile.TopTwenty[uLevel].uHighScore[S] = uScore;
                strcpy(HighscoreFile.TopTwenty[uLevel].szTopTwenty[S],pszName);
            } else {
                // Es reichte nicht für einen neuen Highscore -> nichts machen
                nErrorCode = -2;
            }
        }
    }
    SDL_Log("%s: Code: %d",__FUNCTION__,nErrorCode);
    return nErrorCode;
}
