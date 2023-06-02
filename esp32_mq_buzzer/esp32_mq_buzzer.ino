// This code is for debungging MQ-2 sensor & buzzer

// Moving Average Filter
// https://maker.pro/arduino/tutorial/how-to-clean-up-noisy-sensor-data-with-a-moving-average-filter
const int numReadings = 10;

int readings[numReadings];  // the readings from the analog input
int readIndex = 0;          // the index of the current reading
int total = 0;              // the running total
int average = 0;            // the average

int buzzer = 10;         // Using low-level trigger - active buzzer(no oscilation needed)
int sensorPin = 5;
int sensorValue = 0;
int threshold = 700;    // MQ-2 threshold value for LPG leakage

void setup() {
  pinMode(7, OUTPUT);
  pinMode(10, OUTPUT);

  digitalWrite(7, HIGH);
  // digitalWrite(10, HIGH);

  pinMode(6, OUTPUT);
  digitalWrite(6, HIGH);

  pinMode(sensorPin, INPUT);
  pinMode(buzzer, OUTPUT);
  Serial.begin(115200);
}

void loop() {


  Serial.print("MQ-2 value:");
  Serial.print(sensorValue);
  Serial.print(",");

  // Moving Average Filter
  // subtract the last reading:
  total = total - readings[readIndex];
  // read from the sensor:
  readings[readIndex] = analogRead(sensorPin);
  // add the reading to the total:
  total = total + readings[readIndex];
  // advance to the next position in the array:
  readIndex = readIndex + 1;

  // if we're at the end of the array...
  if (readIndex >= numReadings) {
    // ...wrap around to the beginning:
    readIndex = 0;
  }

  // calculate the average:
  average = total / numReadings;
  // send it to the computer as ASCII digits
  Serial.println(average);
  delay(1);  // delay in between reads for stability

  delay(250);
  // If MQ-2 value is higher than threshold alarm the buzzer
}
