#ifndef LOADLEVEL_H_INCLUDED
#define LOADLEVEL_H_INCLUDED

#include "ezxml.h"

#define LEVEL_XML_FILENAME      "Level.xml"

void PrintPlayfieldValues();

int GetLeveldimensionFromXml(ezxml_t xml,uint32_t *puX,uint32_t *puY);
int GetTitleAuthorVersionHashFromXml(ezxml_t xml);
int GetLevelScoresFromXml(ezxml_t xml);
int GetOtherLevelValuesFromXml(ezxml_t xml);
int GetLevelTimesFromXml(ezxml_t xml);
int GetReplicatorLighbarrierSettingsFromXml(ezxml_t xml);
int GetLetterMessagesFromXml(ezxml_t xml);
int GetYamExplosionsFromXml(ezxml_t xml);
int GetLeveldataFromXml(ezxml_t xml);
int GetMemoryForPlayfield(void);
int InitialisePlayfield(char *pszLevelFilename);
void InitYamExplosions(YAMEXPLOSION *pYamExplosions);
int GetManCoordinates(uint16_t *pLevel, uint32_t uXdim, uint32_t uYdim, uint32_t *puManXpos, uint32_t *puManYpos);
int CheckLevelBorder(void);
int CheckReplicators(uint16_t *pLevel, uint32_t uXdim, uint32_t uYdim);
int CheckAcidPools(uint16_t *pLevel, uint32_t uXdim, uint32_t uYdim);
void CloseAllDoors(void);
void SetCentralExplosionCoordinates(void);
void SetCentralMegaExplosionCoordinates(void);
#endif // LOADLEVEL_H_INCLUDED
