#include "ADS1115.h"
#include "Adafruit_Sensor.h"
#include "DHT.h"
#include "DHT_U.h"
#include "LiquidCrystal.h"//LCD Library  

//RTC LIBRARIES
  // CONNECTIONS:
  // DS1302 CLK/SCLK --> 5
  // DS1302 DAT/IO --> 4
  // DS1302 RST/CE --> 3
  // DS1302 VCC --> 3.3v - 5v
  // DS1302 GND --> GND
#include <ThreeWire.h>  
#include <RtcDS1302.h>

#define DHTPIN 2     // Digital pin connected to the DHT sensor 
#define DHTTYPE    DHT11     // DHT 


ThreeWire myWire(4,5,3); // IO, SCLK, CE
RtcDS1302<ThreeWire> Rtc(myWire);


DHT_Unified dht(DHTPIN, DHTTYPE);
uint32_t delayMS;

ADS1115 adc0(ADS1115_DEFAULT_ADDRESS);
// Wire ADS1115 ALERT/RDY pin to Arduino pin 1 it was set to previously
const int alertReadyPin = 1;


const int analogInPin = A0;
unsigned long int avgValue;
int buf[10];
float ph1=4.01, ph2=9.18, phv1=2568, phv2=2372;
float pH,EC,temp,humidity;
String dhtval, rtcdata;

LiquidCrystal lcd = LiquidCrystal(8,9,10,11,12,13);//LCD PINS 

void setup() {
  lcd.begin(16, 2);//LCD SIZE
  Wire.begin();
  
  Serial.begin(9600);

  Rtc.Begin();
   RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
    printDateTime(compiled);
    Serial.println();

    if (!Rtc.IsDateTimeValid()) 
    {
        // Common Causes:
        //    1) first time you ran and the device wasn't running yet
        //    2) the battery on the device is low or even missing

        //Serial.println("RTC lost confidence in the DateTime!");
        Rtc.SetDateTime(compiled);
    }

    if (Rtc.GetIsWriteProtected())
    {
        //Serial.println("RTC was write protected, enabling writing now");
        Rtc.SetIsWriteProtected(false);
    }

    if (!Rtc.GetIsRunning())
    {
        //Serial.println("RTC was not actively running, starting now");
        Rtc.SetIsRunning(true);
    }

    RtcDateTime now = Rtc.GetDateTime();
    if (now < compiled) 
    {
        //Serial.println("RTC is older than compile time!  (Updating DateTime)");
        Rtc.SetDateTime(compiled);
    }
  
  dht.begin();
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  dht.humidity().getSensor(&sensor);
  delayMS = sensor.min_delay / 1000;
  
  adc0.initialize(); // initialize ADS1115 16 bit A/D chip

  // We're going to do single shot sampling
  adc0.setMode(ADS1115_MODE_SINGLESHOT);
  
  // Slow things down so that we can see that the "poll for conversion" code works
  adc0.setRate(ADS1115_RATE_8);
    
  // Set the gain (PGA) +/- 6.144V
  // Note that any analog input must be higher than –0.3V and less than VDD +0.3
  adc0.setGain(ADS1115_PGA_6P144);
  // ALERT/RDY pin will indicate when conversion is ready
  
  pinMode(alertReadyPin,INPUT_PULLUP);
  adc0.setConversionReadyPinMode();
}



void loop() {
  // put your main code here, to run repeatedly:
  RtcDateTime now = Rtc.GetDateTime();
  pH = phvalue();
  EC = ecvalue();
  dhtval = temperature();
  rtcdata = printDateTime(now);
//  Serial.print(pH);
//  Serial.print(" ");
//  Serial.print(EC);
//  Serial.print(" "+dhtval);
//  Serial.print(" "+rtcdata);
  Serial.println((String)" "+pH+" "+EC+" "+dhtval+" "+rtcdata);
  
    lcd.clear();
    lcd.setCursor(0, 0);                                             
    lcd.print((String)"pH:"+pH);                                      

    lcd.setCursor(8, 0);                                             
    lcd.print((String)"TDS:"+EC); 
                                                        
    lcd.setCursor(0, 1);
    lcd.print((String)"DHT:"+dhtval);

     
  delay(500);
}




float phvalue(){
  for(int i=0;i<10;i++) 
   { 
    adc0.setMultiplexer(ADS1115_MUX_P0_NG);
    adc0.triggerConversion();
    pollAlertReadyPin();
    buf[i] =  adc0.getMilliVolts(false);
    delay(10);
   }
 for(int i=0;i<9;i++)
   {
    for(int j=i+1;j<10;j++)
    {
     if(buf[i]>buf[j])
     {
      temp=buf[i];
      buf[i]=buf[j];
      buf[j]=temp;
     }
    }
    
   }
 avgValue=0;
 for(int i=2;i<8;i++)
 avgValue+=buf[i];
 float pHVol=(float)avgValue/6;
// float phValue;
// phValue=ph1+(pHVol-phv1)*(ph2-ph1)/(phv2-phv1);
 return pHVol;
}



float ecvalue(){
   for(int i=0;i<10;i++) 
   { 
    adc0.setMultiplexer(ADS1115_MUX_P1_NG);
    adc0.triggerConversion();
    pollAlertReadyPin();
    buf[i] =  adc0.getMilliVolts(false);
    delay(10);
   }
 for(int i=0;i<9;i++)
   {
    for(int j=i+1;j<10;j++)
    {
     if(buf[i]>buf[j])
     {
      temp=buf[i];
      buf[i]=buf[j];
      buf[j]=temp;
     }
    }
    
   }
 avgValue=0;
 for(int i=2;i<8;i++)
 avgValue+=buf[i];
 float ECVol=(float)avgValue/6;
// float TDS;
// TDS=ECVol*15.245;
 return ECVol;
  }



String temperature(){
   delay(delayMS);
  // Get temperature event and print its value.
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    temp = 0;
  }
  else {
    temp = event.temperature;
  }
  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    humidity = 0;
  }
  else {
    humidity = event.relative_humidity;
  }
  String res = String(temp) +" "+ String(humidity);
  return res;
  }



void pollAlertReadyPin() {
  for (uint32_t i = 0; i<100000; i++)
    if (!digitalRead(alertReadyPin)) return;
}



/*>>>RTC<<<*/
#define countof(a) (sizeof(a) / sizeof(a[0]))
String printDateTime(const RtcDateTime& dt)
{
    char datestring[20];

    snprintf_P(datestring, 
            countof(datestring),
            PSTR("%02u-%02u-%04u/%02u:%02u:%02u"),
            dt.Day(),
            dt.Month(),
            dt.Year(),
            dt.Hour(),
            dt.Minute(),
            dt.Second() );
    return datestring;
}
