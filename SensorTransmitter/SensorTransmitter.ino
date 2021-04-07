#include <SPI.h>
#include <RF24.h>
#include <RF24Network.h>
#include "LowPower.h"

RF24 radio(7, 8);                    // nRF24L01(+) radio attached using Getting Started board
RF24Network network(radio);          // Network uses that radio

const uint16_t node01 = 00;         // Address of our node in Octal format
const uint16_t this_node = 01;       // Address of the other node in Octal format

const unsigned long interval = 2000; // How often (in ms) to send 'hello world' to the other unit
unsigned long last_sent;             // When did we last send?
unsigned long packets_sent;          // How many have we sent already

// Serial number
const int SERIAL_NUMBER = 1001;
// Request, if '1' transmit to other client, if '0' only receive data
const int REQUEST_FOR_CLIENT = 1;

struct payload_t {                   // Structure of our payload
  unsigned long sn;
  unsigned long data;
  unsigned long req;
};

void setup(void) {
  Serial.begin(9600);
  if (!Serial) {
    // some boards need this because of native USB capability
  }
  Serial.println("<<SensorON>>");

  SPI.begin();
  if (!radio.begin()) {
    Serial.println(F("Radio hardware not responding!"));
    while (1) {
      // hold in infinite loop
    }
  }
  network.begin(/*channel*/ 90, /*node address*/ this_node);
}

void loop() {
  network.update(); // Check the network regularly
  unsigned long now = millis();

  /*
  while (network.available()) {      // Is there anything ready for us?
    RF24NetworkHeader header;        // If so, grab it and print it out
    payload_t payload;
    network.read(header, &payload, sizeof(payload));
    Serial.println("Received packet #");
    Serial.println(payload.sn);
    Serial.println(payload.data);
    Serial.println(payload.req);
  }
  */
  
  // If it's time to send a message, send it!
  if (now - last_sent >= interval) {
    last_sent = now;
    /*
    // set format data to transmit
    String msg = formatData("1");
    int msg_len = msg.length() + 1; 
    // Prepare the character array (the buffer) 
    char dataToSend[msg_len];
    // Copy it over 
    msg.toCharArray(dataToSend, msg_len);
    */
    long data = 1; // data ON (trigger flood)
    payload_t payload = { SERIAL_NUMBER, data, REQUEST_FOR_CLIENT };
    RF24NetworkHeader header(/*to node*/ node01);
    if(network.write(header, &payload, sizeof(payload))){
      Serial.println("<<TRANSMIT>>");
    } else {
      Serial.println("<<FAILED>>");
    }
    delay(50); 
    // deep sleep in 8 seconds
    LowPower.idle(SLEEP_8S, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF, SPI_OFF, USART0_OFF, TWI_OFF);
  }
}
