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

  
                          /* codifica 565 */
#define COLOR_WHITE    7  /* 0xFFFF */  
#define COLOR_RED      1  /* 0xF800 */
#define COLOR_GREEN    2  /* 0x07E0 */
#define COLOR_BLUE     4  /* 0x001F */
#define COLOR_BLACK    0  /* 0x0000 */
#define COLOR_MAGENTA  5  /* 0xF81F */
#define COLOR_CYAN     6  /* 0x07FF */
#define COLOR_YELLOW   3  /* 0xFFE0 */

#define N_VITE         3
#define PUNTEGGIO_VITTORIA   32
#define SENSIBILITA_ROTELLA  16

#define DELTA_T_GIOCO_MS       50
#define DELTA_T_SBAGLIATO_MS 2000
#define DELTA_T_GAME_OVER    5000

#define TONO_OK_HZ          400
#define TONO_OK_MS          100
#define TONO_ERRORE_HZ      200
#define TONO_ERRORE_MS     1000
#define TONO_VITTORIA_HZ    800
#define TONO_VITTORIA_MS   3000
