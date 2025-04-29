#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;

const float LIMIT_ANGLE = 20;
const long LIMIT_TIME = 10000;

int buzzerPin = 9;
long inclinationStartTime = 0;
bool inclinationDetected = false;

bool isInclinationExcessive(float angle) {
  return abs(angle) > LIMIT_ANGLE;
}

bool shouldActivateAlert() {
  float inclinationTime = millis() - inclinationStartTime;

  return inclinationTime >= LIMIT_TIME;
}

float calculateAngle(int16_t ax, int16_t ay) {
  float restPositionAngle = 90;
  float rawAngle = atan2(ax, ay) * 180 / PI;

  return rawAngle - restPositionAngle;
}

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
    inclinationStartTime = millis();
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

  float angle = calculateAngle(ax, ay);
  bool excessiveInclination = isInclinationExcessive(angle);
  bool activeAlert = shouldActivateAlert(); 

  Serial.print("Angle X (estimated): ");
  Serial.println(angle);

  if (excessiveInclination) {
    handleInclination(activeAlert);

    return;
  }

  inclinationDetected = false;
  digitalWrite(buzzerPin, HIGH);
}
