#include "myController.h"

volatile bool feedback = false;

const byte addresses[][6] = {"rc001", "rc003"};

RF24 radio(Chip_Enable, Chip_Select);

void setup() {
  // set up RF
  radio.begin();
  radio.openWritingPipe(addresses[0]);
  radio.openReadingPipe(1, addresses[1]);
  radio.setAutoAck(true);
  radio.setCRCLength(RF24_CRC_8);
  radio.setDataRate(RF24_1MBPS);
  radio.setPALevel(RF24_PA_HIGH);
  radio.startListening();

  // connect RF
  radio.stopListening();
  while (!feedback) {   // keep trying until one is connected
    feedback = radio.write(&package, sizeof(package));
    delay(100);
  }
}

void loop() {
  // main loop
}









void updatePackage() {
  uint16_t temp = analogRead(Throttle);
  package.throttle = temp * (doExp ? exp(0.003 * temp) / 20 : 1) / (doHalve ? 2 : 1);

  temp = analogRead(Aileron);
  package.throttle = temp * (doExp ? exp(0.003 * temp) / 20 : 1) / (doHalve ? 2 : 1);

}
