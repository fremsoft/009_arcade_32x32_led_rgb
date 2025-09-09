/*
   Definizione degli sprites 
   
   il giocatore è rappresentato da un triangolino 3x2 
   si muove sulla riga 31-h (in fondo) ed è vincolato
   a muoversi da x=0 a x=31-w
   
   i nemici  sono alieni da 3x3 che si muovono a destra 
   e sinistra e ogni tanto sparano bombe che cadono a 
   terra

   gli spari sono led 1x1.

   Quando un nemico viene colpito si distrugge

   Quando il giocatore viene colpito muore. 
   Il giocatore ha n vite

*/

#include "defs.h"

const int spriteGiocatore[PLAYER_WIDTH][PLAYER_HEIGHT] = { { COLOR_RED, COLOR_RED }, { COLOR_WHITE, COLOR_WHITE }, { COLOR_WHITE, COLOR_WHITE }, { COLOR_WHITE, COLOR_WHITE }, { COLOR_WHITE, COLOR_WHITE }, { COLOR_WHITE, COLOR_WHITE }, { COLOR_RED, COLOR_RED } };
