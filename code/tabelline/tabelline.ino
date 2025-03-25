/* 
 * Questo programma per Arduino UNO serve a realizzare
 * il gioco delle TABELLINE, utilizzando un solo encoder
 * per selezionare la risposta esatta!
 * 
 * Utilizzando come schermo un display DOT-MATRIX  
 * RGB 32x32 a LED.
 * 
 * Si tratta di un videogame didattico per imparare le
 * tabelline
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
