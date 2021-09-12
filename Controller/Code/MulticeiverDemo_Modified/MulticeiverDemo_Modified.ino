/*
   See documentation at https://nRF24.github.io/RF24
   See License information at root directory of this library
   Author: Brendan Doherty 2bndy5
*/

/**
   A simple example of sending data from as many as 6 nRF24L01 transceivers to
   1 receiving transceiver. This technique is trademarked by
   Nordic Semiconductors as "MultiCeiver".

   This example was written to be used on up to 6 devices acting as TX nodes &
   only 1 device acting as the RX node (that's a maximum of 7 devices).
   Use the Serial Monitor to change each node's behavior.
*/
#include <SPI.h>
#include "printf.h"
#include "RF24.h"

// instantiate an object for the nRF24L01 transceiver
RF24 radio(7, 8); // using pin 7 for the CE pin, and pin 8 for the CSN pin

uint8_t pipe = 0;

uint64_t address[6] = {0xA5A5A5F3F3LL,
                       0xA5A5A5F3F2LL,
                       0xA5A5A5F3CDLL,
                       0xA5A5A5F3A3LL,
                       0xA5A5A5F30FLL,
                       0xA5A5A5F305LL
                      };

typedef struct Data_Package {
  uint16_t mode;          // this word contains the alignment byte/word
  uint16_t throttle;
  uint16_t aileron;
  uint16_t rudder;
  uint16_t elevator;
  uint16_t extra_load0;
  uint16_t extra_load1;
  uint16_t extra_load2;
} Data_Package;

Data_Package payload;

void setRole();

void setup() {

  Serial.begin(115200);
  while (!Serial);

  if (!radio.begin()) {
    Serial.println(F("radio hardware is not responding!!"));
    while (1) ;
  }

  Serial.println(F("RF24/examples/MulticeiverDemo"));

  radio.setPALevel(RF24_PA_MIN); // RF24_PA_MAX is default.

  radio.setPayloadSize(sizeof(payload));

  radio.setChannel(0x40);

  setRole();

  Serial.println("Good!");
} // setup()

void loop() {
  // This device is the RX node
  radio.read(&payload, sizeof(payload));            // fetch payload from FIFO
  delay(800);

  
  pipe = 6;
  
  if (radio.available(&pipe)) {             // is there a payload? get the pipe number that recieved it
    uint8_t bytes = radio.getPayloadSize(); // get the size of the payload
    radio.read(&payload, bytes);            // fetch payload from FIFO
    Serial.print(F("Pipe: "));
    Serial.print(pipe);                     // print the pipe number
    Serial.print(F(". Payload: "));
    Serial.print(payload.mode, HEX);
    Serial.print(" ");
    Serial.print(payload.throttle, HEX);
    Serial.print(" ");
    Serial.print(payload.aileron, HEX);
    Serial.print(" ");
    Serial.print(payload.rudder, HEX);
    Serial.print(" ");
    Serial.print(payload.elevator, HEX);
    Serial.print(" ");
    Serial.print(payload.extra_load0, HEX);
    Serial.print(" ");
    Serial.print(payload.extra_load1, HEX);
    Serial.print(" ");
    Serial.println(payload.extra_load2, HEX);
  }
}

void setRole() {
  // For the RX node

  // Set the addresses for all pipes to TX nodes
  for (uint8_t i = 0; i < 6; ++i)
    radio.openReadingPipe(i, address[i]);

  radio.openWritingPipe(address[0]);

  radio.setAutoAck(true);

  radio.startListening(); // put radio in RX mode
}
