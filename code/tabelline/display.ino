#include "defs.h"
#include "backgrounds.h"

extern const int _0[7];
extern const int _1[7];
extern const int _2[7];
extern const int _3[7];
extern const int _4[7];
extern const int _5[7];
extern const int _6[7];
extern const int _7[7];
extern const int _8[7];
extern const int _9[7];
extern const int _x[7];

uint8_t ledmap[32][32]/*[3]*/;

void displayInit() {

  pinMode(R1, OUTPUT);
  pinMode(G1, OUTPUT);
  pinMode(B1, OUTPUT);
  pinMode(R2, OUTPUT);
  pinMode(G2, OUTPUT);
  pinMode(B2, OUTPUT);
  pinMode(ADDR_A, OUTPUT);
  pinMode(ADDR_B, OUTPUT);
  pinMode(ADDR_C, OUTPUT);
  pinMode(ADDR_D, OUTPUT);
  pinMode(SCK, OUTPUT);
  pinMode(LE, OUTPUT);
  pinMode(OEn, OUTPUT);

  digitalWrite(SCK, 0);
  digitalWrite(LE, 0);
  digitalWrite(OEn, 1);

  memset( ledmap, 0, 32*32/* *3 */);

}

void displayDrawLine(uint8_t line) {
 
  // caricamento bit riga
  for (int i = 0; i < 32; i++) {

    digitalWrite(R1, ledmap[i][line]&1/*[0]*/);
    digitalWrite(G1, ledmap[i][line]&2/*[1]*/);
    digitalWrite(B1, ledmap[i][line]&4/*[2]*/);
    
    digitalWrite(R2, ledmap[i][line+16]&1/*[0]*/);
    digitalWrite(G2, ledmap[i][line+16]&2/*[1]*/);
    digitalWrite(B2, ledmap[i][line+16]&4/*[2]*/);
    
    digitalWrite(SCK, 1);
    
    digitalWrite(SCK, 0);

  }

  // spengo display
  digitalWrite(OEn, 1);

  // carico bit su Latch uscita
  digitalWrite(LE, 1);

  // seleziono linea
  digitalWrite(ADDR_A, bitRead(line, 0));
  digitalWrite(ADDR_B, bitRead(line, 1));
  digitalWrite(ADDR_C, bitRead(line, 2));
  digitalWrite(ADDR_D, bitRead(line, 3));

  // ACCENDO display
  digitalWrite(OEn, 0);

  // DISATTIVO LATCH
  digitalWrite(LE, 0);

}

  
/*void displaySetPixel(uint8_t x, uint8_t y, uint16_t c565 ) { */
void displaySetPixel(uint8_t x, uint8_t y, uint8_t rgb) {
  ledmap[x][y] = rgb;
  /*
  uint8_t r, g, b;
  r = (c565 >> 8) & 0xF8;
  g = (c565 >> 3) & 0xFC;
  b = (c565 << 3) & 0xF8;
  
  ledmap[x][y][0] = r;
  ledmap[x][y][1] = g;
  ledmap[x][y][2] = b;
  */
}

uint16_t displayGetPixel(uint8_t x, uint8_t y) {
/*  
  uint8_t r, g, b;
  r = ledmap[x][y][0];
  g = ledmap[x][y][1];
  b = ledmap[x][y][2];

  return ((r << 8) & 0xF800) | ((g << 5) & 0x07E0) | (b & 0x001F);
*/
  return ledmap[x][y];
}

void displayDrawBackground(int bg) {
  uint8_t i, j;
  if (bg == 1) {
    for (i=0; i<32; i++) {
      for (j=0; j<32; j++) {
        ledmap[j][i] = pgm_read_byte(&move_to_play[i][j]);
      }
    }
  }
  else if (bg == 2) {
    for (i=0; i<32; i++) {
      for (j=0; j<32; j++) {
        ledmap[j][i] = pgm_read_byte(&game_over[i][j]);
      }
    }
  }
  else if (bg == 3) {
    for (i=0; i<32; i++) {
      for (j=0; j<32; j++) {
        ledmap[j][i] = pgm_read_byte(&winner[i][j]);
      }
    }
  }
  else {
    memset( ledmap, 0, 32*32/* *3 */);
  /*  for (i=0; i<32; i++) {
      for (j=0; j<32; j++) {
        ledmap[j][i] = pgm_read_byte(&background[i][j]);
      }
    }*/
  }
}

void displayDigit(uint8_t startx, uint8_t starty, uint8_t digit, uint8_t rgb) {
  uint8_t x, y, row;

  for (y=0; y<7; y++) {
    switch (digit) {
      case 0 : row = _0[y]; break;
      case 1 : row = _1[y]; break;
      case 2 : row = _2[y]; break;
      case 3 : row = _3[y]; break;
      case 4 : row = _4[y]; break;
      case 5 : row = _5[y]; break;
      case 6 : row = _6[y]; break;
      case 7 : row = _7[y]; break;
      case 8 : row = _8[y]; break;
      case 9 : row = _9[y]; break;
      case 'x' : row = _x[y]; break;
      default : row = 0; break; 
    }
    for (x=0; x<5; x++) {
      if ((row & 0x10) != 0) {
        displaySetPixel(startx + x, starty + y, rgb);
      }
      row = row << 1;
    }
  }
}

// dura 1ms per singola riga (coppia di righe)
// significa che un refresh di pagina completo impiega 16mS
// per un totale di 62 Frame/sec
void displayRefresh(void) {
  static uint8_t line=0;

  displayDrawLine(line);
  //delay(1);

  line++;

  // una volta ogni schermata completa esegue il rendering
  if (line >= 16) {
    gameRender();
    line = 0;
  }
}
