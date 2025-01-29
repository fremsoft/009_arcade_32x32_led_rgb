#include "defs.h"

extern const int spritePalettaSx[PLAYER_WIDTH][PLAYER_HEIGHT];
extern const int spritePalettaDx[PLAYER_WIDTH][PLAYER_HEIGHT];
extern const int spritePallina[BALL_WIDTH][BALL_HEIGHT];

struct {
  int16_t x_x10;     // per 10
  int16_t y_x10;     // per 10
  int16_t old_x_x10; // per 10
  int16_t old_y_x10; // per 10
  int16_t vel_x_x10; // per 10
  int16_t vel_y_x10; // per 10
  int8_t visibile;
} giocatore_a, giocatore_b, pallina;

/*
   Stato del gioco: 
   1 = attesa che il giocatore si muova per iniziare
   2 = gioco in corso
   3 = pallina fuori dal campo di gioco
*/
int stato;
int giocatoreIniziale;
unsigned long ultimoTempoMs;
int punti_giocatore_1, punti_giocatore_2;


void gameInit() {

  /* posizioni di gioco iniziali */
  giocatore_a.x_x10 = 0;   giocatore_a.y_x10 = 140; /* vel non ci interessa */
  giocatore_b.x_x10 = 310; giocatore_b.y_x10 = 140; /* vel non ci interessa */
  pallina.x_x10 = 150;     pallina.y_x10 = 150;
  pallina.vel_x_x10 = 0; pallina.vel_y_x10 = 0;
  
  giocatore_a.visibile = 1;
  giocatore_b.visibile = 1;
  pallina.visibile = 1;
  
  stato = 0; 
  ultimoTempoMs = 0;
  giocatoreIniziale = 1;  

  punti_giocatore_1 = 0;
  punti_giocatore_2 = 0;
}


void gameRender() {
  /* STATO = 0; inizio partita */
  /* STATO = 1; attesa che il giocatore si muova per iniziare */
  static int stato=0; 
  static int32_t posEncoder=0, oldPosEncoder=0;
  int xSprite, ySprite, xBuffer, yBuffer;
  
  posEncoder = getPosEncoder();
  giocatore_a.y_x10 = constrain( 
    giocatore_a.y_x10 + 5*(posEncoder - oldPosEncoder),
    0, 320 - (10*PLAYER_HEIGHT));
  oldPosEncoder = posEncoder;

  /* movimento dell'avversario */
  // RANDOM 
  //giocatore_b.y_x10 = constrain( giocatore_b.y_x10 + random(9)-4, 0, 310 - 10*PLAYER_HEIGHT);
  // INSEGUITORE
  giocatore_b.y_x10 = constrain(
    giocatore_b.y_x10 + random(2) +
    constrain( (pallina.y_x10 + (10*BALL_HEIGHT)/2) - (giocatore_b.y_x10 + (10*PLAYER_HEIGHT)/2), -PLAYER_SPEED, PLAYER_SPEED),
    0, 320-(10*PLAYER_HEIGHT));  

  /* elaborazione dello stato del gioco */
  switch( stato ) {
    case 1:  /* 1 = attesa che il giocatore si muova per iniziare */
      giocatore_a.visibile = 1;
      giocatore_b.visibile = 1;
      pallina.visibile = 1;
  
      if ( giocatoreIniziale == 1) {
        pallina.x_x10 = PLAYER_WIDTH * 10;
        pallina.y_x10 = giocatore_a.y_x10+10;
        
        if ( giocatore_a.y_x10 != giocatore_a.old_y_x10) {
          /* il giocatore A si è mosso e può iniziare la partita*/

          tone( BUZZER, TONO_BATTUTA_HZ, TONO_BATTUTA_MS );
          
          pallina.vel_x_x10 = 10;
          if ( giocatore_a.y_x10 > giocatore_a.old_y_x10 ) {
            pallina.vel_y_x10 = 10;
          }
          else {
            pallina.vel_y_x10 = -10;
          }
            
          ultimoTempoMs = millis();
          stato = 2; 
        }
      }
      else /* if ( giocatoreIniziale == 2) */ {
        pallina.x_x10 = 310 - (PLAYER_WIDTH+BALL_WIDTH-1)*10;
        pallina.y_x10 = giocatore_b.y_x10+10;
      
        if ( giocatore_b.y_x10 != giocatore_b.old_y_x10) {
          /* il giocatore B si è mosso e può iniziare la partita*/

          tone( BUZZER, TONO_BATTUTA_HZ, TONO_BATTUTA_MS );
          
          pallina.vel_x_x10 = -10;
          if ( giocatore_b.y_x10 > giocatore_b.old_y_x10 ) {
            pallina.vel_y_x10 = 10;
          }
          else {
            pallina.vel_y_x10 = -10;
          }
          
          ultimoTempoMs = millis();
          stato = 2;
        }
      }
      break;  
    
    case 2: /* 2 = gioco in corso */
      /* aspetto che trascorra un certo delta t */
      if ( millis() > (ultimoTempoMs + DELTA_T_GIOCO_MS) ) {
      
        /* movimentazione pallina */
        /* calcolo posizione teorica */
        pallina.x_x10 = pallina.x_x10 + pallina.vel_x_x10;
        pallina.y_x10 = pallina.y_x10 + pallina.vel_y_x10;

        /* gestione colliosioni Y */
        if (pallina.y_x10 >= 320-((BALL_HEIGHT-1)*10)) {
          pallina.vel_y_x10 = -pallina.vel_y_x10;
          pallina.y_x10     = 320-((BALL_HEIGHT-1)*10) + pallina.vel_y_x10;
        }
        else if (pallina.y_x10 < 0) {
          pallina.vel_y_x10 = -pallina.vel_y_x10;
          pallina.y_x10     =  pallina.vel_y_x10;
        }

        /* gestione collisioni X */
        if (pallina.x_x10 >= 320-(BALL_WIDTH*10)) {
          /* devo verificare se c'è la paletta B */
          if (
               ((pallina.y_x10 + (10*BALL_HEIGHT/2)) >= giocatore_b.y_x10)
              &&
               ((pallina.y_x10 + (10*BALL_HEIGHT/2)) < (giocatore_b.y_x10 + 10*PLAYER_HEIGHT))
             )
          {
            /* il giocatore B ha colpito la pallina */
            pallina.vel_y_x10 = 
               map( (giocatore_b.y_x10 + (10*PLAYER_HEIGHT)/2) - (pallina.y_x10 + (10*BALL_HEIGHT)/2),
              (10*PLAYER_HEIGHT)/2, -(10*PLAYER_HEIGHT)/2, -15, 15);
            
            pallina.vel_x_x10 = -10;
            pallina.x_x10     = 320-(BALL_WIDTH*10) + pallina.vel_x_x10;

            tone( BUZZER, TONO_BATTUTA_HZ, TONO_BATTUTA_MS );
          }
          else
          {
            /* il giocatore A ha fatto un punto */
            punti_giocatore_1 ++;
            
            pallina.vel_x_x10 = 0;
            pallina.vel_y_x10 = 0;
            stato = 3;

            tone( BUZZER, TONO_PUNTO_HZ, TONO_PUNTO_MS );
          }
        }
        if (pallina.x_x10 < 10) {
          /* devo verificare se c'è la paletta A */
          if (
               ((pallina.y_x10 + (10*BALL_HEIGHT/2)) >= giocatore_a.y_x10)
              &&
               ((pallina.y_x10 + (10*BALL_HEIGHT/2)) < (giocatore_a.y_x10 + 10*PLAYER_HEIGHT))
             )
          {
            /* il giocatore A ha colpito la pallina */ 
            pallina.vel_y_x10 = 
               map( (giocatore_a.y_x10 + (10*PLAYER_HEIGHT)/2) - (pallina.y_x10 + (10*BALL_HEIGHT)/2),
              (10*PLAYER_HEIGHT)/2, -(10*PLAYER_HEIGHT)/2, -15, 15);
                 
            pallina.vel_x_x10 = 10;
            pallina.x_x10     = 10 + pallina.vel_x_x10;

            tone( BUZZER, TONO_BATTUTA_HZ, TONO_BATTUTA_MS );
          }
          else
          {
            /* il giocatore B ha fatto un punto */
            punti_giocatore_2 ++;
                        
            pallina.vel_x_x10 = 0;
            pallina.vel_y_x10 = 0;
            stato = 3;

            tone( BUZZER, TONO_PUNTO_HZ, TONO_PUNTO_MS );
          }    
        }        
        
        ultimoTempoMs = millis();  
      } 
      break;
    
    case 3: /* 3 = pallina fuori dal campo di gioco */
      if (millis() < (ultimoTempoMs + DELTA_T_PUNTO_MS)) {
        /* lampeggia il pallino */   
        if (millis()%DELTA_T_LAMPEGGIO_MS > (DELTA_T_LAMPEGGIO_MS/2)) {
          if (pallina.visibile != 0) { pallina.visibile = 0; }
          else { pallina.visibile = 1; }
        }
      }
      else {     
      
        if ((punti_giocatore_1 == 10) || (punti_giocatore_2 == 10)) {
          giocatoreIniziale = 1; 
          punti_giocatore_1 = 0;
          punti_giocatore_2 = 0;

          /* posizioni di gioco iniziali */
          giocatore_a.y_x10 = 140; /* vel non ci interessa */
          giocatore_b.y_x10 = 140; /* vel non ci interessa */
          pallina.x_x10 = 150;
        }
        else {
          if (pallina.x_x10 >= 200) { giocatoreIniziale = 1; }  else  { giocatoreIniziale = 2; }
        }
        stato = 1; 

      }   

      

      break;
    
    default: 
      stato = 1; 
      break;
  }
  
  giocatore_a.old_y_x10 = giocatore_a.y_x10;
  giocatore_b.old_y_x10 = giocatore_b.y_x10;


  /* PARTE IL VERO RENDERING */
  /* cancella il buffer */
  memset( ledmap, 0, 32*32/* *3 */);

  /*
  ledmap[2][2] = ( digitalRead ( BUTTON ) ) ? (COLOR_RED) : (COLOR_BLUE);
  ledmap[5][2] = ( digitalRead ( ENC_A  ) ) ? (COLOR_RED) : (COLOR_BLUE);
  ledmap[6][2] = ( digitalRead ( ENC_B  ) ) ? (COLOR_RED) : (COLOR_BLUE);
  */

  /* posizionamento del primo sprite : paletta */
  if (giocatore_a.visibile != 0) {
    for (ySprite=0, yBuffer=(giocatore_a.y_x10 / 10); ySprite<PLAYER_HEIGHT; ) {
      if ( (yBuffer >= 0) && (yBuffer < 32) ) { /* controllo integrita Y */
        for (xSprite=0, xBuffer=(giocatore_a.x_x10 / 10); xSprite<PLAYER_WIDTH; ) {
          if ( (xBuffer >= 0) && (xBuffer < 32) ) { /* controllo integrita X */
            ledmap[xBuffer][yBuffer] = spritePalettaSx[xSprite][ySprite];
          }
          xSprite++; xBuffer++;      
        }
      }
      ySprite++; yBuffer++;
    }
  }
  
  /* posizionamento del primo sprite : paletta */
  if (giocatore_b.visibile != 0) {
    for (ySprite=0, yBuffer=(giocatore_b.y_x10 / 10); ySprite<PLAYER_HEIGHT; ) {
      if ( (yBuffer >= 0) && (yBuffer < 32) ) { /* controllo integrita Y */
        for (xSprite=0, xBuffer=(giocatore_b.x_x10 / 10); xSprite<PLAYER_WIDTH; ) {
          if ( (xBuffer >= 0) && (xBuffer < 32) ) { /* controllo integrita X */
            ledmap[xBuffer][yBuffer] = spritePalettaDx[xSprite][ySprite];
          }
          xSprite++; xBuffer++;      
        }
      }
      ySprite++; yBuffer++;
    }
  }
  
  /* posizionamento terzo sprite : pallina */ 
  if (pallina.visibile != 0) {
    for (ySprite=0, yBuffer=(pallina.y_x10 / 10); ySprite<BALL_HEIGHT; ) {
      if ( (yBuffer >= 0) && (yBuffer < 32) ) { /* controllo integrita Y */
        for (xSprite=0, xBuffer=(pallina.x_x10 / 10); xSprite<BALL_WIDTH; ) {
          if ( (xBuffer >= 0) && (xBuffer < 32) ) { /* controllo integrita X */
            ledmap[xBuffer][yBuffer] = spritePallina[ySprite][xSprite];
          }
          xSprite++; xBuffer++;      
        }
      }
      ySprite++; yBuffer++;
    }
  }

  /* visualizzazione del tabellone */
  for(int p=0; p<punti_giocatore_1; p++) { ledmap[ 2+p][0] = COLOR_RED; }
  for(int p=0; p<punti_giocatore_2; p++) { ledmap[29-p][0] = COLOR_RED; }
  
  /*char stringa[20];
  sprintf( stringa, "   A  PUNTI  B  " );
  lcd_tabellone.setCursor(0, 0);
  lcd_tabellone.print(stringa);
  
  sprintf( stringa, "%5d     %5d ", punti_giocatore_1, punti_giocatore_2 );
  lcd_tabellone.setCursor(0, 1);
  lcd_tabellone.print(stringa);*/
  
}