#include <SPI.h>
#include <RF24.h>
#include <RF24Network.h>
#define RELAYPIN 9
#define FUELPIN 6
#define CMD 0
#define SERIAL_NUMBER 1002

RF24 radio(7, 8);               // nRF24L01(+) radio attached using Getting Started board

RF24Network network(radio);     // Network uses that radio
const uint16_t this_node = 02;  // Address of our node in Octal format (04, 031, etc)
const uint16_t node01 = 00;    // Address of the other node in Octal format

const unsigned long interval = 2000; // How often (in ms) to send 'hello world' to the other unit
unsigned long last_sent;             // When did we last send?

struct payload_t { 
  unsigned long sn;
  unsigned long data;
  unsigned long req;   
  double batt;
};

struct packet_t { 
  uint16_t node;
  int cmd;
  unsigned long data;
};

typedef struct global_t {
  bool isRelayON = false;
} Global;

Global global; 

void setup(void) {
  pinMode(RELAYPIN, OUTPUT); // relay for trigger
  pinMode(FUELPIN, INPUT);   // fuel input
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
  
  //===== Receiving =====//
  while (network.available()) {      // Is there anything ready for us?
    RF24NetworkHeader header;        // If so, grab it and print it out
    payload_t payload;
    network.read(header, &payload, sizeof(payload));
    /*
    Serial.println("Received packet #");
    Serial.println(payload.sn);
    Serial.println(payload.data);
    Serial.println(payload.req);
    Serial.println(payload.batt);
    */
    if(payload.data == 1) {
      Serial.println("<<TriggerON>>");
      digitalWrite(RELAYPIN, HIGH);
      global.isRelayON = true;
    }
    if(payload.data == 2) {
      Serial.println("<<TriggerOFF>>");
      digitalWrite(RELAYPIN, LOW);
      global.isRelayON = false;
    }
  }

  if(global.isRelayON)
    digitalWrite(RELAYPIN, HIGH);
  else
    digitalWrite(RELAYPIN, LOW);

  //===== Sending =====//
  // if request == 1 is transmit 
  if (now - last_sent >= interval) {
    last_sent = now;
    int fuelExist = 0;
    if(digitalRead(FUELPIN) == HIGH) fuelExist = 1;
    // get current voltage
    double curvolt = double( readVcc() ) / 1000;
    payload_t payload2 = { SERIAL_NUMBER, fuelExist, CMD, curvolt };
    RF24NetworkHeader header2(node01);     // (Address where the data is going)
    if(network.write(header2, &payload2, sizeof(payload2))) {
      Serial.println("<<Transmit>>");
    } else {
      Serial.println("<<Failed>>");
    }
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
