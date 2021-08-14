#include "myController.h"

Data_Package package = { 0xF0, 0, 0, 0, 0, 0, 0 };

volatile bool doExp = false, doHalve = false;
volatile int8_t offset_rudder = EEPROM.read(Address_Rudder), offset_elevator = EEPROM.read(Address_Elevator);
volatile uint8_t button_status = PIND & Button_Mask;

volatile uint16_t init_aileron, init_throttle, init_rudder, init_elevator;

void init_controller() {
  // initialize pins
  DDRB = (1 << Chip_Select) | (1 << MOSI) | (1 << SCK);   // port B
  DDRD = 0x00;                                            // port D, all inputs
  DDRC = (1 << Chip_Enable);                              // port C

  // measure the initial contidions
  init_aileron = analogRead(Aileron);
  init_throttle = analogRead(Throttle);
  init_rudder = analogRead(Rudder);
  init_elevator = analogRead(Elevator);

  // temporarily disable global interrupt
  SREG |= (1 << 7);

  // enable interrupts
  // pin change interrupt
  PCMSK2 = (1 << SW0) | (1 << SW1) | (1 << SW2) | (1 << SW3);
  PCMSK1 = (1 << Rudder_Trim_Clk);
  PCMSK0 = (1 << Aileron_Trim_Clr) | (1 << Rudder_Trim_Clr);
  PCICR = (1 << PCIE2) | (1 << PCIE1) | (1 << PCIE0);
  
  // external interrupts
  EIMSK = (1 << INT1) | (1 << INT0);
  EICRA = 0x0F;     // trigger on RISING edge

  // timer interrupts
  TCCR0A = (1 << WGM01);                // CTC mode
  TCCR0B = (1 << CS02) | (1 << CS00);   // prescalar set to 1024
  OCR0A = 156;                          // interrupt every 10ms
  TIMSK0 = (1 << OCIE0A);               // compare match with OCR0A
  
  // enable global interrupt
  SREG |= (1 << 7);
}



// also the interrupt service routines
// nRF24L01 interrupt output
ISR(INT1_vect) {
  // user defines
}

// Left Encoder Clock
ISR(INT0_vect) {
  // Left_Trim_Data => PD0
  if (PIND & (1 << 0)) {
#if Controller_Mode == 1 || Controller_Mode == 2
    offset_rudder++;
#elif Controller_Mode == 3 || Controller_Mode == 4
    offset_aileron++;
#endif
  }  else {
#if Controller_Mode == 1 || Controller_Mode == 2
    offset_rudder--;
#elif Controller_Mode == 3 || Controller_Mode == 4
    offset_aileron--;
#endif
  }
}

// Right Encoder Clock
ISR(PCINT1_vect) {
  if (PIND & (1 << Aileron_Trim_Clk)) {
    if (PIND & (1 << Aileron_Trim_Data)) {
#if Controller_Mode == 1 || Controller_Mode == 2
      offset_elevator++;
#elif Controller_Mode == 3 || Controller_Mode == 4
      offset_rudder++;
#endif
    } else {
#if Controller_Mode == 1 || Controller_Mode == 2
      offset_elevator--;
#elif Controller_Mode == 3 || Controller_Mode == 4
      offset_rudder--;
#endif
    }
  }
}

// trimmer clear signal; Left_Trim_Clear => PB0, Right_Trim_Clear => PB1
ISR(PCINT0_vect) {
  if (!(PINB & (1 << 0))) {
#if Controller_Mode == 1 || Controller_Mode == 2
    offset_rudder = 0;
#elif Controller_Mode == 3 || Controller_Mode == 4
    offset_elevator = 0;
#endif
  }
  if (!(PINB & (1 << 1))) {
#if Controller_Mode == 1 || Controller_Mode == 2
    offset_elevator = 0;
#elif Controller_Mode == 3 || Controller_Mode == 4
    offset_rudder = 0;
#endif
  }
}

// switches
ISR(PCINT2_vect) {
  doExp = PIND & (1 << Exponential);
  doHalve = PIND & (1 << Halve);

  button_status = PIND & Button_Mask;
}

// timer interrupt
ISR(TIMER0_COMPA_vect){
  
}
