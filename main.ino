#define BLYNK_PRINT Serial 
#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>
#include <DHT.h>
#include <Servo.h>
#include <BMP280_DEV.h>   
#include <LiquidCrystal.h>
#define DHTPIN 2
#define DHTTYPE DHT11  
#define ESP8266_BAUD 9600   

                



char auth[] = "948ca8cad69348feaf7c8f1de074afc1";
char ssid[] = "Xperia";
char pass[] = "987654321";

#include <SoftwareSerial.h>
SoftwareSerial EspSerial(11,10); 
BlynkTimer timer; 

WidgetLED moni(V2);
WidgetLED busy(V3);
WidgetLED ini(V4);
int lowerLimit = 10,curmode = 3; float waterAmount = 0.1;
const int motorPin = 13;
const int sensorPin = A0;
int timerID;
ESP8266 wifi(&EspSerial);
DHT dht(DHTPIN, DHTTYPE);
int ldrPin = A2;
Servo servo_test; 
int val = 0;
int angle = 0;
float temperature, pressure=911.60, altitude = 954.12 ;            
BMP280_DEV bmp280; 
LiquidCrystal lcd(15, 14, 43, 42, 39, 38);
int curr=0;
float h,t;

void setup() {
   Serial.begin(9600);
   EspSerial.begin(ESP8266_BAUD);
   delay(10);
   Blynk.begin(auth, wifi, ssid, pass);
   dht.begin();
   sendSensor();
   mode(0);
   pinMode(motorPin,OUTPUT);
   digitalWrite(motorPin,HIGH);
   pinMode(sensorPin,INPUT);
   delay(1000);
   timerID = timer.setInterval(10000L, monitoringEvent);
   Blynk.notify("Your {DEVICE_NAME} is online.");
   mode(1);
    pinMode(ldrPin, INPUT);
//  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
 bmp280.begin(BMP280_I2C_ALT_ADDR);  
  servo_test.attach(19);
  servo_test.write(angle);
  bmp280.setTimeStandby(TIME_STANDBY_250MS);    
  bmp280.startNormalConversion(); 
  lcd.begin(16, 2);
  analogWrite(44,100);     
  monitoringEvent();   
  
}

void loop() {
  Blynk.run();
  timer.run(); 
       
}

BLYNK_CONNECTED() {  
  Blynk.syncVirtual(V10);  
 }

void monitoringEvent() 
{
    mode(0);
    int x = accusread();
    Blynk.virtualWrite(V1,x);
    readPressure();
    lightSensor();
    sendSensor();
    printlcd();
    
    delay(2000);
    
        if(x <= lowerLimit) {
      watering();
      monitoringEvent();
     }
     else {    
      delay(1000);
     monitoringEvent();}
     
}
int accusread() {
  
  curr = analogRead(sensorPin);
  curr = convertToPercent(curr); 
  Serial.print("Water level = " );
  Serial.print(curr);
  Serial.print("  ");
  return curr;
}
BLYNK_WRITE(V10) {
  waterAmount = param.asFloat();
}

void watering() {
   mode(2);
  timer.deleteTimer(timerID);
  Blynk.notify("Your plant is being watered.");
  digitalWrite(motorPin,LOW);
  delay(350);
  digitalWrite(motorPin,HIGH);
 
}

void mode(int m) { 
  if(curmode == m) 
    return;
  moni.off(); 
  ini.off();
  busy.off();
  if(m==0) 
    ini.on();
  else if(m==1)
    moni.on();
  else if(m==2)
    busy.on();
  curmode = m;
}

int convertToPercent(int value)
{
  int percentValue = 0;
  percentValue = map(value, 1023, 280, 0, 100); 
  return percentValue;
}

void sendSensor()
{
  h = dht.readHumidity();
  t = dht.readTemperature();
  Serial.print("Temperature=");
  Serial.print(t);
  Serial.print("  ");
  Serial.print("Humidity=");
  Serial.print(h);
  Serial.print("  ");
  Blynk.virtualWrite(V5, h);
  Blynk.virtualWrite(V6, t);  
  delay(1000);
}
void lightSensor()
{ 
 
   int val = analogRead(ldrPin);
   Serial.print("Light intensity");
  Serial.print(1023-val);
  Serial.print("  ");
 
  if(val < 150&& angle!=0){
     Serial.println("ROOF CLOSE");
     angle=0;
  /* for(angle=100; angle > 0; angle -= 1)    // command to move from 0 degrees to 180 degrees 
  {                                  
    servo_test.write(angle);  
    delay(15);                       
  } 
  */
  servo_test.write(angle);}
  else if (val>150 && angle!=47){
  
    Serial.println("ROOF OPENING LOOK UP");
    angle = 47;
    /*for(angle = 0; angle<100; angle+=1)     // command to move from 180 degrees to 0 degrees 
  {                                
    servo_test.write(angle);   
    //command to rotate the servo to the specified angle
    delay(15);                       
  } */
  servo_test.write(angle);
  }
  Blynk.virtualWrite(V9, val);
   
}
void readPressure()
{ 
  if (bmp280.getMeasurements(temperature,pressure,altitude))    // Check if the measurement is complete
  {
    /*Serial.print(temperature);                    // Display the results    
    Serial.print(F("*C   "));*/
    Serial.print("Pressure:"); 
    Serial.print(pressure);    
    Serial.print(F("hPa   "));
    Serial.print("  ");
    Serial.print("Altitude:");
    Serial.print(altitude);
    Serial.println(F("m"));
}
else {
  Serial.print("Pressure:"); 
 Serial.print(pressure);    
    Serial.print(F("hPa   "));
    Serial.print("  ");
    Serial.print("Altitude:");
    Serial.print(altitude);
    Serial.println(F("m"));
}
Blynk.virtualWrite(V7, pressure);
 Blynk.virtualWrite(V8, altitude);
}

void printlcd()
{
    lcd.print("Moisture:");
    lcd.print(curr);
    lcd.setCursor(0,2);
    lcd.print("Pressure:");
    lcd.print(pressure);    
    delay(3000);
    lcd.clear();
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Temp:");
    lcd.print(t);
    lcd.setCursor(0,2);
    lcd.print("Hum:");
    lcd.print(h);
    delay(3000);
    lcd.clear();
}









  
  


        
        
        
