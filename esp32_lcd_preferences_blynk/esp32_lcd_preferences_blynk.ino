// Pin & General Setup
int i2c_tr = 6;
int mq2_tr = 7;
int buzzer_tr = 10;
unsigned int counter;

// Watchdog Timer
#include <esp_task_wdt.h>
#define WDT_TIMEOUT 10     // 6 seconds WDT

// Setup for LCD & KEYPAD
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <Keypad_I2C.h>
#include <EEPROM.h> // Include the EEPROM library

#define I2CADDR_KEYPAD 0x38 // Set the Address of the PCF8574 for the keypad
#define I2CADDR_LCD 0x27    // Set the Address of the I2C LCD

const byte ROWS = 4; // Set the number of Rows
const byte COLS = 4; // Set the number of Columns

// Set the Key at Use (4x4)
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

// Define active Pin (4x4)
byte rowPins[ROWS] = {0, 1, 2, 3}; // Connect to Keyboard Row Pin
byte colPins[COLS] = {4, 5, 6, 7}; // Connect to Pin column of keypad.

Keypad_I2C keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS, I2CADDR_KEYPAD, PCF8574);

LiquidCrystal_I2C lcd(I2CADDR_LCD, 16, 2); // Initialize the LCD with the I2C address and size

// Setup for flash memory
#include <Preferences.h>
Preferences preferences;

// Setup for Blynk & WiFi
#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL62PSbydOu"
#define BLYNK_TEMPLATE_NAME "Gas Detector"
#define BLYNK_AUTH_TOKEN "4bVvaoCWg-55uSxbTkmhJEmAMOEY-Gs2"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

String ssid = "";     // Variable to store the entered SSID
String password = ""; // Variable to store the entered password
char auth[] = BLYNK_AUTH_TOKEN;

char ssid_char[20];
char pass_char[20];

BlynkTimer timer;

enum State {
  ENTER_SSID,
  ENTER_PASSWORD,
  AUTHENTICATE,
  BLYNK_RUN
};

// State currentState = ENTER_SSID; // Initial state
State currentState = AUTHENTICATE; // Initial state

void reset_counter() {
  // Configuring flash memory
  preferences.begin("wifi_retry", false);
  counter = preferences.getUInt("counter", 0);
  Serial.println();
  Serial.printf("wifi-retry counter value: %u\n", counter);
  Serial.printf("-- wifi-retry counter reseted. --");
  counter = 0;
  preferences.putUInt("counter", counter);    // Store the counter to the Preferences
  preferences.end();  
}

#include "pitches.h"  // must include open source pitches.h found online in libraries folder
void click_sound() {
  // digitalWrite(buzzer_tr, HIGH);
  // delay(10);
  // digitalWrite(buzzer_tr, LOW);
  tone(buzzer_tr,NOTE_G4,35);
  delay(35);
  tone(buzzer_tr,NOTE_G5,35);
  delay(35);
  tone(buzzer_tr,NOTE_G6,35);
  delay(35);
  noTone(buzzer_tr);
}

void congrats_sound() {
  // digitalWrite(buzzer_tr, HIGH);
  // delay(50);
  // digitalWrite(buzzer_tr, LOW);
  // tone(buzzer_tr,NOTE_E6,125);
  // delay(130);
  // tone(buzzer_tr,NOTE_G6,125);
  // delay(130);
  // tone(buzzer_tr,NOTE_E7,125);
  // delay(130);
  // tone(buzzer_tr,NOTE_C7,125);
  // delay(130);
  // tone(buzzer_tr,NOTE_D7,125);
  // delay(130);
  // tone(buzzer_tr,NOTE_G7,125);
  // delay(125);
  // noTone(buzzer_tr);

    tone(buzzer_tr, 523.25, 133);
    delay(133);
    tone(buzzer_tr, 523.25, 133);
    delay(133);
    tone(buzzer_tr, 523.25, 133);
    delay(133);
    tone(buzzer_tr, 523.25, 400);
    delay(400);
    tone(buzzer_tr, 415.30, 400);
    delay(400);
    tone(buzzer_tr, 466.16, 400);
    delay(400);
    tone(buzzer_tr, 523.25, 133);
    delay(133);
    delay(133);
    tone(buzzer_tr, 466.16, 133);
    delay(133);
    tone(buzzer_tr, 523.25, 1200);
    delay(1200);
}

void setup() {
  // Hardware & Pin Setup
  pinMode(i2c_tr, OUTPUT);
  digitalWrite(i2c_tr, HIGH);
  pinMode(mq2_tr, OUTPUT);
  digitalWrite(mq2_tr, LOW);
  pinMode(buzzer_tr, OUTPUT);
  digitalWrite(buzzer_tr, LOW);

  Serial.begin(115200);

  // Configuring I2C LCD and Keypad
  Wire.begin();          // Call the connection Wire
  keypad.begin();        // Call the connection for the keypad

  lcd.begin(16, 2);      // Initialize the LCD with 16 columns and 2 rows
  lcd.backlight();       // Turn on the LCD backlight
  lcd.print("Embedded System");
  lcd.setCursor(0, 1);
  lcd.print("      (B)");


  // Configuring flash memory
  preferences.begin("wifi_retry", false);
  counter = preferences.getUInt("counter", 0);
  Serial.println();
  Serial.printf("wifi-retry counter value: %u\n", counter);
  counter++;
  preferences.putUInt("counter", counter);    // Store the counter to the Preferences
  preferences.end();                          // Close the Preferences
  // counter = 0;
  // reset_counter();

  // Enable Watchdog timer
  delay(2000);
  esp_task_wdt_init(WDT_TIMEOUT, true);
  esp_task_wdt_add(NULL);


  if (counter >= 3) {
    lcd.clear();
    lcd.print("Enter SSID:");
    currentState = ENTER_SSID;
  }
}

void loop() {
  char key = keypad.getKey(); // Create a variable named key of type char to hold the characters pressed

  // State machine logic
  switch (currentState) {
    case ENTER_SSID:
      if (key != NO_KEY) {
        if (key == '#') {
          click_sound();
          lcd.clear();
          lcd.print("Enter Password:");
          currentState = ENTER_PASSWORD;
        } else {
          click_sound();
          esp_task_wdt_reset(); // Resetting Timer every key pressed
          ssid += key;
          lcd.setCursor(ssid.length() - 1, 1);
          lcd.print(key);
        }
      }
      break;

    case ENTER_PASSWORD:
      if (key != NO_KEY) {
        if (key == '#') {
          click_sound();
          lcd.clear();
          lcd.print("Authenticating...");

          preferences.begin("credentials", false);
          preferences.putString("ssid", ssid);
          preferences.putString("password", password);
          preferences.end();

          currentState = AUTHENTICATE;
        } else if (key == '*') {
          ssid = "";
          lcd.clear();
          lcd.print("Enter SSID:");
          currentState = ENTER_SSID;
        } else {
          click_sound();
          esp_task_wdt_reset(); // Resetting Timer every key pressed
          password += key;
          lcd.setCursor(password.length() - 1, 1);
          lcd.print(key); // Display the entered character on the LCD
        }
      }
      break;

    case AUTHENTICATE:
      esp_task_wdt_reset();

      lcd.clear();
      lcd.print("Authenticating...");
      
      // Configuring flash memory
      preferences.begin("credentials", false);
      ssid = preferences.getString("ssid", "");
      password = preferences.getString("password", "");
      preferences.end(); 

      ssid.toCharArray(ssid_char, sizeof(ssid_char));
      password.toCharArray(pass_char, sizeof(pass_char));

      Blynk.begin(auth, ssid_char, pass_char);

      lcd.clear();
      lcd.print("Wifi Connected.");

      congrats_sound();

      esp_task_wdt_reset();

      // preferences.begin("wifi_reset", false);
      // // int counter_reset = 0;
      // counter = 0;
      // preferences.putUInt("counter", counter);
      // // preferences.end();    // Close the Preferences

      // Configuring flash memory
      // preferences.begin("wifi_retry", false);
      // // counter = preferences.getUInt("counter", 0);
      // counter = 0;
      // Serial.println();
      // Serial.printf("wifi-retry counter value: %u\n", counter);
      // preferences.putUInt("counter", counter);    // Store the counter to the Preferences
      // preferences.end();                          // Close the Preferences

      reset_counter();

      delay(2000);

      lcd.clear();
      lcd.print("  Connected to");
      lcd.setCursor(0, 1);
      lcd.print("     Blynk.");
      delay(2000);

      esp_task_wdt_reset();

      lcd.clear();
      lcd.print("  Turning off");
      lcd.setCursor(0, 1);
      lcd.print(" LCD and keypad.");
      delay(2000);
      lcd.noBacklight();
      delay(500);
      // digitalWrite(i2c_tr, LOW);

      currentState = BLYNK_RUN;
      break;

    case BLYNK_RUN:
      esp_task_wdt_reset();
      
      Blynk.run();
      timer.run();
      break;
  }
}