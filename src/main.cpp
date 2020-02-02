#include <Arduino.h>
#include "DHT.h"

#define SERIESRESISTOR 10000
#define NOMINAL_RESISTANCE 10000
#define NOMINAL_TEMPERATURE 25
#define BCOEFFICIENT 3950
#define TEMP_READINGS 10
#define LIGHT_READINGS 5

uint16_t temperature_uint16, light;
uint8_t temperature_high, temperature_low, light_high, light_low, bt, humidity_uint8;
float ADCvalue, Resistance, temperature, humidity, tmp_temperature, tmp_humidity;

DHT dht;

void setup() {
  pinMode(PA0, INPUT_ANALOG); // Light ADC
  Serial3.begin(9600); // BT UART
  Serial1.begin(9600); // Communication UART

  dht.setup(PA4);
}

void loop() {

  // DHT Readings
  tmp_humidity = dht.getHumidity(); 
  tmp_temperature = dht.getTemperature();
  if (tmp_humidity > 0.0){
    humidity = tmp_humidity;
  }
  if (tmp_temperature > 0.0){
    temperature = tmp_temperature;
  }

  // Average Light Readings
  light = 0;
  for (int i = 0; i < LIGHT_READINGS; i++){
  light += analogRead(PA0);
    delay(2);
  }
  light /= LIGHT_READINGS;

  // BT Serial
  bt = 0;
  if (Serial3.available() > 0){
    bt = Serial3.read();
  }


  // Communication Protocol
  if (Serial1.available() > 0){
  
    Serial1.read();

    temperature_uint16 = uint16(temperature * 100);
    humidity_uint8 = uint8(humidity);

    temperature_high = temperature_uint16 >> 8;
    temperature_low = temperature_uint16 & 0xFF;
    light_high = light >> 8;
    light_low = light & 0xFF;

    Serial1.write(temperature_high);    // 1
    Serial1.write(temperature_low);     // 2
    Serial1.write(light_high);          // 3
    Serial1.write(light_low);           // 4
    Serial1.write(humidity_uint8);      // 5 
    if (bt == 0){
      Serial1.write(0xFF);              // 6
    } else {
      Serial1.write(bt);                // 6
    }

  }

  // DEBUG
  // temperature_uint16 = uint16(temperature * 100);
  // humidity_uint8 = uint8(humidity);

  // temperature_high = temperature_uint16 >> 8;
  // temperature_low = temperature_uint16 & 0xFF;
  // light_high = light >> 8;
  // light_low = light & 0xFF;

  // Serial1.print("TEMP");
  // Serial1.print("\t");
  // Serial1.print(temperature_high);
  // Serial1.print("\t");
  // Serial1.println(temperature_low);
  // Serial1.print("LUCE");
  // Serial1.print("\t");
  // Serial1.print(light_high);
  // Serial1.print("\t");
  // Serial1.println(light_low);
  // Serial1.print("HUM");
  // Serial1.print("\t");
  // Serial1.println(humidity_uint8); //HUMIDITY
  // Serial1.print("BT");
  // Serial1.print("\t");
  // if (bt == 0){
  // Serial1.println(0xFF);
  // } else {
  // Serial1.println(bt);
  // }

  // Sleep
  delay(50);
}