
/**
 * using RF24Network,
 *
 * RECEIVER NODE
 * Listens for messages from the transmitter and prints them out.
 */

#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>
#include <ArduinoJson.h>


RF24 radio(2,4);                // nRF24L01(+) radio attached using Getting Started board 

RF24Network network(radio);      // Network uses that radio
const uint16_t this_node = 0;    // Address of our node in Octal format ( 04,031, etc)
const uint16_t other_node = 2;
// Address of the other node in Octal format
String data;
char datareceived[200];
struct payload_t {                  // Structure of our payload
  char paket[100];
};
//char json[100];


void setup(void)
{
  Serial.begin(9600);
  Serial.println("RF24Network/examples/helloworld_rx/");
 
  SPI.begin();
  radio.begin();
  network.begin(/*channel*/ 90, /*node address*/ this_node);
}

void loop(void){
  
  network.update();                  // Check the network regularly
  StaticJsonDocument<200> doc;
  
  while ( network.available() ) {     // Is there anything ready for us?
    
    RF24NetworkHeader header;        // If so, grab it and print it out
    payload_t payload;
    network.read(header,&payload,sizeof(payload));
    radio.read( &datareceived, sizeof(datareceived) );
    //Serial.print(payload.paket);
    DeserializationError error = deserializeJson(doc, payload.paket);
    if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
    }
    unsigned long up_since = doc["up-since"]; 
    unsigned long tick = doc["tick"];
    Serial.print("up-since : ");
    Serial.println(up_since);
    Serial.print("tick : ");
    Serial.println(tick);
  }
}
