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
#define stepperEnable 12
#define stepsPerRevolution 200
#define microStep 11

// Mode and Arrays
String stepMode = "LEFT";
bool addDebounce = false;
bool nextDebounce = false;
bool stepDebounce = false;
int stepLeftArr[6] = {0, 0, 0, 0, 0, 0};
int stepRightArr[6] = {0, 0, 0, 0, 0, 0};
int currentSlot = 0;

// Customization
String stepPrefix = "Step:";

// Hardware Setup
LCD_I2C lcd(0x27, 16, 2); // Default address of most PCF8574 modules

int arrayToInt(int arr[], int size) {
    int result = 0;
    for (int i = 0; i < size; ++i) {
        result = result * 10 + arr[i];
    }
    return result;
}

void setup() {
    pinMode(add, INPUT_PULLUP);
    pinMode(next, INPUT_PULLUP);
    pinMode(prime_right, INPUT_PULLUP);
    pinMode(prime_left, INPUT_PULLUP);
    pinMode(start, INPUT_PULLUP);
    pinMode(stepPin, OUTPUT);
    pinMode(dirPin, OUTPUT);
    pinMode(stepperEnable, OUTPUT);
    pinMode(microStep, OUTPUT);

    Wire.begin();
    lcd.begin(&Wire);
    lcd.display();
    lcd.backlight();

    Serial.begin(9600);
}

void loop() {
    // Disable stepper
    digitalWrite(stepperEnable, HIGH);

    // Add Button Functionality
    if (digitalRead(add) == LOW && !addDebounce) {
        addDebounce = true;
        if (stepMode == "LEFT") {
            stepLeftArr[currentSlot] = (stepLeftArr[currentSlot] < 9) ? stepLeftArr[currentSlot] + 1 : 0;
        } else if (stepMode == "RIGHT") {
            stepRightArr[currentSlot] = (stepRightArr[currentSlot] < 9) ? stepRightArr[currentSlot] + 1 : 0;
        }
    } else if (digitalRead(add) == HIGH && addDebounce) {
        addDebounce = false;
    }

    // Next Button Functionality
    if (digitalRead(next) == LOW && !nextDebounce) {
        nextDebounce = true;
        if (currentSlot < 5) {
            currentSlot++;
        } else {
            stepMode = (stepMode == "LEFT") ? "RIGHT" : "LEFT";
            currentSlot = 0;
        }
    } else if (digitalRead(next) == HIGH && nextDebounce) {
        nextDebounce = false;
    }

    lcd.clear();

    // Step Mode Handler & Print to LCD
    String printable = stepPrefix;
    if (stepMode == "LEFT") {
        for (int i = 0; i < 6; i++) {
            printable += String(stepLeftArr[i]);
        }
        lcd.print(printable);
        lcd.setCursor(stepPrefix.length() + currentSlot, 1);
        lcd.print("^");
        lcd.setCursor(15, 0);
        lcd.print("LEFT");
    } else if (stepMode == "RIGHT") {
        for (int i = 0; i < 6; i++) {
            printable += String(stepRightArr[i]);
        }
        lcd.print(printable);
        lcd.setCursor(stepPrefix.length() + currentSlot, 1);
        lcd.print("^");
        lcd.setCursor(15, 0);
        lcd.print("RIGHT");
    }

    //-------------STEPPER PRIME-----------//

    // Set motor direction clockwise
    digitalWrite(dirPin, HIGH);
    digitalWrite(microStep, LOW);

    // Spin motor
    bool move_motor = (digitalRead(prime_left) == LOW);
    
    while (move_motor) {
        digitalWrite(stepperEnable, LOW);
        for (int x = 0; x < stepsPerRevolution; x++) {
            digitalWrite(stepPin, HIGH);
            delayMicroseconds(2000);
            digitalWrite(stepPin, LOW);
            delayMicroseconds(2000);
        }
        if (digitalRead(prime_left) == HIGH) {
            move_motor = false;
            digitalWrite(stepperEnable, HIGH);
            return;
        }
    }

    const int stp = 2000;
    if (digitalRead(start) == LOW && stepDebounce == false) {
        stepDebounce = true;
        digitalWrite(stepperEnable, LOW);
        digitalWrite(microStep, HIGH);
        for (int x = 0; x < stp; x++) {
            digitalWrite(stepPin, HIGH);
            delayMicroseconds(2000);
            digitalWrite(stepPin, LOW);
            delayMicroseconds(2000);
        }
        while (true);
    }

    delay(20);
}