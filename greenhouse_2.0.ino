/* Fill-in your Template ID (only if using Blynk.Cloud) */
#define BLYNK_TEMPLATE_NAME "Greenhouse"
#define BLYNK_AUTH_TOKEN "ajeuZJMB4uGp8V4rfbhJ3s71_wKGD4fA"
#define BLYNK_TEMPLATE_ID "TMPL6Xr608Zyd"

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "ravindu"; //WiFi Name
char pass[] = "12121212"; //WiFi Password

//Set the maximum wet and maximum dry value measured by the sensor
int wetSoilVal = 1104 ;  //min value when soil is wet
int drySoilVal = 2635 ;  //max value when soil is dry

//Set ideal moisture range percentage(%) in soil
int moistPerLow =   20 ;  //min moisture %
int moistPerHigh =   80 ;  //max moisture %

int hulow =   85; 
int huhigh =   90 ; 

int templow = 20 ;
int temphigh = 22 ;


#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>  
#include <AceButton.h>
using namespace ace_button; 

// Define connections to sensor
#define Temperature_SensorPino    14   //D14
#define Soil_Moisture_SensorPin   34  //D34
#define Temperature_SensorPin     27 //D14
#define Motor_RelayPin            1 //D25
#define Fan_RelayPin              32
#define Heater_RelayPin           15
#define Vent_RelayPin             19
#define Humidifier_RelayPin       13
#define wifiLed                    2  //D2
#define ModeSwitchPin             33  //D33
#define ModeLed                   3 //D15


#define DHTTYPE DHT22    
#define DHTTYPEo DHT11  
//Change the virtual pins according the rooms
#define VPIN_Moisture    V1 
#define VPIN_TEMPERATURE V2
#define VPIN_HUMIDITY    V3
#define VPIN_MODE_SWITCH V4
#define VPIN_FAN         V5
#define VPIN_VENT        V6
#define VPIN_HUMIDIFIER  V7
#define VPIN_HEATER      V8
#define VPIN_MOTOR       V9
#define VPIN_TO         V10
#define VPIN_HO         V11


int     sensorVal;
int     moisturePercentage;
bool    toggleRelay = HIGH; //Define to remember the toggle state
bool    toggleRelay1 =  HIGH;
bool    toggleRelay2 =  HIGH;
bool    toggleRelay3 =  HIGH;
bool    prevMode = true;
int     temperature1 = 0;
int     humidity1   = 0;
int     temperature1o = 0;
int     humidity1o   = 0;
String  currMode  = "A";

char auth[] = BLYNK_AUTH_TOKEN;



ButtonConfig config2;
AceButton button2(&config2);

void handleEvent2(AceButton*, uint8_t, uint8_t);

BlynkTimer timer;
DHT dht(Temperature_SensorPin, DHTTYPE);
DHT dhto(Temperature_SensorPino, DHTTYPEo);

void checkBlynkStatus() { // called every 3 seconds by SimpleTimer

  bool isconnected = Blynk.connected();
  if (isconnected == false) {
    Serial.print("Blynk Not Connected ");
    digitalWrite(wifiLed, LOW);
  }
  if (isconnected == true) {
    digitalWrite(wifiLed, HIGH);
    //Serial.println("Blynk Connected");
  }
}
BLYNK_CONNECTED() {
  Blynk.syncVirtual(VPIN_Moisture);
  Blynk.syncVirtual(VPIN_FAN);
  Blynk.syncVirtual(VPIN_TEMPERATURE);
  Blynk.syncVirtual(VPIN_HUMIDITY);
  Blynk.syncVirtual(VPIN_VENT);
  Blynk.syncVirtual(VPIN_MOTOR);
  Blynk.syncVirtual(VPIN_HEATER);
  Blynk.syncVirtual(VPIN_HUMIDIFIER);
  Blynk.syncVirtual(VPIN_TO);
  Blynk.syncVirtual(VPIN_HO);
  //Blynk.syncVirtual(VPIN_MODE_SWITCH); 
  Blynk.virtualWrite(VPIN_MODE_SWITCH, prevMode);
}
BLYNK_WRITE(VPIN_MOTOR) {
  if(!prevMode){
    toggleRelay = param.asInt();
    digitalWrite(Motor_RelayPin, toggleRelay);
  }
  else{
    Blynk.virtualWrite(VPIN_MOTOR, toggleRelay);
  }
}
BLYNK_WRITE(VPIN_FAN) {
  if(!prevMode){
    toggleRelay2 = param.asInt();
    digitalWrite(Fan_RelayPin, toggleRelay2);
  }
  else{
    Blynk.virtualWrite(VPIN_FAN, toggleRelay2);
  }
}
BLYNK_WRITE(VPIN_HUMIDIFIER) {
  if(!prevMode){
    toggleRelay3 = param.asInt();
    digitalWrite(Humidifier_RelayPin, toggleRelay3);
  }
  else{
    Blynk.virtualWrite(VPIN_HUMIDIFIER, toggleRelay3);
  }
}
BLYNK_WRITE(VPIN_HEATER) {
  if(!prevMode){
    toggleRelay1 = param.asInt();
    digitalWrite(Heater_RelayPin , toggleRelay1);
  }
  else{
    Blynk.virtualWrite(VPIN_HEATER, toggleRelay1);
  }
}

BLYNK_WRITE(VPIN_MODE_SWITCH) {
  if(prevMode !=  param.asInt()){
    prevMode =  param.asInt();
    currMode = prevMode ? "A" : "M";
    digitalWrite(ModeLed, prevMode);

    if(!prevMode ==HIGH && toggleRelay == LOW){
      digitalWrite(Motor_RelayPin, HIGH);
      toggleRelay = HIGH;
      Blynk.virtualWrite(VPIN_MOTOR, toggleRelay);
    }
     if(!prevMode==HIGH  && toggleRelay2 == LOW){
      digitalWrite(Fan_RelayPin, HIGH);
      toggleRelay2 = HIGH;
      Blynk.virtualWrite(VPIN_FAN, toggleRelay2);
    }
         if(!prevMode==HIGH  && toggleRelay1 == LOW){
      digitalWrite(Heater_RelayPin, HIGH);
      toggleRelay1 = HIGH;
      Blynk.virtualWrite(VPIN_HEATER, toggleRelay1);
    }
         if(!prevMode==HIGH  && toggleRelay3 == LOW){
      digitalWrite(Humidifier_RelayPin, HIGH);
      toggleRelay3 = HIGH;
      Blynk.virtualWrite(VPIN_HUMIDIFIER, toggleRelay3);
    }
   }   
}

void getMoisture(){
  sensorVal = analogRead(Soil_Moisture_SensorPin);

  if (sensorVal > (wetSoilVal - 100) && sensorVal < (drySoilVal + 100) ){
    moisturePercentage = map(sensorVal ,drySoilVal, wetSoilVal, 0, 100);
    // Print result to serial monitor
    Serial.print("Moisture Percentage: ");
    Serial.print(moisturePercentage);
    Serial.println(" %");
  }
  else{
    Serial.println(sensorVal);
  }
  delay(100);
}

void getWeather(){
  
  float h = dht.readHumidity();
  float t = dht.readTemperature(); // or dht.readTemperature(true) for Fahrenheit
  
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  else {
    humidity1 = int(h);
    temperature1 = int(t);
   // Serial.println(temperature1);
   // Serial.println(humidity1);
  }  
}
void getWeathero(){
  
  float ho = dhto.readHumidity();
  float to = dhto.readTemperature(); // or dht.readTemperature(true) for Fahrenheit
  
  if (isnan(ho) || isnan(to)) {
    Serial.println("Failed to read from DHT sensoro!");
    return;
  }
  else {
    humidity1o = int(ho);
    temperature1o = int(to);
   // Serial.println(temperature1);
   // Serial.println(humidity1);
  }  
}

void sendSensor(){ 
  getMoisture(); // get Moisture reading
  getWeather(); // get reading from DHT11
   getWeathero(); 
  
  Blynk.virtualWrite(VPIN_Moisture, moisturePercentage);
  Blynk.virtualWrite(VPIN_TEMPERATURE, temperature1);
  Blynk.virtualWrite(VPIN_HUMIDITY, humidity1);
  Blynk.virtualWrite(VPIN_HO,humidity1o );
  Blynk.virtualWrite(VPIN_TO, temperature1o);
}

void controlMoist(){
  if(prevMode){
    if (moisturePercentage < (moistPerLow)){
      if (toggleRelay ==  HIGH){
      
        digitalWrite(Motor_RelayPin, LOW);
        toggleRelay = LOW;
        Blynk.virtualWrite(VPIN_MOTOR, toggleRelay);
        delay(1000);
      }      
    }
    if (moisturePercentage > (moistPerHigh)){
      if (toggleRelay == LOW){
     
        digitalWrite(Motor_RelayPin, HIGH);
        toggleRelay = HIGH;
        Blynk.virtualWrite(VPIN_MOTOR, toggleRelay);
        delay(1000);
      } 
    } 
  }
  else{
   
  }
}

void controlTemp(){
 if(prevMode){
    if (temperature1 < (templow)){
      if (toggleRelay1 ==  HIGH){
      
        digitalWrite(Heater_RelayPin, LOW);
        toggleRelay1 = LOW;
        Blynk.virtualWrite(VPIN_HEATER, toggleRelay1);
        delay(1000);
      }      
    }
    if(temperature1 > (templow)){
      if (toggleRelay1 == LOW){
        digitalWrite(Heater_RelayPin,  HIGH);
        toggleRelay1 =  HIGH;
        Blynk.virtualWrite(VPIN_HEATER, toggleRelay1);
        delay(1000);
      }
    }
    if (temperature1 > (temphigh)){
      if (toggleRelay2 ==  HIGH){
     
        digitalWrite(Fan_RelayPin, LOW);
        toggleRelay = LOW;
        Blynk.virtualWrite(VPIN_FAN, toggleRelay2);
        delay(1000);
      } 
    }
    if(temperature1 < (temphigh)){
      if (toggleRelay2 == LOW){
        digitalWrite(Fan_RelayPin,  HIGH);
        toggleRelay2 =  HIGH;
        Blynk.virtualWrite(VPIN_FAN, toggleRelay2);
        delay(1000);
      }
    }
 }
  else{
   
  }
}

void controlHumidity(){
  if(prevMode){
    if (humidity1 < (hulow)){
      if (toggleRelay3 ==  HIGH){
      
        digitalWrite(Humidifier_RelayPin, LOW);
        toggleRelay3 = LOW;
        Blynk.virtualWrite(VPIN_HUMIDIFIER, toggleRelay3);
        delay(1000);
      }      
    }
    if (humidity1 > (huhigh)){
      if (toggleRelay3 == LOW){
     
        digitalWrite(Humidifier_RelayPin, HIGH);
        toggleRelay3 =  HIGH;
        Blynk.virtualWrite(VPIN_HUMIDIFIER, toggleRelay3);
        delay(1000);
      } 
    } 
  }
  else{
   
  }
}


void setup() {
  // Set up serial monitor
  Serial.begin(115200);
  
  // Set pinmodes for GPIOs
  pinMode(Motor_RelayPin, OUTPUT);
  pinMode(wifiLed, OUTPUT);
  pinMode(ModeLed, OUTPUT);
  pinMode(Fan_RelayPin , OUTPUT);
  pinMode(Heater_RelayPin , OUTPUT);
  pinMode(Vent_RelayPin , OUTPUT);
  pinMode(Humidifier_RelayPin , OUTPUT);


  pinMode(ModeSwitchPin, INPUT_PULLUP);

  digitalWrite(wifiLed, LOW);
  digitalWrite(ModeLed, LOW);

 
 
  dht.begin();    // Enabling DHT sensor
  dhto.begin();

  config2.setEventHandler(button2Handler);
  
  button2.init(ModeSwitchPin);

  WiFi.begin(ssid, pass);
  timer.setInterval(2000L, checkBlynkStatus); // check if Blynk server is connected every 2 seconds
  timer.setInterval(3000L, sendSensor); // display and send sensor reading every 3 seconds
  Blynk.config(auth);
  //delay(1000);
  digitalWrite(ModeLed, prevMode);
}
 void loop() {

  Blynk.run();
  timer.run(); // Initiates SimpleTimer
  
  button2.check();

  controlMoist(); 
  controlTemp();
  controlHumidity();
}


void button2Handler(AceButton* button, uint8_t eventType, uint8_t buttonState) {
  Serial.println("EVENT2");
  switch (eventType) {
    case AceButton::kEventReleased:
      //Serial.println("kEventReleased");
      if(prevMode ==HIGH && toggleRelay == LOW){
        digitalWrite(Motor_RelayPin,  HIGH);
        toggleRelay =  HIGH;
        Blynk.virtualWrite(VPIN_MOTOR, toggleRelay);
      }
      if(prevMode == HIGH && toggleRelay1 == LOW){
        digitalWrite(Heater_RelayPin,  HIGH);
        toggleRelay1 = HIGH;
        Blynk.virtualWrite(VPIN_HEATER, toggleRelay1);
      }
      if(prevMode==HIGH && toggleRelay2 == LOW){
        digitalWrite(Fan_RelayPin,  HIGH);
        toggleRelay2 =  HIGH;
        Blynk.virtualWrite(VPIN_FAN, toggleRelay2);
      }
      if(prevMode==HIGH && toggleRelay3 == LOW){
        digitalWrite(Humidifier_RelayPin,  HIGH);
        toggleRelay3 = HIGH;
        Blynk.virtualWrite(VPIN_HUMIDIFIER, toggleRelay3);
      }
      prevMode = !prevMode;
      currMode = prevMode ? "A" : "M";
      digitalWrite(ModeLed, prevMode);
      Blynk.virtualWrite(VPIN_MODE_SWITCH, prevMode);

      break;
  }
}