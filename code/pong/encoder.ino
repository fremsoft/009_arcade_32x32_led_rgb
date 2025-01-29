#include "defs.h"
#include "PinChangeInterrupt.h"

volatile int32_t encoderPos;      
volatile int     statoEncoder;    

//void IRAM_ATTR encStateMachine() {
void encStateMachine() {
  
  switch( statoEncoder ) {
    case 0 : 
      if ((digitalRead(ENC_A) == 0) && (digitalRead(ENC_B) == 0)) { 
        statoEncoder = 1; 
      }
      break;

    case 1 : 
      if (digitalRead(ENC_A) == 1) { statoEncoder = 2; encoderPos ++; }
      if (digitalRead(ENC_B) == 1) { statoEncoder = 4; encoderPos --; }
      break;

    case 2 : 
      if (digitalRead(ENC_A) == 0) { statoEncoder = 1; encoderPos --; }
      if (digitalRead(ENC_B) == 1) { statoEncoder = 3; encoderPos ++; }
      break;

    case 3 : 
      if (digitalRead(ENC_A) == 0) { statoEncoder = 4; encoderPos ++; }
      if (digitalRead(ENC_B) == 0) { statoEncoder = 2; encoderPos --; }
      break;

    case 4 : 
      if (digitalRead(ENC_A) == 1) { statoEncoder = 3; encoderPos --; }
      if (digitalRead(ENC_B) == 0) { statoEncoder = 1; encoderPos ++; }
      break;

    default : 
      Serial.println( "STATO SCONOSCIUTO" );
      statoEncoder = 0;
      break;
  }

}

//void IRAM_ATTR readEncoderA() { encStateMachine(); }
//void IRAM_ATTR readEncoderB() { encStateMachine(); }

void setupEncoder() {
  
  pinMode( ENC_A,  INPUT_PULLUP );
  pinMode( ENC_B,  INPUT_PULLUP );
  pinMode( ENC_SW, INPUT_PULLUP );

  //attachInterrupt(digitalPinToInterrupt(ENC_A), encStateMachine /* readEncoderA */, CHANGE);
  //attachInterrupt(digitalPinToInterrupt(ENC_B), encStateMachine /* readEncoderB */, CHANGE);
  attachPCINT(digitalPinToPCINT(ENC_A), encStateMachine, CHANGE);
  attachPCINT(digitalPinToPCINT(ENC_B), encStateMachine, CHANGE);
  
  Serial.println("Encoder configurato");

  encoderPos = 0;      
  statoEncoder = 0;    

}

int32_t getPosEncoder() {
  int32_t t;
  /* disabilitare irq */
  t = encoderPos;
  /* abilitare irq */
  return t;
}

void setPosEncoder(int32_t val) {
  /* disabilitare irq */
  encoderPos = val;
  /* abilitare irq */
}

bool getSwitchEncoder() {
  return !digitalRead(ENC_SW);
}
