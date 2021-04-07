#include <SPI.h>
#include <RF24.h>
#include <RF24Network.h>

RF24 radio(7, 8);                    // nRF24L01(+) radio attached using Getting Started board
RF24Network network(radio);          // Network uses that radio

const uint16_t this_node = 00;       // Address of the command center
const uint16_t node01 = 01;          // Address of the sensor node ( receive only )
const uint16_t node02 = 02;          // Address of the waterpump node ( receive & transmit )

const unsigned long interval = 2000; // How often (in ms) to send 'hello world' to the other unit
unsigned long last_sent;             // When did we last send?
unsigned long packets_sent;          // How many have we sent already

// Serial number
const int SERIAL_NUMBER = 1000;
// Request, if '1' transmit to other client, if '0' only receive data, if '2' receive & transmit
const int REQUEST_FOR_CLIENT = 2;

int simulatePump = 0;

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
  Serial.println("<<CommandCenterON>>");

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

  while (network.available()) {      // Is there anything ready for us?
    RF24NetworkHeader header;        // If so, grab it and print it out
    payload_t payload;
    network.read(header, &payload, sizeof(payload));
    Serial.println("Received packet #");
    Serial.println(payload.sn);
    Serial.println(payload.data);
    Serial.println(payload.req);
    Serial.println(payload.batt);
  }
  
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

    // do simulate pump trigger
    if(simulatePump == 0) {
      simulatePump = 1;
    } else if(simulatePump == 1){
      simulatePump = 2;
    } else if(simulatePump == 2) {
      simulatePump = 0;
    }
    // get current voltage
    double curvolt = double( readVcc() ) / 1000;
    payload_t payload = { SERIAL_NUMBER, simulatePump, REQUEST_FOR_CLIENT, curvolt };
    RF24NetworkHeader header(/*to node*/ node02);
    if(network.write(header, &payload, sizeof(payload))){
      Serial.println("<<TRANSMIT>>");
    } else {
      Serial.println("<<FAILED>>");
    }
    delay(8000);
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
