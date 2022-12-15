#ifndef EDITOR_H_INCLUDED
#define EDITOR_H_INCLUDED

#include "mySDL.h"

#define EDITORPANEL_X           WINDOW_W - EDITORPANEL_W
#define EDITORPANEL_Y           0

#define EDITORPANEL_W           110
#define EDITORPANEL_H           WINDOW_H

#define MAX_PANEL_ELEMENTS      (26 * 4)

#define MAX_X_ELEMENTS          ((WINDOW_W - EDITORPANEL_W) / (FONT_W / 2))

int Editor(SDL_Window *pWindow, SDL_Renderer *pRenderer);
int DrawEditorPanel(SDL_Renderer *pRenderer);
int FillPanel(SDL_Renderer *pRenderer);
uint32_t GetElementByMouseposition(int nMouseXpos, int nMouseYpos);
#endif // EDITOR_H_INCLUDED
