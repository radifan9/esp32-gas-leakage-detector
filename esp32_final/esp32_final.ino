// Pin & General Setup
byte gas = 5;
byte i2c_tr = 6;
byte mq2_tr = 7;
byte buzzer_tr = 10;
byte counter;

unsigned long start_time = 0;

unsigned int data = 0;
unsigned int sensorThres = 1000;

byte num_sens = 1;
byte num_sens_threshold = 10;

byte lpg_triggered = 0;

// Watchdog Timer
#include <esp_task_wdt.h>
#define WDT_TIMEOUT 8     // 8 seconds WDT

// Setup for LCD & KEYPAD
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <Keypad_I2C.h>

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
// #define BLYNK_TEMPLATE_ID "TMPL7LuxdKLE"
// #define BLYNK_TEMPLATE_NAME "Quickstart Template"
// #define BLYNK_AUTH_TOKEN "RRnopBaH6JFnPpFCvg1gm4pjt6K3e3Hg"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

String ssid = "";     // Variable to store the entered SSID
String password = ""; // Variable to store the entered password
char auth[] = BLYNK_AUTH_TOKEN;

char ssid_char[20];
char pass_char[20];

BlynkTimer timer;
void sendSensor() {     // Fungsi untuk mengirim notifikasi

  unsigned int data = analogRead(gas);
  click_sound();
  Blynk.virtualWrite(V0, data);
  Serial.print("Nilai MQ-2: ");
  Serial.println(data);
  esp_task_wdt_reset();

  if (data > 1000) {
    Serial.println("-- Ada kebocoran gas --");
    // Blynk.email("andikonak16@gmail.com", "Alert", "Gas Leakage Detected!");
    Blynk.logEvent("gas_alert", "Ada Kebocoran Gas!!");
    // digitalWrite(buzzer_tr, HIGH);   // Mematikan buzzer
    danger();
    delay(500);                 // Menahan buzzer selama 1 detik sebelum mengulang
    // digitalWrite(buzzer_tr, LOW);  // Menghidupkan buzzer
    delay(500);                 // Menahan buzzer selama 1 detik
    lpg_triggered = 1;
  } else {
    digitalWrite(buzzer_tr, HIGH);
    if (lpg_triggered == 1) {
      num_sens = 1;
      Serial.println("-- resetting number of sensing --");
    }
    lpg_triggered = 0;
  }

  if (lpg_triggered == 0) {
    // Hitung berapa kali sensing untuk masuk ke mode sleep
    Serial.print("Number of Sensing: ");
    Serial.println(num_sens);

    num_sens = num_sens + 1;
    if (num_sens == num_sens_threshold) {
      Serial.println("Going to sleep now");
      Serial.printf("Stopwatch start to sleep: %u\n", millis() - start_time);
      danger();
      esp_deep_sleep_start();
    }
  }
}

// Setup for Deep Sleep mode
#define uS_TO_S_FACTOR 1000000 /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP 20       /* Time ESP32 will go to sleep (in seconds) */

// Save the number of bootCount
RTC_DATA_ATTR int bootCount = 0;

void print_wakeup_reason() {
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch (wakeup_reason) {
    case ESP_SLEEP_WAKEUP_EXT0: Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1: Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER: Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD: Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP: Serial.println("Wakeup caused by ULP program"); break;
    default: Serial.printf("Wakeup was not caused by deep sleep: %d\n", wakeup_reason); break;
  }
}

enum State {
  ENTER_SSID,
  ENTER_PASSWORD,
  AUTHENTICATE,
  BLYNK_RUN
};

State currentState = AUTHENTICATE; // Initial state

void reset_counter() {
  // Configuring flash memory
  preferences.begin("wifi_retry", false);
  counter = preferences.getUInt("counter", 0);
  Serial.println();
  Serial.printf("wifi-retry counter value: %u\n", counter);
  Serial.println("-- resetting wifi-retry counter --");
  Serial.print("\n");
  counter = 0;
  preferences.putUInt("counter", counter);    // Store the counter to the Preferences
  preferences.end();  
}

#include "zelda.h"
#include "pitches.h"
void click_sound() {
  tone(buzzer_tr,NOTE_G4,35);
  delay(35);
  tone(buzzer_tr,NOTE_G5,35);
  delay(35);
  tone(buzzer_tr,NOTE_G6,35);
  delay(35);
  noTone(buzzer_tr);
}

void congrats_sound() {
  int melody_len = sizeof(melody)/sizeof(melody[0]);

  for (int thisNote = 0; thisNote < melody_len; thisNote++) {
    esp_task_wdt_reset();
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(buzzer_tr, melody[thisNote], noteDuration);
    int pauseBetweenNotes = noteDuration * 1.45;
    delay(pauseBetweenNotes);
    noTone(buzzer_tr);
  }

}

void danger() {
  delay(100);
  tone(buzzer_tr,NOTE_E6,850);
  delay(800);
  noTone(buzzer_tr);
}

void setup() {
  // Stopwatch
  start_time = millis();

  // Hardware & Pin Setup
  pinMode(i2c_tr, OUTPUT);
  digitalWrite(i2c_tr, HIGH);
  pinMode(mq2_tr, OUTPUT);
  digitalWrite(mq2_tr, LOW);
  pinMode(buzzer_tr, OUTPUT);
  digitalWrite(buzzer_tr, LOW);
  pinMode(gas, INPUT);

  Serial.begin(115200);

  // Blynk timer
  timer.setInterval(3000L, sendSensor);

  // Sleep mode
  ++bootCount;
  Serial.println("Boot number: " + String(bootCount));
  print_wakeup_reason();

  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) + " Seconds");

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
      digitalWrite(mq2_tr, LOW);
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
      Serial.println("-- Entering AUTHENTICATE state --");
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

      reset_counter();

      delay(1000);

      lcd.clear();
      lcd.print("  Connected to");
      lcd.setCursor(0, 1);
      lcd.print("     Blynk.");
      delay(1000);

      esp_task_wdt_reset();

      lcd.clear();
      lcd.print("  Turning off");
      lcd.setCursor(0, 1);
      lcd.print(" LCD and keypad.");
      delay(1000);
      lcd.noBacklight();
      delay(500);
      digitalWrite(i2c_tr, HIGH);     

      currentState = BLYNK_RUN;
      break;

    case BLYNK_RUN:
      // Serial.println("-- Entering BLYNK_RUN state --");
      esp_task_wdt_reset();

      digitalWrite(mq2_tr, HIGH);
      
      Blynk.run();
      timer.run();
      break;
  }
}