// This code is for debungging MQ-2 sensor & buzzer

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

  // Serial.print("MQ-2 value : ");
  Serial.println(sensorValue);
  delay(500);
  // If MQ-2 value is higher than threshold alarm the buzzer
}
