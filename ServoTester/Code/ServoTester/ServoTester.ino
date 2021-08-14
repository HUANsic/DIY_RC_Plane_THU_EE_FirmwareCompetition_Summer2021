#define AUTO_MODE     0
#define CENTER_MODE   1
#define MANUAL_MODE   2

#define OUTPUT_HIGH   (PORTB |= 1)
#define OUTPUT_LOW    (PORTB &= ~1)

volatile uint8_t MODE = 1, pulse_part = 0;
volatile uint8_t sects[] = {250, 250, 250, 250, 0, 0, 0, 0};
volatile uint16_t pulse = 0;
volatile int8_t sign = 1;

void setup() {
  // configure pins
  DDRB = 1;             // PB0 as output, the others as input

  // set up timer 1
  TCCR1 |= (1 << 7);    // CTC mode
  TCCR1 |= 0b1011;      // set prescalar to 1024; 1 tick = 128us
  OCR1A = 156 - 1;      // period = 156 ticks = 20ms

  // set up timer 0
  TCCR0A |= (1 << 1);   // CTC mode
  TCCR0B |= (1 << 1);   // set prescalar to 8; 1 tick = 1us
  OCR0A = 188;          // default to 50%

  // set up interrupt
  MCUCR |= 0b11;        // external interrupt detection on rising edge
  GIMSK |= (1 << 6);    // enable external interrupt
  TIMSK |= (1 << 6);    // enable timer 1 compare match OCR1A interrupt
  TIMSK |= (1 << 4);    // enable timer 0 compare match OCR0A interrupt
  SREG |= (1 << 7);     // global interrupt enable
}

void loop() {
  // do nothing
}

ISR(INT0_vect) {
  MODE = (MODE + 1) % 3;    // keep it simple, switch mode every time button is pressed
}

ISR(TIMER1_COMPA_vect) {   // since in CTC mode, timer 1 never reaches MAX therefore no overflow interrupt
  if (MODE == CENTER_MODE) {
    sects[4] = 250;
    sects[5] = 250;
    sects[6] = 0;
    sects[7] = 0;
  } else if (MODE == AUTO_MODE) {
    if ((pulse == 999 && sign == 1) || (pulse == 0 && sign == -1)) {
      sign *= -1;
    }
    pulse += sign;
    if (pulse > 250) sects[0] = 250; else sects[0] = pulse;
    if (pulse > 500) sects[1] = 250; else sects[1] = pulse - 250;
    if (pulse > 750) sects[2] = 250; else sects[2] = pulse - 500;
    if (pulse > 1000) sects[3] = 250; else sects[3] = pulse - 750;
  } else if (MODE == MANUAL_MODE) {
    pulse = analogRead(PB3);
    if (pulse > 250) sects[4] = 250; else sects[4] = pulse;
    if (pulse > 500) sects[5] = 250; else sects[5] = pulse - 250;
    if (pulse > 750) sects[6] = 250; else sects[6] = pulse - 500;
    if (pulse > 1000) sects[7] = 250; else sects[7] = pulse - 750;
  }
  OCR0A = sects[0];
  TCNT0 = 0;            // clear timer 0
  OUTPUT_HIGH;          // turn on pin
  TCCR0B |= (1 << 1);   // start tc0
}

ISR(TIMER0_COMPA_vect) {    // same explaination as above
  pulse_part = (pulse_part + 1) % 8;
  if (!pulse_part) {
    TCCR0B &= ~(1 << 1);    // stop tc0
    OUTPUT_LOW;       // turn off pin
  }
  OCR0A = sects[pulse_part];
}
