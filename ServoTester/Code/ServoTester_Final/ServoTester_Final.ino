#define AUTO_MODE     0
#define CENTER_MODE   1
#define MANUAL_MODE   2

#define OUTPUT_HIGH   (PORTB |= 1)
#define OUTPUT_LOW    (PORTB &= ~1)
#define START_TIM0    (TCCR0B = (1 << CS01))     // prescaler is 8
#define STOP_TIM0     (TCCR0B &= ~(1 << CS01))

volatile uint8_t MODE = 1;
volatile uint8_t pulse = 0;   // the pulse length is buffered
volatile int8_t sign = 1;     // to remember the direction of counting

void setup() {
  // configure pins
  DDRB = 1;                   // PB0 as output, the others as input
  
  // set up timer 0
  TCCR0A = (1 << WGM01);      // CTC mode
  OCR0A = 249;                // 250 - 1 (2ms)
  TCNT0 = 0;
  // don't start the timer now; we have a macro for it

  // set up timer 1
  TCCR1 = (1 << CTC1);  // CTC Mode
  TCNT1 = 0;            // clear the timer
  OCR1C = 38;           // 39 - 1
  TCCR1 = (1 << CTC1) | (1 << CS13) | (1 << CS11);     // prescaler is 512 and start the timer

  // set up interrupt
  MCUCR |= (1 << ISC01);// external interrupt detection on falling edge
  GIMSK |= (1 << 6);    // enable external interrupt
  TIMSK |= (1 << TOIE1) | (1 << OCIE0A);       // enable timer interrupt
}

void loop() {

}

ISR(INT0_vect) {
  if (MODE == AUTO_MODE) {
    MODE = CENTER_MODE;
    pulse = 187;
    OCR0A = 187;   // 188 - 1
  }
  else if (MODE == CENTER_MODE) {
    MODE = MANUAL_MODE;
  }
  else if (MODE == MANUAL_MODE) {     // contrain the value and set the sign
    MODE = AUTO_MODE;
    if (pulse > 249) {
      pulse = 249;
      sign = -1;
    }
    else if (pulse < 126) {
      pulse = 126;
      sign = 1;
    }
  }
}

// turn on timer 0 and set output to HIGH
ISR(TIMER1_OVF_vect) {
  START_TIM0;
  digitalWrite(0, HIGH);;
}

// set output to LOW, turn off timer 0 and update the pulse value for next one
ISR(TIMER0_COMPA_vect) {
  digitalWrite(0, LOW);
  STOP_TIM0;
  if (MODE == CENTER_MODE) return;    // no need to change
  OCR0A = pulse;
  // 125 of OCR0A is 1ms
  if (MODE == AUTO_MODE) {
    if ((pulse == 250) || (pulse == 125)) sign *= -1;    // keep it simple
    pulse += sign;
  }
  else if (MODE == MANUAL_MODE) {
    pulse = map(analogRead(A3), 0, 1024, 125, 250);
  }
}
