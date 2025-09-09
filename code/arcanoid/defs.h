#define R1        2
#define G1        3
#define B1        4
#define R2        5
#define G2        6
#define B2        7
#define ADDR_A    8
#define ADDR_B    9
#define ADDR_C   10
#define ADDR_D   11
#define SCK      12
#define BUZZER   13 /* LED BUILTIN */
#define LE       14 /* A0 */
#define OEn      15 /* A1 */
#define ENC_SW   16 /* A2 */
#define ENC_B    17 /* A3 */
#define ENC_A    18 /* A4 */

  
                      /*codifica RGB bicolor */    /* codifica 565 */
#define COLOR_WHITE             0x77                 /* 0xFFFF */  
#define COLOR_RED               0x11                 /* 0xF800 */
#define COLOR_GREEN             0x22                 /* 0x07E0 */
#define COLOR_BLUE              0x44                 /* 0x001F */
#define COLOR_BLACK             0x00                 /* 0x0000 */
#define COLOR_MAGENTA           0x55                 /* 0xF81F */
#define COLOR_CYAN              0x66                 /* 0x07FF */
#define COLOR_YELLOW            0x33                 /* 0xFFE0 */
#define COLOR_DIAMOND           0x70   
#define COLOR_TRIPLE            0x73
#define COLOR_ULTRA             0x15
#define COLOR_LONG              0x46
 
#define PLAYER_WIDTH   7
#define PLAYER_HEIGHT  2

#define BLOCK_WIDTH    3
#define BLOCK_HEIGHT   2
#define N_BLOCKS_PER_RIGA  10

#define CHARMS_WIDTH   2
#define CHARMS_HEIGHT  2

#define TIPO_CHARM_TRIPLE 1
#define TIPO_CHARM_ULTRA  2
#define TIPO_CHARM_LONG   3

#define N_VITE         3

#define DELTA_T_GIOCO_MS      ( 50 - livello )
#define DELTA_T_SLEEP_MS     1000
#define DELTA_T_LAMPEGGIO_MS  200
#define DELTA_T_MORTO_MS     2000
#define DELTA_T_LIVELLO_MS   1000
#define DELTA_T_GAME_OVER    5000

#define ATTESA_CHARMS         5

#define TONO_BATTUTA_HZ     400
#define TONO_BATTUTA_MS      50
#define TONO_ESPLOSIONE_HZ  200
#define TONO_ESPLOSIONE_MS  200
#define TONO_DIAMOND_HZ    1200
#define TONO_DIAMOND_MS     100
#define TONO_CHARMS_HZ      100
#define TONO_CHARMS_MS     2000
#define TONO_MORTO_HZ       440
#define TONO_MORTO_MS      2000
