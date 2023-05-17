// This code is for debungging MQ-2 sensor & buzzer

// Moving Average Filter
// https://maker.pro/arduino/tutorial/how-to-clean-up-noisy-sensor-data-with-a-moving-average-filter
#define WINDOW_SIZE 10
int i { 0 };
int value { 0 };
int sum { 0 };
int readings[WINDOW_SIZE];
int averaged { 0 };

int buzzer = 4;         // Using low-level trigger - active buzzer(no oscilation needed)
int sensorPin = 2;
int sensorValue = 0;
int threshold = 700;    // MQ-2 threshold value for LPG leakage

void setup() {
  pinMode(sensorPin, INPUT);
  pinMode(buzzer, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  sensorValue = analogRead(sensorPin);

  Serial.print(0); // To freeze the lower limit
  Serial.print(" ");
  Serial.print(1000); // To freeze the upper limit
  Serial.print(" ");

  Serial.print("MQ-2 value:");
  Serial.print(sensorValue);
  Serial.print(",");

  // Moving Average Filter
  sum = sum - readings[i];
  value = sensorValue;
  readings[i] = value;
  i = (i+1) % WINDOW_SIZE;

  averaged = sum / WINDOW_SIZE;

  Serial.print("Averaged:");
  Serial.println(averaged);

  delay(250);
  // If MQ-2 value is higher than threshold alarm the buzzer
}
