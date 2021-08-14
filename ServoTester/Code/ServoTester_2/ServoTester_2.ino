#define AUTO_MODE     0
#define CENTER_MODE   1
#define MANUAL_MODE   2

#define OUTPUT_HIGH   (PORTB |= 1)
#define OUTPUT_LOW    (PORTB &= ~1)

volatile uint8_t MODE = 1;
volatile uint16_t pulse = 0;
volatile int8_t sign = 1;

void setup() {
  // configure pins
  DDRB = 1;             // PB0 as output, the others as input

  // set up interrupt
  MCUCR |= 0b11;        // external interrupt detection on rising edge
  GIMSK |= (1 << 6);    // enable external interrupt
  SREG |= (1 << 7);     // global interrupt enable
}

void loop() {
  if (MODE == CENTER_MODE) {
    pulse = 500;
  } else if (MODE == AUTO_MODE) {
    if ((pulse == 1000 && sign == 1) || (pulse <10 && sign == -1)) {
      sign *= -1;
    }
    pulse += 10 * sign;
  } else if (MODE == MANUAL_MODE) {
    pulse = analogRead(PB3);
  }
  OUTPUT_HIGH;    // turn on pin
  delayMicroseconds(2 * pulse + 1000);
  OUTPUT_LOW;     // turn off pin
  delayMicroseconds(12000);
}

ISR(INT0_vect) {
  MODE = (MODE + 1) % 3;    // keep it simple, switch mode every time button is pressed
}
