#include "EmeraldMine.h"
#include "emerald.h"
#include "magicwall.h"
#include "man.h"
#include "sound.h"

extern PLAYFIELD Playfield;

/*----------------------------------------------------------------------------
Name:           ControlEmerald
------------------------------------------------------------------------------
Beschreibung: Steuert einen Emerald.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
R¸ckgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlEmerald(uint32_t I) {
    uint8_t uFree;  // Richtung, in die Emerald rollen kˆnnte: 0 = kann nicht rollen, 1 = kann links rollen, 2 = kann rechts rollen, 3 = kann links und rechts rollen

    // Doppelte Steuerung vermeiden
    if ((Playfield.pStatusAnimation[I] & 0x00FF0000) == EMERALD_ANIM_AVOID_DOUBLE_CONTROL) {
        Playfield.pStatusAnimation[I] = Playfield.pStatusAnimation[I] & 0xFF00FFFF;
        SDL_Log("%s: ack double control",__FUNCTION__);
        return;
    }
    if ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_SINK_IN_MAGIC_WALL) {
        Playfield.pLevel[I] = EMERALD_SPACE;    // Emerald ist in Magic Wall eingetaucht und wird zu Space
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
        return;
    }
    if ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_BLITZ) {
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
    }
    if (    ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_BORN1) ||
            ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_BORN2) ||
            ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_EMERALD_SHRINK) ) {
        // Emerald kann vom Replikator geboren werden, dann hier nichts machen
        // Emerald kann durch Man "geshrinkt" werden, dann hier auch nichts machen
        return;
    } else if (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_SPACE) {   // Ist nach unten frei?
        // neuen Platz mit ung¸ltigem Element besetzen
        Playfield.pLevel[I + Playfield.uLevel_X_Dimension] = EMERALD_INVALID;
        // Damit ung¸ltiges Feld sp‰ter auf richtiges Element gesetzt werden kann
        Playfield.pStatusAnimation[I + Playfield.uLevel_X_Dimension] = EMERALD_ANIM_DOWN_SELF | EMERALD_EMERALD | EMERALD_ANIM_CLEAN_UP;
        // Aktuelles Element auf Animation "unten"
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_DOWN;
    } else if (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_ACIDPOOL) {   // F‰llt Emerald ins S‰urebecken?
        SDL_Log("Emerald falls in pool");
        Playfield.pLevel[I] = EMERALD_ACIDPOOL_DESTROY;
        Playfield.pStatusAnimation[I] = EMERALD_EMERALD;
        PreparePlaySound(SOUND_POOL_BLUB,I);
        return;
    } else {                            // Unten ist nicht frei
        // Emerald bleibt zun‰chst auf Platz liegen
        if ( (Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_DOWN_SELF) {
            Playfield.pStatusAnimation[I] &= 0x00FFFFFF;
            if (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_MAGIC_WALL) { // Emerald trifft auf Magic wall
                if (Playfield.bMagicWallRunning) {
                    SDL_Log("Emerald hit running magic wall");
                    Playfield.pStatusAnimation[I] = EMERALD_ANIM_SINK_IN_MAGIC_WALL;
                    ElementGoesMagicWall(I,EMERALD_SAPPHIRE);
                } else if (!Playfield.bMagicWallWasOn) {
                    Playfield.pStatusAnimation[I] = EMERALD_ANIM_SINK_IN_MAGIC_WALL;
                    SDL_Log("Emerald start magic wall");
                    Playfield.bMagicWallWasOn = true;
                    Playfield.uTimeMagicWallLeft = Playfield.uTimeMagicWall;
                    Playfield.bMagicWallRunning = true;
                    ElementGoesMagicWall(I,EMERALD_SAPPHIRE);
                } else {
                    SDL_Log("Emerald hit used magic wall");
                    PreparePlaySound(SOUND_PING,I);
                }

            } else {
                PreparePlaySound(SOUND_PING,I);
            }
        }
        Playfield.pStatusAnimation[I] |= EMERALD_ANIM_STAND;
        if ((Playfield.uRollUnderground[Playfield.pLevel[I + Playfield.uLevel_X_Dimension]] & EMERALD_CHECKROLL_EMERALD) != 0) {
            uFree = GetFreeRollDirections(I);
            if (uFree == 1) {   // Emerald kann links rollen
                // neuen Platz mit ung¸ltigem Element besetzen
                Playfield.pLevel[I - 1] = EMERALD_INVALID;
                // Damit ung¸ltiges Feld sp‰ter auf richtiges Element gesetzt werden kann
                Playfield.pStatusAnimation[I - 1] = EMERALD_EMERALD | EMERALD_ANIM_CLEAN_RIGHT;
                // Aktuelles Element auf Animation "links"
                Playfield.pStatusAnimation[I] = EMERALD_ANIM_LEFT;

            } else if (uFree == 2) {    // Emerald kann rechts rollen
                // neuen Platz mit ung¸ltigem Element besetzen
                Playfield.pLevel[I + 1] = EMERALD_INVALID;
                // Damit ung¸ltiges Feld sp‰ter auf richtiges Element gesetzt werden kann
                Playfield.pStatusAnimation[I + 1] = EMERALD_EMERALD | EMERALD_ANIM_CLEAN_LEFT;
                // Aktuelles Element auf Animation "rechts"
                Playfield.pStatusAnimation[I] = EMERALD_ANIM_RIGHT;
            } else if (uFree == 3) {    // Emerald kann in beide Richtungen rollen
                // Hier entscheiden, ob links oder rechts gerollt wird
                if ((rand() & 0x01) == 0) {   // links
                    // neuen Platz mit ung¸ltigem Element besetzen
                    Playfield.pLevel[I - 1] = EMERALD_INVALID;
                    // Damit ung¸ltiges Feld sp‰ter auf richtiges Element gesetzt werden kann
                    Playfield.pStatusAnimation[I - 1] = EMERALD_EMERALD | EMERALD_ANIM_CLEAN_RIGHT;
                    // Aktuelles Element auf Animation "links"
                    Playfield.pStatusAnimation[I] = EMERALD_ANIM_LEFT;
                } else {                    // rechts
                    // neuen Platz mit ung¸ltigem Element besetzen
                    Playfield.pLevel[I + 1] = EMERALD_INVALID;
                    // Damit ung¸ltiges Feld sp‰ter auf richtiges Element gesetzt werden kann
                    Playfield.pStatusAnimation[I + 1] = EMERALD_EMERALD | EMERALD_ANIM_CLEAN_LEFT;
                    // Aktuelles Element auf Animation "rechts"
                    Playfield.pStatusAnimation[I] = EMERALD_ANIM_RIGHT;
                }
            }
        }
    }
}








/*
; by MIK 17.7.1995    V 4.0
; green Emerald-Steuerung

emerald1_steuern:
   cmp byte ptr cs:[di+64],80h ; ist das Feld besetzt ?
   jne green_emerald_down_besetzt      ; springe, wenn besetzt
                                ; hier ist nicht besetzt, emerald1 fÑllt noch
   mov byte ptr cs:[di+64],12h  ; Object an neue Stelle (Laby) setzen
   ; ani_stat wird in aniprog zurÅckgesetzt
   mov byte ptr cs:[si],01001111b ; unsinniger Status, Anim = down
   mov byte ptr cs:[si+64],01000100b  ; Animationsphase=down eintragen
   ret

green_emerald_down_besetzt:
   cmp byte ptr cs:[si],00000100b  ; Status=4   emerald1 fÑllt auf etwas
   jne emerald1_ist_ruhig
   mov al,cs:[di+64]                  ; al = getroffenes Object
   cmp al,40h
   jne no_green_emerald_hit9
   call green_emerald_hit_man
   ret
no_green_emerald_hit9:
   cmp al,92h
   jne no_green_emerald_hit10
   call e1_hit_bluewall
   ret
no_green_emerald_hit10:
   cmp al,0a6h
   jne no_green_emerald_hit11
   call e1_hit_bluewall_on
   ret
no_green_emerald_hit11:
   or playingsample,0000000000000001b           ; Voice 1
   mov byte ptr cs:[si],0       ; nicht Besonderes getroffen
   ret

emerald1_ist_ruhig:          ; d.h.  emerald1 liegt auf Etwas = AH
   mov ah,cs:[di+64]
   cmp ah,82h                ; liegt green_emerald auf Sand ?
   jne green_emerald_nicht_auf_sand
   mov byte ptr cs:[si],0b ; ruhiger emerald1 braucht nicht animiert werden
   ret
green_emerald_nicht_auf_sand:
   or ah,ah                  ; liegt green_emerald auf Stein ?
   je green_emerald_could_roll
   cmp ah,12h                ; liegt green_emerald auf green Emerald ?
   je green_emerald_could_roll
   cmp ah,25h                ; liegt green_emerald auf Nut ?
   je green_emerald_could_roll
   cmp ah,2                  ; liegt green_emerald auf blue Emerald ?
   je green_emerald_could_roll
   cmp ah,10h                ; liegt green_emerald auf Bomb ?
   je green_emerald_could_roll
   cmp ah,51h                ; liegt green_emerald auf laufendem Rad ?
   je green_emerald_could_roll
   cmp ah,81h                ; liegt green_emerald auf weich Wall rund ?
   je green_emerald_could_roll
   cmp ah,85h                ; liegt green_emerald auf redkey ?
   je green_emerald_could_roll
   cmp ah,86h                ; liegt green_emerald auf yellowkey ?
   je green_emerald_could_roll
   cmp ah,87h                ; liegt green_emerald auf bluekey ?
   je green_emerald_could_roll
   cmp ah,88h                ; liegt green_emerald auf greenkey ?
   je green_emerald_could_roll
   cmp ah,93h                ; liegt green_emerald auf Rad (stehend) ?
   je green_emerald_could_roll
   cmp ah,95h                ; liegt green_emerald auf linken Teichrand ?
   je green_emerald_could_roll
   cmp ah,96h                ; liegt green_emerald auf rechten Teichrand ?
   je green_emerald_could_roll
   cmp ah,84h                ; liegt green_emerald_auf weich Wall eckig ?
   je green_emerald_could_roll
   cmp ah,83h                ; liegt green_emerald_auf hard Wall ?
   je green_emerald_could_roll
   cmp ah,4eh                ; liegt green_emerald_auf unsichtb. Wall ?
   je green_emerald_could_roll

   mov byte ptr cs:[si],0  ; ruhiger emerald1 braucht nicht animiert werden
   ret

green_emerald_could_roll:
   xor ah,ah
   cmp byte ptr cs:[di+65],80h
   jne right_no_free_for_green_emerald
   cmp byte ptr cs:[di+1],80h
   jne right_no_free_for_green_emerald
   mov ah,1                        ; rechts frei
right_no_free_for_green_emerald:
   cmp byte ptr cs:[di+63],80h
   jne left_no_free_for_green_emerald
   cmp byte ptr cs:[di-1],80h      ; kann emerald1 links runter rollen ?
   jne left_no_free_for_green_emerald
   or ah,2
left_no_free_for_green_emerald:
   cmp ah,3                        ; ist links und rechts frei ?
   jne e1_no_links_und_rechts_free
   lea bx,nut_rnd
   inc rnd_counter
   and rnd_counter,0000000001111111b
   add bx,rnd_counter
   mov ah,cs:[bx]                  ; Zufallszahl 1 oder 3
   cmp ah,1                        ; nach links ?
   je  green_emerald_rolls_left
   jmp green_emerald_rolls_right

e1_no_links_und_rechts_free:
   cmp ah,1                        ; kann emerald1 rechts runter rollen ?
   jne no_green_emerald_roll_right
green_emerald_rolls_right:
   cmp byte ptr cs:[si],0
   jne no_green_emerald_roll
;   mov byte ptr cs:[di],0ffh         ; ungÅltige Stelle setzen
   mov byte ptr cs:[di+1],12h       ; Object an neue Stelle (Laby) setzen
   mov byte ptr cs:[si],00111111b ; unsinnger Status, Anim = rechts
   mov byte ptr cs:[si+1],00110011b  ; Animationsphase=rechts eintragen
   ret
no_green_emerald_roll_right:
   cmp ah,2                        ; kann emerald1 links runter rollen ?
   jne no_green_emerald_roll
green_emerald_rolls_left:
   cmp byte ptr cs:[si],0
   jne no_green_emerald_roll
;   mov byte ptr cs:[di],0ffh         ; ungÅltige Stelle setzen
   mov byte ptr cs:[di-1],12h       ; Object an neue Stelle (Laby) setzen
   mov byte ptr cs:[si],00011111b ; unsinnger Status , Anim = links
   mov byte ptr cs:[si-1],00010001b  ; Animationsphase=links eintragen
   ret
no_green_emerald_roll:
   mov byte ptr cs:[si],0 ; ruhiger emerald1 braucht nicht animiert werden
   ret


e1_hit_bluewall:
cmp bluewallflag,1
je e1_e3_wandlung
cmp bluewalltime,0       ;bluewallflag =0
je e1_wall_end           ;bluewall lief schon mal
   mov bluewallflag,1
   push di
   push si
   push cx

   lea di,labyrinth
   lea si,stat_anim
   add di,65
   add si,65
   mov cx,1919
b0__in__b1:
      cmp byte ptr cs:[di],092h  ; bluewall = off ?
      jne no__b0
      mov byte ptr cs:[di],0a6h  ; blueall in on !
      mov byte ptr cs:[si],00001111b
no__b0:
  inc di
  inc si
  loop b0__in__b1
  pop cx
  pop si
  pop di
e1_e3_wandlung:
   or playingsample,0000000010000000b
   mov byte ptr cs:[di],0a9h     ; E1 in E1_goes verwandeln
   mov byte ptr cs:[si],00001111b
   cmp byte ptr cs:[di+128],80h  ; ist unter BLUE_WALL frei ?
   jne fuer_e1_nicht_frei_0
      mov byte ptr cs:[di+128],0aah ; E3 comes
      mov byte ptr cs:[si+128],00001111b
fuer_e1_nicht_frei_0:
   ret

e1_wall_end:
   mov byte ptr cs:[si],0       ; E1 trifft off BLUE_WALL
   or playingsample,0000000000000001b
ret

e1_hit_bluewall_on:
   or playingsample,0000000010000000b
   mov byte ptr cs:[di],0a9h     ; E1 in E1_goes verwandeln
   mov byte ptr cs:[si],00001111b
   cmp byte ptr cs:[di+128],80h  ; ist unter BLUE_WALL frei ?
   jne fuer_e1_nicht_frei_1
      mov byte ptr cs:[di+128],0aah ; E3 comes
      mov byte ptr cs:[si+128],00001111b
fuer_e1_nicht_frei_1:
   ret



green_emerald_hit_man:
mov al,00001111b                     ; Ani=0 (Stand) und Extrastatus=15
mov ah,07h                           ; Object  Explo
mov cs:[di+64],ah                    ; getroffenes Object in Explo wandeln
mov cs:[si+64],al                 ; Ani=0 (Stand) und Extrastatus=15
mov cs:[di],ah                       ; green_emerald in Explo wandeln
mov cs:[si],al                    ; Ani=0 (Stand) und Extrastatus=15
mov mancontrol,0                     ; Man bewegt sich nun nie mehr
mov mandead,1                        ; Todesflag setzen
or playingsample,00000000000001000000000000000000b ; Man schreit
ret

green_emerald_hit_bluewall:
ret

*/
