// This code is for debungging MQ-2 sensor & buzzer

int buzzer = 4;
int sensorPin = 2;
int sensorValue = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(sensorPin, INPUT);
  pinMode(buzzer, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  sensorValue = analogRead(sensorPin);

  Serial.print("MQ-2 value : ");
  Serial.println(sensorValue);

  delay(1000);
  digitalWrite(buzzer, HIGH);
  delay(2000);
  digitalWrite(buzzer, LOW);
  delay(2000);
}
