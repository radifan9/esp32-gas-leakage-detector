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

// define active Pin (4x4)
byte rowPins[ROWS] = {0, 1, 2, 3}; // Connect to Keyboard Row Pin
byte colPins[COLS] = {4, 5, 6, 7}; // Connect to Pin column of keypad.

Keypad_I2C keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS, I2CADDR_KEYPAD, PCF8574);

LiquidCrystal_I2C lcd(I2CADDR_LCD, 16, 2); // Initialize the LCD with the I2C address and size

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

enum State {
  ENTER_SSID,
  ENTER_PASSWORD,
  AUTHENTICATE,
  ACCESS_GRANTED,
  ACCESS_DENIED
};

State currentState = ENTER_SSID; // Initial state

void setup() {
  Serial.begin(115200);

  Wire.begin();          // Call the connection Wire
  keypad.begin();        // Call the connection for the keypad
  lcd.begin(16, 2);      // Initialize the LCD with 16 columns and 2 rows
  lcd.backlight();       // Turn on the LCD backlight
  lcd.print("Enter SSID:"); // Display initial message on the LCD

  // Set the predefined SSID and password
  // ssid = "192168";
  // password = "1234";

  // Read saved SSID and password from EEPROM
  readFromEEPROM();

  // Print the saved SSID and password to Serial Monitor
  printFromEEPROM();
}

void loop() {
  char key = keypad.getKey(); // Create a variable named key of type char to hold the characters pressed

  // State machine logic
  switch (currentState) {
    case ENTER_SSID:
      if (key != NO_KEY) {
        if (key == '#') {
          lcd.clear();
          lcd.print("Enter Password:");
          currentState = ENTER_PASSWORD;
        } else {
          ssid += key;
          lcd.setCursor(ssid.length() - 1, 1);
          lcd.print(key);
        }
      }
      break;

    case ENTER_PASSWORD:
      if (key != NO_KEY) {
        if (key == '#') {
          lcd.clear();
          lcd.print("Authenticating...");

          char ssid_char[20];
          char pass_char[20];

          ssid.toCharArray(ssid_char, sizeof(ssid_char));
          password.toCharArray(pass_char, sizeof(pass_char));

          Serial.print("SSID: ");
          Serial.println(ssid_char);
          Serial.print("pass: ");
          Serial.println(pass_char);

          Blynk.begin(auth, ssid_char, pass_char);

          currentState = AUTHENTICATE;
        } else if (key == '*') {
          ssid = "";
          lcd.clear();
          lcd.print("Enter SSID:");
          currentState = ENTER_SSID;
        } else {
          password += key;
          lcd.setCursor(password.length() - 1, 1);
          lcd.print(key); // Display the entered character on the LCD
        }
      }
      break;

    case AUTHENTICATE:
      // Simulating authentication by comparing entered SSID and password against saved values
      delay(2000);
      if (ssid == "192168" && password == "1234") {
        lcd.clear();
        lcd.print("Access Granted");
        currentState = ACCESS_GRANTED;

        // Save SSID and password to EEPROM
        saveToEEPROM();

        // Print the EEPROM contents to Serial Monitor
        printFromEEPROM();
      } else {
        lcd.clear();
        lcd.print("Access Denied");
        currentState = ACCESS_DENIED;
      }
      delay(2000);
      lcd.clear();
      lcd.print("Enter SSID:");
      ssid = "";
      password = "";
      currentState = ENTER_SSID;
      break;

    case ACCESS_GRANTED:
      // Access granted, do something here
      break;

    case ACCESS_DENIED:
      // Access denied, do something here
      break;
  }
}

void saveToEEPROM() {
  // Calculate the total number of bytes needed to store the SSID and password
  int ssidSize = ssid.length() + 1;     // Include null terminator
  int passwordSize = password.length() + 1; // Include null terminator
  int totalSize = ssidSize + passwordSize;

  // Write the SSID to EEPROM
  for (int i = 0; i < ssidSize; i++) {
    EEPROM.write(i, ssid[i]);
  }

  // Write the password to EEPROM
  for (int i = 0; i < passwordSize; i++) {
    EEPROM.write(ssidSize + i, password[i]);
  }

  // Write the total size to EEPROM
  EEPROM.write(0, totalSize);

  // Commit the changes to EEPROM
  EEPROM.commit();
}

void readFromEEPROM() {
  // Read the total size from EEPROM
  int totalSize = EEPROM.read(0);

  // Read the SSID from EEPROM
  for (int i = 0; i < totalSize; i++) {
    char c = EEPROM.read(i);
    if (c == '\0') {
      break; // Null terminator reached, stop reading
    }
    ssid += c;
  }

  // Read the password from EEPROM
  for (int i = totalSize; i < totalSize * 2; i++) {
    char c = EEPROM.read(i);
    if (c == '\0') {
      break; // Null terminator reached, stop reading
    }
    password += c;
  }
}

void printFromEEPROM() {
  

  Serial.println("Saved SSID: " + ssid);
  Serial.println("Saved Password: " + password);
}
