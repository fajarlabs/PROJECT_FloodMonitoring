#include <SPI.h>
#include <RF24.h>
#include <RF24Network.h>

#define SERIAL_NUMBER 1001
#define CMD 0                        // do something with command
#define BUZZER A5
#define INPUT4 6
#define INPUT3 4  
#define INPUT2 3
#define INPUT1 2
#define OUTPUT2 10
#define OUTPUT1 9
#define RAIN_GAUGE 5

int is_beep = 0;
int levelFlood = 0;

int rainInTick = 0;
bool bucketPositionA = false;  
const double bucketAmount = 0.053;        // 0.053 inches atau 1.346 mm of rain equivalent of ml to trip tipping-bucket
//note:
//diketahui bahwa 1 inchi = 2.54 cm
//diketahui bahwa 1 tip sensor ini = 2.6 mL
//maka 0.053 inchi of rain didapat dari:
//panjang rain collector = 5.4 cm atau 2.126 inchi
//lebar rain collector = 3.6 cm atau 1.417 inchi
//luas=pxl -> 2.16 inchi x 1.417 inchi = 3.012 inchi persegi
//U.S. measures rain in inches so it would be 3.012 inchi kubik
//lalu dikonversi dari inchi kubik mjd mL dan didapat bahwa 3.012 inchi kubik = 49.358 mL
//yg artinya 1 inchi of rain = 49.358 mL
//sehingga 1 tip sensor ini mewakili 2.6 mL/49.358 mL = 0.053 inchi of rain
 
double countRain = 0.0;                // rain accumulated for the day

RF24 radio(7, 8);                    // nRF24L01(+) radio attached using Getting Started board
RF24Network network(radio);          // Network uses that radio

const uint16_t node01 = 00;          // Address of our node in Octal format
const uint16_t this_node = 01;       // Address of the other node in Octal format

const unsigned long interval = 1000; // How often (in ms) to send 'hello world' to the other unit
unsigned long last_sent;             // When did we last send?

unsigned long beep_interval = 500; 
unsigned long beep_last_sent;                      

struct payload_t {                   // Structure of our payload
  unsigned long sn;
  unsigned long req;   
  char data[35];
};

void setup(void) {
  pinMode(RAIN_GAUGE, INPUT);
  pinMode(INPUT1, INPUT);
  pinMode(INPUT2, INPUT);
  pinMode(INPUT3, INPUT);
  pinMode(INPUT4, INPUT);
  pinMode(OUTPUT1, OUTPUT);
  pinMode(OUTPUT2, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  // init beep after boot
  int beep = 0;
  for (int b=0; b<6; b++){
    if(beep < 1) {
      analogWrite(BUZZER, 255);
      beep = 1;
    } else {
      analogWrite(BUZZER, 0);
      beep = 0;
    }
    delay(1000);
  }
  
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

void countLevelFlood() {
   levelFlood = 0; // reset
   if(digitalRead(INPUT1) == LOW) levelFlood++;
   if(digitalRead(INPUT2) == LOW) levelFlood++;
   if(digitalRead(INPUT3) == LOW) levelFlood++;
   if(digitalRead(INPUT4) == LOW) levelFlood++;
}

void loop() {

  countLevelFlood();

  // ======================= Count the bucket tips ================================
  if ((bucketPositionA==false)&&(digitalRead(RAIN_GAUGE)==LOW)){
    bucketPositionA=true;
    countRain+=bucketAmount;                               // update the daily rain
  }
  
  if ((bucketPositionA==true)&&(digitalRead(RAIN_GAUGE)==HIGH)){
    bucketPositionA=false;  
  } 
  // ==============================================================================
  
  network.update(); // Check the network regularly
  unsigned long now = millis();

  if(levelFlood == 1) beep_interval = 2000;
  if(levelFlood == 2) beep_interval = 1500;
  if(levelFlood == 3) beep_interval = 700;
  if(levelFlood == 4) beep_interval = 200;
  if((levelFlood > 0) || (is_beep == 1)){
    // If it's time to send a message, send it!
    if (now - beep_last_sent >= beep_interval) {
      beep_last_sent = now;
      if(is_beep == 0) {
        is_beep = 1;
        analogWrite(BUZZER, 255);
      } else {
        is_beep = 0;
        analogWrite(BUZZER, 0);
      }
    }
  }
  
  // If it's time to send a message, send it!
  if (now - last_sent >= interval) {
    last_sent = now;

    //============================================================================
    // Flood data static
    //============================================================================
    long floatData = 1; // data ON (trigger flood)
    
    //============================================================================
    // Current voltage DC module
    //============================================================================
    double curvolt = double( readVcc() ) / 1000;
    
    //============================================================================
    // Concat data using delimiter '$'
    //============================================================================
    String data = String(floatData);
    data += "$";
    data += String(curvolt);
    data += "$";
    data += String(countRain);
    data += "$";
    data += String(levelFlood);
    char datachar[data.length()+1];
    data.toCharArray(datachar,data.length()+1);
    
    // payload
    payload_t payload2 = { SERIAL_NUMBER, CMD };
    
    // bind to struct (AC voltage)
    strncpy(payload2.data, datachar, sizeof(payload2.data) - 1);
    
    RF24NetworkHeader header(/*to node*/ node01);
    
    if(network.write(header, &payload2, sizeof(payload2))){
      Serial.println("<<TRANSMIT>>");
    } else {
      Serial.println("<<FAILED>>");
    }

    countRain = 0; // reset rain after sending to gateway    
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
