/* 
 * Questo programma per Arduino UNO serve a realizzare
 * il gioco di ARCANOID, utilizzando un solo encoder
 * per muovere la base mobile a destra e a sinistra, 
 * e il button per lanciare la pallina!
 * 
 * Utilizzando come schermo un display 
 * DOT-MATRIX a LED RGB 32x32
 * 
 * Si tratta di un videogame dimostrativo da presentare al
 * Modena Nerd del 6,7 settembre 2025
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

}

void loop() {

  displayRefresh();   // dura 1ms per singola riga (coppia di righe)

}
