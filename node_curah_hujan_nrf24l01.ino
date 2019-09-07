
/**
 * Using RF24Network 
 *
 * TRANSMITTER SENSOR NODE
 * Every 5 minutes, send a payload to the receiver node.
 */

#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>
#include <ArduinoJson.h>

RF24 radio(9,10);                    // nRF24L01(+) radio attached using Getting Started board 

RF24Network network(radio);          // Network uses that radio

const uint16_t this_node = 2;        // Address of our node in Octal format
const uint16_t other_node = 0;       // Address of the other node in Octal format

const byte BUCKET_PIN = 2;
unsigned int BOUNCE_LIMIT = 1 * 1000;
volatile byte tick = 0;              // Tipping Bucket TICK



const unsigned long interval = 5*60000; //ms  // How often to send to the other unit

unsigned long last_sent;             // When did we last send?
unsigned long packets_sent;          // How many have we sent already

String data;
struct payload_t {                  // Structure of our payload
  char paket[100];

};

void setup(void)
{
  Serial.begin(9600);
  Serial.println("RF24Network/examples/helloworld_tx/");

  pinMode(BUCKET_PIN, INPUT);
  digitalWrite(BUCKET_PIN, HIGH);
  
  SPI.begin();
  radio.begin();
  network.begin(/*channel*/ 90, /*node address*/ this_node);
}

void loop() {
  
  network.update();                          // Check the network regularly
  data="";
  unsigned long now = millis();              // If it's time to send a message, send it!
  ticking();
  if ( now - last_sent >= interval  )
  {
    last_sent = now;
    StaticJsonDocument<16> doc;
    doc["up-since"] = now; 
    doc["tick"] = tick;
    serializeJson(doc, data);
    //Serial.println(data);
    char data1[100];
    payload_t payload = {};
    for (int i = 0; i < sizeof(data1); i++){
      payload.paket[i]= data[i];
    }
    RF24NetworkHeader header(/*to node*/ other_node);
    bool ok = network.write(header,&payload,sizeof(payload));
    Serial.print("Sending...");
    //Serial.println(data1[0]);
    for (int i = 0; i < sizeof(data1); i++){
      Serial.print(payload.paket[i]);
    }
    Serial.println();
    for (int i = 0; i < sizeof(data1); i++){
      Serial.print(data[i]);
    }
    Serial.println();
    if (ok)
      Serial.println("ok.");
    else
      Serial.println("failed.");
  }
}

void ticking() {
  static unsigned long last_millis = 0;
  unsigned long m = millis();
  if (m - last_millis < BOUNCE_LIMIT) {
    // terjadi bounce
  } else {
    if (digitalRead(BUCKET_PIN) == LOW) {
      tick++;
    }
  }
  last_millis = millis();
}
