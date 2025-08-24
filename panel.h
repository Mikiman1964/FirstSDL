#ifndef PANEL_H_INCLUDED
#define PANEL_H_INCLUDED

void CheckPlayTime(void);
int32_t ShowPanel(SDL_Renderer *pRenderer);
int32_t WritePanelText(SDL_Renderer *pRenderer, const char *szText, int32_t nXpos, int32_t nYpos, int32_t nFontSize, uint32_t uFont);
int32_t WritePanelDynamitHammerKeys(SDL_Renderer *pRenderer);
int32_t WriteShieldValue(SDL_Renderer *pRenderer);
int32_t ConfirmMessage(SDL_Renderer *pRenderer);

#endif // PANEL_H_INCLUDED
