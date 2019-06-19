

/*
* Requires
*/
// General
#include <Arduino.h>
#include <Hash.h>

// Wireless 
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

// Socket
#include <WebSockets.h>
#include <WebSocketsClient.h>
#include <WebSocketsServer.h>

/*
* Attributes
*/
// General
#define serial_console Serial1
int SERIAL_BAUD = 9600;

// Wireless 
ESP8266WiFiMulti WiFiMulti;
char* WiRLESS_SSID = "Bar9ou9";
char* WiRLESS_PASSWORD = "3achmenchafek";

// Socket
WebSocketsClient webSocket;
bool isConnected = false;
#define INTERVAL_EMIT_LOCATION 3000
uint64_t messageTimestamp = 0;
char* SOCKET_LINK = "192.168.1.2";
int SOCKET_PORT = 3000;


// GPS

#include <SoftwareSerial.h> 
#include "TinyGPS++.h"

int led = D8;
TinyGPSPlus gps;   
SoftwareSerial SoftSerial(D2, D3); // TX/RX 

 // Location lat-lng
      String data_latitude =String("N/A");
      String data_longitude =String("N/A");
     
      // Location altitude-asimuth
      String data_altitude = String("N/A");
    
      // Speed
      String data_speed =String("N/A");
     
      // Date
      String date_year =String("N/A");
      String date_month =String("N/A");
      String date_day =String("N/A");
    
      // Time
      String time_hour =String("N/A");
      String time_minute =String("N/A");
      String time_second =String("N/A");

     // Satellites
     String data_satellites =String("N/A");
     
/*
* Main Cycles
*/
// Main configuration
void setup() {
 pinMode(led, OUTPUT);
//  pinMode(led2, OUTPUT);
  SoftSerial.begin(9600);  // the SoftSerial baud rate 
//  Serial.begin(SERIAL_BAUD);      // the Serial port of Arduino baud rate. 
//  Serial.println("HELLO");

  // Serial configuration
  configuration_serial();
  
  // Wireless configuration
  configuration_wireless();
  
  // Socket configuration
  configuration_socket();
  
}
// Main loop
void loop() {

  // Keep socket events allive  
  webSocket.loop();

  // Check if the socket is connected
  if(isConnected) {
      // The socket is actif 
      uint64_t now = millis(); // save curent time (to use for intervals)

      // Check the interval between NOW & last checkPoint
      if(now - messageTimestamp > INTERVAL_EMIT_LOCATION) {
          messageTimestamp = now;
            
            String test = "42[\"entity_location\",{";
                    // Manifacture
                    
                    // Model
                    
                    // provider
                    test.concat("\"provider\": \"gps\"");

                    // curent satellites
                    test.concat(",\"curent_satellites\": \"");
                    test.concat(data_satellites);
                    test.concat("\"");

                    // LatLong
                    test.concat(",\"lat_lon\":[\"");
                    test.concat(data_latitude);
                    test.concat("\",\"");
                    test.concat(data_longitude);
                    test.concat("\"]");
                    
                    // Altitude
                    test.concat(",\"altitude\" : \"");
                    test.concat(data_altitude);
                    test.concat("\"");
                    
                    // Accuracy
                    test.concat(",\"accuracy\": \"");
                    test.concat("N/A"); //TODO
                    test.concat("\"");
                   
                      // Speed
                    test.concat(",\"speed\": \"");
                    test.concat(data_speed);
                    test.concat("\"");

                    // Time
                    test.concat(",\"time\": \"");
                    test.concat("N/A"); //TODO
                    test.concat("\"");

                    // elapsedRealTimeNanostime
                    test.concat(",\"elapsedRealTimeNanostime\": \"");
                    test.concat("N/A"); //TODO
                    test.concat("\"");
                    
                    // Date
                    test.concat(",\"date\": \"");
                    test.concat(date_year); //TODO
                    test.concat("/");
                    test.concat(date_month); //TODO
                    test.concat("/");
                    test.concat(date_day); //TODO
                    test.concat(";");
                    test.concat(time_hour); //TODO
                    test.concat(":");
                    test.concat(time_minute ); //TODO
                    test.concat(":");
                    test.concat(time_second); //TODO
                    test.concat("\"");
                    
            test.concat("}]");
            char buff[test.length()+1];
            test.toCharArray(buff, test.length()+1);
//          char*a = "42[\"entity_location\",{\"lat_lon\":[0,-3]}]";
//          Serial.println(a[30]);
//          a[30] = 'e';
//            webSocket.sendTXT("42[\"entity_location\",{\"lat_lon\":[0,-3]}]"); // example emit: sendTXT [42"event", JSON_OBJ]
            webSocket.sendTXT(buff);
      }   
  }

  while (SoftSerial.available() > 0){
    
    // Decode the values from GPS INPUT
    gps.encode(SoftSerial.read());

    // Check if we decoded a Readable location
    if(isLocationFixed()){
      digitalWrite(led, HIGH); // turn the LED on (HIGH is the voltage level)
      Serial.println("----------------------------------------");

      // Location lat-lng
//   data_latitude = ("%.001f",gps.location.lat(),10);
      data_latitude = String(gps.location.lat(),10);
      data_longitude = String(gps.location.lng(),10);
//      Serial.print("LAT: "); 
//      Serial.println(data_latitude , 10);
//      Serial.print("LNG: "); 
//      Serial.println(data_longitude, 10);
//      
      // Location altitude-asimuth
      data_altitude = String(gps.altitude.value(),10);
//      Serial.print("Altitude: "); 
//      Serial.println(data_altitude, 2); 
//      
      // Speed
       data_speed =  String(gps.speed.mps(),2);
//      Serial.print("Speed: "); 
//      Serial.print(data_speed); Serial.println(" m/s");
            
      // Date
      date_year = String(gps.date.year());
      date_month = String(gps.date.month());
      date_day = String(gps.date.day());
//      Serial.print("Date: "); 
//      Serial.print(data_year); Serial.print("/");
//      Serial.print(data_month); Serial.print("/");
//      Serial.print(data_day); Serial.println(" ");
      
       // Time
       time_hour = String(gps.time.hour());
       time_minute = String(gps.time.minute());
       time_second = String(gps.time.second());
//      Serial.print("Time: ");
//      Serial.print(time_hour); Serial.print(":");
//      Serial.print(time_minute); Serial.print(":");
//      Serial.print(time_second); Serial.println(" ");
      
       // Satellites
       data_satellites = String(gps.satellites.value());
//      
//      Serial.println(gps.satellites.value());
    }
    else{
       digitalWrite(led, LOW); // turn the LED on (HIGH is the voltage level)
    }
  
  }
}

// Do we have a location ?
boolean isLocationFixed(){
//    return  true;
     return (gps.satellites.value()>=2) && gps.location.isUpdated();
}




/*
* Methodes
*/
// Serial configuration
void configuration_serial(){
  // serial_console.begin(921600);
  serial_console.begin(SERIAL_BAUD);
  //Serial.setDebugOutput(true);
  serial_console.setDebugOutput(true);
//  serial_console.println("");
//  serial_console.println("");
//  serial_console.println("");
  for(uint8_t t = 4; t > 0; t--) {
      serial_console.printf("[SETUP] BOOT WAIT %d...\n", t);
      serial_console.flush();
      delay(1000);
  }

}
// Wireless configuration
void configuration_wireless(){
  WiFiMulti.addAP(WiRLESS_SSID, WiRLESS_PASSWORD);
  //WiFi.disconnect();
  while(WiFiMulti.run() != WL_CONNECTED) {
      delay(100);
  }
}
// Socket configuration
void configuration_socket(){
  
  webSocket.beginSocketIO(SOCKET_LINK, SOCKET_PORT);
  webSocket.setExtraHeaders();
  webSocket.setExtraHeaders("token: eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJlbnRpdHlfbmFtZSI6Im5vZGUxIiwiZW50aXR5X3R5cGUiOiJJT1Rfbm9kZSIsImVudGl0eV9tYWMiOiIiLCJjX21hbmFnZXIiOiI1Y2RiYTY4ZmUxZTZlMzBhOTk4YTI5ZmQiLCJvcGVyYXRvciI6IjVjZGJhNmNmZTFlNmUzMGE5OThhMjlmZSIsInBvb2xfbmFtZSI6InAxIiwiaWF0IjoxNTU5MzEwODg2fQ.149h6KF5KCjNbpE0IjUWivrmYslrJsCQl9TAQmej3XA\r\nbar: not");
  //webSocket.setAuthorization("user", "Password"); // HTTP Basic Authorization

  webSocket.onEvent(webSocketEvent);

  
  

}
// Socket Events
void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
    switch(type) {
        case WStype_DISCONNECTED:
            serial_console.printf("[WSc] Disconnected!\n");
            isConnected = false;
            break;
        case WStype_CONNECTED:
            {
                serial_console.printf("[WSc] Connected to url: %s\n",  payload);
                isConnected = true;

          // send message to server when Connected
                // socket.io upgrade confirmation message (required)
        webSocket.sendTXT("5");
            }
            break;
        case WStype_TEXT:
            {
                // #####################
                // handle STOMP protocol
                // #####################

                String text = (char*) payload;
                serial_console.printf("[WSc] get text: %s\n", payload);

                if (text.startsWith("CONNECTED")) {

                    // subscribe to some channels

                    String msg = "SUBSCRIBE\nid:sub-0\ndestination:/user/queue/messages\n\n";
                    sendMessage(msg);
                    delay(1000);

                    // and send a message

                    msg = "SEND\ndestination:/app/message\n\n{\"user\":\"esp\",\"message\":\"Hello!\"}";
                    sendMessage(msg);
                    delay(1000);
                    
                } else {

                    // do something with messages
                    
                }

                break;
            }
            
        case WStype_BIN:
            serial_console.printf("[WSc] get binary length: %u\n", length);
            hexdump(payload, length);

            // send data to server
            // webSocket.sendBIN(payload, length);
            break;
    }

}

void sendMessage(String & msg) {
    webSocket.sendTXT(msg.c_str(), msg.length() + 1);
}
