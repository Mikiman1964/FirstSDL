#ifndef LEVELGROUPOPERATION_H_INCLUDED
#define LEVELGROUPOPERATION_H_INCLUDED

#include "mySDL.h"
#include "mystd.h"


uint8_t *GetLevelFromLevelgroup(uint8_t *pszLevelgroup, uint32_t uLevelNumber);
int GetLevelTitleFromLevel(uint8_t *pszLevel, uint8_t *pszLevelTitle);
int LevelgroupOperaton_Edit(uint32_t uLevelNumber, DYNSTRING *NewLevel);
int LevelgroupOperaton_Copy(uint32_t uLevelNumber);
int LevelgroupOperaton_CopyClipboard(uint32_t uLevelNumber);
int LevelgroupOperaton_Delete(uint32_t uLevelNumber);
int LevelgroupOperaton_Move(uint32_t uSrcLevelNumber,uint32_t uDestLevelNumber);
int LevelgroupOperaton_NewGroup(void);
int LevelgroupOperaton_RenameGroupname(SDL_Renderer *pRenderer);
int LevelgroupOperaton_Password(SDL_Renderer *pRenderer);
int LevelgroupOperaton_AskPassword(SDL_Renderer *pRenderer);
int LevelgroupOperaton_ImportDC3(SDL_Renderer *pRenderer);
#endif // LEVELGROUPOPERATION_H_INCLUDED
