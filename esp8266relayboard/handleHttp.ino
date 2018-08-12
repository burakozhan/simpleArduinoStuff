/** Handle root or redirect to captive portal */
void handleRoot() {
  digitalWrite(r5,1);
  if (captivePortal()) { // If caprive portal redirect instead of displaying the page.
    return;
  }
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/html", ""); // Empty content inhibits Content-length header so we have to close the socket ourselves.
  server.sendContent(
    "<html><head></head><body>"
    "<h1>HELLO WORLD!!</h1>"
  );
  if (server.client().localIP() == apIP) {
    server.sendContent(String("<p>You are connected through the soft AP: ") + softAP_ssid + "</p>");
  } else {
    server.sendContent(String("<p>You are connected through the wifi network: ") + ssid + "</p>");
  }
  server.sendContent(
    "<p>You may want to <a href='/wifi'>config the wifi connection</a>.</p>"
    "<p>You also may want to <a href='/led'>configure the LEDs</a>.</p>"
    "</body></html>"
  );
  server.client().stop(); // Stop is needed because we sent no content length
  digitalWrite(r5,0);
}

/** Redirect to captive portal if we got a request for another domain. Return true in that case so the page handler do not try to handle the request again. */
boolean captivePortal() {
  digitalWrite(r4,1);
  if (!isIp(server.hostHeader()) && server.hostHeader() != (String(myHostname)+".local")) {
    Serial.print("Request redirected to captive portal");
    server.sendHeader("Location", String("http://") + toStringIp(server.client().localIP()), true);
    server.send ( 302, "text/plain", ""); // Empty content inhibits Content-length header so we have to close the socket ourselves.
    server.client().stop(); // Stop is needed because we sent no content length
    digitalWrite(r4,0);
    return true;
  }
  digitalWrite(r4,0);
  return false;
}

/** Wifi config page handler */
void handleWifi() {
  digitalWrite(r3,1);
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/html", ""); // Empty content inhibits Content-length header so we have to close the socket ourselves.
  server.sendContent(
    "<html><head></head><body>"
    "<h1>Wifi config</h1>"
  );
  if (server.client().localIP() == apIP) {
    server.sendContent(String("<p>You are connected through the soft AP: ") + softAP_ssid + "</p>");
  } else {
    server.sendContent(String("<p>You are connected through the wifi network: ") + ssid + "</p>");
  }
  server.sendContent(
    "\r\n<br />"
    "<table><tr><th align='left'>SoftAP config</th></tr>"
  );
  server.sendContent(String() + "<tr><td>SSID " + String(softAP_ssid) + "</td></tr>");
  server.sendContent(String() + "<tr><td>IP " + toStringIp(WiFi.softAPIP()) + "</td></tr>");
  server.sendContent(
    "</table>"
    "\r\n<br />"
    "<table><tr><th align='left'>WLAN config</th></tr>"
  );
  server.sendContent(String() + "<tr><td>SSID " + String(ssid) + "</td></tr>");
  server.sendContent(String() + "<tr><td>IP " + toStringIp(WiFi.localIP()) + "</td></tr>");
  server.sendContent(
    "</table>"
    "\r\n<br />"
    "<table><tr><th align='left'>WLAN list (refresh if any missing)</th></tr>"
  );
  Serial.println("scan start");
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n > 0) {
    for (int i = 0; i < n; i++) {
      server.sendContent(String() + "\r\n<tr><td>SSID " + WiFi.SSID(i) + String((WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":" *") + " (" + WiFi.RSSI(i) + ")</td></tr>");
    }
  } else {
    server.sendContent(String() + "<tr><td>No WLAN found</td></tr>");
  }
  server.sendContent(
    "</table>"
    "\r\n<br /><form method='POST' action='wifisave'><h4>Connect to network:</h4>"
    "<input type='text' placeholder='network' name='n'/>"
    "<br /><input type='password' placeholder='password' name='p'/>"
    "<br /><input type='submit' value='Connect/Disconnect'/></form>"
    "<p>You may want to <a href='/'>return to the home page</a>.</p>"
    "</body></html>"
  );
  server.client().stop(); // Stop is needed because we sent no content length
  digitalWrite(r3,0);
}

/** Handle the WLAN save form and redirect to WLAN config page again */
void handleWifiSave() {
  Serial.println("wifi save");
  server.arg("n").toCharArray(ssid, sizeof(ssid) - 1);
  server.arg("p").toCharArray(password, sizeof(password) - 1);
  server.sendHeader("Location", "wifi", true);
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.send ( 302, "text/plain", "");  // Empty content inhibits Content-length header so we have to close the socket ourselves.
  server.client().stop(); // Stop is needed because we sent no content length
  saveCredentials();
  connect = strlen(ssid) > 0; // Request WLAN connect with new credentials if there is a SSID
}

void handleNotFound() {
  digitalWrite(r2,1);
  if (captivePortal()) { // If caprive portal redirect instead of displaying the error page.
    return;
  }
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for ( uint8_t i = 0; i < server.args(); i++ ) {
    message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
  }
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.send ( 404, "text/plain", message );
  digitalWrite(r2, 0);
}

void handleLed(){
  Serial.println("do LED stuff");
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/html", ""); // Empty content inhibits Content-length header so we have to close the socket ourselves.
  server.sendContent(
    "<html><head></head><body>"
    "<h1>LED stuff config</h1>"
  );

  String message = "";
  for (int i = 0; i < server.args(); i++) {
      message += "Arg nº" + (String)i + " –> ";
      message += server.argName(i) + ": ";
      message += server.arg(i) + "\n";
      setRelay(server.argName(i), getState(server.arg(i)));
  } 
  Serial.println(message);

  server.sendContent(
    "<table border=\"1\">"
    "<tr><th>Relay 1</th><th>Relay 2</th><th>Relay 3</th><th>Relay 4</th><th>Relay 5</th><th>Relay 6</th></tr>"
    "\n<tr><td>"
    +getHumanReadable(r1state)+
    "</td><td>"
    +getHumanReadable(r2state)+
    "</td><td>"
    +getHumanReadable(r3state)+
    "</td><td>"
    +getHumanReadable(r4state)+
    "</td><td>"
    +getHumanReadable(r5state)+
    "</td><td>"
    +getHumanReadable(r6state)+
    "</td></tr>"
    "\n<tr><td>"
    "<a href='/led?r1=ON'>ON</a>"
    "</td><td>"
    "<a href='/led?r2=ON'>ON</a>"
    "</td><td>"
    "<a href='/led?r3=ON'>ON</a>"
    "</td><td>"
    "<a href='/led?r4=ON'>ON</a>"
    "</td><td>"
    "<a href='/led?r5=ON'>ON</a>"
    "</td><td>"
    "<a href='/led?r6=ON'>ON</a>"
    "</td></tr>"
    "\n<tr><td>"
    "<a href='/led?r1=OFF'>OFF</a>"
    "</td><td>"
    "<a href='/led?r2=OFF'>OFF</a>"
    "</td><td>"
    "<a href='/led?r3=OFF'>OFF</a>"
    "</td><td>"
    "<a href='/led?r4=OFF'>OFF</a>"
    "</td><td>"
    "<a href='/led?r5=OFF'>OFF</a>"
    "</td><td>"
    "<a href='/led?r6=OFF'>OFF</a>"
    "</td></tr>"
    "\n</table>"
  );

  server.sendContent(
    "</br></br>We are done here."
    "<p>You may want to <a href='/'>return to the home page</a>.</p>"
    "</body></html>"
  );
  setAllRelays();
  server.client().stop(); // Stop is needed because we sent no content length
  Serial.println("LED is done");
}

void setRelay(String name, boolean value){
  if(String(name) == String("r1")){
    r1state = value;
  } else if(String(name) == String("r2")){
    r2state = value;
  } else if(String(name) == String("r3")){
    r3state = value;
  } else if(String(name) == String("r4")){
    r4state = value;
  } else if(String(name) == String("r5")){
    r5state = value;
  } else if(String(name) == String("r6")){
    r6state = value;
  }
}

boolean getState(String value){
  if(String(value) == String("ON")) return true;
  if(String(value) == String("on")) return true;
  if(String(value) == String("TRUE")) return true;
  if(String(value) == String("true")) return true;
  if(String(value) == String("1")) return true;
  return false;
}

void setAllRelays(void){
  digitalWrite(r1,r1state);
  digitalWrite(r2,r2state);
  digitalWrite(r3,r3state);
  digitalWrite(r4,r4state);
  digitalWrite(r5,r5state);
  digitalWrite(r6,r6state);
}

String getHumanReadable(boolean state){
  if (state) {
    return String("ON");
  } else {
    return String("OFF");
  }
}



