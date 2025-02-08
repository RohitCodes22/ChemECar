#include <Encoder.h>
#include <Wire.h>
#include "Adafruit_VEML7700.h"

// Pin Definitions
#define start 2
#define vemlcatch 5
#define encoderDigital 8
#define encoderAnalog 9

// Hardware Setup
Adafruit_VEML7700 veml = Adafruit_VEML7700();
Encoder obstructionSensor(encoderDigital, encoderAnalog);

void setup() {
    pinMode(start, INPUT_PULLUP);
    pinMode(vemlcatch, INPUT_PULLUP);

    Wire.begin();

    veml.begin();

    Serial.begin(9600);
}

void loop() {
    // Read VEML Sensor
    float lux = veml.readLux(VEML_LUX_AUTO);
    // Read & Update Obstruction Sensor Encoder
    long obstructionRead = 0.0;
    obstructionRead = obstructionSensor.read();

    Serial.print(obstructionRead);
    Serial.print('\n');
    Serial.print(lux);
    Serial.print('\n');

    delay(20);
}