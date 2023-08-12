#include <stdint.h>
#include "GetTextureIndexByElement.h"
#include "EmeraldMine.h"

extern PLAYFIELD Playfield;

/*----------------------------------------------------------------------------
Name:           GetTextureIndexByElement
------------------------------------------------------------------------------
Beschreibung: Holt den entsprechenden Texture-Index anhand eines Elements und des
              Animations-Schrittes
Parameter
      Eingang: uElement, uint16_t, Element, z.B. EMERALD_MINE_RIGHT
               nAnimationCount, int, Animationsschritt
      Ausgang: pfAngle, float *, Winkel
Rückgabewert:  uint32_t , Texture, wenn keine Texture ermittelt werden kann, wird
                SPACE (EMERALD_SPACE) zurückgegeben.
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
uint32_t GetTextureIndexByElement(uint16_t uElement,int nAnimationCount,float *pfAngle) {
    uint32_t uTextureIndex;
    uint32_t K;

    *pfAngle = 0;
    switch (uElement) {
        case (EMERALD_FONT_EXCLAMATION):
            uTextureIndex = 1;
            break;
        case (EMERALD_FONT_ARROW_RIGHT):
            uTextureIndex = 4;
            break;
        case (EMERALD_FONT_ARROW_UP):
            uTextureIndex = 5;
            break;
        case (EMERALD_FONT_ARROW_DOWN):
            uTextureIndex = 6;
            break;
        case (EMERALD_FONT_APOSTROPHE):
            uTextureIndex = 7;
            break;
        case (EMERALD_FONT_BRACE_OPEN):
            uTextureIndex = 8;
            break;
        case (EMERALD_FONT_BRACE_CLOSE):
            uTextureIndex = 9;
            break;
        case (EMERALD_FONT_COPYRIGHT):
            uTextureIndex = 10;
            break;
        case (EMERALD_FONT_PLUS):
            uTextureIndex = 11;
            break;
        case (EMERALD_FONT_COMMA):
            uTextureIndex = 12;
            break;
        case (EMERALD_FONT_MINUS):
            uTextureIndex = 13;
            break;
        case (EMERALD_FONT_POINT):
            uTextureIndex = 14;
            break;
        case (EMERALD_FONT_SLASH):
            uTextureIndex = 15;
            break;
        case (EMERALD_FONT_0):
            uTextureIndex = 16;
            break;
        case (EMERALD_FONT_1):
            uTextureIndex = 17;
            break;
        case (EMERALD_FONT_2):
            uTextureIndex = 18;
            break;
        case (EMERALD_FONT_3):
            uTextureIndex = 19;
            break;
        case (EMERALD_FONT_4):
            uTextureIndex = 20;
            break;
        case (EMERALD_FONT_5):
            uTextureIndex = 21;
            break;
        case (EMERALD_FONT_6):
            uTextureIndex = 22;
            break;
        case (EMERALD_FONT_7):
            uTextureIndex = 23;
            break;
        case (EMERALD_FONT_8):
            uTextureIndex = 24;
            break;
        case (EMERALD_FONT_9):
            uTextureIndex = 25;
            break;
        case (EMERALD_FONT_DOUBLE_POINT):
            uTextureIndex = 26;
            break;
        case (EMERALD_FONT_PLATE):
            uTextureIndex = 27;
            break;
        case (EMERALD_FONT_ARROW_LEFT):
            uTextureIndex = 28;
            break;
        case (EMERALD_FONT_PAFF):
            uTextureIndex = 29;
            break;
        case (EMERALD_FONT_QUESTION_MARK):
            uTextureIndex = 31;
            break;
        case (EMERALD_FONT_A):
            uTextureIndex = 33;
            break;
        case (EMERALD_FONT_B):
            uTextureIndex = 34;
            break;
        case (EMERALD_FONT_C):
            uTextureIndex = 35;
            break;
        case (EMERALD_FONT_D):
            uTextureIndex = 36;
            break;
        case (EMERALD_FONT_E):
            uTextureIndex = 37;
            break;
        case (EMERALD_FONT_F):
            uTextureIndex = 38;
            break;
        case (EMERALD_FONT_G):
            uTextureIndex = 39;
            break;
        case (EMERALD_FONT_H):
            uTextureIndex = 40;
            break;
        case (EMERALD_FONT_I):
            uTextureIndex = 41;
            break;
        case (EMERALD_FONT_J):
            uTextureIndex = 42;
            break;
        case (EMERALD_FONT_K):
            uTextureIndex = 43;
            break;
        case (EMERALD_FONT_L):
            uTextureIndex = 44;
            break;
        case (EMERALD_FONT_M):
            uTextureIndex = 45;
            break;
        case (EMERALD_FONT_N):
            uTextureIndex = 46;
            break;
        case (EMERALD_FONT_O):
            uTextureIndex = 47;
            break;
        case (EMERALD_FONT_P):
            uTextureIndex = 48;
            break;
        case (EMERALD_FONT_Q):
            uTextureIndex = 49;
            break;
        case (EMERALD_FONT_R):
            uTextureIndex = 50;
            break;
        case (EMERALD_FONT_S):
            uTextureIndex = 51;
            break;
        case (EMERALD_FONT_T):
            uTextureIndex = 52;
            break;
        case (EMERALD_FONT_U):
            uTextureIndex = 53;
            break;
        case (EMERALD_FONT_V):
            uTextureIndex = 54;
            break;
        case (EMERALD_FONT_W):
            uTextureIndex = 55;
            break;
        case (EMERALD_FONT_X):
            uTextureIndex = 56;
            break;
        case (EMERALD_FONT_Y):
            uTextureIndex = 57;
            break;
        case (EMERALD_FONT_Z):
            uTextureIndex = 58;
            break;
        case (EMERALD_FONT_AE):
            uTextureIndex = 65;
            break;
        case (EMERALD_FONT_OE):
            uTextureIndex = 66;
            break;
        case (EMERALD_FONT_UE):
            uTextureIndex = 67;
            break;




        case (EMERALD_FONT_STEEL_EXCLAMATION):
            uTextureIndex = 674;
            break;
        case (EMERALD_FONT_STEEL_ARROW_RIGHT):
            uTextureIndex = 700;
            break;
        case (EMERALD_FONT_STEEL_ARROW_UP):
            uTextureIndex = 698;
            break;
        case (EMERALD_FONT_STEEL_ARROW_DOWN):
            uTextureIndex = 701;
            break;
        case (EMERALD_FONT_STEEL_APOSTROPHE):
            uTextureIndex = 673;
            break;
        case (EMERALD_FONT_STEEL_BRACE_OPEN):
            uTextureIndex = 688;
            break;
        case (EMERALD_FONT_STEEL_BRACE_CLOSE):
            uTextureIndex = 689;
            break;
        case (EMERALD_FONT_STEEL_COPYRIGHT):
            uTextureIndex = 677;
            break;
        case (EMERALD_FONT_STEEL_PLUS):
            uTextureIndex = 702;
            break;
        case (EMERALD_FONT_STEEL_COMMA):
            uTextureIndex = 690;
            break;
        case (EMERALD_FONT_STEEL_MINUS):
            uTextureIndex = 693;
            break;
        case (EMERALD_FONT_STEEL_POINT):
            uTextureIndex = 703;
            break;
        case (EMERALD_FONT_STEEL_SLASH):
            uTextureIndex = 707;
            break;
        case (EMERALD_FONT_STEEL_0):
            uTextureIndex = 662;
            break;
        case (EMERALD_FONT_STEEL_1):
            uTextureIndex = 663;
            break;
        case (EMERALD_FONT_STEEL_2):
            uTextureIndex = 664;
            break;
        case (EMERALD_FONT_STEEL_3):
            uTextureIndex = 665;
            break;
        case (EMERALD_FONT_STEEL_4):
            uTextureIndex = 666;
            break;
        case (EMERALD_FONT_STEEL_5):
            uTextureIndex = 667;
            break;
        case (EMERALD_FONT_STEEL_6):
            uTextureIndex = 668;
            break;
        case (EMERALD_FONT_STEEL_7):
            uTextureIndex = 669;
            break;
        case (EMERALD_FONT_STEEL_8):
            uTextureIndex = 670;
            break;
        case (EMERALD_FONT_STEEL_9):
            uTextureIndex = 671;
            break;
        case (EMERALD_FONT_STEEL_DOUBLE_POINT):
            uTextureIndex = 679;
            break;
        case (EMERALD_FONT_STEEL_PLATE):
            uTextureIndex = 709;
            break;
        case (EMERALD_FONT_STEEL_ARROW_LEFT):
            uTextureIndex = 699;
            break;
        case (EMERALD_FONT_STEEL_QUESTION_MARK):
            uTextureIndex = 682;
            break;
        case (EMERALD_FONT_STEEL_A):
            uTextureIndex = 675;
            break;
        case (EMERALD_FONT_STEEL_B):
            uTextureIndex = 676;
            break;
        case (EMERALD_FONT_STEEL_C):
            uTextureIndex = 678;
            break;
        case (EMERALD_FONT_STEEL_D):
            uTextureIndex = 680;
            break;
        case (EMERALD_FONT_STEEL_E):
            uTextureIndex = 681;
            break;
        case (EMERALD_FONT_STEEL_F):
            uTextureIndex = 683;
            break;
        case (EMERALD_FONT_STEEL_G):
            uTextureIndex = 684;
            break;
        case (EMERALD_FONT_STEEL_H):
            uTextureIndex = 685;
            break;
        case (EMERALD_FONT_STEEL_I):
            uTextureIndex = 686;
            break;
        case (EMERALD_FONT_STEEL_J):
            uTextureIndex = 687;
            break;
        case (EMERALD_FONT_STEEL_K):
            uTextureIndex = 691;
            break;
        case (EMERALD_FONT_STEEL_L):
            uTextureIndex = 692;
            break;
        case (EMERALD_FONT_STEEL_M):
            uTextureIndex = 694;
            break;
        case (EMERALD_FONT_STEEL_N):
            uTextureIndex = 695;
            break;
        case (EMERALD_FONT_STEEL_O):
            uTextureIndex = 697;
            break;
        case (EMERALD_FONT_STEEL_P):
            uTextureIndex = 704;
            break;
        case (EMERALD_FONT_STEEL_Q):
            uTextureIndex = 705;
            break;
        case (EMERALD_FONT_STEEL_R):
            uTextureIndex = 706;
            break;
        case (EMERALD_FONT_STEEL_S):
            uTextureIndex = 708;
            break;
        case (EMERALD_FONT_STEEL_T):
            uTextureIndex = 710;
            break;
        case (EMERALD_FONT_STEEL_U):
            uTextureIndex = 712;
            break;
        case (EMERALD_FONT_STEEL_V):
            uTextureIndex = 713;
            break;
        case (EMERALD_FONT_STEEL_W):
            uTextureIndex = 714;
            break;
        case (EMERALD_FONT_STEEL_X):
            uTextureIndex = 715;
            break;
        case (EMERALD_FONT_STEEL_Y):
            uTextureIndex = 716;
            break;
        case (EMERALD_FONT_STEEL_Z):
            uTextureIndex = 717;
            break;
        case (EMERALD_FONT_STEEL_AE):
            uTextureIndex = 672;
            break;
        case (EMERALD_FONT_STEEL_OE):
            uTextureIndex = 696;
            break;
        case (EMERALD_FONT_STEEL_UE):
            uTextureIndex = 711;
            break;
        case (EMERALD_FONT_GREEN_EXCLAMATION):
            uTextureIndex = 562;
            break;
        case (EMERALD_FONT_GREEN_ARROW_RIGHT):
            uTextureIndex = 588;
            break;
        case (EMERALD_FONT_GREEN_ARROW_UP):
            uTextureIndex = 586;
            break;
        case (EMERALD_FONT_GREEN_ARROW_DOWN):
            uTextureIndex = 589;
            break;
        case (EMERALD_FONT_GREEN_APOSTROPHE):
            uTextureIndex = 561;
            break;
        case (EMERALD_FONT_GREEN_BRACE_OPEN):
            uTextureIndex = 576;
            break;
        case (EMERALD_FONT_GREEN_BRACE_CLOSE):
            uTextureIndex = 577;
            break;
        case (EMERALD_FONT_GREEN_COPYRIGHT):
            uTextureIndex = 565;
            break;
        case (EMERALD_FONT_GREEN_PLUS):
            uTextureIndex = 590;
            break;
        case (EMERALD_FONT_GREEN_COMMA):
            uTextureIndex = 578;
            break;
        case (EMERALD_FONT_GREEN_MINUS):
            uTextureIndex = 581;
            break;
        case (EMERALD_FONT_GREEN_POINT):
            uTextureIndex = 591;
            break;
        case (EMERALD_FONT_GREEN_SLASH):
            uTextureIndex = 595;
            break;
        case (EMERALD_FONT_GREEN_0):
            uTextureIndex = 84;
            break;
        case (EMERALD_FONT_GREEN_1):
            uTextureIndex = 85;
            break;
        case (EMERALD_FONT_GREEN_2):
            uTextureIndex = 86;
            break;
        case (EMERALD_FONT_GREEN_3):
            uTextureIndex = 87;
            break;
        case (EMERALD_FONT_GREEN_4):
            uTextureIndex = 88;
            break;
        case (EMERALD_FONT_GREEN_5):
            uTextureIndex = 89;
            break;
        case (EMERALD_FONT_GREEN_6):
            uTextureIndex = 90;
            break;
        case (EMERALD_FONT_GREEN_7):
            uTextureIndex = 91;
            break;
        case (EMERALD_FONT_GREEN_8):
            uTextureIndex = 92;
            break;
        case (EMERALD_FONT_GREEN_9):
            uTextureIndex = 93;
            break;
        case (EMERALD_FONT_GREEN_DOUBLE_POINT):
            uTextureIndex = 567;
            break;
        case (EMERALD_FONT_GREEN_PLATE):
            uTextureIndex = 597;
            break;
        case (EMERALD_FONT_GREEN_ARROW_LEFT):
            uTextureIndex = 587;
            break;
        case (EMERALD_FONT_GREEN_QUESTION_MARK):
            uTextureIndex = 570;
            break;
        case (EMERALD_FONT_GREEN_A):
            uTextureIndex = 563;
            break;
        case (EMERALD_FONT_GREEN_B):
            uTextureIndex = 564;
            break;
        case (EMERALD_FONT_GREEN_C):
            uTextureIndex = 566;
            break;
        case (EMERALD_FONT_GREEN_D):
            uTextureIndex = 568;
            break;
        case (EMERALD_FONT_GREEN_E):
            uTextureIndex = 569;
            break;
        case (EMERALD_FONT_GREEN_F):
            uTextureIndex = 571;
            break;
        case (EMERALD_FONT_GREEN_G):
            uTextureIndex = 572;
            break;
        case (EMERALD_FONT_GREEN_H):
            uTextureIndex = 573;
            break;
        case (EMERALD_FONT_GREEN_I):
            uTextureIndex = 574;
            break;
        case (EMERALD_FONT_GREEN_J):
            uTextureIndex = 575;
            break;
        case (EMERALD_FONT_GREEN_K):
            uTextureIndex = 579;
            break;
        case (EMERALD_FONT_GREEN_L):
            uTextureIndex = 580;
            break;
        case (EMERALD_FONT_GREEN_M):
            uTextureIndex = 582;
            break;
        case (EMERALD_FONT_GREEN_N):
            uTextureIndex = 583;
            break;
        case (EMERALD_FONT_GREEN_O):
            uTextureIndex = 585;
            break;
        case (EMERALD_FONT_GREEN_P):
            uTextureIndex = 592;
            break;
        case (EMERALD_FONT_GREEN_Q):
            uTextureIndex = 593;
            break;
        case (EMERALD_FONT_GREEN_R):
            uTextureIndex = 594;
            break;
        case (EMERALD_FONT_GREEN_S):
            uTextureIndex = 596;
            break;
        case (EMERALD_FONT_GREEN_T):
            uTextureIndex = 598;
            break;
        case (EMERALD_FONT_GREEN_U):
            uTextureIndex = 600;
            break;
        case (EMERALD_FONT_GREEN_V):
            uTextureIndex = 601;
            break;
        case (EMERALD_FONT_GREEN_W):
            uTextureIndex = 602;
            break;
        case (EMERALD_FONT_GREEN_X):
            uTextureIndex = 603;
            break;
        case (EMERALD_FONT_GREEN_Y):
            uTextureIndex = 604;
            break;
        case (EMERALD_FONT_GREEN_Z):
            uTextureIndex = 605;
            break;
        case (EMERALD_FONT_GREEN_AE):
            uTextureIndex = 560;
            break;
        case (EMERALD_FONT_GREEN_OE):
            uTextureIndex = 584;
            break;
        case (EMERALD_FONT_GREEN_UE):
            uTextureIndex = 599;
            break;
        case (EMERALD_FONT_STEEL_GREEN_EXCLAMATION):
            uTextureIndex = 618;
            break;
        case (EMERALD_FONT_STEEL_GREEN_ARROW_RIGHT):
            uTextureIndex = 644;
            break;
        case (EMERALD_FONT_STEEL_GREEN_ARROW_UP):
            uTextureIndex = 642;
            break;
        case (EMERALD_FONT_STEEL_GREEN_ARROW_DOWN):
            uTextureIndex = 645;
            break;
        case (EMERALD_FONT_STEEL_GREEN_APOSTROPHE):
            uTextureIndex = 617;
            break;
        case (EMERALD_FONT_STEEL_GREEN_BRACE_OPEN):
            uTextureIndex = 632;
            break;
        case (EMERALD_FONT_STEEL_GREEN_BRACE_CLOSE):
            uTextureIndex = 633;
            break;
        case (EMERALD_FONT_STEEL_GREEN_COPYRIGHT):
            uTextureIndex = 621;
            break;
        case (EMERALD_FONT_STEEL_GREEN_PLUS):
            uTextureIndex = 646;
            break;
        case (EMERALD_FONT_STEEL_GREEN_COMMA):
            uTextureIndex = 634;
            break;
        case (EMERALD_FONT_STEEL_GREEN_MINUS):
            uTextureIndex = 637;
            break;
        case (EMERALD_FONT_STEEL_GREEN_POINT):
            uTextureIndex = 647;
            break;
        case (EMERALD_FONT_STEEL_GREEN_SLASH):
            uTextureIndex = 651;
            break;
        case (EMERALD_FONT_STEEL_GREEN_0):
            uTextureIndex = 606;
            break;
        case (EMERALD_FONT_STEEL_GREEN_1):
            uTextureIndex = 607;
            break;
        case (EMERALD_FONT_STEEL_GREEN_2):
            uTextureIndex = 608;
            break;
        case (EMERALD_FONT_STEEL_GREEN_3):
            uTextureIndex = 609;
            break;
        case (EMERALD_FONT_STEEL_GREEN_4):
            uTextureIndex = 610;
            break;
        case (EMERALD_FONT_STEEL_GREEN_5):
            uTextureIndex = 611;
            break;
        case (EMERALD_FONT_STEEL_GREEN_6):
            uTextureIndex = 612;
            break;
        case (EMERALD_FONT_STEEL_GREEN_7):
            uTextureIndex = 613;
            break;
        case (EMERALD_FONT_STEEL_GREEN_8):
            uTextureIndex = 614;
            break;
        case (EMERALD_FONT_STEEL_GREEN_9):
            uTextureIndex = 615;
            break;
        case (EMERALD_FONT_STEEL_GREEN_DOUBLE_POINT):
            uTextureIndex = 623;
            break;
        case (EMERALD_FONT_STEEL_GREEN_PLATE):
            uTextureIndex = 653;
            break;
        case (EMERALD_FONT_STEEL_GREEN_ARROW_LEFT):
            uTextureIndex = 643;
            break;
        case (EMERALD_FONT_STEEL_GREEN_QUESTION_MARK):
            uTextureIndex = 626;
            break;
        case (EMERALD_FONT_STEEL_GREEN_A):
            uTextureIndex = 619;
            break;
        case (EMERALD_FONT_STEEL_GREEN_B):
            uTextureIndex = 620;
            break;
        case (EMERALD_FONT_STEEL_GREEN_C):
            uTextureIndex = 622;
            break;
        case (EMERALD_FONT_STEEL_GREEN_D):
            uTextureIndex = 624;
            break;
        case (EMERALD_FONT_STEEL_GREEN_E):
            uTextureIndex = 625;
            break;
        case (EMERALD_FONT_STEEL_GREEN_F):
            uTextureIndex = 627;
            break;
        case (EMERALD_FONT_STEEL_GREEN_G):
            uTextureIndex = 628;
            break;
        case (EMERALD_FONT_STEEL_GREEN_H):
            uTextureIndex = 629;
            break;
        case (EMERALD_FONT_STEEL_GREEN_I):
            uTextureIndex = 630;
            break;
        case (EMERALD_FONT_STEEL_GREEN_J):
            uTextureIndex = 631;
            break;
        case (EMERALD_FONT_STEEL_GREEN_K):
            uTextureIndex = 635;
            break;
        case (EMERALD_FONT_STEEL_GREEN_L):
            uTextureIndex = 636;
            break;
        case (EMERALD_FONT_STEEL_GREEN_M):
            uTextureIndex = 638;
            break;
        case (EMERALD_FONT_STEEL_GREEN_N):
            uTextureIndex = 639;
            break;
        case (EMERALD_FONT_STEEL_GREEN_O):
            uTextureIndex = 641;
            break;
        case (EMERALD_FONT_STEEL_GREEN_P):
            uTextureIndex = 648;
            break;
        case (EMERALD_FONT_STEEL_GREEN_Q):
            uTextureIndex = 649;
            break;
        case (EMERALD_FONT_STEEL_GREEN_R):
            uTextureIndex = 650;
            break;
        case (EMERALD_FONT_STEEL_GREEN_S):
            uTextureIndex = 652;
            break;
        case (EMERALD_FONT_STEEL_GREEN_T):
            uTextureIndex = 654;
            break;
        case (EMERALD_FONT_STEEL_GREEN_U):
            uTextureIndex = 656;
            break;
        case (EMERALD_FONT_STEEL_GREEN_V):
            uTextureIndex = 657;
            break;
        case (EMERALD_FONT_STEEL_GREEN_W):
            uTextureIndex = 658;
            break;
        case (EMERALD_FONT_STEEL_GREEN_X):
            uTextureIndex = 659;
            break;
        case (EMERALD_FONT_STEEL_GREEN_Y):
            uTextureIndex = 660;
            break;
        case (EMERALD_FONT_STEEL_GREEN_Z):
            uTextureIndex = 661;
            break;
        case (EMERALD_FONT_STEEL_GREEN_AE):
            uTextureIndex = 616;
            break;
        case (EMERALD_FONT_STEEL_GREEN_OE):
            uTextureIndex = 640;
            break;
        case (EMERALD_FONT_STEEL_GREEN_UE):
            uTextureIndex = 655;
            break;
        case (EMERALD_DYNAMITE_ON):
            uTextureIndex = 555 + nAnimationCount / 4;
            break;
        case (EMERALD_SWAMP_STONE):
            uTextureIndex = 554;                // Für Editor
            break;
        case (EMERALD_WALL_WITH_CRYSTAL):
            uTextureIndex = 533;
            break;
        case (EMERALD_WALL_WITH_KEY_RED):
            uTextureIndex = 534;
            break;
        case (EMERALD_WALL_WITH_KEY_GREEN):
            uTextureIndex = 535;
            break;
        case (EMERALD_WALL_WITH_KEY_BLUE):
            uTextureIndex = 536;
            break;
        case (EMERALD_WALL_WITH_KEY_YELLOW):
            uTextureIndex = 537;
            break;
        case (EMERALD_WALL_WITH_KEY_WHITE):
            uTextureIndex = 538;
            break;
        case (EMERALD_WALL_WITH_KEY_GENERAL):
            uTextureIndex = 539;
            break;
        case (EMERALD_WALL_WITH_TIME_COIN):
            uTextureIndex = 735;
            break;
        case (EMERALD_WALL_WITH_BOMB):
            uTextureIndex = 540;
            break;
        case (EMERALD_WALL_WITH_MEGABOMB):
            uTextureIndex = 541;
            break;
        case (EMERALD_WALL_WITH_STONE):
            uTextureIndex = 542;
            break;
        case (EMERALD_WALL_WITH_NUT):
            uTextureIndex = 543;
            break;
        case (EMERALD_WALL_WITH_WHEEL):
            uTextureIndex = 544;
            break;
        case (EMERALD_WALL_WITH_DYNAMITE):
            uTextureIndex = 545;
            break;
        case (EMERALD_WALL_WITH_ENDDOOR):
            uTextureIndex = 546;
            break;
        case (EMERALD_WALL_WITH_ENDDOOR_READY):
            uTextureIndex = 547;
            break;
        case (EMERALD_WALL_WITH_MINE_UP):
            uTextureIndex = 548;
            break;
        case (EMERALD_WALL_WITH_BEETLE_UP):
            uTextureIndex = 549;
            break;
        case (EMERALD_WALL_WITH_YAM):
            uTextureIndex = 550;
            break;
        case (EMERALD_WALL_WITH_ALIEN):
            uTextureIndex = 551;
            break;
        case (EMERALD_WALL_WITH_MOLE_UP):
            uTextureIndex = 552;
            break;
        case (EMERALD_WALL_WITH_GREEN_CHEESE):
            uTextureIndex = 553;
            break;
        case (EMERALD_WALL_WITH_EMERALD):
            uTextureIndex = 529;
            break;
        case (EMERALD_WALL_WITH_RUBY):
            uTextureIndex = 530;
            break;
        case (EMERALD_WALL_WITH_SAPPHIRE):
            uTextureIndex = 531;
            break;
        case (EMERALD_WALL_WITH_PERL):
            uTextureIndex = 532;
            break;
        case (EMERALD_DOOR_WHITE):
            uTextureIndex = 521;
            break;
        case (EMERALD_DOOR_WHITE_WOOD):
            uTextureIndex = 522;
            break;
        case (EMERALD_DOOR_GREY_WHITE):
            uTextureIndex = 523;
            break;
        case (EMERALD_KEY_WHITE):
            uTextureIndex = 519;
            break;
        case (EMERALD_KEY_GENERAL):
            uTextureIndex = 520;
            break;
        case (EMERALD_SWITCHDOOR_OPEN):
            uTextureIndex = 506;
            break;
        case (EMERALD_SWITCHDOOR_CLOSED):
            uTextureIndex = 502;
            break;
        case (EMERALD_SWITCH_SWITCHDOOR):
            uTextureIndex = 507;
            break;
        case (EMERALD_DOOR_TIME):
            uTextureIndex = 501;
            break;
        case (EMERALD_WHEEL_TIMEDOOR):
            uTextureIndex = 493 + nAnimationCount / 4;
            break;
        case (EMERALD_DOOR_EMERALD):
            uTextureIndex = 487;
            break;
        case (EMERALD_DOOR_MULTICOLOR):
            uTextureIndex = 488;
            break;
        case (EMERALD_DOOR_ONLY_UP_STEEL):
            uTextureIndex = 489;
            break;
        case (EMERALD_DOOR_ONLY_DOWN_STEEL):
            uTextureIndex = 490;
            break;
        case (EMERALD_DOOR_ONLY_LEFT_STEEL):
            uTextureIndex = 491;
            break;
        case (EMERALD_DOOR_ONLY_RIGHT_STEEL):
            uTextureIndex = 492;
            break;
        case (EMERALD_DOOR_ONLY_UP_WALL):
            uTextureIndex = 964;
            break;
        case (EMERALD_DOOR_ONLY_DOWN_WALL):
            uTextureIndex = 965;
            break;
        case (EMERALD_DOOR_ONLY_LEFT_WALL):
            uTextureIndex = 962;
            break;
        case (EMERALD_DOOR_ONLY_RIGHT_WALL):
            uTextureIndex = 963;
            break;
        case (EMERALD_STEEL_INVISIBLE):
            uTextureIndex = 484;
            break;
        case (EMERALD_WALL_INVISIBLE):
            uTextureIndex = 485;
            break;
        case (EMERALD_LIGHT_SWITCH):
            uTextureIndex = 486;
            break;
        case (EMERALD_STEEL_HEART):
            uTextureIndex = 474;
            break;
        case (EMERALD_STEEL_PLAYERHEAD):
            uTextureIndex = 475;
            break;
        case (EMERALD_STEEL_NO_ENTRY):
            uTextureIndex = 476;
            break;
        case (EMERALD_STEEL_GIVE_WAY):
            uTextureIndex = 477;
            break;
        case (EMERALD_STEEL_YING):
            uTextureIndex = 478;
            break;
        case (EMERALD_STEEL_WHEELCHAIR):
            uTextureIndex = 479;
            break;
        case (EMERALD_STEEL_ARROW_DOWN):
            uTextureIndex = 480;
            break;
        case (EMERALD_STEEL_ARROW_UP):
            uTextureIndex = 481;
            break;
        case (EMERALD_STEEL_ARROW_LEFT):
            uTextureIndex = 482;
            break;
        case (EMERALD_STEEL_ARROW_RIGHT):
            uTextureIndex = 483;
            break;
        case (EMERALD_STANDMINE):
            if ((((Playfield.uFrameCounter & 0xFFFFFFFC) >> 2) % 32) == 0) {
                uTextureIndex = 448;
            } else if ((((Playfield.uFrameCounter & 0xFFFFFFFC) >> 2) % 16) == 0) {
                uTextureIndex = 449;
            } else {
                uTextureIndex = 447;
            }
            break;
        case (EMERALD_MAGIC_WALL_SWITCH):
            uTextureIndex = 435;
            break;
        case (EMERALD_LIGHTBARRIER_RED_UP):
            if (Playfield.bLightBarrierRedOn) {
                uTextureIndex = 423;
            } else {
                uTextureIndex = 419;
            }
            break;
        case (EMERALD_LIGHTBARRIER_RED_DOWN):
            if (Playfield.bLightBarrierRedOn) {
                uTextureIndex = 424;
            } else {
                uTextureIndex = 420;
            }
            break;
        case (EMERALD_LIGHTBARRIER_RED_LEFT):
            if (Playfield.bLightBarrierRedOn) {
                uTextureIndex = 425;
            } else {
                uTextureIndex = 421;
            }
            break;
        case (EMERALD_LIGHTBARRIER_RED_RIGHT):
            if (Playfield.bLightBarrierRedOn) {
                uTextureIndex = 426;
            } else {
                uTextureIndex = 422;
            }
            break;
        case (EMERALD_LIGHTBARRIER_GREEN_UP):
            if (Playfield.bLightBarrierGreenOn) {
                uTextureIndex = 399;
            } else {
                uTextureIndex = 395;
            }
            break;
        case (EMERALD_LIGHTBARRIER_GREEN_DOWN):
            if (Playfield.bLightBarrierGreenOn) {
                uTextureIndex = 400;
            } else {
                uTextureIndex = 396;
            }
            break;
        case (EMERALD_LIGHTBARRIER_GREEN_LEFT):
            if (Playfield.bLightBarrierGreenOn) {
                uTextureIndex = 401;
            } else {
                uTextureIndex = 397;
            }
            break;
        case (EMERALD_LIGHTBARRIER_GREEN_RIGHT):
            if (Playfield.bLightBarrierGreenOn) {
                uTextureIndex = 402;
            } else {
                uTextureIndex = 398;
            }
            break;
        case (EMERALD_LIGHTBARRIER_BLUE_UP):
            if (Playfield.bLightBarrierBlueOn) {
                uTextureIndex = 407;
            } else {
                uTextureIndex = 403;
            }
            break;
        case (EMERALD_LIGHTBARRIER_BLUE_DOWN):
            if (Playfield.bLightBarrierBlueOn) {
                uTextureIndex = 408;
            } else {
                uTextureIndex = 404;
            }
            break;
        case (EMERALD_LIGHTBARRIER_BLUE_LEFT):
            if (Playfield.bLightBarrierBlueOn) {
                uTextureIndex = 409;
            } else {
                uTextureIndex = 405;
            }
            break;
        case (EMERALD_LIGHTBARRIER_BLUE_RIGHT):
            if (Playfield.bLightBarrierBlueOn) {
                uTextureIndex = 410;
            } else {
                uTextureIndex = 406;
            }
            break;
        case (EMERALD_LIGHTBARRIER_YELLOW_UP):
            if (Playfield.bLightBarrierYellowOn) {
                uTextureIndex = 415;
            } else {
                uTextureIndex = 411;
            }
            break;
        case (EMERALD_LIGHTBARRIER_YELLOW_DOWN):
            if (Playfield.bLightBarrierYellowOn) {
                uTextureIndex = 416;
            } else {
                uTextureIndex = 412;
            }
            break;
        case (EMERALD_LIGHTBARRIER_YELLOW_LEFT):
            if (Playfield.bLightBarrierYellowOn) {
                uTextureIndex = 417;
            } else {
                uTextureIndex = 413;
            }
            break;
        case (EMERALD_LIGHTBARRIER_YELLOW_RIGHT):
            if (Playfield.bLightBarrierYellowOn) {
                uTextureIndex = 418;
            } else {
                uTextureIndex = 414;
            }
            break;
        case (EMERALD_BEAM_RED_VERTICAL):
            uTextureIndex = 433;
            break;
        case (EMERALD_BEAM_RED_HORIZONTAL):
            uTextureIndex = 434;
            break;
        case (EMERALD_BEAM_GREEN_VERTICAL):
            uTextureIndex = 427;
            break;
        case (EMERALD_BEAM_GREEN_HORIZONTAL):
            uTextureIndex = 428;
            break;
        case (EMERALD_BEAM_BLUE_VERTICAL):
            uTextureIndex = 429;
            break;
        case (EMERALD_BEAM_BLUE_HORIZONTAL):
            uTextureIndex = 430;
            break;
        case (EMERALD_BEAM_YELLOW_VERTICAL):
            uTextureIndex = 431;
            break;
        case (EMERALD_BEAM_YELLOW_HORIZONTAL):
            uTextureIndex = 432;
            break;
        case (EMERALD_BEAM_CROSS):
            uTextureIndex = 0;
            break;
        case (EMERALD_LIGHTBARRIER_RED_SWITCH):
            uTextureIndex = 383;
            break;
        case (EMERALD_LIGHTBARRIER_GREEN_SWITCH):
            uTextureIndex = 389;
            break;
        case (EMERALD_LIGHTBARRIER_BLUE_SWITCH):
            uTextureIndex = 392;
            break;
        case (EMERALD_LIGHTBARRIER_YELLOW_SWITCH):
            uTextureIndex = 386;
            break;
        case (EMERALD_YAM):
            K = Playfield.uFrameCounter % 11;       // Y von 0 bis 10
            if (K <= 5) {                           // 0,1,2,3,4,5
                uTextureIndex = 362 + K;            // 362 - 367
            } else {                                // 6,7,8,9,10
                uTextureIndex = 367 + 5 - K;        // 366 - 362
            }
            break;
        case (EMERALD_MESSAGE_1):
        case (EMERALD_MESSAGE_2):
        case (EMERALD_MESSAGE_3):
        case (EMERALD_MESSAGE_4):
        case (EMERALD_MESSAGE_5):
        case (EMERALD_MESSAGE_6):
        case (EMERALD_MESSAGE_7):
        case (EMERALD_MESSAGE_8):
            uTextureIndex = 356;
            break;
        case (EMERALD_STEEL_MARKER_LEFT_UP):
            uTextureIndex = 348;
            break;
        case (EMERALD_STEEL_MARKER_UP):
            uTextureIndex = 349;
            break;
        case (EMERALD_STEEL_MARKER_RIGHT_UP):
            uTextureIndex = 350;
            break;
        case (EMERALD_STEEL_MARKER_LEFT):
            uTextureIndex = 351;
            break;
        case (EMERALD_STEEL_MARKER_RIGHT):
            uTextureIndex = 352;
            break;
        case (EMERALD_STEEL_MARKER_LEFT_BOTTOM):
            uTextureIndex = 353;
            break;
        case (EMERALD_STEEL_MARKER_BOTTOM):
            uTextureIndex = 354;
            break;
        case (EMERALD_STEEL_MARKER_RIGHT_BOTTOM):
            uTextureIndex = 355;
            break;
        case (EMERALD_GREEN_DROP):
            uTextureIndex = 346;
            break;
        case (EMERALD_GREEN_CHEESE):
            uTextureIndex = 343;
            break;
        case (EMERALD_STEEL_FORBIDDEN):
            uTextureIndex = 334;
            break;
        case (EMERALD_STEEL_EXIT):
            uTextureIndex = 332;
            break;
        case (EMERALD_STEEL_RADIOACTIVE):
            uTextureIndex = 337;
            break;
        case (EMERALD_STEEL_EXPLOSION):
            uTextureIndex = 333;
            break;
        case (EMERALD_STEEL_ACID):
            uTextureIndex = 329;
            break;
        case (EMERALD_STEEL_NOT_ROUND):
            uTextureIndex = 335;
            break;
        case (EMERALD_WALL_NOT_ROUND):
            uTextureIndex = 342;
            break;
        case (EMERALD_STEEL_PARKING):
            uTextureIndex = 336;
            break;
        case (EMERALD_STEEL_STOP):
            uTextureIndex = 340;
            break;
        case (EMERALD_STEEL_DEADEND):
            uTextureIndex = 331;
            break;
        case (EMERALD_STEEL_BIOHAZARD):
            uTextureIndex = 330;
            break;
        case (EMERALD_STEEL_WARNING):
            uTextureIndex = 341;
            break;
        case (EMERALD_STEEL_ROUND):
            uTextureIndex = 338;
            break;
        case (EMERALD_STEEL_ROUND_PIKE):
            uTextureIndex = 339;
            break;
        case (EMERALD_EMERALD):
            uTextureIndex = 226 + nAnimationCount / 2;     // Emerald, liegend
            break;
        case (EMERALD_RUBY):
            uTextureIndex = 301 + nAnimationCount / 2;     // Rubin, liegend
            break;
        case (EMERALD_CRYSTAL):
            uTextureIndex = 309;
            break;
        case (EMERALD_SAPPHIRE):
            uTextureIndex = 248 + ((Playfield.uFrameCounter & 0xFFFFFFFE) >> 1) % 9; // Saphir fallend
            break;
        case (EMERALD_TIME_COIN):
            uTextureIndex = 310 + ((Playfield.uFrameCounter & 0xFFFFFFF8) >> 3) % 6; // Zeitmünze, drehend
            break;
        case (EMERALD_HAMMER):
            uTextureIndex = 318;                            // Hammer
            break;
        case (EMERALD_DYNAMITE_OFF):
            uTextureIndex = 286;                            // Dynamit, aus
            break;
        case (EMERALD_NUT):
            uTextureIndex = 234;
            break;
        case (EMERALD_STONE):
            uTextureIndex = 71;
            break;
        case (EMERALD_ALIEN):
            if ((nAnimationCount >= 4) && (nAnimationCount <= 11)) {
                uTextureIndex = 135;                        // Alien geht 2, Flügel voll ausgebreitet
            } else {
                uTextureIndex = 136;                        // Alien geht 1
            }
            break;
        case (EMERALD_MOLE_UP):
            *pfAngle = 90;
             uTextureIndex = 450 + Playfield.uFrameCounter % 11;     // Mole links
            break;
        case (EMERALD_MOLE_RIGHT):
            *pfAngle = 180;
             uTextureIndex = 450 + Playfield.uFrameCounter % 11;     // Mole links
             break;
        case (EMERALD_MOLE_DOWN):
            *pfAngle = 270;
            uTextureIndex = 450 + Playfield.uFrameCounter % 11;     // Mole links
            break;
        case( EMERALD_MOLE_LEFT):
            *pfAngle = 0;
             uTextureIndex = 450 + Playfield.uFrameCounter % 11;     // Mole links
            break;
        case (EMERALD_MINE_UP):
            *pfAngle = 90;
            if ( ((nAnimationCount >= 0) && (nAnimationCount <= 3)) || ((nAnimationCount >= 8) && (nAnimationCount <=11)) ) {
                uTextureIndex = 73;     // Mine links
            } else {
                uTextureIndex = 74;     // Mine links an
            }
            break;
        case (EMERALD_MINE_RIGHT):
            *pfAngle = 180;
            if ( ((nAnimationCount >= 0) && (nAnimationCount <= 3)) || ((nAnimationCount >= 8) && (nAnimationCount <=11)) ) {
                uTextureIndex = 73;     // Mine links
            } else {
                uTextureIndex = 74;     // Mine links an
            }
            break;
        case (EMERALD_MINE_DOWN):
            *pfAngle = 270;
            if ( ((nAnimationCount >= 0) && (nAnimationCount <= 3)) || ((nAnimationCount >= 8) && (nAnimationCount <=11)) ) {
                uTextureIndex = 73;     // Mine links
            } else {
                uTextureIndex = 74;     // Mine links an
            }
            break;
        case( EMERALD_MINE_LEFT):
            *pfAngle = 0;
            if ( ((nAnimationCount >= 0) && (nAnimationCount <= 3)) || ((nAnimationCount >= 8) && (nAnimationCount <=11)) ) {
                uTextureIndex = 73;     // Mine links
            } else {
                uTextureIndex = 74;     // Mine links an
            }
            break;
        case (EMERALD_BEETLE_UP):
            *pfAngle = 90;
            uTextureIndex = 75 + nAnimationCount % 8;     // Käfer links
            break;
        case (EMERALD_BEETLE_RIGHT):
            *pfAngle = 180;
            uTextureIndex = 75 + nAnimationCount % 8;     // Käfer links
            break;
        case (EMERALD_BEETLE_DOWN):
            *pfAngle = 270;
            uTextureIndex = 75 + nAnimationCount % 8;     // Käfer links
            break;
        case (EMERALD_BEETLE_LEFT):
            *pfAngle = 0;
            uTextureIndex = 75 + nAnimationCount % 8;     // Käfer links
            break;
        case (EMERALD_SPACE):
            uTextureIndex = 0;      // Space
            break;
        case (EMERALD_STEEL):
            uTextureIndex = 72;     // Mauer hart
            break;
        case (EMERALD_KEY_RED):
            uTextureIndex = 98;     // roter Schlüssel
            break;
        case (EMERALD_KEY_YELLOW):
            uTextureIndex = 101;    // gelber Schlüssel
            break;
        case (EMERALD_KEY_BLUE):
            uTextureIndex = 100;    // blauer Schlüssel
            break;
        case (EMERALD_KEY_GREEN):
            uTextureIndex = 99;     // grüner Schlüssel
            break;
        case (EMERALD_DOOR_RED):
            uTextureIndex = 94;     // rote Tür
            break;
        case (EMERALD_DOOR_YELLOW):
            uTextureIndex = 97;     // gelbe Tür
            break;
        case (EMERALD_DOOR_BLUE):
            uTextureIndex = 96;     // blaue Tür
            break;
        case (EMERALD_DOOR_GREEN):
            uTextureIndex = 95;     // grüne Tür
            break;
        case (EMERALD_DOOR_RED_WOOD):     // rote Holztür
            uTextureIndex = 462;
            break;
        case (EMERALD_DOOR_YELLOW_WOOD):  // gelbe Holztür
            uTextureIndex = 465;
            break;
        case (EMERALD_DOOR_BLUE_WOOD):    // blaue Holztür
            uTextureIndex = 464;
            break;
        case (EMERALD_DOOR_GREEN_WOOD):   // grüne Holztür
            uTextureIndex = 463;
            break;
        case (EMERALD_MAN):
            uTextureIndex = 102;    // Man
            //uTextureIndex = 119 + nAnimationCount % 8;     // Man runter
            break;
        case (EMERALD_REPLICATOR_RED_TOP_LEFT):
            uTextureIndex = 138;
            break;
        case (EMERALD_REPLICATOR_RED_TOP_MID):
            uTextureIndex = 139;
            break;
        case (EMERALD_REPLICATOR_RED_TOP_RIGHT):
            uTextureIndex = 140;
            break;
        case (EMERALD_REPLICATOR_RED_BOTTOM_LEFT):
            uTextureIndex = 141;
            break;
        case (EMERALD_REPLICATOR_RED_BOTTOM_RIGHT):
            uTextureIndex = 142;
            break;
        case (EMERALD_REPLICATOR_RED_SWITCH):
            uTextureIndex = 154;
            break;
        case (EMERALD_REPLICATOR_YELLOW_TOP_LEFT):
            uTextureIndex = 190;
            break;
        case (EMERALD_REPLICATOR_YELLOW_TOP_MID):
            uTextureIndex = 191;
            break;
        case (EMERALD_REPLICATOR_YELLOW_TOP_RIGHT):
            uTextureIndex = 192;
            break;
        case (EMERALD_REPLICATOR_YELLOW_BOTTOM_LEFT):
            uTextureIndex = 193;
            break;
        case (EMERALD_REPLICATOR_YELLOW_BOTTOM_RIGHT):
            uTextureIndex = 194;
            break;
        case (EMERALD_REPLICATOR_YELLOW_SWITCH):
            uTextureIndex = 206;
            break;
        case (EMERALD_REPLICATOR_GREEN_TOP_LEFT):
            uTextureIndex = 172;
            break;
        case (EMERALD_REPLICATOR_GREEN_TOP_MID):
            uTextureIndex = 173;
            break;
        case (EMERALD_REPLICATOR_GREEN_TOP_RIGHT):
            uTextureIndex = 174;
            break;
        case (EMERALD_REPLICATOR_GREEN_BOTTOM_LEFT):
            uTextureIndex = 175;
            break;
        case (EMERALD_REPLICATOR_GREEN_BOTTOM_RIGHT):
            uTextureIndex = 176;
            break;
        case (EMERALD_REPLICATOR_GREEN_SWITCH):
            uTextureIndex = 188;
            break;
        case (EMERALD_REPLICATOR_BLUE_TOP_LEFT):
            uTextureIndex = 208;
            break;
        case (EMERALD_REPLICATOR_BLUE_TOP_MID):
            uTextureIndex = 209;
            break;
        case (EMERALD_REPLICATOR_BLUE_TOP_RIGHT):
            uTextureIndex = 210;
            break;
        case (EMERALD_REPLICATOR_BLUE_BOTTOM_LEFT):
            uTextureIndex = 211;
            break;
        case (EMERALD_REPLICATOR_BLUE_BOTTOM_RIGHT):
            uTextureIndex = 212;
            break;
        case (EMERALD_REPLICATOR_BLUE_SWITCH):
            uTextureIndex = 224;
            break;
        case (EMERALD_ACIDPOOL_TOP_LEFT):
            uTextureIndex = 257;
            break;
        case (EMERALD_ACIDPOOL_TOP_MID):
            uTextureIndex = 262 + (Playfield.uFrameCounter / 13) % 4;  // Säurebecken, oben mitte (aktives Feld), verlangsamte Animation (13 Frames für eine Animations-Phase)
            break;
        case (EMERALD_ACIDPOOL_TOP_RIGHT):
            uTextureIndex = 259;
            break;
        case (EMERALD_ACIDPOOL_BOTTOM_LEFT):
            uTextureIndex = 258;
            break;
        case (EMERALD_ACIDPOOL_BOTTOM_MID):
            uTextureIndex = 261;
            break;
        case (EMERALD_ACIDPOOL_BOTTOM_RIGHT):
            uTextureIndex = 260;
            break;
        case (EMERALD_SAND):
            uTextureIndex = 171;
            break;
        case (EMERALD_SAND_INVISIBLE):
            uTextureIndex = 798;
            break;
        case (EMERALD_SANDMINE):
            uTextureIndex = 781;
            break;
        case (EMERALD_GRASS):
            uTextureIndex = 765;
            break;
        case (EMERALD_SWAMP):
            uTextureIndex = 266;
            break;
        case (EMERALD_PERL):
            uTextureIndex = 436;
            break;
        case (EMERALD_MEGABOMB):
            uTextureIndex = 524 + ((Playfield.uFrameCounter & 0xFFFFFFFC) >> 2) % 5;
            break;
        case (EMERALD_BOMB):
            uTextureIndex = 271;
            break;
        case (EMERALD_WHEEL):
            uTextureIndex = 137;
            *pfAngle = nAnimationCount * 11;
            break;
        case (EMERALD_DOOR_GREY_RED):            // Für Editor
            uTextureIndex = 289;
            break;
        case (EMERALD_DOOR_GREY_GREEN):         // Für Editor
            uTextureIndex = 290;
            break;
        case (EMERALD_DOOR_GREY_BLUE):         // Für Editor
            uTextureIndex = 291;
            break;
        case (EMERALD_DOOR_GREY_YELLOW):         // Für Editor
            uTextureIndex = 292;
            break;
        case (EMERALD_MAGIC_WALL_STEEL):
            uTextureIndex = 466 + nAnimationCount / 2;
            break;
        case (EMERALD_MAGIC_WALL):
            uTextureIndex = 293 + nAnimationCount / 2;
            break;
        case (EMERALD_WALL_CORNERED):
            uTextureIndex = 316;
            break;
        case (EMERALD_WALL_ROUND):
            uTextureIndex = 317;
            break;
        case (EMERALD_DOOR_END_NOT_READY):
            uTextureIndex = 287;
            break;
        case (EMERALD_DOOR_END_READY):
            uTextureIndex = 319 + ((Playfield.uFrameCounter & 0xFFFFFFF8) >> 3) % 8; // Endtür, blinkend
            break;
        case (EMERALD_DOOR_END_NOT_READY_STEEL):
            uTextureIndex = 509;
            break;
        case (EMERALD_DOOR_END_READY_STEEL):
            uTextureIndex = 510 + ((Playfield.uFrameCounter & 0xFFFFFFF8) >> 3) % 8; // Endtür, blinkend
            break;
        case (EMERALD_WALL_ROUND_PIKE):
            uTextureIndex = 328;
            break;
        case (EMERALD_STEEL_TRASHCAN):
            uTextureIndex = 736;
            break;
        case (EMERALD_STEEL_JOYSTICK):
            uTextureIndex = 737;
            break;
        case (EMERALD_STEEL_EDIT_LEVEL):
            uTextureIndex = 738;
            break;
        case (EMERALD_STEEL_MOVE_LEVEL):
            uTextureIndex = 739;
            break;
        case (EMERALD_STEEL_ADD_LEVELGROUP):
            uTextureIndex = 740;
            break;
        case (EMERALD_STEEL_COPY_LEVEL):
            uTextureIndex = 741;
            break;
        case (EMERALD_STEEL_CLIPBOARD_LEVEL):
            uTextureIndex = 742;
            break;
        case (EMERALD_STEEL_DC3_IMPORT):
            uTextureIndex = 743;
            break;
        case (EMERALD_STEEL_RENAME_LEVELGROUP):
            uTextureIndex = 744;
            break;
        case (EMERALD_STEEL_PASSWORD):
            uTextureIndex = 745;
            break;
        case (EMERALD_CONVEYORBELT_RED):
            uTextureIndex = 799;
            break;
        case (EMERALD_CONVEYORBELT_SWITCH_RED):
            uTextureIndex = 800;
            break;
        case (EMERALD_CONVEYORBELT_GREEN):
            uTextureIndex = 803;
            break;
        case (EMERALD_CONVEYORBELT_SWITCH_GREEN):
            uTextureIndex = 804;
            break;
        case (EMERALD_CONVEYORBELT_BLUE):
            uTextureIndex = 807;
            break;
        case (EMERALD_CONVEYORBELT_SWITCH_BLUE):
            uTextureIndex = 808;
            break;
        case (EMERALD_CONVEYORBELT_YELLOW):
            uTextureIndex = 811;
            break;
        case (EMERALD_CONVEYORBELT_SWITCH_YELLOW):
            uTextureIndex = 812;
            break;
        case (EMERALD_LEVELEDITOR_MESSAGE_1_4):
            uTextureIndex = 815;
            break;
        case (EMERALD_LEVELEDITOR_MESSAGE_2_4):
            uTextureIndex = 816;
            break;
        case (EMERALD_LEVELEDITOR_MESSAGE_3_4):
            uTextureIndex = 817;
            break;
        case (EMERALD_LEVELEDITOR_MESSAGE_4_4):
            uTextureIndex = 818;
            break;
        case (EMERALD_STEEL_GROW_LEFT):
            uTextureIndex = 884;
            break;
        case (EMERALD_STEEL_GROW_RIGHT):
            uTextureIndex = 885;
            break;
        case (EMERALD_STEEL_GROW_UP):
            uTextureIndex = 886;
            break;
        case (EMERALD_STEEL_GROW_DOWN):
            uTextureIndex = 887;
            break;
        case (EMERALD_STEEL_GROW_LEFT_RIGHT):
            uTextureIndex = 888;
            break;
        case (EMERALD_STEEL_GROW_UP_DOWN):
            uTextureIndex = 889;
            break;
        case (EMERALD_STEEL_GROW_ALL):
            uTextureIndex = 890;
            break;
        case (EMERALD_WALL_GROW_LEFT):
            uTextureIndex = 955;
            break;
        case (EMERALD_WALL_GROW_RIGHT):
            uTextureIndex = 956;
            break;
        case (EMERALD_WALL_GROW_UP):
            uTextureIndex = 957;
            break;
        case (EMERALD_WALL_GROW_DOWN):
            uTextureIndex = 958;
            break;
        case (EMERALD_WALL_GROW_LEFT_RIGHT):
            uTextureIndex = 959;
            break;
        case (EMERALD_WALL_GROW_UP_DOWN):
            uTextureIndex = 960;
            break;
        case (EMERALD_WALL_GROW_ALL):
            uTextureIndex = 961;
            break;
        case (EMERALD_TELEPORTER_RED):
            uTextureIndex = 966 + ((Playfield.uFrameCounter & 0xFFFFFFFE) >> 1) % 13; // Teleporter, rot
            break;
        case (EMERALD_TELEPORTER_YELLOW):
            uTextureIndex = 979 + ((Playfield.uFrameCounter & 0xFFFFFFFE) >> 1) % 13; // Teleporter, gelb
            break;
        case (EMERALD_TELEPORTER_GREEN):
            uTextureIndex = 992 + ((Playfield.uFrameCounter & 0xFFFFFFFE) >> 1) % 13; // Teleporter, grün
            break;
        case (EMERALD_TELEPORTER_BLUE):
            uTextureIndex = 1005 + ((Playfield.uFrameCounter & 0xFFFFFFFE) >> 1) % 13; // Teleporter, blau
            break;
        case (EMERALD_SWITCH_REMOTEBOMB_UP):
            uTextureIndex = 1019;
            break;
        case (EMERALD_SWITCH_REMOTEBOMB_DOWN):
            uTextureIndex = 1021;
            break;
        case (EMERALD_SWITCH_REMOTEBOMB_LEFT):
            uTextureIndex = 1023;
            break;
        case (EMERALD_SWITCH_REMOTEBOMB_RIGHT):
            uTextureIndex = 1025;
            break;
        case (EMERALD_SWITCH_REMOTEBOMB_IGNITION):
            uTextureIndex = 1027;
            break;
        case (EMERALD_REMOTEBOMB):
            uTextureIndex = 1028;
            break;
        case (EMERALD_STEEL_MODERN_LEFT_END):
            uTextureIndex = 1029;
            break;
        case (EMERALD_STEEL_MODERN_LEFT_RIGHT):
            uTextureIndex = 1030;
            break;
        case (EMERALD_STEEL_MODERN_RIGHT_END):
            uTextureIndex = 1031;
            break;
        case (EMERALD_STEEL_MODERN_UP_END):
            uTextureIndex = 1032;
            break;
        case (EMERALD_STEEL_MODERN_UP_DOWN):
            uTextureIndex = 1033;
            break;
        case (EMERALD_STEEL_MODERN_DOWN_END):
            uTextureIndex = 1034;
            break;
        case (EMERALD_STEEL_MODERN_MIDDLE):
            uTextureIndex = 1035;
            break;
        default:
            SDL_Log("%s: unknown element: %x     T:%u",__FUNCTION__,uElement,SDL_GetTicks());
            uTextureIndex = 0;     // Space
            break;
    }
    return uTextureIndex;
}
