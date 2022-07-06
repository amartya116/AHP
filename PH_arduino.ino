#define SensorPin A0          // the pH meter Analog output is connected with the Arduino’s Analog
unsigned long int avgValue;  //Store the average value of the sensor feedback
float b;
int buf[10],temp;
 
void setup()
{
  pinMode(13,OUTPUT);  
  Serial.begin(9600);  
  Serial.println("Ready");    //Test the serial monitor
}
void loop()
{
  float val = analogRead(SensorPin);
  Serial.println(val);
  digitalWrite(13, HIGH);       
  delay(800);
  digitalWrite(13, LOW); 
 
}
