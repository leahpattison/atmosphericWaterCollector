.#include <Time.h>
#include <TimeLib.h>

#include "HX711.h"
HX711 scale;


#include "DHT.h"
#define DHTPIN 2     // what digital pin we're connected to
#define DHTTYPE DHT11   // DHT 11
DHT dht(DHTPIN, DHTTYPE);


void setup() {
  Serial.begin(9600);
  setTime(20,00,00, 8,5,2019);
  // Humidity & temperature sensing  //
  dht.begin();
  
  // initialize digital pin 13 as an output.
  pinMode(A5, OUTPUT);// Set A5 as OUTPUT

  // Setting up scale //
  // HX711.DOUT  - pin #A1
  // HX711.PD_SCK - pin #A0
  scale.begin(A1, A0);


  // Scale factor:
  // 1Kg cell: 2020 for reading in gms
  scale.set_scale(6545.f);                      // this value is obtained by calibrating the scale with known weights; see the README for details
  scale.tare();               // reset the scale to 0

}

void loop() {
  digitalWrite(A5, HIGH);   // atomize

  //Scale 
  int t, i, n, T;
  double val, sum, sumsq, mean;
  float stddev;

  // TEMPERATURE & HUMIDITY SENSING //
 
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float te = dht.readTemperature();


  //Serial.print(F("Humidity: "));
  //HUMIDITY
  Serial.print(h);
  Serial.print(',');
  //Serial.print(F("%  Temperature: "));
  //TEMPERATURE
  Serial.print(te);
  Serial.print(',');
  //Serial.println(F("C "));
  
  val = ((scale.read() - scale.get_offset()) / scale.get_scale());

  Serial.print(val); 
  Serial.println(',');
  scale.power_down();             // put the ADC in sleep mode
  delay(2000);
  digitalWrite(A5, LOW);    // atomization stopped
  scale.power_up();
  delay(200) ; 
   
  
}

