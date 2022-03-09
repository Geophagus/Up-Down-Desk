//Up & Down desk based on  https://tttapa.github.io/ESP8266/Chap10%20-%20Simple%20Web%20Server.html
//Added fancy schmancy html
//Added reset function
//Added height variable so its position is known. Now won't go 'up' if already 'up' and vice versa.
//Added error checking in Serial.Println()
//fixed big error in code - can't have two buttons on the same line. At least not in the way I was doing it.
//added position notification and error page when desk is already in position
//tidied up position indicator to string
//Added flashing IP to notify last 2/3 digits of IP address
//added fix-it function

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WiFiMulti.h> 
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>

ESP8266WiFiMulti wifiMulti;     // Create an instance of the ESP8266WiFiMulti class, called 'wifiMulti'
ESP8266WebServer server(80);    // Create a webserver object that listens for HTTP request on port 80

const int led = 2;
const int up = D6;
const int down = D7;
String posit;

void setup(void){
  
  pinMode(led, OUTPUT);
  pinMode(up, OUTPUT);
  pinMode(down, OUTPUT);
  digitalWrite(led, HIGH);
  
  Serial.begin(115200);         // Start the Serial communication to send messages to the computer
  delay(10);
  Serial.println('\n');

  wifiMulti.addAP("SSID1", "Pass1");   // add Wi-Fi networks you want to connect to
  wifiMulti.addAP("SSID2", "Pass2");
  wifiMulti.addAP("ssid_from_AP_3", "your_password_for_AP_3");

  Serial.println("Connecting ...");
  int i = 0;
  while (wifiMulti.run() != WL_CONNECTED) { // Wait for the Wi-Fi to connect: scan for Wi-Fi networks, and connect to the strongest of the networks above
    delay(250);
    Serial.print('.');
  }
  Serial.println('\n');                     //I don't know what this does.
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());              // Tell us what network we're connected to
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());           // Send the IP address of the ESP8266 to the computer
  
//Display IP address via flashing onboard LED 
  IPAddress ip = WiFi.localIP();            // save IP address as ip
  int last3 = ip[3];                        // save last couplet of IP as last3
  unsigned ip1 = (last3 / 100U) % 10;       // Save last three digits separately
  unsigned ip2 = (last3 / 10U) % 10;
  unsigned ip3 = (last3 / 1U) % 10;
  Serial.println(ip1);  
  Serial.println(ip2);
  Serial.println(ip3);

// flash onboard LED the the last numbers of IP address. Repeat it 2 times incase you are a doofus and not paying attention.
  for (int doofus = 0; doofus < 2; doofus++) {   
    for (int i = 0; i < ip1; i++) {
      digitalWrite(led, LOW);
      delay(500);
      digitalWrite(led, HIGH);
      delay(250);
    }
    
    delay(750);
    
    for (int i = 0; i < ip2; i++) {
      digitalWrite(led, LOW);
      delay(500);
      digitalWrite(led, HIGH);
      delay(250);
    }

    delay(750);

    for (int i = 0; i < ip3; i++) {
      digitalWrite(led, LOW);
      delay(500);
      digitalWrite(led, HIGH);
      delay(250);
    }
  delay(1500);
  }

//Start server
  
  if (MDNS.begin("esp8266")) {              // Start the mDNS responder for esp8266.local
    Serial.println("mDNS responder started");
  } else {
    Serial.println("Error setting up MDNS responder!");
  }

  server.on("/", HTTP_GET, handleRoot);     // Call the 'handleRoot' function when a client requests URI "/"
  server.on("/Up", HTTP_POST, handleUp);
  server.on("/Down", HTTP_POST, handleDown);  
  server.on("/Reset", HTTP_POST, handleReset);
  server.on("/Fix", HTTP_POST, handleFix);
  server.onNotFound(handleNotFound);        // When a client requests an unknown URI (i.e. something other than "/"), call function "handleNotFound"

  server.begin();                           // Actually start the server
  Serial.println("HTTP server started");

//Sets desk to initial standing height
  digitalWrite(up, HIGH);
  delay(1000);
  digitalWrite(up, LOW);
  digitalWrite(down, HIGH);
  delay(500);
  digitalWrite(down, LOW);
  posit = "raised"; 
  Serial.println(posit);
}

void loop(void){
  server.handleClient();                    // Listen for HTTP requests from clients
}

void handleRoot() {                         //  Form action = value to post. input type value = display on button.
  String webpage;
      webpage += "<!DOCTYPE HTML>\r\n";
      webpage += "<html>\r\n";
      webpage += "<center>";
      webpage += "<br>";
      webpage += "<br>";
      webpage += "<br>";
      webpage += "<header><title>ESP8266 Desk Control</title><h1>Olle's Standing Desk</h1></header>";
      webpage += "<head>";    
      webpage += "<meta name='mobile-web-app-capable' content='yes' />";
      webpage += "<meta name='viewport' content='width=device-width' />";
      webpage += "<style>body { font-family: Arial, Helvetica, Sans-Serif; Color: #000000; }\"</style>"; // for reference: "<style>body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000000; }\"</style>";
      webpage += "</head>";
      webpage += "<br>";
      webpage += "<body>";
      webpage += "<form action=\"/Up\" method=\"POST\"><input type=\"submit\" value=\"Stand Up\"></form>";
      webpage += "<br>";
      webpage += "<form action=\"/Down\" method=\"POST\"><input type=\"submit\" value=\"Sit Down\"></form>";
      webpage += "<br>";
      webpage += "<br>";
      webpage += "<h4>Desk is currently " + String(posit) + ".</h4>";
      webpage += "<br>";
      webpage += "<form action=\"/Reset\" method=\"POST\"><input type=\"submit\" value=\"Reset\"></form>";
      webpage += "<br>";
      webpage += "<br>";
      webpage += "<br>";
      webpage += "<br>";
      webpage += "<br>";
      webpage += "<br>";
      webpage += "<br>";
      webpage += "<br>";
      webpage += "<form action=\"/Fix\" method=\"POST\"><input type=\"submit\" value=\"Fix it! Fix it! Fix it!\"></form>";
      webpage += "</body>";
      webpage += "</center>";

  server.send(200, "text/html", webpage);
}

void handleUp() {                          // If a POST request is made to URI /Up

  if(posit == "lowered") {
    digitalWrite(led, LOW);
    digitalWrite(up, HIGH);
    Serial.println("Going Up");
    delay(5000);
    digitalWrite(up, LOW);
    digitalWrite(led, HIGH);
    posit = "raised";
    Serial.println(posit);
    }
  else{
    Serial.println("Already Up");
    String uppage;
      uppage += "<!DOCTYPE HTML>\r\n";
      uppage += "<html>\r\n";
      uppage += "<center>";
      uppage += "<br>";
      uppage += "<br>";
      uppage += "<br>";
      uppage += "<header><title>ESP8266 Desk Control</title><h1>Desk is too damn high.</h1></header>";
      uppage += "<head>";    
      uppage += "<meta name='mobile-web-app-capable' content='yes' />";
      uppage += "<meta name='viewport' content='width=device-width' />";
      uppage += "<meta http-equiv=\"refresh\" content=\"0;url=/\">"; //refresh to root
      uppage += "<style>body { font-family: Arial, Helvetica, Sans-Serif; Color: #000000; }\"</style>";
      uppage += "</head>";
      uppage += "</center>";
    server.send(200, "text/html", uppage);
    delay(3000);
  }
  server.sendHeader("Location","/");        // Add a header to respond with a new location for the browser to go to the home page again
  server.send(303);                         // Send it back to the browser with an HTTP status 303 (See Other) to redirect 
}

void handleDown() {                         // If a POST request is made to URI /Down
  if(posit == "raised") {
    digitalWrite(led, LOW);
    digitalWrite(down, HIGH);
    Serial.println("Going Down");
    delay(5000);
    digitalWrite(down, LOW);
    digitalWrite(led, HIGH);
    posit = "lowered";
    Serial.println(posit);
    }
    else{
      Serial.println("Already Down");
      String downpage;
      downpage += "<!DOCTYPE HTML>\r\n";
      downpage += "<html>\r\n";
      downpage += "<center>";
      downpage += "<br>";
      downpage += "<br>";
      downpage += "<br>";
      downpage += "<header><title>ESP8266 Desk Control</title><h1>Desk is too damn low.</h1></header>";
      downpage += "<head>";    
      downpage += "<meta name='mobile-web-app-capable' content='yes' />";
      downpage += "<meta name='viewport' content='width=device-width' />";
      downpage += "<meta http-equiv=\"refresh\" content=\"0;url=/\">"; //refresh to root
      downpage += "<style>body { font-family: Arial, Helvetica, Sans-Serif; Color: #000000; }\"</style>";
      downpage += "</head>";
      downpage += "</center>";
      server.send(200, "text/html", downpage);
      delay(3000);
    }
  server.sendHeader("Location","/");        // Add a header to respond with a new location for the browser to go to the home page again
  server.send(303);// Send it back to the browser with an HTTP status 303 (See Other) to redirect 
}

void handleReset() {                        //to reset the desk to a known position. Goes up as far as it can go and then down to standing height.
  server.sendHeader("Location","/");
  server.send(303);
  digitalWrite(led, LOW);
  digitalWrite(up, HIGH);
  delay(10000);
  digitalWrite(up, LOW);
  digitalWrite(down,HIGH);
  delay(5000);
  digitalWrite(down,LOW);
  posit = "raised"; 
  digitalWrite(led, HIGH);
  Serial.println(posit);
}

void handleFix() {                        //to reset the desk if it is stuck. All the way down, down a bit more and then up to standing
  String timer ="";
  for (int i = 5; i >= 0; i--){
      Serial.println("CountDown");
      Serial.println(i);
      timer = String(i);  //converts i to a string for the countdown.
      String countdown;      
      countdown += "<!DOCTYPE HTML>\r\n";
      countdown += "<html>\r\n";
      countdown += "<center>";
      countdown += "<br>";
      countdown += "<br>";
      countdown += "<br>";
      countdown += "<header><title>ESP8266 Desk Control</title><h1>Clear the Area!</h1></header>";
      countdown += "<br>";
      countdown += "<header><title>ESP8266 Desk Control</title><h2>Resetting in: " + String(timer) + "</h2></header>";
      countdown += "<br>";
      countdown += "<head>";    
      countdown += "<meta name='mobile-web-app-capable' content='yes' />";
      countdown += "<meta name='viewport' content='width=device-width' />";
      countdown += "<meta http-equiv=\"refresh\" content=\"0;url=/\">"; //refresh to root
      countdown += "<style>body { font-family: Arial, Helvetica, Sans-Serif; Color: #000000; }\"</style>";
      countdown += "</head>";
      countdown += "</center>";
      server.send(200, "text/html", countdown);
      delay(1000);
     
  }
  
  digitalWrite(led, LOW);
  digitalWrite(down, HIGH);
  delay(15000);
  digitalWrite(down, LOW);
  delay(300);
  digitalWrite(down,HIGH);
  delay(3000);
  digitalWrite(down,LOW);
  digitalWrite(up,HIGH);
  delay(5000);
  digitalWrite(up,LOW);
  posit = "lowered"; 
  digitalWrite(led, HIGH);
  Serial.println(posit);
  server.sendHeader("Location","/");
  server.send(303);
}

void handleNotFound(){
  server.send(404, "text/plain", "Are you sitting at the right desk?"); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
}
