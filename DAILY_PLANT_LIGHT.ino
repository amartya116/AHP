#include "ESP8266WiFi.h"
#include "NTPClient.h"
#include "WiFiUdp.h"

// WiFi parameters to be configured
const char* ssid = "Breach_v1.1"; // Write here your router's username
const char* password = "Nandemonai::"; // Write here your router's passward
const long utcOffsetInSeconds = 19800;
//char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

int hh, mm, pir;

void setup(void)
{
  Serial.begin(9600);
  // Connect to WiFi
  WiFi.begin(ssid, password);

  // while wifi not connected yet, print '.'
  // then after it connected, get out of the loop
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  //print a new line, then print WiFi connected and the IP address
  Serial.println("");
  Serial.println("WiFi connected");
  // Print the IP address
  Serial.println(WiFi.localIP());
  timeClient.begin();
  pinMode(2, OUTPUT);
  pinMode(16, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(4, INPUT);
  digitalWrite(2, HIGH);
  digitalWrite(16, HIGH);
  digitalWrite(5, HIGH);
}
void loop() {
  timeClient.update();

  //Serial.print(daysOfTheWeek[timeClient.getDay()]);

  hh = timeClient.getHours();
  mm = timeClient.getMinutes();
  pir = digitalRead(4);

  if (pir == 1) {
    Serial.println("Movement Detected");
    digitalWrite(2, LOW);
  }


  if ((pir == 1) || (hh >= 4 && hh <= 11) || (hh >= 12 && hh <= 18) || (hh >= 20 && hh <= 23)) {
    Serial.println((String)"Operating Time Active HH:" + int(hh) + ", MM:" + int(mm));
    if (mm >= 49 && mm <= 59) {
      digitalWrite(16, HIGH);
      Serial.println("Light is OFF");
      
    }
    else if (mm >= 1 && mm <= 48){
      digitalWrite(16, LOW);
      Serial.println("Light is ON");
    }
  }
  else{
    digitalWrite(16, HIGH);
    Serial.println("Light is OFF");
    }
  if (mm >= 35 && mm <= 59) {
    digitalWrite(5, LOW);
    Serial.println("Fan is ON");
  }
  else if (mm >= 1 && mm <= 34) {
    digitalWrite(5, HIGH);
    Serial.println("Fan is OFF");
  }
  if(pir==0) {
    Serial.println("No Movement Detected");
    digitalWrite(2, HIGH);
  }
  delay(1000);
}
