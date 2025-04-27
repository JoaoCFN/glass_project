#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;

const float LIMIT_ANGLE = 25;
const long LIMIT_TIME = 10000;

int buzzerPin = 9;
long inclinedTime = 0;
bool inclinationDetected = false;

void setupAngleSensor() {
  bool mpuIsConnected = mpu.testConnection();

  if (!mpuIsConnected) {
    Serial.println("Erro: MPU6050 não conectado.");
    while(1);
  }

  Serial.println("Sensor MPU6050 inicializado com sucesso!");
}

void emitAlert() {
  digitalWrite(buzzerPin, LOW);
  delay(500);
  digitalWrite(buzzerPin, HIGH);
  delay(500);

  Serial.println("Buzzer Pulse");
}

void handleInclination(bool activeAlert) {
  if (!inclinationDetected) {
    inclinedTime = millis();
    inclinationDetected = true;
  }
  else if (activeAlert) {
    emitAlert();
  }
}

void setup() {
  Serial.begin(9600);
  Wire.begin();
  mpu.initialize();

  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);

  setupAngleSensor();
}

void loop() {
  delay(200);

  int16_t ax, ay, az;
  mpu.getAcceleration(&ax, &ay, &az);

  float angle = atan2(ax, az) * 180 / PI;
  bool isInclined = abs(angle) > LIMIT_ANGLE;
  bool activeAlert = millis() - inclinedTime >= LIMIT_TIME; 

  Serial.print("Angle X (estimated): ");
  Serial.println(angle);

  if (isInclined) {
    handleInclination(activeAlert);

    return;
  }

  inclinationDetected = false;
  digitalWrite(buzzerPin, HIGH);
}
