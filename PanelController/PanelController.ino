#include <SPI.h>
#include <RF24.h>
#include <RF24Network.h>
#define RELAYPIN 9
#define FUELPIN 6
#define CMD 0
#define SERIAL_NUMBER 1002
#include <Filters.h>
#define voltageSensorPin A0 // input voltage sensor
#define voltageDCSensorPin A1
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

float testFrequency = 50; // Standard current freqwensi indonesia (Hz)
float windowLength = 40.0/testFrequency;    
float intercept = -0.04;
float slope = 0.0405;
float volts; // data voltage
unsigned long voltCheckPeriode = 1000; 
unsigned long voltCurTime = 0;
RunningStatistics inputStats;   // init statistic

// Floats for ADC voltage & Input voltage
float adc_voltage = 0.0;
float in_voltage = 0.0;
// Floats for resistor values in divider (in ohms)
float R1 = 30000.0;
float R2 = 7500.0; 
// Float for Reference Voltage
float ref_voltage = 5.0;
// Integer for ADC value
int adc_value = 0;

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

struct command_t {
  uint16_t node;
  unsigned long req;
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
  inputStats.input(analogRead(voltageSensorPin));

  if((unsigned long)(millis() - voltCurTime) >= voltCheckPeriode) {   
    // Alternating Current Voltage Data
    volts = intercept + slope * inputStats.sigma(); //offset y amplitud
    volts = volts*(40.3231); 
    voltCurTime = millis(); 
    
    // Directing current Voltage data
    adc_value = analogRead(voltageDCSensorPin);
    // Determine voltage at ADC input
    adc_voltage  = (adc_value * ref_voltage) / 1024.0; 
    // Calculate voltage at divider input
    in_voltage = adc_voltage / (R2/(R1+R2)); 
  }
  
  //===== Receiving =====//
  while (network.available()) {      // Is there anything ready for us?
    RF24NetworkHeader header;        // If so, grab it and print it out
    command_t cmd_payload;
    network.read(header, &cmd_payload, sizeof(cmd_payload));
    /*
    Serial.println("Received packet #");
    Serial.println(cmd_payload.node);
    Serial.println(cmd_payload.req);
    Serial.println(cmd_payload.data);
    */
    if(cmd_payload.req == 1){
      if(cmd_payload.data == 1) {
        Serial.println("<<TriggerON>>");
        digitalWrite(RELAYPIN, HIGH);
        global.isRelayON = true;
      }
      if(cmd_payload.data == 2) {
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
    data += "$";
    data += String(in_voltage);
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
