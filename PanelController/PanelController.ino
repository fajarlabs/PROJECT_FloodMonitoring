#include <SPI.h>
#include <RF24.h>
#include <RF24Network.h>

RF24 radio(7, 8);               // nRF24L01(+) radio attached using Getting Started board

RF24Network network(radio);     // Network uses that radio
const uint16_t this_node = 00;  // Address of our node in Octal format (04, 031, etc)
const uint16_t node01 = 01;    // Address of the other node in Octal format

const unsigned long interval = 2000; // How often (in ms) to send 'hello world' to the other unit
unsigned long last_sent;             // When did we last send?

// Serial number
const int SERIAL_NUMBER = 1002;
// Request, if '1' transmit to other client, if '0' only receive data
const int REQUEST_FOR_CLIENT = 0;

struct payload_t {              // Structure of our payload
  unsigned long sn;
  unsigned long data;
  unsigned long req;   // if '1' is transmit to other client & if '0' only receive
};

void setup(void) {
  Serial.begin(9600);
  if (!Serial) {
    // some boards need this because of native USB capability
  }
  Serial.println(F("<<ControllerON>"));

  SPI.begin();
  if (!radio.begin()) {
    Serial.println(F("Radio hardware not responding!"));
    while (1) {
      // hold in infinite loop
    }
  }
  network.begin(/*channel*/ 90, /*node address*/ this_node);
}

void loop(void) {
  network.update();                  // Check the network regularly
  unsigned long now = millis();
  while (network.available()) {      // Is there anything ready for us?
    RF24NetworkHeader header;        // If so, grab it and print it out
    payload_t payload;
    network.read(header, &payload, sizeof(payload));
    Serial.println("Received packet #");
    Serial.println(payload.sn);
    Serial.println(payload.data);
    Serial.println(payload.req);
  }

  if (now - last_sent >= interval) {
    last_sent = now;
    //===== Sending =====//
    long data = 12345; // solar data
    payload_t payload2 = { SERIAL_NUMBER, data, REQUEST_FOR_CLIENT };
    RF24NetworkHeader header2(node01);     // (Address where the data is going)
    if(network.write(header2, &payload2, sizeof(payload2))) {
      Serial.println("<<Transmit>>");
    } else {
      Serial.println("<<Failed>>");
    }
  }
}
