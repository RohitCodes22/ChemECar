#include <LCD-I2C.h>
#include <Wire.h>
#include "Adafruit_VEML7700.h"

// Pin Definitions
#define add 6
#define next 4
#define prime_right 3
#define prime_left 7
#define start 2
#define dirPin 8
#define stepPin 9
#define stepper_enable 12
#define stepsPerRevolution 200

// Mode and Arrays
String stepMode = "LEFT";
bool addDebounce = false;
bool nextDebounce = false;
int stepLeftArr[6] = {0, 0, 0, 0, 0, 0};
int stepRightArr[6] = {0, 0, 0, 0, 0, 0};
int currentSlot = 0;

// Customization
String stepPrefix = "Step:";

// Hardware Setup
LCD_I2C lcd(0x27, 16, 2); // Default address of most PCF8574 modules

void setup() {
    pinMode(add, INPUT_PULLUP);
    pinMode(next, INPUT_PULLUP);
    pinMode(prime_right, INPUT_PULLUP);
    pinMode(prime_left, INPUT_PULLUP);
    pinMode(start, INPUT_PULLUP);    
    pinMode(stepPin, OUTPUT);
    pinMode(dirPin, OUTPUT);
    pinMode(stepper_enable, OUTPUT);

    Wire.begin();
    lcd.begin(&Wire);
    lcd.display();
    lcd.backlight();

    Serial.begin(9600);
}

void loop() {
    //disable stepper
    digitalWrite(stepper_enable, HIGH);
    // Add Button Functionality
    if (digitalRead(add) == LOW && addDebounce == false) {
        addDebounce = true;
        if (stepMode == "LEFT") {
            if (stepLeftArr[currentSlot] < 9) {
              stepLeftArr[currentSlot] = stepLeftArr[currentSlot] + 1;
            } else {
              stepLeftArr[currentSlot] = 0;
            }
        } else if (stepMode == "RIGHT") {
            if (stepRightArr[currentSlot] < 9) {
              stepRightArr[currentSlot] = stepRightArr[currentSlot] + 1;
            } else {
              stepRightArr[currentSlot] = 0;
            }
        }
    } else if (digitalRead(add) == HIGH && addDebounce == true) {
        addDebounce = false;
    }

    // Next Button Functionality
    if (digitalRead(next) == LOW && nextDebounce == false) {
        nextDebounce = true;
        if (currentSlot < 5) {
            currentSlot += 1;
        } else {
            if (stepMode == "LEFT") {
              stepMode = "RIGHT";
            } else {
              stepMode = "LEFT";
            }
            currentSlot = 0;
        }
    } else if (digitalRead(next) == HIGH && nextDebounce == true) {
        nextDebounce = false;
    }

    lcd.clear();

    // Step Mode Handler & Print to LCD
    if (stepMode == "LEFT") {
        String printable = stepPrefix;
        for (int i = 0; i < 6; i++) {
            printable += String(stepLeftArr[i]);
        }
        lcd.print(printable);
        lcd.setCursor(stepPrefix.length() + currentSlot, 1);
        lcd.print("^");
        lcd.setCursor(15, 0);
        lcd.print("LEFT");
    } else if (stepMode == "RIGHT") {
        String printable = stepPrefix;
        for (int i = 0; i < 6; i++) {
            printable += String(stepRightArr[i]);
        }
        lcd.print(printable);
        lcd.setCursor(stepPrefix.length() + currentSlot, 1);
        lcd.print("^");
        lcd.setCursor(15, 0);
        lcd.print("RIGHT");
    }

    delay(20);

    //-------------STEPPER PRIME-----------//

    // Set motor direction clockwise
    digitalWrite(dirPin, HIGH);

    // Spin motor
    bool move_motor = false;
    if (digitalRead(prime_left)==0){
      move_motor=true;      
    }
    else {
      move_motor=false;
    }
    while(move_motor){
      digitalWrite(stepper_enable, LOW);
      for(int x = 0; x < stepsPerRevolution; x++)
          {
              digitalWrite(stepPin, HIGH);
              delayMicroseconds(2000);
              digitalWrite(stepPin, LOW);
              delayMicroseconds(2000);
          }
      if (digitalRead(prime_left==1)){
        move_motor=false;
        digitalWrite(stepper_enable, HIGH);
        return 0;
      }
    }
}