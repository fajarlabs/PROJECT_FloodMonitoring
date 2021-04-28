#include <SPI.h>
#include <RF24.h>
#include <RF24Network.h>
#define RELAYPIN 9
#define FUELPIN 6
#define CMD 0
#define SERIAL_NUMBER 1002
#include <Filters.h>

float testFrequency = 50;                 // Standard current freqwensi indonesia (Hz)
float windowLength = 40.0/testFrequency;    
int voltageSensorPin = A0; // input voltage sensor
float intercept = -0.04;
float slope = 0.0405;
float volts; // data voltage
unsigned long voltCheckPeriode = 1000; 
unsigned long voltCurTime = 0;
RunningStatistics inputStats;   // init statistic


RF24 radio(7, 8);               // nRF24L01(+) radio attached using Getting Started board

RF24Network network(radio);     // Network uses that radio
const uint16_t this_node = 02;  // Address of our node in Octal format (04, 031, etc)
const uint16_t node01 = 00;    // Address of the other node in Octal format

const unsigned long interval = 2000; // How often (in ms) to send 'hello world' to the other unit
unsigned long last_sent;             // When did we last send?

struct payload_t { 
  unsigned long sn;
  unsigned long req;   
  char data[25];
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
  inputStats.setWindowSecs(windowLength);
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

  voltageSensorPin = analogRead(A0);
  inputStats.input(voltageSensorPin);

  if((unsigned long)(millis() - voltCurTime) >= voltCheckPeriode) {   
    volts = intercept + slope * inputStats.sigma(); //offset y amplitud
    volts = volts*(40.3231); 
    voltCurTime = millis(); 
  }
  
  //===== Receiving =====//
  while (network.available()) {      // Is there anything ready for us?
    RF24NetworkHeader header;        // If so, grab it and print it out
    payload_t payload;
    network.read(header, &payload, sizeof(payload));
    /*
    Serial.println("Received packet #");
    Serial.println(payload.sn);
    Serial.println(payload.req);
    Serial.println(payload.data);
    */
    if(payload.req == 1){
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
  }

  if(global.isRelayON)
    digitalWrite(RELAYPIN, HIGH);
  else
    digitalWrite(RELAYPIN, LOW);

  //===== Sending =====//
  // if request == 1 is transmit 
  if (now - last_sent >= interval) {
    last_sent = now;

    //============================================================================
    // Fuel data
    //============================================================================
    int fuelExist = 0;
    if(digitalRead(FUELPIN) == HIGH) fuelExist = 1;

    //============================================================================
    // Module voltage
    //============================================================================
    double curvolt = double( readVcc() ) / 1000; // (current module voltage DC)
    
    //============================================================================
    // AC voltage sensor
    //============================================================================
    // AC voltage variable
    char curACStr[4];
    // format float 
    dtostrf(volts, 3, 0, curACStr);

    //============================================================================
    // Concat data using delimiter '$'
    //============================================================================
    String data = String(fuelExist);
    data += "$";
    data += String(curvolt);
    data += "$";
    data += String(curACStr);
    char datachar[data.length()+1];
    data.toCharArray(datachar,data.length()+1);
    // payload
    payload_t payload2 = { SERIAL_NUMBER, CMD, curvolt };
    // bind to struct (AC voltage)
    strncpy(payload2.data, datachar, sizeof(payload2.data) - 1);
    
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
