/*
   Definizione degli sprites 
   
   Il giocatore è rappresentato da un triangolino 3x2 
   si muove sulla riga 31-h (in fondo) ed è vincolato
   a muoversi da x=0 a x=31-w
   
   I nemici  sono alieni da 3x3 che si muovono a destra 
   e sinistra e ogni tanto sparano bombe che cadono a 
   terra

   Ad ogni livello cambiano le sembianze dei nemici, 
   ruotando tra tre diversi sprites

   Gli spari sono led 1x1.

   Quando un nemico viene colpito si distrugge

   Quando il giocatore viene colpito perde una vita
    
   Il giocatore ha n vite
*/

#include "defs.h"

const int spriteGiocatore[PLAYER_WIDTH][PLAYER_HEIGHT] = { { COLOR_BLACK, COLOR_BLUE }, { COLOR_BLUE, COLOR_WHITE }, { COLOR_BLACK, COLOR_BLUE } };
const int spriteNemico1[ENEMY_WIDTH][ENEMY_HEIGHT] = { { COLOR_BLACK, COLOR_GREEN, COLOR_YELLOW }, { COLOR_GREEN, COLOR_MAGENTA, COLOR_BLACK }, { COLOR_BLACK, COLOR_GREEN, COLOR_YELLOW } };
const int spriteNemico2[ENEMY_WIDTH][ENEMY_HEIGHT] = { { COLOR_BLACK, COLOR_BLUE, COLOR_BLUE }, { COLOR_BLUE, COLOR_MAGENTA, COLOR_YELLOW }, { COLOR_BLACK, COLOR_BLUE, COLOR_BLUE } };
const int spriteNemico3[ENEMY_WIDTH][ENEMY_HEIGHT] = { { COLOR_WHITE, COLOR_BLACK, COLOR_WHITE }, { COLOR_BLACK, COLOR_MAGENTA, COLOR_BLACK }, { COLOR_WHITE, COLOR_BLACK, COLOR_WHITE } };
const int spriteBomba[1][1]={ { COLOR_MAGENTA } };
const int spriteSparo[1][1]={ { COLOR_WHITE } };
