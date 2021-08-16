//Up & Down desk based on  https://tttapa.github.io/ESP8266/Chap10%20-%20Simple%20Web%20Server.html


#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WiFiMulti.h> 
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>

ESP8266WiFiMulti wifiMulti;     // Create an instance of the ESP8266WiFiMulti class, called 'wifiMulti'

ESP8266WebServer server(80);    // Create a webserver object that listens for HTTP request on port 80

const int led = 2;
const int up = 3;
const int down = 4;

void handleRoot();              // function prototypes for HTTP handlers
void handleUp();
void handleDown();
void handleNotFound();

void setup(void){
  Serial.begin(57600);         // Start the Serial communication to send messages to the computer
  delay(10);
  Serial.println('\n');

  pinMode(led, OUTPUT);
  pinMode(up, OUTPUT);
  pinMode(down, OUTPUT);

  wifiMulti.addAP("ssid", "password");   // add Wi-Fi networks you want to connect to
  wifiMulti.addAP("ssid_from_AP_2", "your_password_for_AP_2");
  wifiMulti.addAP("ssid_from_AP_3", "your_password_for_AP_3");

  Serial.println("Connecting ...");
  int i = 0;
  while (wifiMulti.run() != WL_CONNECTED) { // Wait for the Wi-Fi to connect: scan for Wi-Fi networks, and connect to the strongest of the networks above
    delay(250);
    Serial.print('.');
  }
  Serial.println('\n');
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());              // Tell us what network we're connected to
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());           // Send the IP address of the ESP8266 to the computer

  if (MDNS.begin("esp8266")) {              // Start the mDNS responder for esp8266.local
    Serial.println("mDNS responder started");
  } else {
    Serial.println("Error setting up MDNS responder!");
  }

  server.on("/", HTTP_GET, handleRoot);     // Call the 'handleRoot' function when a client requests URI "/"
  server.on("/Up", HTTP_POST, handleUp);
  server.on("/Down", HTTP_POST, handleDown);  // Call the 'handleLED' function when a POST request is made to URI "/LED"
  server.onNotFound(handleNotFound);        // When a client requests an unknown URI (i.e. something other than "/"), call function "handleNotFound"

  server.begin();                           // Actually start the server
  Serial.println("HTTP server started");
}

void loop(void){
  server.handleClient();                    // Listen for HTTP requests from clients
}

void handleRoot() {                         //  Form action = value to post. input type value = display on button.
  server.send(200, "text/html", "<form action=\"/Up\" method=\"POST\"><input type=\"submit\" value=\"Stand Up\"></form><form action=\"/Down\" method=\"POST\"><input type=\"submit\" value=\"Sit Down\"></form>");
}

void handleUp() {                          // If a POST request is made to URI /Up
  digitalWrite(led, LOW);
  digitalWrite(up, HIGH);
  delay(5000);
  digitalWrite(up, LOW);
  digitalWrite(led, HIGH);
  server.sendHeader("Location","/");        // Add a header to respond with a new location for the browser to go to the home page again
  server.send(303);                         // Send it back to the browser with an HTTP status 303 (See Other) to redirect
}

void handleDown() {                         // If a POST request is made to URI /Down
  digitalWrite(led, LOW);
  digitalWrite(down, HIGH);
  delay(5000);
  digitalWrite(down, LOW);
  digitalWrite(led, HIGH);
  server.sendHeader("Location","/");        // Add a header to respond with a new location for the browser to go to the home page again
  server.send(303);                         // Send it back to the browser with an HTTP status 303 (See Other) to redirect  
}

void handleNotFound(){
  server.send(404, "text/plain", "404: Not found"); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
}
