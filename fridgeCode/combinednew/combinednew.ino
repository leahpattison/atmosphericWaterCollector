#include <Time.h>
#include <TimeLib.h>
#include "HX711.h"
#include <EEPROM.h>
HX711 scale;

#include "DHT.h"
#define DHTPIN 2          // what digital pin we're connected to
#define DHTTYPE DHT11   // DHT 11
DHT dht(DHTPIN, DHTTYPE);
uint32_t timeStamp; 

int addr = 2 ;
int initialAddr = 0 ;
bool record = true;
float cal = 6500.f ;

void setup() {
  Serial.begin(9600);
  setTime(20,00,00, 8,5,2019);
  // Humidity & temperature sensing  //
  dht.begin();
  timeStamp = now(); 
  // initialize digital pin 13 as an output.
  pinMode(A5, OUTPUT);// Set A5 as OUTPUT

  // Setting up scale //
  // HX711.DOUT  - pin #A1
  // HX711.PD_SCK - pin #A0
  scale.begin(A1, A0);

  // Scale factor:
  scale.set_scale(cal);                      // this value is obtained by calibrating the scale with known weights; see the README for details
 
  Serial.println("setup");
}

void loop() {
  //Scale 
  int t, i, n, T, dynamicAddress, address;
  double val, sum, sumsq, mean;
  float stddev;

  EEPROM.get(initialAddr, address);;
  
  char trigger = Serial.read(); 

  if(trigger == 'A'){
    Serial.print("send data");
    sendData(address);
    }

  if(trigger == 'C'){
    clearEEPROM();
    record = true;
    }

  if(trigger == 'S'){
    val = ((scale.read() - scale.get_offset()) / scale.get_scale());
    Serial.println(val); 
  }

  if(trigger == 'R'){
    scale.tare();               // reset the scale to 0 scale.tare();               // reset the scale to 0
    Serial.println("tar");
  }

  if(trigger == 'E'){
    calibrate();
  }

  // Atomise water
  digitalWrite(A5, HIGH);   
  delay(200); //60000
  uint32_t ti = now();

  t = ti - timeStamp ;
  
  if(t > 120){
    if(record == true){
      scale.power_up();
      dynamicAddress = address;
      // Recording data Values
      timeStamp = now();
      float h = dht.readHumidity();
      float te = dht.readTemperature();
      val = ((scale.read() - scale.get_offset()) / scale.get_scale());
      scale.power_down();             //Put ADC in sleep mode - prevents drift
     
      //typecasting
      Serial.println(val); 
      val = val*100;
      if (val < 0 ){
        val = 0;
        }
        
      uint8_t convertedVal = (uint8_t)val;
      uint8_t convertedHumid = (uint8_t)h;
      te = te*10;
      uint16_t convertedTemp = (uint16_t)te ;
      
      //writing dataset to EEPROM
      EEPROM.put(dynamicAddress, timeStamp); 
      dynamicAddress += 4 ;
      EEPROM.put(dynamicAddress, convertedHumid); 
      dynamicAddress += 1 ;
      EEPROM.put(dynamicAddress, convertedTemp);
      dynamicAddress += 2 ;
      EEPROM.put(dynamicAddress, convertedVal);
      
      dynamicAddress += 1 ; 
      address += 8 ; 
      
      EEPROM.put(initialAddr, address); 
      
      if( address + 8  > EEPROM.length())
      {
        record = false ;
       Serial.println("stop");
      }
    }
    
  }

  digitalWrite(A5, LOW);         // atomization stopped
  
  }   
   
  

void sendData(int address){ 
  uint32_t timeRead;
  uint8_t humidRead;
  uint16_t tempRead;
  uint8_t valueRead;
  float temp;
  float value;
  
  int addrRead = 2 ;
   

  while(addrRead < EEPROM.length()){
    EEPROM.get(addrRead,timeRead);
    addrRead+=4;
    //convert to datetime
    Serial.print(timeRead);
    Serial.print(',');
    EEPROM.get(addrRead, humidRead);
    addrRead+=1;
    Serial.print(humidRead);
    Serial.print(',');
    EEPROM.get(addrRead, tempRead);
    addrRead+=2;
    temp = (float)tempRead;
    temp = temp/10 ;
    Serial.print(temp);
    Serial.print(',');
    EEPROM.get(addrRead, valueRead); 
    addrRead+=1;
    value = (float)valueRead;
    value = value/100;
    Serial.print(value);
    Serial.println(',');
  }
}


void clearEEPROM(){
  // set first value as 2, the rest as 0
  Serial.println("clear data");
  setTime(20,00,00, 8,5,2019);
  int resetAddr = 2 ;
  int val1 = 2 ;
  int valElse = 0;
  int val2 ;
  while (resetAddr < EEPROM.length()){
      EEPROM.put(resetAddr, valElse);
      resetAddr += 1; 
    } 

  EEPROM.put(initialAddr, val1);   
  }


void calibrate(){
  Serial.println("calibrating, place pound on scale");

  float val = ((scale.read() - scale.get_offset()) / scale.get_scale());

  if(val != 8.75){
  cal = cal*val/8.75 ; 
  
  scale.set_scale(cal);   
  }
  
  }

void fetchData(){

}


