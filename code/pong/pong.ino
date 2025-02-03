/* 
 * Questo programma per Arduino UNO serve a realizzare
 * il gioco del PONG, utilizzando un solo Joystick analogico
 * per muovere la paletta di sinistra, mentre la paletta
 * di destra (l'avversario) la muove Arduino!
 * 
 * Utilizzando come schermo un display DOT-MATRIX  RGB 32x32
 * a LED:  https://s.click.aliexpress.com/e/_ooQXSOl
 * 
 * Si tratta di un videogame dimostrativo da presentare al
 * Nerd Show del 1,2 febrbaio 2025
 * 
 * Visualizza l'esperienza di laboratorio completa: 
 * 1) https://youtube.com/live/A-FgUqgYTtE
 * 2) https://youtube.com/live/IezOPdaz0oE
 * 
 */

#include "defs.h"

void setup() {

  Serial.begin(115200);

  setupEncoder();
  gameInit();
  displayInit();

  /* TEST RGB565
  displaySetPixel(  5,  2, 0xF800 );
  displaySetPixel( 10,  3, 0x07E0 );
  displaySetPixel( 22, 22, 0x001F );
  */
  
  /* TEST RGB8
  displaySetPixel(  0,  0, COLOR_WHITE );
  displaySetPixel(  0,  1, COLOR_RED );
  displaySetPixel(  0,  2, COLOR_GREEN );
  displaySetPixel(  0,  3, COLOR_BLUE );
  displaySetPixel(  0,  4, COLOR_BLACK );
  displaySetPixel(  0,  5, COLOR_MAGENTA );
  displaySetPixel(  0,  6, COLOR_CYAN );
  displaySetPixel(  0,  7, COLOR_YELLOW );
  */

}

void loop() {

  displayRefresh();   // dura 1ms per singola riga (coppia di righe)



}