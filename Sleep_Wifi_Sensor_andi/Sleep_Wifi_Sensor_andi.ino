// Setup for Blynk & WiFi
#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL62PSbydOu"
#define BLYNK_TEMPLATE_NAME "Gas Detector"
#define BLYNK_AUTH_TOKEN "4bVvaoCWg-55uSxbTkmhJEmAMOEY-Gs2"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>


// Setup for Deep Sleep mode
#define uS_TO_S_FACTOR 1000000 /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP 15       /* Time ESP32 will go to sleep (in seconds) */

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

// Setup for flash memory
#include <Preferences.h>
Preferences preferences;
// preferences.begin("my-app", false);
// String ssid_str = preferences.getString("ssid", "inti");
// String pass_str = preferences.getString("pass", "laughtale");

// Your WiFi credentials.
// Set password to "" for open networks.
// char ssid[] = ssid_str.c_str();
// char pass[] = pass_str.c_str();

char auth[] = BLYNK_AUTH_TOKEN;

int smokeA0 = 4;
int tr = 6;
int buzzer = 7;

int data = 0;
int sensorThres = 1000;

int num_sens = 0;
int num_sens_threshold = 10;

int lpg_triggered = 0;

BlynkTimer timer;

void sendSensor() {

  int data = analogRead(smokeA0);
  Blynk.virtualWrite(V0, data);
  Serial.print("Pin A0: ");
  Serial.println(data);


  if (data > 1000) {
    // Blynk.email("andikonak16@gmail.com", "Alert", "Gas Leakage Detected!");
    Blynk.logEvent("gas_alert", "Ada Kebocoran Gas!!");
    digitalWrite(buzzer, LOW);   // Mematikan buzzer
    delay(500);                 // Menahan buzzer selama 1 detik sebelum mengulang
    digitalWrite(buzzer, HIGH);  // Menghidupkan buzzer
    delay(500);                 // Menahan buzzer selama 1 detik
    lpg_triggered = 1;
  } else {
     digitalWrite(buzzer, LOW);

      if (lpg_triggered == 1) {
        num_sens_threshold = num_sens_threshold + 2;
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
      esp_deep_sleep_start();
    }
  }
  

}
void setup() {
  preferences.begin("my-app", false);
  String ssid_str = preferences.getString("ssid", "inti");
  String pass_str = preferences.getString("pass", "laughtale");

  // const char ssid[] {ssid_str.c_str()};
  // const char pass[] {pass_str.c_str()};
  char ssid[20];
  char pass[20];
  ssid_str.toCharArray(ssid, sizeof(ssid));
  pass_str.toCharArray(pass, sizeof(pass));
  


  pinMode(smokeA0, INPUT);
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);
  //dht.begin();
  timer.setInterval(2500L, sendSensor);

  pinMode(tr, OUTPUT);
  digitalWrite(tr, HIGH);

  pinMode(buzzer, OUTPUT);
  digitalWrite(buzzer, LOW);

  ++bootCount;
  Serial.println("Boot number: " + String(bootCount));
  print_wakeup_reason();

  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) + " Seconds");

  Serial.println("Turning on LED for 2 sec\n");
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(2000);  // Turn on LED for 2 sec
}

void loop() {
  Blynk.run();
  timer.run();

}