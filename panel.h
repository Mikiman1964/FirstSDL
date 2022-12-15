#ifndef PANEL_H_INCLUDED
#define PANEL_H_INCLUDED

void CheckPlayTime(void);
int ShowPanel(SDL_Renderer *pRenderer);
int WritePanelText(SDL_Renderer *pRenderer, const char *szText, int nXpos, int nYpos, int nFontSize, bool bGreenNumbers);
int WritePanelDynamitHammerKeys(SDL_Renderer *pRenderer);
int ConfirmMessage(SDL_Renderer *pRenderer);

#endif // PANEL_H_INCLUDED
