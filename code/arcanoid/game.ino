#include "defs.h"
#include "charms.h"
#include "schemi.h"

extern const int spriteGiocatore[PLAYER_WIDTH][PLAYER_HEIGHT];
extern const int spriteCharm[CHARMS_WIDTH][CHARMS_HEIGHT];

struct {
  int16_t x_x10;     // per 10
  int16_t y_x10;     // per 10
  int16_t old_x_x10; // per 10
  int16_t old_y_x10; // per 10
  int16_t vel_x_x10; // per 10
  int16_t vel_y_x10; // per 10
  int8_t  visibile;
} giocatore, pallina[3], charm;

/*
   Stato del gioco:
   1 = attesa che il giocatore si muova per iniziare
   2 = gioco in corso
   3 = tutte le palline sono cadute di sotto
   4 = game over
*/
int stato;
int schema_corrente;
uint8_t schema[10][N_BLOCKS_PER_RIGA];
uint8_t tipo_charm;
unsigned long ultimoTempoMs;
int vite_giocatore;
int bg;

void caricaSchema() {
    uint8_t r, c;

    for (r=0; r<10; r++) {
        for (c=0; c<N_BLOCKS_PER_RIGA; c++) {
            schema[r][c] = pgm_read_byte(&schemi[schema_corrente][r][c]);
        }
    }
}

int disegnaSchema() {
    uint8_t r, c;
    uint8_t x, y, x_offset, y_offset, col;
    int n = 0;
    for (r=0; r<10; r++) {
        for (c=0; c<N_BLOCKS_PER_RIGA; c++) {
            col = schema[r][c];
            if (col != 0) {
              n++;
              if ((col & 0xF0) == 0) { col = (col << 4) | col; } 
            
              if ((r&1) == 0) {
                // righe pari
                x_offset = c*BLOCK_WIDTH;
                y_offset = r*BLOCK_HEIGHT + 2;
              }
              else {
                // righe dispari
                x_offset = 2 + c*BLOCK_WIDTH;
                y_offset = r*BLOCK_HEIGHT + 2;
              }
              for (x=0; x<BLOCK_WIDTH; x++) {
                for (y=0; y<BLOCK_HEIGHT; y++) {
                    ledmap[x_offset + x][y_offset + y] = col;
                }
              }
            } 
        }
    }
 
  return n;
}

void gameInit() {

  /* posizione di gioco iniziali */
  giocatore.x_x10 = (320 - (10 * PLAYER_WIDTH)) / 2;
  giocatore.y_x10 = 320 - (10 * PLAYER_HEIGHT); /* vel non ci interessa */
  giocatore.visibile = 1;
  pallina[0].x_x10 = (320 - (10)) / 2;
  pallina[0].y_x10 = (320 - (10 * (1+PLAYER_HEIGHT))); 
  pallina[0].vel_x_x10 = 0;      // vel 0,0 significa che la pallina
  pallina[0].vel_y_x10 = 0;      // e' agganciata alla base
  pallina[0].visibile = 1;
  for (int i = 1; i < 3; i++) {
    pallina[i].visibile = 0;
  }
  charm.visibile = 0;
  tipo_charm = 0;
  
  stato = 0;
  schema_corrente = 0;
  caricaSchema();
  
  ultimoTempoMs = 0;
  vite_giocatore = N_VITE;

}


void gameRender() {
  /* STATO = 0; inizio partita */
  /* STATO = 1; attesa che il giocatore si muova per iniziare */
  static int32_t posEncoder = 0, oldPosEncoder = 0;
  static int32_t tastoPremuto = 0, oldSwitchPremuto = 0;
  int xt, yt, xSprite, ySprite, xBuffer, yBuffer;
  static int contatore_charms = 0;
  static int livello = 1;

  static long int timeoutSleep = 0;

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

  bg = 0;

  /* elaborazione dello stato del gioco */
  switch ( stato ) {
    case 1:  /* 1 = attesa che il giocatore si muova per iniziare */
      livello = 1;

      giocatore.visibile = 1;
      pallina[0].x_x10 = (320 - (10)) / 2;
      pallina[0].y_x10 = (320 - (10 * (1+PLAYER_HEIGHT))); 
      pallina[0].vel_x_x10 = 0;      // vel 0,0 significa che la pallina
      pallina[0].vel_y_x10 = 0;      // e' agganciata alla base
      pallina[0].visibile = 1;
      for (int i = 1; i < 3; i++) {
          pallina[i].visibile = 0;
      }  
      charm.visibile = 0;
      contatore_charms = 0;

      if ( giocatore.x_x10 != giocatore.old_x_x10) {
        /* il giocatore si è mosso e può iniziare la partita*/
        ultimoTempoMs = millis();
        stato = 2;
      }
      break;

    case 2: /* 2 = gioco in corso */

      /* aspetto che trascorra un certo delta t */
      if ( millis() > (ultimoTempoMs + DELTA_T_GIOCO_MS) ) {

        /* gestione palline */
        for (int i = 0; i < 3; i++) {
          if (pallina[i].visibile != 0) {
            // vel 0,0 significa che la pallina
            // e' agganciata alla base
            if ((pallina[0].vel_x_x10 == 0) && (pallina[0].vel_y_x10 == 0)) {
              if (tastoPremuto != 0) {
                tastoPremuto = 0;

                tone( BUZZER, TONO_BATTUTA_HZ, TONO_BATTUTA_MS );
                pallina[i].vel_y_x10 = -10;
                pallina[i].vel_x_x10 = 1;
              }
              else {
                pallina[i].x_x10 = giocatore.x_x10 + (PLAYER_WIDTH*10/2);
                pallina[i].y_x10 = giocatore.y_x10 - 10;              
              }
            }
            else {
              pallina[i].y_x10 += pallina[i].vel_y_x10;
              if (pallina[i].y_x10 <= 20) {
                pallina[i].y_x10 = 20 + (20 - pallina[i].y_x10);
                pallina[i].vel_y_x10 = 0 - pallina[i].vel_y_x10;
              }
              
              if (pallina[i].y_x10 >= 320) {
                pallina[i].visibile = 0;

                if ((pallina[0].visibile == 0) && (pallina[1].visibile == 0) && (pallina[2].visibile == 0)) {
                  // MORTO
                  vite_giocatore --;
                  stato = 3;
                  tone ( BUZZER, TONO_MORTO_HZ, TONO_MORTO_MS );
                }
              }
              else {
                pallina[i].x_x10 += pallina[i].vel_x_x10;
                if (pallina[i].x_x10 <= 0) {
                  pallina[i].x_x10 = 0 - pallina[i].x_x10;
                  pallina[i].vel_x_x10 = 0 - pallina[i].vel_x_x10;
                }
                if ((pallina[i].x_x10 + 10) >= 320) {
                  pallina[i].x_x10 = 320 - (pallina[i].x_x10 - 320);
                  pallina[i].vel_x_x10 = 0 - pallina[i].vel_x_x10;
                }
              }
            }
          }
        }

        /* gestione charms */
        if (charm.visibile != 0) {
          charm.y_x10 += charm.vel_y_x10;
          if (charm.y_x10 >= 320) {
            charm.visibile = 0;
          }
        }

        if (timeoutSleep > 0) {
          timeoutSleep --;
          if (timeoutSleep <= 0) {
            /* se era in sleep, torna alla modalita' normale */


          }
        }

        /* gestione collisioni palline */
        for (int i = 0; i < 3; i++) {
          if (pallina[i].visibile != 0) {
            /* base colpita */
            if ( ((pallina[i].y_x10 + 10) > (320-(10*PLAYER_HEIGHT))) &&
                 ((pallina[i].x_x10 + 10) > (giocatore.x_x10)) &&
                 ((pallina[i].x_x10)      < (giocatore.x_x10+(10*PLAYER_WIDTH))) )
            {
              float gradi = map(
                (2*giocatore.x_x10+(10*PLAYER_WIDTH))/2
                -
                (2*pallina[i].x_x10 + 10)/2, -6*PLAYER_WIDTH, 6*PLAYER_WIDTH, 80, -80);
              pallina[i].vel_y_x10 = -1 -10 * cos(6.28*gradi/360);
              pallina[i].vel_x_x10 =  10 * sin(6.28*gradi/360);
              pallina[i].y_x10 = 320 - 10 - (10*PLAYER_HEIGHT);
 /*        
         Serial.print(gradi);
         Serial.print(" sin:");
         Serial.print(sin(6.28*gradi/360));
         Serial.print(" cos:");
         Serial.println(cos(6.28*gradi/360));
 */              
               tone( BUZZER, TONO_BATTUTA_HZ, TONO_BATTUTA_MS );
               
 //Serial.println("battuta");
            }

            xt = (pallina[i].x_x10 + 5)/10;
            yt = (pallina[i].y_x10 + 5)/10 - 2;
            // cerco il blocco corrispondente nello schema
            yt=yt/2;
            if ((yt&1) == 0) {
              //righe pari
              xt = constrain(xt/3, 0, N_BLOCKS_PER_RIGA-1);
            }
            else {  //righe dipari
              xt = constrain((xt - 2)/3, 0, N_BLOCKS_PER_RIGA-1);
            }
            if (yt < 10) {
              if (schema[yt][xt] != 0) {
                int center_block_x, center_block_y;

                if ((schema[yt][xt] & 0xF0) != 0) {
                  tone( BUZZER, TONO_DIAMOND_HZ, TONO_DIAMOND_MS );
                }
                else {
                  tone( BUZZER, TONO_ESPLOSIONE_HZ, TONO_ESPLOSIONE_MS );
                  if (charm.visibile == 0) { contatore_charms ++; }
                  if (contatore_charms >= ATTESA_CHARMS) {
                    contatore_charms = 0;
                    tipo_charm = pallottoliere_charms[ random(32) % 32];
                    if (tipo_charm != 0) {
                      charm.x_x10 = center_block_x - 10;
                      charm.y_x10 = center_block_y;
                      charm.vel_x_x10 = 0;
                      charm.vel_y_x10 = 4;
                      charm.visibile = tipo_charm;
Serial.print("Nuovo charm ");
Serial.println(tipo_charm);

                    }
                  }
                }
                  
                if ((yt&1) == 0) {
                  //righe pari
                  center_block_x = (10*BLOCK_WIDTH)/2 + 10*BLOCK_WIDTH*xt;
                  center_block_y = 10 + 10*BLOCK_HEIGHT*yt;
                }
                else {
                  //righe dispari
                  center_block_x = 20 + (10*BLOCK_WIDTH)/2 + 10*BLOCK_WIDTH*xt;
                  center_block_y = 10 + 10*BLOCK_HEIGHT*yt;
                }

                if (pallina[i].visibile == 1) {
                  if (abs(pallina[i].vel_y_x10) > abs(pallina[i].vel_x_x10)) {
                    //if ((pallina[i].y_x10 < (center_block_y - 2)) || (pallina[i].y_x10 > (center_block_x + 2))) {
               
                    // colpito blocco da sopra o da sotto
                    pallina[i].vel_y_x10 = 0-pallina[i].vel_y_x10;
                    pallina[i].y_x10 += 2*pallina[i].vel_y_x10;
                  }
                  else {
                    // colpito blocco da sinistra o da destra
                    pallina[i].vel_x_x10 = 0-pallina[i].vel_x_x10;
                    pallina[i].x_x10 += 2*pallina[i].vel_x_x10;
                  }
                }
 /*
 Serial.print("colpito blocco ");
 Serial.print(xt);
 Serial.print(",");
 Serial.print(yt);
 Serial.print("=");
 Serial.println(schema[yt][xt]);
 */
                 schema[yt][xt] = schema[yt][xt] >> 4;
 /*Serial.println(schema[yt][xt]);
 */
              }
            }
          }         
        }

        /* gestione collisioni charm */
        if (charm.visibile != 0) {
          if ( ((charm.y_x10 + (10*CHARMS_HEIGHT)) >= (320 - (PLAYER_HEIGHT * 10))) &&
               ((charm.x_x10 + (10*CHARMS_WIDTH)) >= (giocatore.x_x10)) &&
               ((charm.x_x10) <= (giocatore.x_x10 + (PLAYER_WIDTH * 10))) ) {
            /* preso */

            if ( charm.visibile == TIPO_CHARM_TRIPLE ) {
Serial.println("preso charm triple");              
              for (int i=0; i<3; i++) {
                // cerca visibile
                if (pallina[i].visibile != 0) {
                  for (int j=0; j<3; j++) {
                    pallina[j].x_x10 = pallina[i].x_x10;
                    pallina[j].y_x10 = pallina[i].y_x10;
                    pallina[j].vel_x_x10 = random(20) - 10;
                    pallina[j].vel_y_x10 = -2 -random(10);
                    pallina[j].visibile = pallina[i].visibile;
                  }
                }
              }
            }
            if ( charm.visibile == TIPO_CHARM_ULTRA ) {
Serial.println("preso charm ultra");              
              for (int i=0; i<3; i++) {
                // cerca visibile
                if (pallina[i].visibile != 0) {
                  pallina[i].visibile = 2;
                }
              }
            }
            charm.visibile = 0;
            tone( BUZZER, TONO_CHARMS_HZ, TONO_CHARMS_MS );
          }
        }

        ultimoTempoMs = millis();
      }
      break;

    case 3: /* 3 = morto */

      if (millis() < (ultimoTempoMs + DELTA_T_MORTO_MS)) {
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
        /* posizione di gioco iniziali */
        giocatore.x_x10 = (320 - (10 * PLAYER_WIDTH)) / 2;
        giocatore.y_x10 = 320 - (10 * PLAYER_HEIGHT); /* vel non ci interessa */
        giocatore.visibile = 1;
        pallina[0].x_x10 = (320 - (10)) / 2;
        pallina[0].y_x10 = (320 - (10 * (1+PLAYER_HEIGHT))); 
        pallina[0].vel_x_x10 = 0;      // vel 0,0 significa che la pallina
        pallina[0].vel_y_x10 = 0;      // e' agganciata alla base
        pallina[0].visibile = 1;
        for (int i = 1; i < 3; i++) {
          pallina[i].visibile = 0;
        }  
        charm.visibile = 0;

        if (vite_giocatore <= 0) {
          ultimoTempoMs = 0;
          vite_giocatore = N_VITE;

          stato = 4;
          ultimoTempoMs = millis();
        }
        else {
          giocatore.visibile = 1;
          stato = 2;
        }
      }
      break;

    case 4: /* 4 = game over */

      if ((millis() - ultimoTempoMs) > DELTA_T_GAME_OVER) {
        stato = 1;
        
        livello = 1;
        schema_corrente = 1;
        caricaSchema();        
      }
      break;

    case 5: /* 5 = next level */
      if ((millis() - ultimoTempoMs) > DELTA_T_LIVELLO_MS) {
        stato = 2;
        schema_corrente = ((schema_corrente+1) % 10);
        caricaSchema();

        /* posizione di gioco iniziali */
        giocatore.x_x10 = (320 - (10 * PLAYER_WIDTH)) / 2;
        giocatore.y_x10 = 320 - (10 * PLAYER_HEIGHT); /* vel non ci interessa */
        giocatore.visibile = 1;
        for (int i=0; i<3; i++) {
          pallina[i].x_x10 = (320 - (10)) / 2;
          pallina[i].y_x10 = (320 - (10 * (1+PLAYER_HEIGHT))); 
          pallina[i].vel_x_x10 = 0;      // vel 0,0 significa che la pallina
          pallina[i].vel_y_x10 = 0;      // e' agganciata alla base
          pallina[i].visibile = (i==0)?(1):(0);
        }
      }
      break; 
        
    default:
      stato = 1;
      break;
  }

  giocatore.old_x_x10 = giocatore.x_x10;
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
 
  /* posizionamento dei blocchi */
  if (disegnaSchema() == 0) {
    if ((stato == 2) && (livello < 20)) { livello ++; }
    stato = 5; /* next level */
  }  

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

  /* posizionamento delle palline */
  for (int i = 0; i < 3; i++) {
    if (pallina[i].visibile != 0) {
      yBuffer = (pallina[i].y_x10 / 10); 
      if ( (yBuffer >= 0) && (yBuffer < 32) ) { /* controllo integrita Y */
        xBuffer = (pallina[i].x_x10 / 10);
        if ( (xBuffer >= 0) && (xBuffer < 32) ) { /* controllo integrita X */
          if (pallina[i].visibile == 1) {
            ledmap[xBuffer][yBuffer] = COLOR_YELLOW;
          }
          else {
            ledmap[xBuffer][yBuffer] = COLOR_RED;
          }
        }
      }
    }
  }

  /* posizionamento dei charms */
  if (charm.visibile != 0) {
    for (ySprite = 0, yBuffer = (charm.y_x10 / 10); ySprite < CHARMS_HEIGHT; ) {
      if ( (yBuffer >= 0) && (yBuffer < 32) ) { /* controllo integrita Y */
        for (xSprite = 0, xBuffer = (charm.x_x10 / 10); xSprite < CHARMS_WIDTH; ) {
          if ( (xBuffer >= 0) && (xBuffer < 32) ) { /* controllo integrita X */
            if (tipo_charm == TIPO_CHARM_TRIPLE) {
              ledmap[xBuffer][yBuffer] = COLOR_TRIPLE;
            }
            else
            if (tipo_charm == TIPO_CHARM_TRIPLE) {
              ledmap[xBuffer][yBuffer] = COLOR_TRIPLE;
            }
            else
            if (tipo_charm == TIPO_CHARM_ULTRA) {
              ledmap[xBuffer][yBuffer] = COLOR_ULTRA;
            }
            else
            if (tipo_charm == TIPO_CHARM_LONG) {
              ledmap[xBuffer][yBuffer] = COLOR_LONG;
            }
            else {
              ledmap[xBuffer][yBuffer] = tipo_charm;
            }
            
          }
          xSprite++; xBuffer++;
        }
      }
      ySprite++; yBuffer++;
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
