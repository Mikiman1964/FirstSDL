#include "gfx/textures.h"
#include <stdint.h>
#include "GetTextureIndexByElement.h"
#include "EmeraldMine.h"

extern PLAYFIELD Playfield;
extern uint8_t ge_ExitDoorSequence[8];


/*----------------------------------------------------------------------------
Name:           GetTextureIndexByElement
------------------------------------------------------------------------------
Beschreibung: Holt den entsprechenden Texture-Index anhand eines Elements und des
              Animations-Schrittes
Parameter
      Eingang: uElement, uint16_t, Element, z.B. EMERALD_MINE_RIGHT
               nAnimationCount, int32_t, Animationsschritt
      Ausgang: pfAngle, float *, Winkel, darf NULL sein
Rückgabewert:  uint32_t , Texture, wenn keine Texture ermittelt werden kann, wird
                SPACE (EMERALD_SPACE) zurückgegeben.
Seiteneffekte: Playfield.x, ge_ExitDoorSequence[]
------------------------------------------------------------------------------*/
uint32_t GetTextureIndexByElement(uint16_t uElement,int32_t nAnimationCount,float *pfAngle) {
    uint32_t uTextureIndex;
    uint32_t K;
    float fAngle;

    fAngle = 0;
    switch (uElement) {
        case (EMERALD_FONT_BLUE_EXCLAMATION):
            uTextureIndex = TEX_FONT_BLUE_EXCLAMATION_MARK;
            break;
        case (EMERALD_FONT_BLUE_DOUBLE_QUOTE):
            uTextureIndex = TEX_FONT_BLUE_DOUBLE_QUOTE;
            break;
        case (EMERALD_FONT_BLUE_SEMICOLON):
            uTextureIndex = TEX_FONT_BLUE_SEMICOLON;
            break;
        case (EMERALD_FONT_BLUE_ARROW_RIGHT):
            uTextureIndex = TEX_FONT_BLUE_ARROW_RIGHT;
            break;
        case (EMERALD_FONT_BLUE_ARROW_UP):
            uTextureIndex = TEX_FONT_BLUE_ARROW_UP;
            break;
        case (EMERALD_FONT_BLUE_ARROW_DOWN):
            uTextureIndex = TEX_FONT_BLUE_ARROW_DOWN;
            break;
        case (EMERALD_FONT_BLUE_APOSTROPHE):
            uTextureIndex = TEX_FONT_BLUE_APOSTROPHE;
            break;
        case (EMERALD_FONT_BLUE_BRACE_OPEN):
            uTextureIndex = TEX_FONT_BLUE_BRACKET_OPEN;
            break;
        case (EMERALD_FONT_BLUE_BRACE_CLOSE):
            uTextureIndex = TEX_FONT_BLUE_BRACKET_CLOSE;
            break;
        case (EMERALD_FONT_BLUE_COPYRIGHT):
            uTextureIndex = TEX_FONT_BLUE_COPYRIGHT;
            break;
        case (EMERALD_FONT_BLUE_PLUS):
            uTextureIndex = TEX_FONT_BLUE_PLUS;
            break;
        case (EMERALD_FONT_BLUE_COMMA):
            uTextureIndex = TEX_FONT_BLUE_COMMA;
            break;
        case (EMERALD_FONT_BLUE_MINUS):
            uTextureIndex = TEX_FONT_BLUE_MINUS;
            break;
        case (EMERALD_FONT_BLUE_DOT):
            uTextureIndex = TEX_FONT_BLUE_DOT;
            break;
        case (EMERALD_FONT_BLUE_SLASH):
            uTextureIndex = TEX_FONT_BLUE_SLASH;
            break;
        case (EMERALD_FONT_BLUE_0):
            uTextureIndex = TEX_FONT_BLUE_0;
            break;
        case (EMERALD_FONT_BLUE_1):
            uTextureIndex = TEX_FONT_BLUE_1;
            break;
        case (EMERALD_FONT_BLUE_2):
            uTextureIndex = TEX_FONT_BLUE_2;
            break;
        case (EMERALD_FONT_BLUE_3):
            uTextureIndex = TEX_FONT_BLUE_3;
            break;
        case (EMERALD_FONT_BLUE_4):
            uTextureIndex = TEX_FONT_BLUE_4;
            break;
        case (EMERALD_FONT_BLUE_5):
            uTextureIndex = TEX_FONT_BLUE_5;
            break;
        case (EMERALD_FONT_BLUE_6):
            uTextureIndex = TEX_FONT_BLUE_6;
            break;
        case (EMERALD_FONT_BLUE_7):
            uTextureIndex = TEX_FONT_BLUE_7;
            break;
        case (EMERALD_FONT_BLUE_8):
            uTextureIndex = TEX_FONT_BLUE_8;
            break;
        case (EMERALD_FONT_BLUE_9):
            uTextureIndex = TEX_FONT_BLUE_9;
            break;
        case (EMERALD_FONT_BLUE_DOUBLE_DOT):
            uTextureIndex = TEX_FONT_BLUE_DOUBLE_DOT;
            break;
        case (EMERALD_FONT_BLUE_PLATE):
            uTextureIndex = TEX_FONT_BLUE_AT;
            break;
        case (EMERALD_FONT_BLUE_ARROW_LEFT):
            uTextureIndex = TEX_FONT_BLUE_ARROW_LEFT;
            break;
        case (EMERALD_FONT_BLUE_QUESTION_MARK):
            uTextureIndex = TEX_FONT_BLUE_QUESTION_MARK;
            break;
        case (EMERALD_FONT_BLUE_A):
            uTextureIndex = TEX_FONT_BLUE_A;
            break;
        case (EMERALD_FONT_BLUE_B):
            uTextureIndex = TEX_FONT_BLUE_B;
            break;
        case (EMERALD_FONT_BLUE_C):
            uTextureIndex = TEX_FONT_BLUE_C;
            break;
        case (EMERALD_FONT_BLUE_D):
            uTextureIndex = TEX_FONT_BLUE_D;
            break;
        case (EMERALD_FONT_BLUE_E):
            uTextureIndex = TEX_FONT_BLUE_E;
            break;
        case (EMERALD_FONT_BLUE_F):
            uTextureIndex = TEX_FONT_BLUE_F;
            break;
        case (EMERALD_FONT_BLUE_G):
            uTextureIndex = TEX_FONT_BLUE_G;
            break;
        case (EMERALD_FONT_BLUE_H):
            uTextureIndex = TEX_FONT_BLUE_H;
            break;
        case (EMERALD_FONT_BLUE_I):
            uTextureIndex = TEX_FONT_BLUE_I;
            break;
        case (EMERALD_FONT_BLUE_J):
            uTextureIndex = TEX_FONT_BLUE_J;
            break;
        case (EMERALD_FONT_BLUE_K):
            uTextureIndex = TEX_FONT_BLUE_K;
            break;
        case (EMERALD_FONT_BLUE_L):
            uTextureIndex = TEX_FONT_BLUE_L;
            break;
        case (EMERALD_FONT_BLUE_M):
            uTextureIndex = TEX_FONT_BLUE_M;
            break;
        case (EMERALD_FONT_BLUE_N):
            uTextureIndex = TEX_FONT_BLUE_N;
            break;
        case (EMERALD_FONT_BLUE_O):
            uTextureIndex = TEX_FONT_BLUE_O;
            break;
        case (EMERALD_FONT_BLUE_P):
            uTextureIndex = TEX_FONT_BLUE_P;
            break;
        case (EMERALD_FONT_BLUE_Q):
            uTextureIndex = TEX_FONT_BLUE_Q;
            break;
        case (EMERALD_FONT_BLUE_R):
            uTextureIndex = TEX_FONT_BLUE_R;
            break;
        case (EMERALD_FONT_BLUE_S):
            uTextureIndex = TEX_FONT_BLUE_S;
            break;
        case (EMERALD_FONT_BLUE_T):
            uTextureIndex = TEX_FONT_BLUE_T;
            break;
        case (EMERALD_FONT_BLUE_U):
            uTextureIndex = TEX_FONT_BLUE_U;
            break;
        case (EMERALD_FONT_BLUE_V):
            uTextureIndex = TEX_FONT_BLUE_V;
            break;
        case (EMERALD_FONT_BLUE_W):
            uTextureIndex = TEX_FONT_BLUE_W;
            break;
        case (EMERALD_FONT_BLUE_X):
            uTextureIndex = TEX_FONT_BLUE_X;
            break;
        case (EMERALD_FONT_BLUE_Y):
            uTextureIndex = TEX_FONT_BLUE_Y;
            break;
        case (EMERALD_FONT_BLUE_Z):
            uTextureIndex = TEX_FONT_BLUE_Z;
            break;
        case (EMERALD_FONT_BLUE_AE):
            uTextureIndex = TEX_FONT_BLUE_AE;
            break;
        case (EMERALD_FONT_BLUE_OE):
            uTextureIndex = TEX_FONT_BLUE_OE;
            break;
        case (EMERALD_FONT_BLUE_UE):
            uTextureIndex = TEX_FONT_BLUE_UE;
            break;
        case (EMERALD_FONT_BLUE_STEEL_EXCLAMATION):
            uTextureIndex = TEX_FONT_BLUE_STEEL_EXCLAMATION_MARK;
            break;
        case (EMERALD_FONT_BLUE_STEEL_DOUBLE_QUOTE):
            uTextureIndex = TEX_FONT_BLUE_STEEL_DOUBLE_QUOTE;
            break;
        case (EMERALD_FONT_BLUE_STEEL_SEMICOLON):
            uTextureIndex = TEX_FONT_BLUE_STEEL_SEMICOLON;
            break;
        case (EMERALD_FONT_BLUE_STEEL_ARROW_RIGHT):
            uTextureIndex = TEX_FONT_BLUE_STEEL_ARROW_RIGHT;
            break;
        case (EMERALD_FONT_BLUE_STEEL_ARROW_UP):
            uTextureIndex = TEX_FONT_BLUE_STEEL_ARROW_UP;
            break;
        case (EMERALD_FONT_BLUE_STEEL_ARROW_DOWN):
            uTextureIndex = TEX_FONT_BLUE_STEEL_ARROW_DOWN;
            break;
        case (EMERALD_FONT_BLUE_STEEL_APOSTROPHE):
            uTextureIndex = TEX_FONT_BLUE_STEEL_APOSTROPHE;
            break;
        case (EMERALD_FONT_BLUE_STEEL_BRACE_OPEN):
            uTextureIndex = TEX_FONT_BLUE_STEEL_BRACKET_OPEN;
            break;
        case (EMERALD_FONT_BLUE_STEEL_BRACE_CLOSE):
            uTextureIndex = TEX_FONT_BLUE_STEEL_BRACKET_CLOSE;
            break;
        case (EMERALD_FONT_BLUE_STEEL_COPYRIGHT):
            uTextureIndex = TEX_FONT_BLUE_STEEL_COPYRIGHT;
            break;
        case (EMERALD_FONT_BLUE_STEEL_PLUS):
            uTextureIndex = TEX_FONT_BLUE_STEEL_PLUS;
            break;
        case (EMERALD_FONT_BLUE_STEEL_COMMA):
            uTextureIndex = TEX_FONT_BLUE_STEEL_COMMA;
            break;
        case (EMERALD_FONT_BLUE_STEEL_MINUS):
            uTextureIndex = TEX_FONT_BLUE_STEEL_MINUS;
            break;
        case (EMERALD_FONT_BLUE_STEEL_DOT):
            uTextureIndex = TEX_FONT_BLUE_STEEL_DOT;
            break;
        case (EMERALD_FONT_BLUE_STEEL_SLASH):
            uTextureIndex = TEX_FONT_BLUE_STEEL_SLASH;
            break;
        case (EMERALD_FONT_BLUE_STEEL_0):
            uTextureIndex = TEX_FONT_BLUE_STEEL_0;
            break;
        case (EMERALD_FONT_BLUE_STEEL_1):
            uTextureIndex = TEX_FONT_BLUE_STEEL_1;
            break;
        case (EMERALD_FONT_BLUE_STEEL_2):
            uTextureIndex = TEX_FONT_BLUE_STEEL_2;
            break;
        case (EMERALD_FONT_BLUE_STEEL_3):
            uTextureIndex = TEX_FONT_BLUE_STEEL_3;
            break;
        case (EMERALD_FONT_BLUE_STEEL_4):
            uTextureIndex = TEX_FONT_BLUE_STEEL_4;
            break;
        case (EMERALD_FONT_BLUE_STEEL_5):
            uTextureIndex = TEX_FONT_BLUE_STEEL_5;
            break;
        case (EMERALD_FONT_BLUE_STEEL_6):
            uTextureIndex = TEX_FONT_BLUE_STEEL_6;
            break;
        case (EMERALD_FONT_BLUE_STEEL_7):
            uTextureIndex = TEX_FONT_BLUE_STEEL_7;
            break;
        case (EMERALD_FONT_BLUE_STEEL_8):
            uTextureIndex = TEX_FONT_BLUE_STEEL_8;
            break;
        case (EMERALD_FONT_BLUE_STEEL_9):
            uTextureIndex = TEX_FONT_BLUE_STEEL_9;
            break;
        case (EMERALD_FONT_BLUE_STEEL_DOUBLE_DOT):
            uTextureIndex = TEX_FONT_BLUE_STEEL_DOUBLE_DOT;
            break;
        case (EMERALD_FONT_BLUE_STEEL_PLATE):
            uTextureIndex = TEX_FONT_BLUE_STEEL_AT;
            break;
        case (EMERALD_FONT_BLUE_STEEL_ARROW_LEFT):
            uTextureIndex = TEX_FONT_BLUE_STEEL_ARROW_LEFT;
            break;
        case (EMERALD_FONT_BLUE_STEEL_QUESTION_MARK):
            uTextureIndex = TEX_FONT_BLUE_STEEL_QUESTION_MARK;
            break;
        case (EMERALD_FONT_BLUE_STEEL_A):
            uTextureIndex = TEX_FONT_BLUE_STEEL_A;
            break;
        case (EMERALD_FONT_BLUE_STEEL_B):
            uTextureIndex = TEX_FONT_BLUE_STEEL_B;
            break;
        case (EMERALD_FONT_BLUE_STEEL_C):
            uTextureIndex = TEX_FONT_BLUE_STEEL_C;
            break;
        case (EMERALD_FONT_BLUE_STEEL_D):
            uTextureIndex = TEX_FONT_BLUE_STEEL_D;
            break;
        case (EMERALD_FONT_BLUE_STEEL_E):
            uTextureIndex = TEX_FONT_BLUE_STEEL_E;
            break;
        case (EMERALD_FONT_BLUE_STEEL_F):
            uTextureIndex = TEX_FONT_BLUE_STEEL_F;
            break;
        case (EMERALD_FONT_BLUE_STEEL_G):
            uTextureIndex = TEX_FONT_BLUE_STEEL_G;
            break;
        case (EMERALD_FONT_BLUE_STEEL_H):
            uTextureIndex = TEX_FONT_BLUE_STEEL_H;
            break;
        case (EMERALD_FONT_BLUE_STEEL_I):
            uTextureIndex = TEX_FONT_BLUE_STEEL_I;
            break;
        case (EMERALD_FONT_BLUE_STEEL_J):
            uTextureIndex = TEX_FONT_BLUE_STEEL_J;
            break;
        case (EMERALD_FONT_BLUE_STEEL_K):
            uTextureIndex = TEX_FONT_BLUE_STEEL_K;
            break;
        case (EMERALD_FONT_BLUE_STEEL_L):
            uTextureIndex = TEX_FONT_BLUE_STEEL_L;
            break;
        case (EMERALD_FONT_BLUE_STEEL_M):
            uTextureIndex = TEX_FONT_BLUE_STEEL_M;
            break;
        case (EMERALD_FONT_BLUE_STEEL_N):
            uTextureIndex = TEX_FONT_BLUE_STEEL_N;
            break;
        case (EMERALD_FONT_BLUE_STEEL_O):
            uTextureIndex = TEX_FONT_BLUE_STEEL_O;
            break;
        case (EMERALD_FONT_BLUE_STEEL_P):
            uTextureIndex = TEX_FONT_BLUE_STEEL_P;
            break;
        case (EMERALD_FONT_BLUE_STEEL_Q):
            uTextureIndex = TEX_FONT_BLUE_STEEL_Q;
            break;
        case (EMERALD_FONT_BLUE_STEEL_R):
            uTextureIndex = TEX_FONT_BLUE_STEEL_R;
            break;
        case (EMERALD_FONT_BLUE_STEEL_S):
            uTextureIndex = TEX_FONT_BLUE_STEEL_S;
            break;
        case (EMERALD_FONT_BLUE_STEEL_T):
            uTextureIndex = TEX_FONT_BLUE_STEEL_T;
            break;
        case (EMERALD_FONT_BLUE_STEEL_U):
            uTextureIndex = TEX_FONT_BLUE_STEEL_U;
            break;
        case (EMERALD_FONT_BLUE_STEEL_V):
            uTextureIndex = TEX_FONT_BLUE_STEEL_V;
            break;
        case (EMERALD_FONT_BLUE_STEEL_W):
            uTextureIndex = TEX_FONT_BLUE_STEEL_W;
            break;
        case (EMERALD_FONT_BLUE_STEEL_X):
            uTextureIndex = TEX_FONT_BLUE_STEEL_X;
            break;
        case (EMERALD_FONT_BLUE_STEEL_Y):
            uTextureIndex = TEX_FONT_BLUE_STEEL_Y;
            break;
        case (EMERALD_FONT_BLUE_STEEL_Z):
            uTextureIndex = TEX_FONT_BLUE_STEEL_Z;
            break;
        case (EMERALD_FONT_BLUE_STEEL_AE):
            uTextureIndex = TEX_FONT_BLUE_STEEL_AE;
            break;
        case (EMERALD_FONT_BLUE_STEEL_OE):
            uTextureIndex = TEX_FONT_BLUE_STEEL_OE;
            break;
        case (EMERALD_FONT_BLUE_STEEL_UE):
            uTextureIndex = TEX_FONT_BLUE_STEEL_UE;
            break;
        case (EMERALD_FONT_GREEN_EXCLAMATION):
            uTextureIndex = TEX_FONT_GREEN_EXCLAMATION_MARK;
            break;
        case (EMERALD_FONT_GREEN_DOUBLE_QUOTE):
            uTextureIndex = TEX_FONT_GREEN_DOUBLE_QUOTE;
            break;
        case (EMERALD_FONT_GREEN_SEMICOLON):
            uTextureIndex = TEX_FONT_GREEN_SEMICOLON;
            break;
        case (EMERALD_FONT_GREEN_ARROW_RIGHT):
            uTextureIndex = TEX_FONT_GREEN_ARROW_RIGHT;
            break;
        case (EMERALD_FONT_GREEN_ARROW_UP):
            uTextureIndex = TEX_FONT_GREEN_ARROW_UP;
            break;
        case (EMERALD_FONT_GREEN_ARROW_DOWN):
            uTextureIndex = TEX_FONT_GREEN_ARROW_DOWN;
            break;
        case (EMERALD_FONT_GREEN_APOSTROPHE):
            uTextureIndex = TEX_FONT_GREEN_APOSTROPHE;
            break;
        case (EMERALD_FONT_GREEN_BRACE_OPEN):
            uTextureIndex = TEX_FONT_GREEN_BRACKET_OPEN;
            break;
        case (EMERALD_FONT_GREEN_BRACE_CLOSE):
            uTextureIndex = TEX_FONT_GREEN_BRACKET_CLOSE;
            break;
        case (EMERALD_FONT_GREEN_COPYRIGHT):
            uTextureIndex = TEX_FONT_GREEN_COPYRIGHT;
            break;
        case (EMERALD_FONT_GREEN_PLUS):
            uTextureIndex = TEX_FONT_GREEN_PLUS;
            break;
        case (EMERALD_FONT_GREEN_COMMA):
            uTextureIndex = TEX_FONT_GREEN_COMMA;
            break;
        case (EMERALD_FONT_GREEN_MINUS):
            uTextureIndex = TEX_FONT_GREEN_MINUS;
            break;
        case (EMERALD_FONT_GREEN_DOT):
            uTextureIndex = TEX_FONT_GREEN_DOT;
            break;
        case (EMERALD_FONT_GREEN_SLASH):
            uTextureIndex = TEX_FONT_GREEN_SLASH;
            break;
        case (EMERALD_FONT_GREEN_0):
            uTextureIndex = TEX_FONT_GREEN_0;
            break;
        case (EMERALD_FONT_GREEN_1):
            uTextureIndex = TEX_FONT_GREEN_1;
            break;
        case (EMERALD_FONT_GREEN_2):
            uTextureIndex = TEX_FONT_GREEN_2;
            break;
        case (EMERALD_FONT_GREEN_3):
            uTextureIndex = TEX_FONT_GREEN_3;
            break;
        case (EMERALD_FONT_GREEN_4):
            uTextureIndex = TEX_FONT_GREEN_4;
            break;
        case (EMERALD_FONT_GREEN_5):
            uTextureIndex = TEX_FONT_GREEN_5;
            break;
        case (EMERALD_FONT_GREEN_6):
            uTextureIndex = TEX_FONT_GREEN_6;
            break;
        case (EMERALD_FONT_GREEN_7):
            uTextureIndex = TEX_FONT_GREEN_7;
            break;
        case (EMERALD_FONT_GREEN_8):
            uTextureIndex = TEX_FONT_GREEN_8;
            break;
        case (EMERALD_FONT_GREEN_9):
            uTextureIndex = TEX_FONT_GREEN_9;
            break;
        case (EMERALD_FONT_GREEN_DOUBLE_DOT):
            uTextureIndex = TEX_FONT_GREEN_DOUBLE_DOT;
            break;
        case (EMERALD_FONT_GREEN_PLATE):
            uTextureIndex = TEX_FONT_GREEN_AT;
            break;
        case (EMERALD_FONT_GREEN_ARROW_LEFT):
            uTextureIndex = TEX_FONT_GREEN_ARROW_LEFT;
            break;
        case (EMERALD_FONT_GREEN_QUESTION_MARK):
            uTextureIndex = TEX_FONT_GREEN_QUESTION_MARK;
            break;
        case (EMERALD_FONT_GREEN_A):
            uTextureIndex = TEX_FONT_GREEN_A;
            break;
        case (EMERALD_FONT_GREEN_B):
            uTextureIndex = TEX_FONT_GREEN_B;
            break;
        case (EMERALD_FONT_GREEN_C):
            uTextureIndex = TEX_FONT_GREEN_C;
            break;
        case (EMERALD_FONT_GREEN_D):
            uTextureIndex = TEX_FONT_GREEN_D;
            break;
        case (EMERALD_FONT_GREEN_E):
            uTextureIndex = TEX_FONT_GREEN_E;
            break;
        case (EMERALD_FONT_GREEN_F):
            uTextureIndex = TEX_FONT_GREEN_F;
            break;
        case (EMERALD_FONT_GREEN_G):
            uTextureIndex = TEX_FONT_GREEN_G;
            break;
        case (EMERALD_FONT_GREEN_H):
            uTextureIndex = TEX_FONT_GREEN_H;
            break;
        case (EMERALD_FONT_GREEN_I):
            uTextureIndex = TEX_FONT_GREEN_I;
            break;
        case (EMERALD_FONT_GREEN_J):
            uTextureIndex = TEX_FONT_GREEN_J;
            break;
        case (EMERALD_FONT_GREEN_K):
            uTextureIndex = TEX_FONT_GREEN_K;
            break;
        case (EMERALD_FONT_GREEN_L):
            uTextureIndex = TEX_FONT_GREEN_L;
            break;
        case (EMERALD_FONT_GREEN_M):
            uTextureIndex = TEX_FONT_GREEN_M;
            break;
        case (EMERALD_FONT_GREEN_N):
            uTextureIndex = TEX_FONT_GREEN_N;
            break;
        case (EMERALD_FONT_GREEN_O):
            uTextureIndex = TEX_FONT_GREEN_O;
            break;
        case (EMERALD_FONT_GREEN_P):
            uTextureIndex = TEX_FONT_GREEN_P;
            break;
        case (EMERALD_FONT_GREEN_Q):
            uTextureIndex = TEX_FONT_GREEN_Q;
            break;
        case (EMERALD_FONT_GREEN_R):
            uTextureIndex = TEX_FONT_GREEN_R;
            break;
        case (EMERALD_FONT_GREEN_S):
            uTextureIndex = TEX_FONT_GREEN_S;
            break;
        case (EMERALD_FONT_GREEN_T):
            uTextureIndex = TEX_FONT_GREEN_T;
            break;
        case (EMERALD_FONT_GREEN_U):
            uTextureIndex = TEX_FONT_GREEN_U;
            break;
        case (EMERALD_FONT_GREEN_V):
            uTextureIndex = TEX_FONT_GREEN_V;
            break;
        case (EMERALD_FONT_GREEN_W):
            uTextureIndex = TEX_FONT_GREEN_W;
            break;
        case (EMERALD_FONT_GREEN_X):
            uTextureIndex = TEX_FONT_GREEN_X;
            break;
        case (EMERALD_FONT_GREEN_Y):
            uTextureIndex = TEX_FONT_GREEN_Y;
            break;
        case (EMERALD_FONT_GREEN_Z):
            uTextureIndex = TEX_FONT_GREEN_Z;
            break;
        case (EMERALD_FONT_GREEN_AE):
            uTextureIndex = TEX_FONT_GREEN_AE;
            break;
        case (EMERALD_FONT_GREEN_OE):
            uTextureIndex = TEX_FONT_GREEN_OE;
            break;
        case (EMERALD_FONT_GREEN_UE):
            uTextureIndex = TEX_FONT_GREEN_UE;
            break;
        case (EMERALD_FONT_GREEN_STEEL_EXCLAMATION):
            uTextureIndex = TEX_FONT_GREEN_STEEL_EXCLAMATION_MARK;
            break;
        case (EMERALD_FONT_GREEN_STEEL_DOUBLE_QUOTE):
            uTextureIndex = TEX_FONT_GREEN_STEEL_DOUBLE_QUOTE;
            break;
        case (EMERALD_FONT_GREEN_STEEL_SEMICOLON):
            uTextureIndex = TEX_FONT_GREEN_STEEL_SEMICOLON;
            break;
        case (EMERALD_FONT_GREEN_STEEL_ARROW_RIGHT):
            uTextureIndex = TEX_FONT_GREEN_STEEL_ARROW_RIGHT;
            break;
        case (EMERALD_FONT_GREEN_STEEL_ARROW_UP):
            uTextureIndex = TEX_FONT_GREEN_STEEL_ARROW_UP;
            break;
        case (EMERALD_FONT_GREEN_STEEL_ARROW_DOWN):
            uTextureIndex = TEX_FONT_GREEN_STEEL_ARROW_DOWN;
            break;
        case (EMERALD_FONT_GREEN_STEEL_APOSTROPHE):
            uTextureIndex = TEX_FONT_GREEN_STEEL_APOSTROPHE;
            break;
        case (EMERALD_FONT_GREEN_STEEL_BRACE_OPEN):
            uTextureIndex = TEX_FONT_GREEN_STEEL_BRACKET_OPEN;
            break;
        case (EMERALD_FONT_GREEN_STEEL_BRACE_CLOSE):
            uTextureIndex = TEX_FONT_GREEN_STEEL_BRACKET_CLOSE;
            break;
        case (EMERALD_FONT_GREEN_STEEL_COPYRIGHT):
            uTextureIndex = TEX_FONT_GREEN_STEEL_COPYRIGHT;
            break;
        case (EMERALD_FONT_GREEN_STEEL_PLUS):
            uTextureIndex = TEX_FONT_GREEN_STEEL_PLUS;
            break;
        case (EMERALD_FONT_GREEN_STEEL_COMMA):
            uTextureIndex = TEX_FONT_GREEN_STEEL_COMMA;
            break;
        case (EMERALD_FONT_GREEN_STEEL_MINUS):
            uTextureIndex = TEX_FONT_GREEN_STEEL_MINUS;
            break;
        case (EMERALD_FONT_GREEN_STEEL_DOT):
            uTextureIndex = TEX_FONT_GREEN_STEEL_DOT;
            break;
        case (EMERALD_FONT_GREEN_STEEL_SLASH):
            uTextureIndex = TEX_FONT_GREEN_STEEL_SLASH;
            break;
        case (EMERALD_FONT_GREEN_STEEL_0):
            uTextureIndex = TEX_FONT_GREEN_STEEL_0;
            break;
        case (EMERALD_FONT_GREEN_STEEL_1):
            uTextureIndex = TEX_FONT_GREEN_STEEL_1;
            break;
        case (EMERALD_FONT_GREEN_STEEL_2):
            uTextureIndex = TEX_FONT_GREEN_STEEL_2;
            break;
        case (EMERALD_FONT_GREEN_STEEL_3):
            uTextureIndex = TEX_FONT_GREEN_STEEL_3;
            break;
        case (EMERALD_FONT_GREEN_STEEL_4):
            uTextureIndex = TEX_FONT_GREEN_STEEL_4;
            break;
        case (EMERALD_FONT_GREEN_STEEL_5):
            uTextureIndex = TEX_FONT_GREEN_STEEL_5;
            break;
        case (EMERALD_FONT_GREEN_STEEL_6):
            uTextureIndex = TEX_FONT_GREEN_STEEL_6;
            break;
        case (EMERALD_FONT_GREEN_STEEL_7):
            uTextureIndex = TEX_FONT_GREEN_STEEL_7;
            break;
        case (EMERALD_FONT_GREEN_STEEL_8):
            uTextureIndex = TEX_FONT_GREEN_STEEL_8;
            break;
        case (EMERALD_FONT_GREEN_STEEL_9):
            uTextureIndex = TEX_FONT_GREEN_STEEL_9;
            break;
        case (EMERALD_FONT_GREEN_STEEL_DOUBLE_DOT):
            uTextureIndex = TEX_FONT_GREEN_STEEL_DOUBLE_DOT;
            break;
        case (EMERALD_FONT_GREEN_STEEL_PLATE):
            uTextureIndex = TEX_FONT_GREEN_STEEL_AT;
            break;
        case (EMERALD_FONT_GREEN_STEEL_ARROW_LEFT):
            uTextureIndex = TEX_FONT_GREEN_STEEL_ARROW_LEFT;
            break;
        case (EMERALD_FONT_GREEN_STEEL_QUESTION_MARK):
            uTextureIndex = TEX_FONT_GREEN_STEEL_QUESTION_MARK;
            break;
        case (EMERALD_FONT_GREEN_STEEL_A):
            uTextureIndex = TEX_FONT_GREEN_STEEL_A;
            break;
        case (EMERALD_FONT_GREEN_STEEL_B):
            uTextureIndex = TEX_FONT_GREEN_STEEL_B;
            break;
        case (EMERALD_FONT_GREEN_STEEL_C):
            uTextureIndex = TEX_FONT_GREEN_STEEL_C;
            break;
        case (EMERALD_FONT_GREEN_STEEL_D):
            uTextureIndex = TEX_FONT_GREEN_STEEL_D;
            break;
        case (EMERALD_FONT_GREEN_STEEL_E):
            uTextureIndex = TEX_FONT_GREEN_STEEL_E;
            break;
        case (EMERALD_FONT_GREEN_STEEL_F):
            uTextureIndex = TEX_FONT_GREEN_STEEL_F;
            break;
        case (EMERALD_FONT_GREEN_STEEL_G):
            uTextureIndex = TEX_FONT_GREEN_STEEL_G;
            break;
        case (EMERALD_FONT_GREEN_STEEL_H):
            uTextureIndex = TEX_FONT_GREEN_STEEL_H;
            break;
        case (EMERALD_FONT_GREEN_STEEL_I):
            uTextureIndex = TEX_FONT_GREEN_STEEL_I;
            break;
        case (EMERALD_FONT_GREEN_STEEL_J):
            uTextureIndex = TEX_FONT_GREEN_STEEL_J;
            break;
        case (EMERALD_FONT_GREEN_STEEL_K):
            uTextureIndex = TEX_FONT_GREEN_STEEL_K;
            break;
        case (EMERALD_FONT_GREEN_STEEL_L):
            uTextureIndex = TEX_FONT_GREEN_STEEL_L;
            break;
        case (EMERALD_FONT_GREEN_STEEL_M):
            uTextureIndex = TEX_FONT_GREEN_STEEL_M;
            break;
        case (EMERALD_FONT_GREEN_STEEL_N):
            uTextureIndex = TEX_FONT_GREEN_STEEL_N;
            break;
        case (EMERALD_FONT_GREEN_STEEL_O):
            uTextureIndex = TEX_FONT_GREEN_STEEL_O;
            break;
        case (EMERALD_FONT_GREEN_STEEL_P):
            uTextureIndex = TEX_FONT_GREEN_STEEL_P;
            break;
        case (EMERALD_FONT_GREEN_STEEL_Q):
            uTextureIndex = TEX_FONT_GREEN_STEEL_Q;
            break;
        case (EMERALD_FONT_GREEN_STEEL_R):
            uTextureIndex = TEX_FONT_GREEN_STEEL_R;
            break;
        case (EMERALD_FONT_GREEN_STEEL_S):
            uTextureIndex = TEX_FONT_GREEN_STEEL_S;
            break;
        case (EMERALD_FONT_GREEN_STEEL_T):
            uTextureIndex = TEX_FONT_GREEN_STEEL_T;
            break;
        case (EMERALD_FONT_GREEN_STEEL_U):
            uTextureIndex = TEX_FONT_GREEN_STEEL_U;
            break;
        case (EMERALD_FONT_GREEN_STEEL_V):
            uTextureIndex = TEX_FONT_GREEN_STEEL_V;
            break;
        case (EMERALD_FONT_GREEN_STEEL_W):
            uTextureIndex = TEX_FONT_GREEN_STEEL_W;
            break;
        case (EMERALD_FONT_GREEN_STEEL_X):
            uTextureIndex = TEX_FONT_GREEN_STEEL_X;
            break;
        case (EMERALD_FONT_GREEN_STEEL_Y):
            uTextureIndex = TEX_FONT_GREEN_STEEL_Y;
            break;
        case (EMERALD_FONT_GREEN_STEEL_Z):
            uTextureIndex = TEX_FONT_GREEN_STEEL_Z;
            break;
        case (EMERALD_FONT_GREEN_STEEL_AE):
            uTextureIndex = TEX_FONT_GREEN_STEEL_AE;
            break;
        case (EMERALD_FONT_GREEN_STEEL_OE):
            uTextureIndex = TEX_FONT_GREEN_STEEL_OE;
            break;
        case (EMERALD_FONT_GREEN_STEEL_UE):
            uTextureIndex = TEX_FONT_GREEN_STEEL_UE;
            break;
        case (EMERALD_DYNAMITE_ON):
            uTextureIndex = TEX_DYNAMITE_ON_1 + nAnimationCount / 4;
            break;
        case (EMERALD_QUICKSAND):
            uTextureIndex = TEX_QUICKSAND;
            break;
        case (EMERALD_QUICKSAND_STONE):
            uTextureIndex = TEX_QUICKSAND_STONE;                // Für Editor
            break;
        case (EMERALD_QUICKSAND_SLOW):
            uTextureIndex = TEX_QUICKSAND_SLOW;
            break;
        case (EMERALD_QUICKSAND_STONE_SLOW):
            uTextureIndex = TEX_QUICKSAND_SLOW_STONE;           // Für Editor
            break;
        case (EMERALD_WALL_WITH_CRYSTAL):
            uTextureIndex = TEX_WALL_CRYSTAL;
            break;
        case (EMERALD_WALL_WITH_KEY_RED):
            uTextureIndex = TEX_WALL_KEY_RED;
            break;
        case (EMERALD_WALL_WITH_KEY_GREEN):
            uTextureIndex = TEX_WALL_KEY_GREEN;
            break;
        case (EMERALD_WALL_WITH_KEY_BLUE):
            uTextureIndex = TEX_WALL_KEY_BLUE;
            break;
        case (EMERALD_WALL_WITH_KEY_YELLOW):
            uTextureIndex = TEX_WALL_KEY_YELLOW;
            break;
        case (EMERALD_WALL_WITH_KEY_WHITE):
            uTextureIndex = TEX_WALL_KEY_WHITE;
            break;
        case (EMERALD_WALL_WITH_KEY_GENERAL):
            uTextureIndex = TEX_WALL_KEY_GENERAL;
            break;
        case (EMERALD_WALL_WITH_TIME_COIN):
            uTextureIndex = TEX_WALL_COIN_TIME;
            break;
        case (EMERALD_WALL_WITH_SHIELD_COIN):
            uTextureIndex = TEX_WALL_COIN_SHIELD;
            break;
        case (EMERALD_WALL_WITH_BOMB):
            uTextureIndex = TEX_WALL_BOMB;
            break;
        case (EMERALD_WALL_WITH_MEGABOMB):
            uTextureIndex = TEX_WALL_BOMB_MEGA;
            break;
        case (EMERALD_WALL_WITH_STONE):
            uTextureIndex = TEX_WALL_STONE;
            break;
        case (EMERALD_WALL_WITH_NUT):
            uTextureIndex = TEX_WALL_NUT;
            break;
        case (EMERALD_WALL_WITH_WHEEL):
            uTextureIndex = TEX_WALL_WHEEL;
            break;
        case (EMERALD_WALL_WITH_DYNAMITE):
            uTextureIndex = TEX_WALL_DYNAMITE;
            break;
        case (EMERALD_WALL_WITH_ENDDOOR):
            uTextureIndex = TEX_WALL_DOOR_EXIT;
            break;
        case (EMERALD_WALL_WITH_ENDDOOR_READY):
            uTextureIndex = TEX_WALL_DOOR_EXIT_READY;
            break;
        case (EMERALD_WALL_WITH_MINE_UP):
            uTextureIndex = TEX_WALL_MINE_UP;
            break;
        case (EMERALD_WALL_WITH_BEETLE_UP):
            uTextureIndex = TEX_WALL_BEETLE_UP;
            break;
        case (EMERALD_WALL_WITH_YAM):
            uTextureIndex = TEX_WALL_YAM;
            break;
        case (EMERALD_WALL_WITH_SLIME):
            uTextureIndex = TEX_WALL_SLIME;
            break;
        case (EMERALD_WALL_WITH_ALIEN):
            uTextureIndex = TEX_WALL_ALIEN;
            break;
        case (EMERALD_WALL_WITH_MOLE_UP):
            uTextureIndex = TEX_WALL_MOLE_UP;
            break;
        case (EMERALD_WALL_WITH_GREEN_CHEESE):
            uTextureIndex = TEX_WALL_CHEESE_GREEN;
            break;
        case (EMERALD_WALL_WITH_YELLOW_CHEESE):
            uTextureIndex = TEX_WALL_CHEESE_YELLOW;
            break;
        case (EMERALD_WALL_WITH_EMERALD):
            uTextureIndex = TEX_WALL_EMERALD;
            break;
        case (EMERALD_WALL_WITH_RUBY):
            uTextureIndex = TEX_WALL_RUBY;
            break;
        case (EMERALD_WALL_WITH_SAPPHIRE):
            uTextureIndex = TEX_WALL_SAPPHIRE;
            break;
        case (EMERALD_WALL_WITH_PERL):
            uTextureIndex = TEX_WALL_PERL;
            break;
        case (EMERALD_DOOR_WHITE):
            uTextureIndex = TEX_DOOR_WHITE;
            break;
        case (EMERALD_DOOR_WHITE_WOOD):
            uTextureIndex = TEX_DOOR_WHITE_WOOD;
            break;
        case (EMERALD_DOOR_GREY_WHITE):
            uTextureIndex = TEX_DOOR_GREY_WHITE;
            break;
        case (EMERALD_KEY_WHITE):
            uTextureIndex = TEX_KEY_WHITE;
            break;
        case (EMERALD_KEY_GENERAL):
            uTextureIndex = TEX_KEY_GENERAL;
            break;
        case (EMERALD_SWITCHDOOR_OPEN):
            uTextureIndex = TEX_DOOR_SWITCHED_5;
            break;
        case (EMERALD_SWITCHDOOR_CLOSED):
            uTextureIndex = TEX_DOOR_SWITCHED_1;
            break;
        case (EMERALD_SWITCH_SWITCHDOOR):
            uTextureIndex = TEX_SWITCH_DOOR_SWITCHED_1;
            break;
        case (EMERALD_DOOR_TIME):
            uTextureIndex = TEX_DOOR_TIME_1;
            break;
        case (EMERALD_WHEEL_TIMEDOOR):
            uTextureIndex = TEX_WHEEL_DOOR_TIME_1 + nAnimationCount / 4;
            break;
        case (EMERALD_DOOR_EMERALD):
            uTextureIndex = TEX_DOOR_EMERALD;
            break;
        case (EMERALD_DOOR_MULTICOLOR):
            uTextureIndex = TEX_DOOR_MULTICOLOR;
            break;
        case (EMERALD_DOOR_ONLY_UP_STEEL):
            uTextureIndex = TEX_DOOR_UP_STEEL;
            break;
        case (EMERALD_DOOR_ONLY_DOWN_STEEL):
            uTextureIndex = TEX_DOOR_DOWN_STEEL;
            break;
        case (EMERALD_DOOR_ONLY_LEFT_STEEL):
            uTextureIndex = TEX_DOOR_LEFT_STEEL;
            break;
        case (EMERALD_DOOR_ONLY_RIGHT_STEEL):
            uTextureIndex = TEX_DOOR_RIGHT_STEEL;
            break;
        case (EMERALD_DOOR_ONLY_UP_WALL):
            uTextureIndex = TEX_DOOR_UP_WALL;
            break;
        case (EMERALD_DOOR_ONLY_DOWN_WALL):
            uTextureIndex = TEX_DOOR_DOWN_WALL;
            break;
        case (EMERALD_DOOR_ONLY_LEFT_WALL):
            uTextureIndex = TEX_DOOR_LEFT_WALL;
            break;
        case (EMERALD_DOOR_ONLY_RIGHT_WALL):
            uTextureIndex = TEX_DOOR_RIGHT_WALL;
            break;
        case (EMERALD_STEEL_INVISIBLE):
            uTextureIndex = TEX_STEEL_INVISIBLE;
            break;
        case (EMERALD_WALL_INVISIBLE):
            uTextureIndex = TEX_WALL_INVISIBLE;
            break;
        case (EMERALD_LIGHT_SWITCH):
            uTextureIndex = TEX_SWITCH_LIGHT;
            break;
        case (EMERALD_STEEL_HEART):
            uTextureIndex = TEX_STEEL_HEART;
            break;
        case (EMERALD_STEEL_PLAYERHEAD):
            uTextureIndex = TEX_STEEL_PLAYER;
            break;
        case (EMERALD_STEEL_PLAYERHEAD_2):
            uTextureIndex = TEX_STEEL_PLAYER2;
            break;
        case (EMERALD_STEEL_NO_ENTRY):
            uTextureIndex = TEX_STEEL_ALL_VEHICLES_PROHIBITED;
            break;
        case (EMERALD_STEEL_GIVE_WAY):
            uTextureIndex = TEX_STEEL_GIVE_WAY;
            break;
        case (EMERALD_STEEL_YING):
            uTextureIndex = TEX_STEEL_YING_YANG;
            break;
        case (EMERALD_STEEL_WHEELCHAIR):
            uTextureIndex = TEX_STEEL_WHEELCHAIR;
            break;
        case (EMERALD_STEEL_ARROW_DOWN):
            uTextureIndex = TEX_STEEL_ARROW_DOWN;
            break;
        case (EMERALD_STEEL_ARROW_UP):
            uTextureIndex = TEX_STEEL_ARROW_UP;
            break;
        case (EMERALD_STEEL_ARROW_LEFT):
            uTextureIndex = TEX_STEEL_ARROW_LEFT;
            break;
        case (EMERALD_STEEL_ARROW_RIGHT):
            uTextureIndex = TEX_STEEL_ARROW_RIGHT;
            break;
        case (EMERALD_MINE_CONTACT):
            if ((((Playfield.uFrameCounter & 0xFFFFFFFC) >> 2) % 32) == 0) {
                uTextureIndex = TEX_MINE_CONTACT_ON_1;
            } else if ((((Playfield.uFrameCounter & 0xFFFFFFFC) >> 2) % 16) == 0) {
                uTextureIndex = TEX_MINE_CONTACT_ON_2;
            } else {
                uTextureIndex = TEX_MINE_CONTACT_OFF;
            }
            break;
        case (EMERALD_MAGIC_WALL_SWITCH):
            uTextureIndex = TEX_SWITCH_MAGIC_WALL;
            break;
        case (EMERALD_LIGHTBARRIER_RED_UP):
            if (Playfield.bLightBarrierRedOn) {
                uTextureIndex = TEX_LIGHT_BARRIER_RED_UP_ON;
            } else {
                uTextureIndex = TEX_LIGHT_BARRIER_RED_UP_OFF;
            }
            break;
        case (EMERALD_LIGHTBARRIER_RED_DOWN):
            if (Playfield.bLightBarrierRedOn) {
                uTextureIndex = TEX_LIGHT_BARRIER_RED_DOWN_ON;
            } else {
                uTextureIndex = TEX_LIGHT_BARRIER_RED_DOWN_OFF;
            }
            break;
        case (EMERALD_LIGHTBARRIER_RED_LEFT):
            if (Playfield.bLightBarrierRedOn) {
                uTextureIndex = TEX_LIGHT_BARRIER_RED_LEFT_ON;
            } else {
                uTextureIndex = TEX_LIGHT_BARRIER_RED_LEFT_OFF;
            }
            break;
        case (EMERALD_LIGHTBARRIER_RED_RIGHT):
            if (Playfield.bLightBarrierRedOn) {
                uTextureIndex = TEX_LIGHT_BARRIER_RED_RIGHT_ON;
            } else {
                uTextureIndex = TEX_LIGHT_BARRIER_RED_RIGHT_OFF;
            }
            break;
        case (EMERALD_LIGHTBARRIER_GREEN_UP):
            if (Playfield.bLightBarrierGreenOn) {
                uTextureIndex = TEX_LIGHT_BARRIER_GREEN_UP_ON;
            } else {
                uTextureIndex = TEX_LIGHT_BARRIER_GREEN_UP_OFF;
            }
            break;
        case (EMERALD_LIGHTBARRIER_GREEN_DOWN):
            if (Playfield.bLightBarrierGreenOn) {
                uTextureIndex = TEX_LIGHT_BARRIER_GREEN_DOWN_ON;
            } else {
                uTextureIndex = TEX_LIGHT_BARRIER_GREEN_DOWN_OFF;
            }
            break;
        case (EMERALD_LIGHTBARRIER_GREEN_LEFT):
            if (Playfield.bLightBarrierGreenOn) {
                uTextureIndex = TEX_LIGHT_BARRIER_GREEN_LEFT_ON;
            } else {
                uTextureIndex = TEX_LIGHT_BARRIER_GREEN_LEFT_OFF;
            }
            break;
        case (EMERALD_LIGHTBARRIER_GREEN_RIGHT):
            if (Playfield.bLightBarrierGreenOn) {
                uTextureIndex = TEX_LIGHT_BARRIER_GREEN_RIGHT_ON;
            } else {
                uTextureIndex = TEX_LIGHT_BARRIER_GREEN_RIGHT_OFF;
            }
            break;
        case (EMERALD_LIGHTBARRIER_BLUE_UP):
            if (Playfield.bLightBarrierBlueOn) {
                uTextureIndex = TEX_LIGHT_BARRIER_BLUE_UP_ON;
            } else {
                uTextureIndex = TEX_LIGHT_BARRIER_BLUE_UP_OFF;
            }
            break;
        case (EMERALD_LIGHTBARRIER_BLUE_DOWN):
            if (Playfield.bLightBarrierBlueOn) {
                uTextureIndex = TEX_LIGHT_BARRIER_BLUE_DOWN_ON;
            } else {
                uTextureIndex = TEX_LIGHT_BARRIER_BLUE_DOWN_OFF;
            }
            break;
        case (EMERALD_LIGHTBARRIER_BLUE_LEFT):
            if (Playfield.bLightBarrierBlueOn) {
                uTextureIndex = TEX_LIGHT_BARRIER_BLUE_LEFT_ON;
            } else {
                uTextureIndex = TEX_LIGHT_BARRIER_BLUE_LEFT_OFF;
            }
            break;
        case (EMERALD_LIGHTBARRIER_BLUE_RIGHT):
            if (Playfield.bLightBarrierBlueOn) {
                uTextureIndex = TEX_LIGHT_BARRIER_BLUE_RIGHT_ON;
            } else {
                uTextureIndex = TEX_LIGHT_BARRIER_BLUE_RIGHT_OFF;
            }
            break;
        case (EMERALD_LIGHTBARRIER_YELLOW_UP):
            if (Playfield.bLightBarrierYellowOn) {
                uTextureIndex = TEX_LIGHT_BARRIER_YELLOW_UP_ON;
            } else {
                uTextureIndex = TEX_LIGHT_BARRIER_YELLOW_UP_OFF;
            }
            break;
        case (EMERALD_LIGHTBARRIER_YELLOW_DOWN):
            if (Playfield.bLightBarrierYellowOn) {
                uTextureIndex = TEX_LIGHT_BARRIER_YELLOW_DOWN_ON;
            } else {
                uTextureIndex = TEX_LIGHT_BARRIER_YELLOW_DOWN_OFF;
            }
            break;
        case (EMERALD_LIGHTBARRIER_YELLOW_LEFT):
            if (Playfield.bLightBarrierYellowOn) {
                uTextureIndex = TEX_LIGHT_BARRIER_YELLOW_LEFT_ON;
            } else {
                uTextureIndex = TEX_LIGHT_BARRIER_YELLOW_LEFT_OFF;
            }
            break;
        case (EMERALD_LIGHTBARRIER_YELLOW_RIGHT):
            if (Playfield.bLightBarrierYellowOn) {
                uTextureIndex = TEX_LIGHT_BARRIER_YELLOW_RIGHT_ON;
            } else {
                uTextureIndex = TEX_LIGHT_BARRIER_YELLOW_RIGHT_OFF;
            }
            break;
        case (EMERALD_BEAM_RED_VERTICAL):
            uTextureIndex = TEX_BEAM_RED_VERTICAL;
            break;
        case (EMERALD_BEAM_RED_HORIZONTAL):
            uTextureIndex = TEX_BEAM_RED_HORIZONTAL;
            break;
        case (EMERALD_BEAM_GREEN_VERTICAL):
            uTextureIndex = TEX_BEAM_GREEN_VERTICAL;
            break;
        case (EMERALD_BEAM_GREEN_HORIZONTAL):
            uTextureIndex = TEX_BEAM_GREEN_HORIZONTAL;
            break;
        case (EMERALD_BEAM_BLUE_VERTICAL):
            uTextureIndex = TEX_BEAM_BLUE_VERTICAL;
            break;
        case (EMERALD_BEAM_BLUE_HORIZONTAL):
            uTextureIndex = TEX_BEAM_BLUE_HORIZONTAL;
            break;
        case (EMERALD_BEAM_YELLOW_VERTICAL):
            uTextureIndex = TEX_BEAM_YELLOW_VERTICAL;
            break;
        case (EMERALD_BEAM_YELLOW_HORIZONTAL):
            uTextureIndex = TEX_BEAM_YELLOW_HORIZONTAL;
            break;
        case (EMERALD_BEAM_CROSS):
            uTextureIndex = TEX_SPACE;
            break;
        case (EMERALD_LIGHTBARRIER_RED_SWITCH):
            uTextureIndex = TEX_SWITCH_LIGHT_BARRIER_RED_OFF;
            break;
        case (EMERALD_LIGHTBARRIER_GREEN_SWITCH):
            uTextureIndex = TEX_SWITCH_LIGHT_BARRIER_GREEN_OFF;
            break;
        case (EMERALD_LIGHTBARRIER_BLUE_SWITCH):
            uTextureIndex = TEX_SWITCH_LIGHT_BARRIER_BLUE_OFF;
            break;
        case (EMERALD_LIGHTBARRIER_YELLOW_SWITCH):
            uTextureIndex = TEX_SWITCH_LIGHT_BARRIER_YELLOW_OFF;
            break;
        case (EMERALD_YAM):
            K = Playfield.uFrameCounter % 11;       // K von 0 bis 10
            if (K <= 5) {                           // 0,1,2,3,4,5
                uTextureIndex = TEX_YAM_MOVE_1 + K;            // 362 - 367
            } else {                                // 6,7,8,9,10
                uTextureIndex = TEX_YAM_MOVE_6 + 5 - K;        // 366 - 362
            }
            break;
        case (EMERALD_MESSAGE_0):                   // Dummy-Element für Editor
            uTextureIndex = TEX_MESSAGE;
            break;
        case (EMERALD_MESSAGE_1):
            uTextureIndex = TEX_MESSAGE_1;
            break;
        case (EMERALD_MESSAGE_2):
            uTextureIndex = TEX_MESSAGE_2;
            break;
        case (EMERALD_MESSAGE_3):
            uTextureIndex = TEX_MESSAGE_3;
            break;
        case (EMERALD_MESSAGE_4):
            uTextureIndex = TEX_MESSAGE_4;
            break;
        case (EMERALD_MESSAGE_5):
            uTextureIndex = TEX_MESSAGE_5;
            break;
        case (EMERALD_MESSAGE_6):
            uTextureIndex = TEX_MESSAGE_6;
            break;
        case (EMERALD_MESSAGE_7):
            uTextureIndex = TEX_MESSAGE_7;
            break;
        case (EMERALD_MESSAGE_8):
            uTextureIndex = TEX_MESSAGE_8;
            break;
        case (EMERALD_TREASURECHEST_0):
            uTextureIndex = TEX_TREASURECHEST_CLOSED;
            break;
        case (EMERALD_TREASURECHEST_1):
            uTextureIndex = TEX_TREASURECHEST_1;
            break;
        case (EMERALD_TREASURECHEST_2):
            uTextureIndex = TEX_TREASURECHEST_2;
            break;
        case (EMERALD_TREASURECHEST_3):
            uTextureIndex = TEX_TREASURECHEST_3;
            break;
        case (EMERALD_TREASURECHEST_4):
            uTextureIndex = TEX_TREASURECHEST_4;
            break;
        case (EMERALD_TREASURECHEST_5):
            uTextureIndex = TEX_TREASURECHEST_5;
            break;
        case (EMERALD_TREASURECHEST_6):
            uTextureIndex = TEX_TREASURECHEST_6;
            break;
        case (EMERALD_TREASURECHEST_7):
            uTextureIndex = TEX_TREASURECHEST_7;
            break;
        case (EMERALD_TREASURECHEST_8):
            uTextureIndex = TEX_TREASURECHEST_8;
            break;
        case (EMERALD_TREASURECHEST_OPEN):
            uTextureIndex = TEX_TREASURECHEST_OPENED;
            break;
        case (EMERALD_STEEL_STRIPE_LEFT_TOP):
            uTextureIndex = TEX_STEEL_STRIPE_LEFT_TOP;
            break;
        case (EMERALD_STEEL_STRIPE_TOP):
            uTextureIndex = TEX_STEEL_STRIPE_TOP;
            break;
        case (EMERALD_STEEL_STRIPE_RIGHT_TOP):
            uTextureIndex = TEX_STEEL_STRIPE_RIGHT_TOP;
            break;
        case (EMERALD_STEEL_STRIPE_LEFT):
            uTextureIndex = TEX_STEEL_STRIPE_LEFT;
            break;
        case (EMERALD_STEEL_STRIPE_RIGHT):
            uTextureIndex = TEX_STEEL_STRIPE_RIGHT;
            break;
        case (EMERALD_STEEL_STRIPE_LEFT_BOTTOM):
            uTextureIndex = TEX_STEEL_STRIPE_LEFT_BOTTOM;
            break;
        case (EMERALD_STEEL_STRIPE_BOTTOM):
            uTextureIndex = TEX_STEEL_STRIPE_BOTTOM;
            break;
        case (EMERALD_STEEL_STRIPE_RIGHT_BOTTOM):
            uTextureIndex = TEX_STEEL_STRIPE_RIGHT_BOTTOM;
            break;
        case (EMERALD_STEEL_STRIPE_CORNER_LEFT_TOP):
            uTextureIndex = TEX_STEEL_STRIPE_CORNER_LEFT_TOP;
            break;
        case (EMERALD_STEEL_STRIPE_CORNER_RIGHT_TOP):
            uTextureIndex = TEX_STEEL_STRIPE_CORNER_RIGHT_TOP;
            break;
        case (EMERALD_STEEL_STRIPE_CORNER_LEFT_BOTTOM):
            uTextureIndex = TEX_STEEL_STRIPE_CORNER_LEFT_BOTTOM;
            break;
        case (EMERALD_STEEL_STRIPE_CORNER_RIGHT_BOTTOM):
            uTextureIndex = TEX_STEEL_STRIPE_CORNER_RIGHT_BOTTOM;
            break;
        case (EMERALD_GREEN_DROP):
            uTextureIndex = TEX_DROP_GREEN;
            break;
        case (EMERALD_YELLOW_DROP):
            uTextureIndex = TEX_DROP_YELLOW;
            break;
        case (EMERALD_GREEN_CHEESE):
            uTextureIndex = TEX_CHEESE_GREEN_1;
            break;
        case (EMERALD_YELLOW_CHEESE):
            uTextureIndex = TEX_CHEESE_YELLOW_1;
            break;
        case (EMERALD_STEEL_FORBIDDEN):
            uTextureIndex = TEX_STEEL_FORBIDDEN;
            break;
        case (EMERALD_STEEL_EXIT):
            uTextureIndex = TEX_STEEL_EXIT;
            break;
        case (EMERALD_STEEL_RADIOACTIVE):
            uTextureIndex = TEX_STEEL_RADIOACTIVE;
            break;
        case (EMERALD_STEEL_EXPLOSION):
            uTextureIndex = TEX_STEEL_EXPLOSION;
            break;
        case (EMERALD_STEEL_ACID):
            uTextureIndex = TEX_STEEL_ACID;
            break;
        case (EMERALD_STEEL_NOT_ROUND):
            uTextureIndex = TEX_STEEL_NOT_ROUND;
            break;
        case (EMERALD_WALL_NOT_ROUND):
            uTextureIndex = TEX_WALL_NOT_ROUND;
            break;
        case (EMERALD_STEEL_PARKING):
            uTextureIndex = TEX_STEEL_PARKING;
            break;
        case (EMERALD_STEEL_STOP):
            uTextureIndex = TEX_STEEL_STOP;
            break;
        case (EMERALD_STEEL_DEADEND):
            uTextureIndex = TEX_STEEL_DEADEND;
            break;
        case (EMERALD_STEEL_BIOHAZARD):
            uTextureIndex = TEX_STEEL_BIOHAZARD;
            break;
        case (EMERALD_STEEL_WARNING):
            uTextureIndex = TEX_STEEL_WARNING;
            break;
        case (EMERALD_STEEL_ROUND):
            uTextureIndex = TEX_STEEL_ROUND;
            break;
        case (EMERALD_STEEL_ROUND_PIKE):
            uTextureIndex = TEX_STEEL_ROUND_PIKE;
            break;
        case (EMERALD_EMERALD):
            uTextureIndex = TEX_EMERALD_1 + nAnimationCount / 2;     // Emerald, liegend
            break;
        case (EMERALD_RUBY):
            uTextureIndex = TEX_RUBY_1 + nAnimationCount / 2;     // Rubin, liegend
            break;
        case (EMERALD_CRYSTAL):
            uTextureIndex = TEX_CRYSTAL;
            break;
        case (EMERALD_SAPPHIRE):
            uTextureIndex = TEX_SAPPHIRE_1 + ((Playfield.uFrameCounter & 0xFFFFFFFE) >> 1) % 9; // Saphir fallend
            break;
        case (EMERALD_TIME_COIN):
            uTextureIndex = TEX_COIN_TIME_1 + ((Playfield.uFrameCounter & 0xFFFFFFF8) >> 3) % 6; // Zeitmünze, drehend
            break;
        case (EMERALD_SHIELD_COIN):
            uTextureIndex = TEX_COIN_SHIELD_1 + ((Playfield.uFrameCounter & 0xFFFFFFF8) >> 3) % 6; // Schildmünze, drehend
            break;
        case (EMERALD_HAMMER):
            uTextureIndex = TEX_HAMMER;                            // Hammer
            break;
        case (EMERALD_DYNAMITE_OFF):
            uTextureIndex = TEX_DYNAMITE_OFF;                            // Dynamit, aus
            break;
        case (EMERALD_NUT):
            uTextureIndex = TEX_NUT;
            break;
        case (EMERALD_STONE):
            uTextureIndex = TEX_STONE;
            break;
        case (EMERALD_ALIEN):
            if ((nAnimationCount >= 4) && (nAnimationCount <= 11)) {
                uTextureIndex = TEX_ALIEN_MOVE_1;                        // Alien geht 1
            } else {
                uTextureIndex = TEX_ALIEN_MOVE_2;                        // Alien geht 2, Flügel voll ausgebreitet
            }
            break;
        case (EMERALD_MOLE_UP):
            fAngle = 90;
             uTextureIndex = TEX_MOLE_01 + Playfield.uFrameCounter % 11;     // Mole links
            break;
        case (EMERALD_MOLE_RIGHT):
            fAngle = 180;
             uTextureIndex = TEX_MOLE_01 + Playfield.uFrameCounter % 11;     // Mole links
             break;
        case (EMERALD_MOLE_DOWN):
            fAngle = 270;
            uTextureIndex = TEX_MOLE_01 + Playfield.uFrameCounter % 11;     // Mole links
            break;
        case( EMERALD_MOLE_LEFT):
            fAngle = 0;
             uTextureIndex = TEX_MOLE_01 + Playfield.uFrameCounter % 11;     // Mole links
            break;
        case (EMERALD_MINE_UP):
            fAngle = 90;
            if ( ((nAnimationCount >= 0) && (nAnimationCount <= 3)) || ((nAnimationCount >= 8) && (nAnimationCount <=11)) ) {
                uTextureIndex = TEX_MINE_LEFT_OFF;     // Mine links
            } else {
                uTextureIndex = TEX_MINE_LEFT_ON;     // Mine links an
            }
            break;
        case (EMERALD_MINE_RIGHT):
            fAngle = 180;
            if ( ((nAnimationCount >= 0) && (nAnimationCount <= 3)) || ((nAnimationCount >= 8) && (nAnimationCount <=11)) ) {
                uTextureIndex = TEX_MINE_LEFT_OFF;     // Mine links
            } else {
                uTextureIndex = TEX_MINE_LEFT_ON;     // Mine links an
            }
            break;
        case (EMERALD_MINE_DOWN):
            fAngle = 270;
            if ( ((nAnimationCount >= 0) && (nAnimationCount <= 3)) || ((nAnimationCount >= 8) && (nAnimationCount <=11)) ) {
                uTextureIndex = TEX_MINE_LEFT_OFF;     // Mine links
            } else {
                uTextureIndex = TEX_MINE_LEFT_ON;     // Mine links an
            }
            break;
        case( EMERALD_MINE_LEFT):
            fAngle = 0;
            if ( ((nAnimationCount >= 0) && (nAnimationCount <= 3)) || ((nAnimationCount >= 8) && (nAnimationCount <=11)) ) {
                uTextureIndex = TEX_MINE_LEFT_OFF;     // Mine links
            } else {
                uTextureIndex = TEX_MINE_LEFT_ON;     // Mine links an
            }
            break;
        case (EMERALD_BEETLE_UP):
            fAngle = 90;
            uTextureIndex = TEX_BEETLE_LEFT_1 + nAnimationCount % 8;     // Käfer links
            break;
        case (EMERALD_BEETLE_RIGHT):
            fAngle = 180;
            uTextureIndex = TEX_BEETLE_LEFT_1 + nAnimationCount % 8;     // Käfer links
            break;
        case (EMERALD_BEETLE_DOWN):
            fAngle = 270;
            uTextureIndex = TEX_BEETLE_LEFT_1 + nAnimationCount % 8;     // Käfer links
            break;
        case (EMERALD_BEETLE_LEFT):
            fAngle = 0;
            uTextureIndex = TEX_BEETLE_LEFT_1 + nAnimationCount % 8;     // Käfer links
            break;
        case (EMERALD_SLIME):
            uTextureIndex = TEX_SLIME;   // Schleim
            break;
        case (EMERALD_SPACE):
            uTextureIndex = TEX_SPACE;      // Space
            break;
        case (EMERALD_STEEL):
            uTextureIndex = TEX_STEEL;     // Stahl
            break;
        case (EMERALD_KEY_RED):
            uTextureIndex = TEX_KEY_RED;     // roter Schlüssel
            break;
        case (EMERALD_KEY_YELLOW):
            uTextureIndex = TEX_KEY_YELLOW;    // gelber Schlüssel
            break;
        case (EMERALD_KEY_BLUE):
            uTextureIndex = TEX_KEY_BLUE;    // blauer Schlüssel
            break;
        case (EMERALD_KEY_GREEN):
            uTextureIndex = TEX_KEY_GREEN;     // grüner Schlüssel
            break;
        case (EMERALD_DOOR_RED):
            uTextureIndex = TEX_DOOR_RED;     // rote Tür
            break;
        case (EMERALD_DOOR_YELLOW):
            uTextureIndex = TEX_DOOR_YELLOW;     // gelbe Tür
            break;
        case (EMERALD_DOOR_BLUE):
            uTextureIndex = TEX_DOOR_BLUE;     // blaue Tür
            break;
        case (EMERALD_DOOR_GREEN):
            uTextureIndex = TEX_DOOR_GREEN;     // grüne Tür
            break;
        case (EMERALD_DOOR_RED_WOOD):     // rote Holztür
            uTextureIndex = TEX_DOOR_RED_WOOD;
            break;
        case (EMERALD_DOOR_YELLOW_WOOD):  // gelbe Holztür
            uTextureIndex = TEX_DOOR_YELLOW_WOOD;
            break;
        case (EMERALD_DOOR_BLUE_WOOD):    // blaue Holztür
            uTextureIndex = TEX_DOOR_BLUE_WOOD;
            break;
        case (EMERALD_DOOR_GREEN_WOOD):   // grüne Holztür
            uTextureIndex = TEX_DOOR_GREEN_WOOD;
            break;
        case (EMERALD_MAN):
            uTextureIndex = TEX_MAN;    // Man
            //uTextureIndex = 119 + nAnimationCount % 8;     // Man runter
            break;
        case (EMERALD_REPLICATOR_RED_TOP_LEFT):
            uTextureIndex = TEX_REPLICATOR_RED_LEFT_TOP_OFF;
            break;
        case (EMERALD_REPLICATOR_RED_TOP_MID):
            uTextureIndex = TEX_REPLICATOR_RED_MIDDLE_TOP_OFF;
            break;
        case (EMERALD_REPLICATOR_RED_TOP_RIGHT):
            uTextureIndex = TEX_REPLICATOR_RED_RIGHT_TOP_OFF;
            break;
        case (EMERALD_REPLICATOR_RED_BOTTOM_LEFT):
            uTextureIndex = TEX_REPLICATOR_RED_LEFT_BOTTOM_OFF;
            break;
        case (EMERALD_REPLICATOR_RED_BOTTOM_RIGHT):
            uTextureIndex = TEX_REPLICATOR_RED_RIGHT_BOTTOM_OFF;
            break;
        case (EMERALD_REPLICATOR_RED_SWITCH):
            uTextureIndex = TEX_SWITCH_REPLICATOR_RED_OFF;
            break;
        case (EMERALD_REPLICATOR_YELLOW_TOP_LEFT):
            uTextureIndex = TEX_REPLICATOR_YELLOW_LEFT_TOP_OFF;
            break;
        case (EMERALD_REPLICATOR_YELLOW_TOP_MID):
            uTextureIndex = TEX_REPLICATOR_YELLOW_MIDDLE_TOP_OFF;
            break;
        case (EMERALD_REPLICATOR_YELLOW_TOP_RIGHT):
            uTextureIndex = TEX_REPLICATOR_YELLOW_RIGHT_TOP_OFF;
            break;
        case (EMERALD_REPLICATOR_YELLOW_BOTTOM_LEFT):
            uTextureIndex = TEX_REPLICATOR_YELLOW_LEFT_BOTTOM_OFF;
            break;
        case (EMERALD_REPLICATOR_YELLOW_BOTTOM_RIGHT):
            uTextureIndex = TEX_REPLICATOR_YELLOW_RIGHT_BOTTOM_OFF;
            break;
        case (EMERALD_REPLICATOR_YELLOW_SWITCH):
            uTextureIndex = TEX_SWITCH_REPLICATOR_YELLOW_OFF;
            break;
        case (EMERALD_REPLICATOR_GREEN_TOP_LEFT):
            uTextureIndex = TEX_REPLICATOR_GREEN_LEFT_TOP_OFF;
            break;
        case (EMERALD_REPLICATOR_GREEN_TOP_MID):
            uTextureIndex = TEX_REPLICATOR_GREEN_MIDDLE_TOP_OFF;
            break;
        case (EMERALD_REPLICATOR_GREEN_TOP_RIGHT):
            uTextureIndex = TEX_REPLICATOR_GREEN_RIGHT_TOP_OFF;
            break;
        case (EMERALD_REPLICATOR_GREEN_BOTTOM_LEFT):
            uTextureIndex = TEX_REPLICATOR_GREEN_LEFT_BOTTOM_OFF;
            break;
        case (EMERALD_REPLICATOR_GREEN_BOTTOM_RIGHT):
            uTextureIndex = TEX_REPLICATOR_GREEN_RIGHT_BOTTOM_OFF;
            break;
        case (EMERALD_REPLICATOR_GREEN_SWITCH):
            uTextureIndex = TEX_SWITCH_REPLICATOR_GREEN_OFF;
            break;
        case (EMERALD_REPLICATOR_BLUE_TOP_LEFT):
            uTextureIndex = TEX_REPLICATOR_BLUE_LEFT_TOP_OFF;
            break;
        case (EMERALD_REPLICATOR_BLUE_TOP_MID):
            uTextureIndex = TEX_REPLICATOR_BLUE_MIDDLE_TOP_OFF;
            break;
        case (EMERALD_REPLICATOR_BLUE_TOP_RIGHT):
            uTextureIndex = TEX_REPLICATOR_BLUE_RIGHT_TOP_OFF;
            break;
        case (EMERALD_REPLICATOR_BLUE_BOTTOM_LEFT):
            uTextureIndex = TEX_REPLICATOR_BLUE_LEFT_BOTTOM_OFF;
            break;
        case (EMERALD_REPLICATOR_BLUE_BOTTOM_RIGHT):
            uTextureIndex = TEX_REPLICATOR_BLUE_RIGHT_BOTTOM_OFF;
            break;
        case (EMERALD_REPLICATOR_BLUE_SWITCH):
            uTextureIndex = TEX_SWITCH_REPLICATOR_BLUE_OFF;
            break;
        case (EMERALD_ACIDPOOL_TOP_LEFT):
            uTextureIndex = TEX_ACID_LEFT_TOP;
            break;
        case (EMERALD_ACIDPOOL_TOP_MID):
            uTextureIndex = TEX_ACID_1 + (Playfield.uFrameCounter / 13) % 4;  // Säurebecken, oben mitte (aktives Feld), verlangsamte Animation (13 Frames für eine Animations-Phase)
            break;
        case (EMERALD_ACIDPOOL_TOP_RIGHT):
            uTextureIndex = TEX_ACID_RIGHT_TOP;
            break;
        case (EMERALD_ACIDPOOL_BOTTOM_LEFT):
            uTextureIndex = TEX_ACID_LEFT_BOTTOM;
            break;
        case (EMERALD_ACIDPOOL_BOTTOM_MID):
            uTextureIndex = TEX_ACID_BOTTOM;
            break;
        case (EMERALD_ACIDPOOL_BOTTOM_RIGHT):
            uTextureIndex = TEX_ACID_RIGHT_BOTTOM;
            break;
        case (EMERALD_EARTH):
            uTextureIndex = TEX_EARTH;
            break;
        case (EMERALD_EARTH_INVISIBLE):
            uTextureIndex = TEX_EARTH_INVISIBLE;
            break;
        case (EMERALD_MINE_EARTH):
            uTextureIndex = TEX_MINE_EARTH;
            break;
        case (EMERALD_GRASS):
            uTextureIndex = TEX_GRASS;
            break;
        case (EMERALD_PERL):
            uTextureIndex = TEX_PERL_1;
            break;
        case (EMERALD_MEGABOMB):
            uTextureIndex = TEX_BOMB_MEGA_1 + ((Playfield.uFrameCounter & 0xFFFFFFFC) >> 2) % 5;
            break;
        case (EMERALD_BOMB):
            uTextureIndex = TEX_BOMB_1;
            break;
        case (EMERALD_WHEEL):
            uTextureIndex = TEX_WHEEL;
            fAngle = nAnimationCount * 11;
            break;
        case (EMERALD_DOOR_GREY_RED):            // Für Editor
            uTextureIndex = TEX_DOOR_GREY_RED;
            break;
        case (EMERALD_DOOR_GREY_GREEN):         // Für Editor
            uTextureIndex = TEX_DOOR_GREY_GREEN;
            break;
        case (EMERALD_DOOR_GREY_BLUE):         // Für Editor
            uTextureIndex = TEX_DOOR_GREY_BLUE;
            break;
        case (EMERALD_DOOR_GREY_YELLOW):         // Für Editor
            uTextureIndex = TEX_DOOR_GREY_YELLOW;
            break;
        case (EMERALD_DOOR_GREY_NOKEY):
            uTextureIndex = TEX_DOOR_GREY;
            break;
        case (EMERALD_MAGIC_WALL_STEEL):
            uTextureIndex = TEX_MAGIC_WALL_STEEL_1 + nAnimationCount / 2;
            break;
        case (EMERALD_MAGIC_WALL):
            uTextureIndex = TEX_MAGIC_WALL_1 + nAnimationCount / 2;
            break;
        case (EMERALD_WALL_CORNERED):
            uTextureIndex = TEX_WALL;
            break;
        case (EMERALD_WALL_ROUND):
            uTextureIndex = TEX_WALL_ROUND;
            break;
        case (EMERALD_DOOR_END_NOT_READY):
            uTextureIndex = TEX_DOOR_EXIT;
            break;
        case (EMERALD_DOOR_END_READY):
            uTextureIndex = TEX_DOOR_EXIT_1 + ge_ExitDoorSequence[((Playfield.uFrameCounter & 0xFFFFFFF8) >> 3) % 8]; // Endtür, blinkend
            break;
        case (EMERALD_DOOR_END_NOT_READY_STEEL):
            uTextureIndex = TEX_DOOR_EXIT_STEEL;
            break;
        case (EMERALD_DOOR_END_READY_STEEL):
            uTextureIndex = TEX_DOOR_EXIT_STEEL_1 + ge_ExitDoorSequence[((Playfield.uFrameCounter & 0xFFFFFFF8) >> 3) % 8]; // Stahl-Endtür, blinkend
            break;
        case (EMERALD_WALL_ROUND_PIKE):
            uTextureIndex = TEX_WALL_ROUND_PIKE;
            break;
        case (EMERALD_STEEL_TRASHCAN):
            uTextureIndex = TEX_STEEL_TRASHCAN;
            break;
        case (EMERALD_STEEL_JOYSTICK):
            uTextureIndex = TEX_STEEL_JOYSTICK;
            break;
        case (EMERALD_STEEL_EDIT_LEVEL):
            uTextureIndex = TEX_STEEL_EDIT;
            break;
        case (EMERALD_STEEL_MOVE_LEVEL):
            uTextureIndex = TEX_STEEL_MOVE;
            break;
        case (EMERALD_STEEL_ADD_LEVELGROUP):
            uTextureIndex = TEX_STEEL_ADD;
            break;
        case (EMERALD_STEEL_COPY_LEVEL):
            uTextureIndex = TEX_STEEL_COPY;
            break;
        case (EMERALD_STEEL_CLIPBOARD_LEVEL):
            uTextureIndex = TEX_STEEL_CLIPBOARD;
            break;
        case (EMERALD_STEEL_DC3_IMPORT):
            uTextureIndex = TEX_STEEL_DC3;
            break;
        case (EMERALD_STEEL_RENAME_LEVELGROUP):
            uTextureIndex = TEX_STEEL_RENAME_LEVELGROUP;
            break;
        case (EMERALD_STEEL_PASSWORD):
            uTextureIndex = TEX_STEEL_PASSWORD;
            break;
        case (EMERALD_CONVEYORBELT_RED):
            uTextureIndex = TEX_CONVEYOR_RED;
            break;
        case (EMERALD_CONVEYORBELT_SWITCH_RED):
            uTextureIndex = TEX_SWITCH_CONVEYOR_RED_OFF;
            break;
        case (EMERALD_CONVEYORBELT_GREEN):
            uTextureIndex = TEX_CONVEYOR_GREEN;
            break;
        case (EMERALD_CONVEYORBELT_SWITCH_GREEN):
            uTextureIndex = TEX_SWITCH_CONVEYOR_GREEN_OFF;
            break;
        case (EMERALD_CONVEYORBELT_BLUE):
            uTextureIndex = TEX_CONVEYOR_BLUE;
            break;
        case (EMERALD_CONVEYORBELT_SWITCH_BLUE):
            uTextureIndex = TEX_SWITCH_CONVEYOR_BLUE_OFF;
            break;
        case (EMERALD_CONVEYORBELT_YELLOW):
            uTextureIndex = TEX_CONVEYOR_YELLOW;
            break;
        case (EMERALD_CONVEYORBELT_SWITCH_YELLOW):
            uTextureIndex = TEX_SWITCH_CONVEYOR_YELLOW_OFF;
            break;
        case (EMERALD_LEVELEDITOR_MESSAGE_1_4):
            uTextureIndex = TEX_ENLARGE_WINDOW_FOR_LEVELEDITOR_1;
            break;
        case (EMERALD_LEVELEDITOR_MESSAGE_2_4):
            uTextureIndex = TEX_ENLARGE_WINDOW_FOR_LEVELEDITOR_2;
            break;
        case (EMERALD_LEVELEDITOR_MESSAGE_3_4):
            uTextureIndex = TEX_ENLARGE_WINDOW_FOR_LEVELEDITOR_3;
            break;
        case (EMERALD_LEVELEDITOR_MESSAGE_4_4):
            uTextureIndex = TEX_ENLARGE_WINDOW_FOR_LEVELEDITOR_4;
            break;
        case (EMERALD_STEEL_GROW_LEFT):
            uTextureIndex = TEX_STEEL_GROW_LEFT;
            break;
        case (EMERALD_STEEL_GROW_RIGHT):
            uTextureIndex = TEX_STEEL_GROW_RIGHT;
            break;
        case (EMERALD_STEEL_GROW_UP):
            uTextureIndex = TEX_STEEL_GROW_UP;
            break;
        case (EMERALD_STEEL_GROW_DOWN):
            uTextureIndex = TEX_STEEL_GROW_DOWN;
            break;
        case (EMERALD_STEEL_GROW_LEFT_RIGHT):
            uTextureIndex = TEX_STEEL_GROW_LEFT_RIGHT;
            break;
        case (EMERALD_STEEL_GROW_UP_DOWN):
            uTextureIndex = TEX_STEEL_GROW_UP_DOWN;
            break;
        case (EMERALD_STEEL_GROW_ALL):
            uTextureIndex = TEX_STEEL_GROW_ALL;
            break;
        case (EMERALD_WALL_GROW_LEFT):
            uTextureIndex = TEX_WALL_GROW_LEFT;
            break;
        case (EMERALD_WALL_GROW_RIGHT):
            uTextureIndex = TEX_WALL_GROW_RIGHT;
            break;
        case (EMERALD_WALL_GROW_UP):
            uTextureIndex = TEX_WALL_GROW_UP;
            break;
        case (EMERALD_WALL_GROW_DOWN):
            uTextureIndex = TEX_WALL_GROW_DOWN;
            break;
        case (EMERALD_WALL_GROW_LEFT_RIGHT):
            uTextureIndex = TEX_WALL_GROW_LEFT_RIGHT;
            break;
        case (EMERALD_WALL_GROW_UP_DOWN):
            uTextureIndex = TEX_WALL_GROW_UP_DOWN;
            break;
        case (EMERALD_WALL_GROW_ALL):
            uTextureIndex = TEX_WALL_GROW_ALL;
            break;
        case (EMERALD_TELEPORTER_RED):
            uTextureIndex = TEX_TELEPORTER_RED_01 + ((Playfield.uFrameCounter & 0xFFFFFFFE) >> 1) % 13; // Teleporter, rot
            break;
        case (EMERALD_TELEPORTER_YELLOW):
            uTextureIndex = TEX_TELEPORTER_YELLOW_01 + ((Playfield.uFrameCounter & 0xFFFFFFFE) >> 1) % 13; // Teleporter, gelb
            break;
        case (EMERALD_TELEPORTER_GREEN):
            uTextureIndex = TEX_TELEPORTER_GREEN_01 + ((Playfield.uFrameCounter & 0xFFFFFFFE) >> 1) % 13; // Teleporter, grün
            break;
        case (EMERALD_TELEPORTER_BLUE):
            uTextureIndex = TEX_TELEPORTER_BLUE_01 + ((Playfield.uFrameCounter & 0xFFFFFFFE) >> 1) % 13; // Teleporter, blau
            break;
        case (EMERALD_SWITCH_REMOTEBOMB_UP):
            uTextureIndex = TEX_SWITCH_BOMB_REMOTE_UP_OFF;
            break;
        case (EMERALD_SWITCH_REMOTEBOMB_DOWN):
            uTextureIndex = TEX_SWITCH_BOMB_REMOTE_DOWN_OFF;
            break;
        case (EMERALD_SWITCH_REMOTEBOMB_LEFT):
            uTextureIndex = TEX_SWITCH_BOMB_REMOTE_LEFT_OFF;
            break;
        case (EMERALD_SWITCH_REMOTEBOMB_RIGHT):
            uTextureIndex = TEX_SWITCH_BOMB_REMOTE_RIGHT_OFF;
            break;
        case (EMERALD_SWITCH_REMOTEBOMB_IGNITION):
            uTextureIndex = TEX_SWITCH_BOMB_REMOTE_IGNITION_OFF;
            break;
        case (EMERALD_REMOTEBOMB):
            uTextureIndex = TEX_BOMB_REMOTE;
            break;
        case (EMERALD_STEEL_MODERN_LEFT_END):
            uTextureIndex = TEX_STEEL_MODERN_LEFT_END;
            break;
        case (EMERALD_STEEL_MODERN_LEFT_RIGHT):
            uTextureIndex = TEX_STEEL_MODERN_LEFT_RIGHT;
            break;
        case (EMERALD_STEEL_MODERN_RIGHT_END):
            uTextureIndex = TEX_STEEL_MODERN_RIGHT_END;
            break;
        case (EMERALD_STEEL_MODERN_UP_END):
            uTextureIndex = TEX_STEEL_MODERN_TOP_END;
            break;
        case (EMERALD_STEEL_MODERN_UP_DOWN):
            uTextureIndex = TEX_STEEL_MODERN_TOP_BOTTOM;
            break;
        case (EMERALD_STEEL_MODERN_DOWN_END):
            uTextureIndex = TEX_STEEL_MODERN_BOTTOM_END;
            break;
        case (EMERALD_STEEL_MODERN_MIDDLE):
            uTextureIndex = TEX_STEEL_MODERN_MIDDLE;
            break;
        case (EMERALD_PIPE_UP_DOWN):
            uTextureIndex = TEX_PIPE_UP_DOWN;
            break;
        case (EMERALD_PIPE_LEFT_RIGHT):
            uTextureIndex = TEX_PIPE_LEFT_RIGHT;
            break;
        case (EMERALD_PIPE_LEFT_UP):
            uTextureIndex = TEX_PIPE_LEFT_UP;
            break;
        case (EMERALD_PIPE_LEFT_DOWN):
            uTextureIndex = TEX_PIPE_LEFT_DOWN;
            break;
        case (EMERALD_PIPE_RIGHT_UP):
            uTextureIndex = TEX_PIPE_RIGHT_UP;
            break;
        case (EMERALD_PIPE_RIGHT_DOWN):
            uTextureIndex = TEX_PIPE_RIGHT_DOWN;
            break;
        case (EMERALD_PIPE_LEFT_UP_DOWN):
            uTextureIndex = TEX_PIPE_LEFT_UP_DOWN;
            break;
        case (EMERALD_PIPE_RIGHT_UP_DOWN):
            uTextureIndex = TEX_PIPE_RIGHT_UP_DOWN;
            break;
        case (EMERALD_PIPE_LEFT_RIGHT_UP):
            uTextureIndex = TEX_PIPE_LEFT_RIGHT_UP;
            break;
        case (EMERALD_PIPE_LEFT_RIGHT_DOWN):
            uTextureIndex = TEX_PIPE_LEFT_RIGHT_DOWN;
            break;
        case (EMERALD_PIPE_LEFT_RIGHT_UP_DOWN):
            uTextureIndex = TEX_PIPE_LEFT_RIGHT_UP_DOWN;
            break;
        case (EMERALD_SMILEY):
            uTextureIndex = TEX_SMILEY_128;
            break;
        case (EMERALD_FONT_PAFF):
            uTextureIndex = TEX_MONKEY_PAFF;
            break;
        default:
            SDL_Log("%s: unknown element: %x     T:%u",__FUNCTION__,uElement,SDL_GetTicks());
            uTextureIndex = TEX_SPACE;     // Space
            break;
    }
    if (pfAngle != NULL) {
        *pfAngle = fAngle;
    }
    return uTextureIndex;
}
