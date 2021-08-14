void setup() {
  DDRD |= (1 << DDD6);    // set pin 6 as output
  
  // enable timer 1 compare match with OCR1A interrupt in CTC mode
  TCCR1A |= (1 << 1);             // enable CTC mode 0b00000001 = 1 = 0x01
  TCCR1B |= 0b100;                // prescalar 256
  OCR1AH = 62500 >> 8;            // period = 1s
  OCR1AL = 62500;
  TIMSK1 |= (1 << 1);             // enable compare match with OCR1A interrupt
}

void loop() {
}

ISR(TIMER1_COMPA_vect){
  PIND |= (1 << PIND6);   // toggle pin 6
}
