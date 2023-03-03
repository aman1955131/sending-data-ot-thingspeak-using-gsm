#include <SoftwareSerial.h>
#include <TinyGPS++.h>

SoftwareSerial SIM800A(7, 8); // RX, TX
TinyGPSPlus gps;

void setup() {
  Serial.begin(9600);
  SIM800A.begin(9600);

  delay(2000);

  sendATcommand("AT", "OK", 1000);

  sendATcommand("AT+SAPBR=3,1,\"airtelgprs.com\",\"airtelgprs.com\"", "OK", 10000);

   sendATcommand("AT+CSQ", "+CSQ:", 1000);

  sendATcommand("AT+SAPBR=1,1", "OK", 20000);

  sendATcommand("AT+HTTPINIT", "OK", 20000);
}

void loop() {
   sendATcommand("AT+CSQ", "+CSQ:", 1000); // signal out of 31

   sendATcommand("AT+SAPBR=1,1", "OK", 5000);
  
  while (gps.location.isValid() == false) {
    while (Serial.available()) {
      gps.encode(Serial.read());
    }
  }

  float latitude = gps.location.lat();
  float longitude = gps.location.lng();
  Serial.println(latitude);
  Serial.println(longitude);

  String url = "AT+HTTPPARA=\"URL\",\"https://api.thingspeak.com/update?api_key=GET https://api.thingspeak.com/update?api_key=85XDANANOYXX99Z0&field1=" + String(latitude, 6) + "&field2=" + String(longitude, 6) + "\"";
  sendATcommand(url, "OK", 10000);

  sendATcommand("AT+HTTPACTION=0", "+HTTPACTION: 0,200", 30000);

  delay(15000);
}

void sendATcommand(String command, String expectedResponse, unsigned int timeout) {
  SIM800A.println(command);

  long int time = millis();
  String response = "";

  while ((time + timeout) > millis()) {
    while (SIM800A.available()) {
      char c = SIM800A.read();
      response += c;
    }
  }

  if (response.indexOf(expectedResponse) == -1) {
    Serial.println(response);
    Serial.println("Error: Expected response not received.");
  } else {
    Serial.println(response);
  }
}
