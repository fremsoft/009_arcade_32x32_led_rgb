/*
   Definizione degli sprites 
   
   il giocatore A è composto da 5 led in verticale 
   si muove sulla colonna 0 (a sinistra) ed è vincolato
   a muoversi da y=0 a y=31-h
   
   il giocatore B è composto da 5 led in verticale 
   si muove sulla colonna 31 (a destra) ed è vincolato
   a muoversi da y=0 a y=31-h
   
   la pallina è rappresentata da 2x2 led 
   si muove in tutto lo schermo liberamente e quando 
   incontra un giocatore (una paletta) deve rimbalzare, 
   se incontra il fondale termina la manche

*/

#include "defs.h"

const int spritePalettaSx[PLAYER_WIDTH][PLAYER_HEIGHT] = { { COLOR_GREEN, COLOR_GREEN, COLOR_GREEN, COLOR_GREEN, COLOR_GREEN } };
const int spritePalettaDx[PLAYER_WIDTH][PLAYER_HEIGHT] = { { COLOR_BLUE, COLOR_BLUE, COLOR_BLUE,  COLOR_BLUE,  COLOR_BLUE }  };
//const int spritePallina[BALL_WIDTH][BALL_HEIGHT]={ { COLOR_WHITE, COLOR_WHITE }, { COLOR_WHITE, COLOR_WHITE } };
const int spritePallina[BALL_WIDTH][BALL_HEIGHT]={ { COLOR_BLACK, COLOR_WHITE, COLOR_BLACK }, { COLOR_WHITE, COLOR_BLACK, COLOR_WHITE }, { COLOR_BLACK, COLOR_WHITE, COLOR_BLACK } };

