#include <SPI.h>
#include <RF24.h>
#include <RF24Network.h>
#include <stdarg.h>

#define P_BUZ 10
#define SERIAL_NUMBER 1000

RF24 radio(7, 8);                    // nRF24L01(+) radio attached using Getting Started board
RF24Network network(radio);          // Network uses that radio

const uint16_t this_node = 00;       // Address of the command center

const unsigned long interval = 2000; // How often (in ms) to send 'hello world' to the other unit
unsigned long last_sent;             // When did we last send?
unsigned long packets_sent;          // How many have we sent already

// Serial data STRING
String serialData;

struct payload_t {
  unsigned long sn;    // SERIAL NUMBER
  unsigned long data;  // data
  unsigned long req;   // command
  double batt;         // optional BATTERY DATA
};

struct command_t {
  uint16_t node;
  unsigned long req;
  unsigned long data;
};

void setup(void) {
  Serial.begin(9600);
  pinMode(P_BUZ, OUTPUT);
  
  for(int ibz=3; ibz > 0; ibz--) {
      tone(P_BUZ,3000);delay(500);
      noTone(P_BUZ);delay(500);    
  }
  
  if (!Serial) {
    // some boards need this because of native USB capability
  }
  Serial.println("<<CommandCenterON>>");

  SPI.begin();
  if (!radio.begin()) {
    Serial.println(F("Radio hardware not responding!"));
    while (1) {
      // hold in infinite loop & alert via buzzer
      tone(P_BUZ,1000);delay(500);
      noTone(P_BUZ);delay(500);  
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
    payloadPrint(payload);
  }
}

/*
 * 02 -> node destination
 * 1  -> command for accessing relay
 * 1  -> data TRUE for enable relay, OR 0 is FALSE for disable relay
 * 
 * Serial command '<<CMD02~1~1>>'
 */
void payloadPrint(payload_t pload) {
    Serial.print("<<DATA");
    Serial.print(pload.sn);
    Serial.print("~");
    Serial.print(pload.data);
    Serial.print("~");
    Serial.print(pload.req);
    Serial.print("~");
    Serial.print(pload.batt);
    Serial.println(">>");
}

/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */
void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    // add it to the inputString:
    serialData += inChar;
    if (inChar == '\n') {  // use '\n' for new line and '\r' for Carriage return 
      serialData.trim(); //trim string to remove handshaking characters CR and LF
      Serial.println(serialData);
      command_t cmd = parsingCommand(serialData);
      if(cmd.req == 0){
        // test ping <<CMD02~0~0>>
        for(int ibz=3; ibz > 0; ibz--) {
            tone(P_BUZ,3000);delay(500);
            noTone(P_BUZ);delay(500);    
        }
      } else {
        transmitData(cmd.node, cmd.req, cmd.data);
      }
      serialData = "";
    }
  }
}

/*
 * 02 -> node destination
 * 1  -> command for accessing relay
 * 1  -> data TRUE for enable relay, OR 0 is FALSE for disable relay
 * 
 * Serial command '<<CMD02~1~1>>'
 */
command_t parsingCommand(String cmdstr) {
  cmdstr.replace("<<CMD","");
  cmdstr.replace(">>","");
  command_t cmd;
  cmd.node = atoi(gval(cmdstr,'~',0).c_str());
  cmd.req = gval(cmdstr,'~',1).toInt();
  cmd.data = gval(cmdstr,'~',2).toInt();
  return cmd;
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

void transmitData(uint16_t node, int req, long data) {
  // get current voltage
  double curvolt = double( readVcc() ) / 1000;
  payload_t packets = { SERIAL_NUMBER, data, req, curvolt };
  RF24NetworkHeader header(/*to node*/ node);
  if(network.write(header, &packets, sizeof(packets))){
    Serial.println("<<TRANSMIT>>");
  } else {
    Serial.println("<<FAILED>>");
  }  
}

String gval(String data, char separator, int index) {
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
