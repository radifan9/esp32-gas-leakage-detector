# Laporan Akhir Embedded System: Gas Leak Detection (GLD)

The project deals with the design and implementation of a gas leak detection system for safety measures. 

## Key Devices and Modules

- **ESP32 C3**: This is the main module making use of several pins/ports for interfacing with the other modules. 

- **MQ 2 Gas Sensor**: It is employed for the detection of gas leaks. Its Analog Out is connected to GPIO 5.

- **4x4 Keypad (PCF8574)**: It allows the user to enter WiFi credentials. It's connected to the ESP32 via I2C at address 0x38 and SDA: D21, SCL: D22.

- **16x2 Character LCD (HLF8574)**: Serves as a user interface displaying system status, like WiFi connection. It's connected to the ESP32 via I2C at address 0x27 and SDA: D21, SCL: D22.

- **Buzzer**: It is used for audible alarms during leaks. The Buzzer is driven by GPIO 10.

- **Power Gating MQ 2**: Enables power supply to MQ 2 Gas Sensor through GPIO 7.

## Software and Libraries Used:

- Arduino IDE
- <Wire.h> and <LiquidCrystal_I2C.h> libraries for interfacing with the I2C devices (LCD and Keypad).
- <esp_task_wdt.h> serving as the Watchdog Timer library.

## Sequence of Operation:

1. User at home inputs the SSID and password of the WiFi network on the 4x4 Keypad.
2. The entered credentials get authenticated.
3. If the WiFi connection is successful, the LCD displays "WiFi Connected". 

This setup allows the Gas Leak Detector (GLD) device to maintain connectivity with the Blynk app. Upon detection of a gas leak, the system triggers an alarm and reports the incident via Blynk to the smartphone user.
