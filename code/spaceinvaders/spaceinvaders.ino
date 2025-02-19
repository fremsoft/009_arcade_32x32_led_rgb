/* 
 * Questo programma per Arduino UNO serve a realizzare
 * il gioco di SPACE INVADERS, leggermente rivisitato,
 * utilizzando un solo encoder per muovere la astronave 
 * a destra e a sinistra, e il button dello stesso encoder
 * per sparare max 3 spari!
 * 
 * Utilizzando come schermo un display DOT-MATRIX  RGB 32x32
 * a LED:  https://s.click.aliexpress.com/e/_ooQXSOl
 * 
 * Si tratta di un videogame dimostrativo creato durante 
 * il Nerd Show del 1, 2 febbraio 2025
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