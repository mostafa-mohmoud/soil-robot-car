/*
 * Soil Robot Car
 * - تفادي العقبات بسنسور الألتراسونيك + سيرفو
 * - قياس رطوبة التربة ونسبة الملوحة
 * البورد: Arduino Uno / Nano
 * سواقة الموتور: L298N
 */

#include <Servo.h>

// ================= التوصيلات =================
// موتورات (L298N)
const int ENA = 5;    // PWM سرعة الموتور الأيمن
const int IN1 = 7;
const int IN2 = 8;
const int ENB = 6;    // PWM سرعة الموتور الأيسر
const int IN3 = 9;
const int IN4 = 10;

// ألتراسونيك HC-SR04
const int TRIG_PIN = 12;
const int ECHO_PIN = 13;

// سيرفو (بيلف الألتراسونيك)
const int SERVO_PIN = 11;

// سنسور رطوبة التربة
const int MOISTURE_PIN       = A0;
const int MOISTURE_POWER_PIN = 4;   // بنشغله وقت القياس بس (يقلل الصدأ)

// سنسور الملوحة (EC)
const int SALINITY_PIN       = A1;
const int SALINITY_POWER_PIN = 2;

// ================= الإعدادات =================
const int  MOTOR_SPEED      = 150;   // 0 - 255
const int  TURN_SPEED       = 160;
const int  OBSTACLE_DIST_CM = 25;    // أقل مسافة قبل ما العربية تقف
const unsigned long TURN_TIME_MS      = 450;
const unsigned long BACK_TIME_MS      = 400;
const unsigned long MEASURE_INTERVAL  = 15000;  // قياس التربة كل 15 ثانية

// معايرة سنسور الرطوبة (اقرا القيم دي بنفسك)
// AIR_VALUE   = القراءة والسنسور في الهوا (جاف)
// WATER_VALUE = القراءة والسنسور في كوباية مية (مبلول)
const int MOISTURE_AIR_VALUE   = 520;
const int MOISTURE_WATER_VALUE = 260;

// معايرة سنسور الملوحة (عدّلها حسب السنسور بتاعك)
const int SALINITY_MIN_RAW = 0;     // مية عذبة
const int SALINITY_MAX_RAW = 700;   // محلول ملح مشبع / أعلى قراءة

Servo scanServo;
unsigned long lastMeasure = 0;

// ================= الموتورات =================
void setMotors(int leftSpeed, int rightSpeed) {
  // الموجب = قدام ، السالب = ورا
  digitalWrite(IN1, rightSpeed >= 0 ? HIGH : LOW);
  digitalWrite(IN2, rightSpeed >= 0 ? LOW : HIGH);
  digitalWrite(IN3, leftSpeed  >= 0 ? HIGH : LOW);
  digitalWrite(IN4, leftSpeed  >= 0 ? LOW : HIGH);
  analogWrite(ENA, constrain(abs(rightSpeed), 0, 255));
  analogWrite(ENB, constrain(abs(leftSpeed),  0, 255));
}

void moveForward()  { setMotors(MOTOR_SPEED, MOTOR_SPEED); }
void moveBackward() { setMotors(-MOTOR_SPEED, -MOTOR_SPEED); }
void turnLeft()     { setMotors(-TURN_SPEED, TURN_SPEED); }
void turnRight()    { setMotors(TURN_SPEED, -TURN_SPEED); }
void stopMotors()   { setMotors(0, 0); }

// ================= الألتراسونيك =================
long readDistanceCm() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000UL);  // timeout 30ms
  if (duration == 0) return 400;                     // مفيش صدى = مفيش عقبة
  return duration * 0.034 / 2;
}

// متوسط 3 قراءات لتقليل الأخطاء
long readDistanceAvg() {
  long sum = 0;
  for (int i = 0; i < 3; i++) {
    sum += readDistanceCm();
    delay(15);
  }
  return sum / 3;
}

// بيلف السيرفو ويقيس المسافة في اتجاه معين
long scanAt(int angle) {
  scanServo.write(angle);
  delay(350);
  return readDistanceAvg();
}

// ================= سنسورات التربة =================
int readAnalogAvg(int pin, int samples = 10) {
  long sum = 0;
  for (int i = 0; i < samples; i++) {
    sum += analogRead(pin);
    delay(10);
  }
  return sum / samples;
}

int readSoilMoisturePercent() {
  digitalWrite(MOISTURE_POWER_PIN, HIGH);
  delay(100);
  int raw = readAnalogAvg(MOISTURE_PIN);
  digitalWrite(MOISTURE_POWER_PIN, LOW);

  int percent = map(raw, MOISTURE_AIR_VALUE, MOISTURE_WATER_VALUE, 0, 100);
  return constrain(percent, 0, 100);
}

int readSalinityPercent() {
  digitalWrite(SALINITY_POWER_PIN, HIGH);
  delay(100);
  int raw = readAnalogAvg(SALINITY_PIN);
  digitalWrite(SALINITY_POWER_PIN, LOW);

  int percent = map(raw, SALINITY_MIN_RAW, SALINITY_MAX_RAW, 0, 100);
  return constrain(percent, 0, 100);
}

const char* moistureStatus(int p) {
  if (p < 30) return "جافة";
  if (p < 60) return "متوسطة";
  return "رطبة";
}

const char* salinityStatus(int p) {
  if (p < 25) return "منخفضة";
  if (p < 55) return "متوسطة";
  return "عالية";
}

void measureSoil() {
  stopMotors();
  scanServo.write(90);
  delay(500);

  int moisture = readSoilMoisturePercent();
  int salinity = readSalinityPercent();

  Serial.println(F("------ قياس التربة ------"));
  Serial.print(F("الرطوبة: "));  Serial.print(moisture);
  Serial.print(F("%  ("));       Serial.print(moistureStatus(moisture));
  Serial.println(F(")"));
  Serial.print(F("الملوحة: "));  Serial.print(salinity);
  Serial.print(F("%  ("));       Serial.print(salinityStatus(salinity));
  Serial.println(F(")"));
  Serial.println(F("-------------------------"));
}

// ================= تفادي العقبات =================
void avoidObstacle() {
  stopMotors();
  delay(200);

  long leftDist  = scanAt(160);  // شمال
  long rightDist = scanAt(20);   // يمين
  scanServo.write(90);
  delay(300);

  moveBackward();
  delay(BACK_TIME_MS);
  stopMotors();

  if (leftDist > rightDist && leftDist > OBSTACLE_DIST_CM) {
    turnLeft();
    delay(TURN_TIME_MS);
  } else if (rightDist > OBSTACLE_DIST_CM) {
    turnRight();
    delay(TURN_TIME_MS);
  } else {
    // مسدود من الجهتين: لف دورة كبيرة
    turnRight();
    delay(TURN_TIME_MS * 2);
  }
  stopMotors();
}

// ================= الإعداد =================
void setup() {
  Serial.begin(9600);

  pinMode(ENA, OUTPUT); pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT); pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  pinMode(MOISTURE_POWER_PIN, OUTPUT);
  pinMode(SALINITY_POWER_PIN, OUTPUT);
  digitalWrite(MOISTURE_POWER_PIN, LOW);
  digitalWrite(SALINITY_POWER_PIN, LOW);

  scanServo.attach(SERVO_PIN);
  scanServo.write(90);

  stopMotors();
  Serial.println(F("الروبوت جاهز..."));
  delay(1500);
  lastMeasure = millis();
}

// ================= الحلقة الرئيسية =================
void loop() {
  // قياس التربة بشكل دوري
  if (millis() - lastMeasure >= MEASURE_INTERVAL) {
    measureSoil();
    lastMeasure = millis();
  }

  long distance = readDistanceAvg();

  if (distance > 0 && distance < OBSTACLE_DIST_CM) {
    avoidObstacle();
  } else {
    moveForward();
  }

  delay(50);
}
