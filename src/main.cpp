#include <Arduino.h>

#define SERIESRESISTOR 10000
#define NOMINAL_RESISTANCE 10000
#define NOMINAL_TEMPERATURE 25
#define BCOEFFICIENT 3950
#define TEMP_READINGS 10
#define LIGHT_READINGS 5

uint16_t light;
uint8_t bt;
uint16_t steinhart_uint16;
uint8_t steinhart_high, steinhart_low;
float ADCvalue, Resistance, steinhart, temperature;

void setup() {
  pinMode(PA0, INPUT_ANALOG); // Light ADC
  pinMode(PA4, INPUT_ANALOG); // Temperature ADC
  Serial3.begin(9600); // BT UART
  Serial1.begin(9600); // Communication UART
}

void loop() {

  // Average Temperature Readings
  temperature = 0.0;
  for (int i = 0; i < TEMP_READINGS; i++){
    temperature += analogRead(PA4);
    delay(2);
  }

  // Steinhart Convertion
  ADCvalue = temperature / TEMP_READINGS;
  Resistance = (1023 / ADCvalue) - 1;
  Resistance = SERIESRESISTOR / Resistance;
  steinhart = Resistance / NOMINAL_RESISTANCE; // (R/Ro)
  steinhart = log(steinhart); // ln(R/Ro)
  steinhart /= BCOEFFICIENT; // 1/B * ln(R/Ro)
  steinhart += 1.0 / (NOMINAL_TEMPERATURE + 273.15); // + (1/To)
  steinhart = 1.0 / steinhart; // Invert
  steinhart -= 273.15; // convert to C

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

    steinhart_uint16 = uint16(steinhart * 100);

    steinhart_high = steinhart_uint16 >> 8;
    steinhart_low = steinhart_uint16 & 0xFF;

    Serial1.write((uint16_t) steinhart * 100);
    Serial1.write(light);
    Serial1.write(0xFF); //HUMIDITY
    if (bt == 0){
      Serial1.write(0xFF);
    } else {
      Serial1.write(bt);
    }
  }

  // Sleep
  delay(50);
}