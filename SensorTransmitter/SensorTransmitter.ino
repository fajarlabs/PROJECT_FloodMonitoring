#include <SPI.h>
#include <RF24.h>
#include <RF24Network.h>
#include "LowPower.h"

#define SERIAL_NUMBER 1001
#define CMD 0                        // do something with command

RF24 radio(7, 8);                    // nRF24L01(+) radio attached using Getting Started board
RF24Network network(radio);          // Network uses that radio

const uint16_t node01 = 00;          // Address of our node in Octal format
const uint16_t this_node = 01;       // Address of the other node in Octal format

const unsigned long interval = 2000; // How often (in ms) to send 'hello world' to the other unit
unsigned long last_sent;             // When did we last send?
unsigned long packets_sent;          // How many have we sent already

struct payload_t {                   // Structure of our payload
  unsigned long sn;
  unsigned long data;
  unsigned long req;
  double batt;
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
  
  // If it's time to send a message, send it!
  if (now - last_sent >= interval) {
    last_sent = now;

    long data = 1; // data ON (trigger flood)
    // get current voltage
    double curvolt = double( readVcc() ) / 1000;
    payload_t payload = { SERIAL_NUMBER, data, CMD, curvolt };
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

//read internal voltage
long readVcc() {
  long result;
  // Read 1.1V reference against AVcc
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Convert
  while (bit_is_set(ADCSRA, ADSC));
  result = ADCL;
  result |= ADCH << 8;
  result = 1126400L / result; // Back-calculate AVcc in mV
  return result;
}
