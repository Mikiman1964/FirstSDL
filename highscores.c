#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include "highscores.h"
#include "loadlevel.h"
#include "mystd.h"

HIGHSCOREFILE HighscoreFile;
extern uint32_t g_LevelgroupFilesCount;
extern LEVELGROUPFILE LevelgroupFiles[EMERALD_MAX_LEVELGROUPFILES];


/*----------------------------------------------------------------------------
Name:           CheckHighScoresDir
------------------------------------------------------------------------------
Beschreibung: Prüft das Highscores-Directory und legt dieses an, falls es nicht existiert.
Parameter
      Eingang: -
      Ausgang: -

Rückgabewert:  int, 0 = kein Fehler, sonst Fehler
Seiteneffekte: -
------------------------------------------------------------------------------*/
int CheckHighScoresDir(void) {
    return CheckAndCreateDir(EMERALD_HIGHSCORES_DIRECTORYNAME);
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
        strcat(szFilename,"/");                                     // Funktioniert auch unter Windows
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
               bWellDone, bool, true = Level wurde geschafft
      Ausgang: -

Rückgabewert:  int, >= 0 = Alles OK, Neuer Highscore auf Index X
                    -1 = Fehler
                    -2 = Alles OK, kein neuer Highscore
Seiteneffekte: HighscoreFile.x
------------------------------------------------------------------------------*/
int InsertScore(char *pszName, uint32_t uLevel, uint32_t uScore,bool bWellDone) {
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
                if (bWellDone) {
                    HighscoreFile.TopTwenty[uLevel].uHighScore[S] |= 0x80000000;
                }
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


/*----------------------------------------------------------------------------
Name:           CleanUpHighScoreDir
------------------------------------------------------------------------------
Beschreibung: Löscht Highscore-Dateien, für die es keine Levelgruppe (mehr) gibt.
              Die verfügbaren Levelgroupfiles müssen bereits zur Verfügung stehen,
              d.h. die Funktion GetLevelgroupFiles() wurde erfolgreich aufgerufen.
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  int, 0 = Alles OK, sonst Fehler
Seiteneffekte: LevelgroupFiles.x, g_LevelgroupFilesCount
------------------------------------------------------------------------------*/
int CleanUpHighScoreDir(void) {
    int nErrorCode;
    DIR *dir;
    struct dirent *entry;
    size_t FilenameLen;
    char  szHiscoreMD5Hash[32 + 1];
    uint8_t uHiscoreMD5Hash[16];
    char szFilename[256];
    bool bActive;
    uint32_t I;

    nErrorCode = -1;
    if ((dir = opendir(EMERALD_HIGHSCORES_DIRECTORYNAME)) == NULL) {
        SDL_Log("%s: can not open directory: %s, error: %s",__FUNCTION__,EMERALD_HIGHSCORES_DIRECTORYNAME,strerror(errno));
        return nErrorCode;
    }
    nErrorCode = 0;
    while (((entry = readdir(dir)) != NULL) && (nErrorCode == 0) ) {
        FilenameLen = strlen(entry->d_name);
        if (FilenameLen == 41) {
            // Nur Dateien der folgenden Form prüfen: high_16228DC49F7E51AFA051D6FFCF13AD02.dat  5 + 32 + 4 = 41 Zeichen
            if ((memcmp(entry->d_name + FilenameLen - 4,EMERALD_HIGHSCORES_FILENAME_EXTENSION,4) == 0)  && (memcmp(entry->d_name,EMERALD_HIGHSCORES_FILENAME,5) == 0)) {
                memset(szHiscoreMD5Hash,0,sizeof(szHiscoreMD5Hash));
                memcpy(szHiscoreMD5Hash,entry->d_name + 5,32);
                GetMd5HashFromString(szHiscoreMD5Hash,uHiscoreMD5Hash);
                // Prüfen, ob der Hash in den Levelgruppen zu finden ist
                bActive = false;
                for (I = 0; (I < g_LevelgroupFilesCount) && (!bActive); I++) {
                    bActive = (memcmp(uHiscoreMD5Hash,LevelgroupFiles[I].uMd5Hash,16) == 0);
                }
                if (!bActive) {
                    strcpy(szFilename,EMERALD_HIGHSCORES_DIRECTORYNAME);
                    strcat(szFilename,"/");                                     // Funktioniert auch unter Windows
                    strcat(szFilename,entry->d_name);
                    SDL_Log("%s: deleting %s ...",__FUNCTION__,entry->d_name);
                    nErrorCode = unlink(szFilename);
                }
            }
        }
    }
    return nErrorCode;
}
