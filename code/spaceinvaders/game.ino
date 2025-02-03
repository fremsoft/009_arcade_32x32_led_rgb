#include "defs.h"

extern const int spriteGiocatore[PLAYER_WIDTH][PLAYER_HEIGHT];
extern const int spriteNemico1[ENEMY_WIDTH][ENEMY_HEIGHT];
extern const int spriteNemico2[ENEMY_WIDTH][ENEMY_HEIGHT];
extern const int spriteNemico3[ENEMY_WIDTH][ENEMY_HEIGHT];
extern const int spriteBomba[1][1];
extern const int spriteSparo[1][1];

struct {
  int16_t x_x10;     // per 10
  int16_t y_x10;     // per 10
  int16_t old_x_x10; // per 10
  int16_t old_y_x10; // per 10
  int16_t vel_x_x10; // per 10
  int16_t vel_y_x10; // per 10
  int8_t  visibile;
} giocatore, nemici[16], spari[N_MAX_SPARI], bombe[N_MAX_BOMBE];

int16_t passo_nemici_x10; // per 10

/*
   Stato del gioco:
   1 = attesa che il giocatore si muova per iniziare
   2 = gioco in corso
   3 = il giocatore e' stato colpito
   4 = game over
*/
int stato;
int numeroAvversari;
unsigned long ultimoTempoMs;
int vite_giocatore;
int bg;


void gameInit() {

  /* posizione di gioco iniziali */
  giocatore.x_x10 = (320 - (10 * PLAYER_WIDTH)) / 2;
  giocatore.y_x10 = 320 - (10 * PLAYER_HEIGHT); /* vel non ci interessa */
  giocatore.visibile = 1;
  for (int i = 0; i < N_MAX_SPARI; i++) {
    spari[i].visibile = 0;
  }

  for (int i = 0; i < 16; i++) {
    nemici[i].visibile = 0;
    bombe[i].visibile = 0;
  }

  stato = 0;
  ultimoTempoMs = 0;
  vite_giocatore = N_VITE;

}


void gameRender() {
  /* STATO = 0; inizio partita */
  /* STATO = 1; attesa che il giocatore si muova per iniziare */
  static int32_t posEncoder = 0, oldPosEncoder = 0;
  static int32_t tastoPremuto = 0, oldSwitchPremuto = 0;
  int xSprite, ySprite, xBuffer, yBuffer;
  int delta_x_nemici, delta_y_nemici;
  static int timeoutBomba = TIMEOUT_BOMBE;
  static int livello = 1;

  posEncoder = getPosEncoder();
  giocatore.x_x10 = constrain(
                      giocatore.x_x10 + 5 * (posEncoder - oldPosEncoder),
                      0, 320 - (10 * PLAYER_WIDTH));
  oldPosEncoder = posEncoder;

  if ( getSwitchEncoder() != 0) {
    if (oldSwitchPremuto == 0) {
      tastoPremuto = 1;
    }
    oldSwitchPremuto = 1;
  } else {
    oldSwitchPremuto = 0;
  }

  /* movimento dei nemici */
  /*  0  1  2  3  4  5 */
  /* 13              6 */
  /* 12 11 10  9  8  7 */
  switch (passo_nemici_x10 / 10) {
    case 0 :
    case 1 :
    case 2 :
    case 3 :
    case 4 :
    case 5 :
      delta_x_nemici = passo_nemici_x10;
      delta_y_nemici = 0;
      break;
    case 6 :
      delta_x_nemici = 50;
      delta_y_nemici = 10;
      break;
    case 7 :
    case 8 :
    case 9 :
    case 10 :
    case 11 :
    case 12 :
      delta_x_nemici = 120 - passo_nemici_x10;
      delta_y_nemici = 20;
      break;
    default /* 13 */ :
      delta_x_nemici = 0;
      delta_y_nemici = 10;
      break;
  }


  for (int i = 0; i < 16; i++) {
    // posizione base dei nemici
    if (i < 4) {
      nemici[i].x_x10 = i * 70 + delta_x_nemici;
      nemici[i].y_x10 =  30  + delta_y_nemici;
    }
    else if (i < 8) {
      nemici[i].x_x10 = 30 + (i - 4) * 70 + delta_x_nemici;
      nemici[i].y_x10 =  70  + delta_y_nemici;
    }
    else if (i < 12) {
      nemici[i].x_x10 = (i - 8) * 70 + delta_x_nemici;
      nemici[i].y_x10 = 110  + delta_y_nemici;
    }
    else
      /* if (i < 16) */ {
      nemici[i].x_x10 = 30 + (i - 12) * 70 + delta_x_nemici;
      nemici[i].y_x10 = 150  + delta_y_nemici;
    }
  }

  bg = 0;

  /* elaborazione dello stato del gioco */
  switch ( stato ) {
    case 1:  /* 1 = attesa che il giocatore si muova per iniziare */
      livello = 1;

      giocatore.visibile = 1;
      for (int i = 0; i < N_MAX_SPARI; i++) {
        spari[i].visibile = 0;
      }
      for (int i = 0; i < 16; i++) {
        nemici[i].visibile = 1;
        bombe[i].visibile = 0;
      }

      if ( giocatore.x_x10 != giocatore.old_x_x10) {
        /* il giocatore si è mosso e può iniziare la partita*/
        tone( BUZZER, TONO_BATTUTA_HZ, TONO_BATTUTA_MS );

        ultimoTempoMs = millis();
        stato = 2;
      }
      break;

    case 2: /* 2 = gioco in corso */
      /* aspetto che trascorra un certo delta t */
      if ( millis() > (ultimoTempoMs + DELTA_T_GIOCO_MS) ) {

        /* movimentazione dei nemici */
        passo_nemici_x10 = (passo_nemici_x10 + livello) % 140;

        /* gestione spari */
        for (int i = 0; i < N_MAX_SPARI; i++) {
          if (spari[i].visibile != 0) {
            spari[i].y_x10 += spari[i].vel_y_x10;
            if (spari[i].y_x10 <= 0) {
              spari[i].visibile = 0;
            }
          }
        }

        if (tastoPremuto != 0) {
          tastoPremuto = 0;
          /* controlla se ho spari  disponibili */

          int s = -1;
          for (int i = 0; i < N_MAX_SPARI; i++) {
            if (spari[i].visibile == 0) {
              s = i;
            }
          }

          if (s >= 0) {
            spari[s].x_x10 = giocatore.x_x10 + (PLAYER_WIDTH * 10) / 2;
            spari[s].y_x10 = 320 - (PLAYER_HEIGHT * 10);
            spari[s].vel_x_x10 = 0;
            spari[s].vel_y_x10 = -10;
            spari[s].visibile = 1;
          }
        }

        /* gestione bombe */
        for (int i = 0; i < N_MAX_BOMBE; i++) {
          if (bombe[i].visibile != 0) {
            bombe[i].y_x10 += bombe[i].vel_y_x10;
            if (bombe[i].y_x10 >= 320) {
              bombe[i].visibile = 0;
            }
          }
        }

        timeoutBomba --;
        if (timeoutBomba <= 0) {
          timeoutBomba = TIMEOUT_BOMBE / livello;
          /* controlla se ho bombe  disponibili */

          int b = -1;
          for (int i = 0; i < N_MAX_BOMBE; i++) {
            if (bombe[i].visibile == 0) {
              b = i;
            }
          }

          if (b >= 0) {
            int n;
            do {
              n = random(16) % 16;
            } while (nemici[n].visibile == 0);
            bombe[b].x_x10 = nemici[n].x_x10 + (ENEMY_WIDTH * 10) / 2;
            bombe[b].y_x10 = nemici[n].y_x10 + (ENEMY_HEIGHT * 10);
            bombe[b].vel_x_x10 = 0;
            bombe[b].vel_y_x10 = 3;
            bombe[b].visibile = 1;
          }
        }

        /* gestione collisioni spari */
        for (int i = 0; i < N_MAX_SPARI; i++) {
          if (spari[i].visibile != 0) {
            for (int n = 0; n < 16; n++) {
              if (nemici[n].visibile != 0) {
                if ( ((spari[i].x_x10 + 5) >= (nemici[n].x_x10)) &&
                     ((spari[i].x_x10 - 5) <= (nemici[n].x_x10 + (ENEMY_WIDTH * 10))) &&
                     ((spari[i].y_x10 + 5) >= (nemici[n].y_x10)) &&
                     ((spari[i].y_x10 - 5) <= (nemici[n].y_x10 + (ENEMY_HEIGHT * 10))) ) {
                  /* nemico n colpito */
                  spari[i].visibile = 0;
                  nemici[n].visibile = 0;
                  tone( BUZZER, TONO_ESPLOSIONE_HZ, TONO_ESPLOSIONE_MS );

                  /* controlla se li ho colpiti tutti */
                  int n = 0;
                  for (int i = 0; i < 16; i++) {
                    if (nemici[i].visibile != 0) {
                      n++;
                    }
                  }

                  if (n == 0) {
                    /* nuova orda di nemici */
                    for (int i = 0; i < 16; i++) {
                      nemici[i].visibile = 1;
                    }
                    livello++;
                  }
                }
              }
            }
          }
        }

        /* gestione collisioni bombe */
        for (int i = 0; i < N_MAX_BOMBE; i++) {
          if (bombe[i].visibile != 0) {
            if ( ((bombe[i].x_x10 + 5) >= (giocatore.x_x10)) &&
                 ((bombe[i].x_x10 - 5) <= (giocatore.x_x10 + (PLAYER_WIDTH * 10))) &&
                 ((bombe[i].y_x10 + 5) >= (giocatore.y_x10)) &&
                 ((bombe[i].y_x10 - 5) <= (giocatore.y_x10 + (PLAYER_HEIGHT * 10))) ) {
              /* colpito */
              bombe[i].visibile = 0;
              vite_giocatore --;
              stato = 3;
              tone( BUZZER, TONO_ESPLOSIONE_HZ, TONO_ESPLOSIONE_MS );
            }
          }
        }

        ultimoTempoMs = millis();
      }
      break;

    case 3: /* 3 = colpito */

      if (millis() < (ultimoTempoMs + DELTA_T_PUNTO_MS)) {
        /* lampeggia il player */
        if (millis() % DELTA_T_LAMPEGGIO_MS > (DELTA_T_LAMPEGGIO_MS / 2)) {
          if (giocatore.visibile != 0) {
            giocatore.visibile = 0;
          }
          else {
            giocatore.visibile = 1;
          }
        }
      }
      else {

        if (vite_giocatore <= 0) {
          /* posizione di gioco iniziali */
          giocatore.x_x10 = (320 - (10 * PLAYER_WIDTH)) / 2;
          giocatore.y_x10 = 320 - (10 * PLAYER_HEIGHT); /* vel non ci interessa */
          giocatore.visibile = 1;
          for (int i = 0; i < N_MAX_SPARI; i++) {
            spari[i].visibile = 0;
          }

          for (int i = 0; i < 16; i++) {
            nemici[i].visibile = 0;
            bombe[i].visibile = 0;
          }
          
          ultimoTempoMs = 0;
          vite_giocatore = N_VITE;

          stato = 4;
          ultimoTempoMs = millis();
        }
        else {
          for (int i = 0; i < N_MAX_BOMBE; i++) {
            bombe[i].visibile = 0;
          }
          giocatore.visibile = 1;
          stato = 2;
        }
      }
      break;

    case 4: /* 4 = game over */

      if ((millis() - ultimoTempoMs) > DELTA_T_GAME_OVER) {
        stato = 1;
      }
      break;

    default:
      stato = 1;
      giocatore.old_x_x10 = giocatore.x_x10;
      break;
  }

  giocatore.old_y_x10 = giocatore.y_x10;


  /* PARTE IL VERO RENDERING */
  /* cancella il buffer */
  if ((stato == 1) && (vite_giocatore == N_VITE)) {
    bg = 1;
  }
  if (stato == 4) {
    bg = 2;
  }

  displayDrawBackground( bg );

  /* posizionamento del primo sprite : player */
  if (giocatore.visibile != 0) {
    for (ySprite = 0, yBuffer = (giocatore.y_x10 / 10); ySprite < PLAYER_HEIGHT; ) {
      if ( (yBuffer >= 0) && (yBuffer < 32) ) { /* controllo integrita Y */
        for (xSprite = 0, xBuffer = (giocatore.x_x10 / 10); xSprite < PLAYER_WIDTH; ) {
          if ( (xBuffer >= 0) && (xBuffer < 32) ) { /* controllo integrita X */
            ledmap[xBuffer][yBuffer] = spriteGiocatore[xSprite][ySprite];
          }
          xSprite++; xBuffer++;
        }
      }
      ySprite++; yBuffer++;
    }
  }

  /* posizionamento degli spari */
  for (int i = 0; i < N_MAX_SPARI; i++) {
    if (spari[i].visibile != 0) {
      for (ySprite = 0, yBuffer = (spari[i].y_x10 / 10); ySprite < 1; ) {
        if ( (yBuffer >= 0) && (yBuffer < 32) ) { /* controllo integrita Y */
          for (xSprite = 0, xBuffer = (spari[i].x_x10 / 10); xSprite < 1; ) {
            if ( (xBuffer >= 0) && (xBuffer < 32) ) { /* controllo integrita X */
              ledmap[xBuffer][yBuffer] = spriteSparo[xSprite][ySprite];
            }
            xSprite++; xBuffer++;
          }
        }
        ySprite++; yBuffer++;
      }
    }
  }

  /* posizionamento delle bombe */
  for (int i = 0; i < N_MAX_BOMBE; i++) {
    if (bombe[i].visibile != 0) {
      for (ySprite = 0, yBuffer = (bombe[i].y_x10 / 10); ySprite < 1; ) {
        if ( (yBuffer >= 0) && (yBuffer < 32) ) { /* controllo integrita Y */
          for (xSprite = 0, xBuffer = (bombe[i].x_x10 / 10); xSprite < 1; ) {
            if ( (xBuffer >= 0) && (xBuffer < 32) ) { /* controllo integrita X */
              ledmap[xBuffer][yBuffer] = spriteBomba[xSprite][ySprite];
            }
            xSprite++; xBuffer++;
          }
        }
        ySprite++; yBuffer++;
      }
    }
  }

  /* posizionamento dei nemici */
  if (stato > 1) {
    for (int i = 0; i < 16; i++) {
      if (nemici[i].visibile != 0) {
        for (ySprite = 0, yBuffer = (nemici[i].y_x10 / 10); ySprite < ENEMY_HEIGHT; ) {
          if ( (yBuffer >= 0) && (yBuffer < 32) ) { /* controllo integrita Y */
            for (xSprite = 0, xBuffer = (nemici[i].x_x10 / 10); xSprite < ENEMY_WIDTH; ) {
              if ( (xBuffer >= 0) && (xBuffer < 32) ) { /* controllo integrita X */
                switch (livello % 3) {
                  case 1 : ledmap[xBuffer][yBuffer] = spriteNemico1[xSprite][ySprite]; break;
                  case 2 : ledmap[xBuffer][yBuffer] = spriteNemico2[xSprite][ySprite]; break;
                  default : ledmap[xBuffer][yBuffer] = spriteNemico3[xSprite][ySprite]; break;
                }
              }
              xSprite++; xBuffer++;
            }
          }
          ySprite++; yBuffer++;
        }
      }
    }
  }

  /* visualizzazione del tabellone */
  for (int v = 0; v < vite_giocatore; v++) {
    ledmap[ 2 + v ][0] = COLOR_RED;
  }
  for (int l = 0; l < livello; l++) {
    ledmap[ 29 - l ][0] = COLOR_CYAN;
  }
}
