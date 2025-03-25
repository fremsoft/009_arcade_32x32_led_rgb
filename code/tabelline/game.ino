#include "defs.h"

/*
   Stato del gioco:
   1 = attesa che il giocatore si muova per iniziare
   2 = gioco in corso
   3 = il giocatore ha sbagliato    
   4 = game over
   5 = vittoria
*/
int stato;
unsigned long ultimoTempoMs;
int vite_giocatore, punteggio;
int bg;
int displayVisibile;
int fattore1, fattore2, risultato1, risultato2;

void gameInit() {

  stato = 0;
  ultimoTempoMs = 0;
  vite_giocatore = N_VITE;
  punteggio = 0;
  fattore1 = fattore2 = -1;
  randomSeed( analogRead(A5));
  
}


void gameRender() {
  /* STATO = 0; inizio partita */
  /* STATO = 1; attesa che il giocatore si muova per iniziare */
  static int32_t posEncoder = 0, oldPosEncoder = 0;
  char str[128];
  uint8_t digit;
  
  posEncoder = getPosEncoder();
  //oldPosEncoder = posEncoder;

  bg = 0;

  /* elaborazione dello stato del gioco */
  switch ( stato ) {
    case 1:  /* 1 = attesa che il giocatore si muova per iniziare */
      displayVisibile = 1;
      vite_giocatore = N_VITE;
      punteggio = 0;
      fattore1 = fattore2 = -1;
      
      if ( posEncoder != oldPosEncoder) {
        /* il giocatore si è mosso e può iniziare la partita*/
        tone( BUZZER, TONO_OK_HZ, TONO_OK_MS );

        ultimoTempoMs = millis();
        stato = 2;
      }
      break;

    case 2: /* 2 = gioco in corso */
      if ((fattore1 == -1) && (fattore2 == -1)) {
        // devo pescare due numeri casuali
        fattore1 = random(10);
        fattore2 = random(10);
        
        if (random(2) == 1) {
          risultato1 = fattore1 * fattore2;
          do {
            risultato2 = (random(3) - 1 + fattore1) * (random(3) - 1 + fattore2);
          } while ((risultato1 == risultato2) || (risultato2 <= 0) || (risultato2 >= 100));
        }
        else {
          risultato2 = fattore1 * fattore2;
          do {
            risultato1 = (random(3) - 1 + fattore1) * (random(3) - 1 + fattore2);
          } while ((risultato1 == risultato2) || (risultato1 <= 0) || (risultato1 >= 100));
        }

        delay(100);
        oldPosEncoder = posEncoder = 0; setPosEncoder(0);
        displayVisibile = 1;
        
//        sprintf( str, "Fattore 1: %d, Fattore 2: %d, Risultato 1: %d, Risultato 2: %d", fattore1, fattore2, risultato1, risultato2 );
//        Serial.println( str );     
      }
      else {
        if ((posEncoder - oldPosEncoder) > SENSIBILITA_ROTELLA) {
          // e' stato selezionato il numero di destra
          if ((fattore1 * fattore2) == risultato2) {
            tone( BUZZER, TONO_OK_HZ, TONO_OK_MS );
            punteggio++;
            fattore1 = fattore2 = -1;
            displayVisibile = 0;

            if (punteggio >= PUNTEGGIO_VITTORIA) {
              tone( BUZZER, TONO_VITTORIA_HZ, TONO_VITTORIA_MS );
              stato = 5;
            }
          } else {
            tone( BUZZER, TONO_ERRORE_HZ, TONO_ERRORE_MS );
            vite_giocatore--;
            stato = 3; // sbagliato risposta
          }
        }
        else if ((posEncoder - oldPosEncoder) < -SENSIBILITA_ROTELLA) {
          // e' stato selezionato il numero di sinistra
          if ((fattore1 * fattore2) == risultato1) {
            tone( BUZZER, TONO_OK_HZ, TONO_OK_MS );
            punteggio++;
            fattore1 = fattore2 = -1;
            displayVisibile = 0;

            if (punteggio >= PUNTEGGIO_VITTORIA) {
              tone( BUZZER, TONO_VITTORIA_HZ, TONO_VITTORIA_MS );
              stato = 5;
            }
          } else {
            tone( BUZZER, TONO_ERRORE_HZ, TONO_ERRORE_MS );
            vite_giocatore--;
            stato = 3; // sbagliato risposta
          }
        }
      }
      ultimoTempoMs = millis();
      break;

    case 3: /* 3 = sbagliato la risposta */
      if (millis() < (ultimoTempoMs + DELTA_T_SBAGLIATO_MS)) {
        /* lampeggia il display */
        if (millis() % 600 > 300) {
          if (displayVisibile != 0) {
            displayVisibile = 0;
          }
          else {
            displayVisibile = 1;
          }
        }
      }
      else {
        if (vite_giocatore <= 0) {
          /* posizione di gioco iniziali */
          stato = 4;   /* GAME OVER */
          ultimoTempoMs = millis();
        }
        else {
          fattore1 = fattore2 = -1;
          stato = 2;
        }
 
        displayVisibile = 1;
      }
      break;

    case 4: /* 4 = game over */
      if ((millis() - ultimoTempoMs) > DELTA_T_GAME_OVER) {
        stato = 1;
        oldPosEncoder = posEncoder;
      }
      break;

    case 5: /* 5 = vittoria */
      if ((millis() - ultimoTempoMs) > DELTA_T_GAME_OVER) {
        stato = 1;
        oldPosEncoder = posEncoder;
      }
      break;

    default:
      stato = 1;
      displayVisibile = 1;
      oldPosEncoder = posEncoder;
      break;
  }

  
  /* PARTE IL VERO RENDERING */
  /* cancella il buffer */
  if (stato == 1) { bg = 1; }
  if (stato == 4) { bg = 2; }
  if (stato == 5) { bg = 3; }
  displayDrawBackground( bg );

  /* posizionamento del primo sprite : player */
  if ((stato == 2) || (stato == 3)) {
    if (displayVisibile) {
      // fattore 1 
      displayDigit(7, 5, fattore1, COLOR_GREEN);

      // simbolo del X
      displayDigit(14, 5, 'x', COLOR_RED);
      
      // fattore 2 
      displayDigit(21, 5, fattore2, COLOR_BLUE);

      // risultato1
      digit = risultato1 / 10;
      if (digit != 0) { displayDigit(2, 20, digit, COLOR_YELLOW); }
      digit = risultato1 % 10;
      displayDigit(8, 20, digit, COLOR_YELLOW);

      // risultato2
      digit = risultato2 / 10;
      if (digit != 0) { 
        displayDigit(19, 20, digit, COLOR_YELLOW);
        digit = risultato2 % 10;
        displayDigit(25, 20, digit, COLOR_YELLOW);
      }
      else {
        digit = risultato2 % 10;
        displayDigit(19, 20, digit, COLOR_YELLOW);
      }
    }
  }

  /* visualizzazione del tabellone */
  for (int v = 0; v < vite_giocatore; v++) {
    ledmap[ 2 + v ][0] = COLOR_RED;
  }
  for (int p = 0; p < punteggio; p++) {
    ledmap[ p ][1] = COLOR_CYAN;
  }
}
