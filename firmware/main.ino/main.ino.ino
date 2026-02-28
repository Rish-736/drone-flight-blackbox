#include <Wire.h>
#include <MPU6050.h>
#include <Preferences.h>

MPU6050 mpu;
Preferences prefs;

// ---------- EVENT IDS ----------
#define EVENT_FREE_FALL 1
#define EVENT_IMPACT    2

// ---------- STATE MACHINE ----------
enum State {
  NORMAL,
  FREE_FALL,
  IMPACT
};

State currentState = NORMAL;

// ---------- FREE FALL ----------
unsigned long freeFallStartTime = 0;
const float FREE_FALL_THRESHOLD = 0.3;   // g
const unsigned long FREE_FALL_TIME = 50; // ms

// ---------- IMPACT ----------
const float IMPACT_THRESHOLD = 3.0; // g

// ---------- ACCEL ----------
int16_t ax_raw, ay_raw, az_raw;
float ax, ay, az, a_mag;

// ---------- FLASH LOGGING ----------
void logEvent(uint8_t eventID, float value) {
  prefs.putUChar("event", eventID);
  prefs.putFloat("value", value);
}

void dumpLastEvent() {
  uint8_t eventID = prefs.getUChar("event", 0);
  float value = prefs.getFloat("value", 0);

  if (eventID == 0) return;

  Serial.println("⚠️ PREVIOUS CRASH EVENT FOUND:");

  if (eventID == EVENT_FREE_FALL) {
    Serial.println("🟡 FREE FALL DETECTED");
  }

  if (eventID == EVENT_IMPACT) {
    Serial.print("🔴 IMPACT DETECTED: ");
    Serial.print(value, 2);
    Serial.println(" g");
  }

  Serial.println("-------------------------");
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  // ---- FLASH INIT ----
  prefs.begin("blackbox", false);
  dumpLastEvent();

  // ---- I2C ----
  Wire.begin(21, 22);

  Serial.println("Initializing MPU6050...");
  mpu.initialize();

  if (!mpu.testConnection()) {
    Serial.println("❌ MPU6050 connection failed!");
    while (1);
  }

  Serial.println("✅ MPU6050 connected");
  Serial.println("System armed");
}

void loop() {
  mpu.getAcceleration(&ax_raw, &ay_raw, &az_raw);

  ax = ax_raw / 16384.0;
  ay = ay_raw / 16384.0;
  az = az_raw / 16384.0;

  a_mag = sqrt(ax * ax + ay * ay + az * az);

  switch (currentState) {

    case NORMAL:
      if (a_mag < FREE_FALL_THRESHOLD) {
        if (freeFallStartTime == 0) {
          freeFallStartTime = millis();
        }

        if (millis() - freeFallStartTime > FREE_FALL_TIME) {
          Serial.println("🟡 FREE FALL DETECTED");
          logEvent(EVENT_FREE_FALL, 0);
          currentState = FREE_FALL;
          freeFallStartTime = 0;
        }
      } else {
        freeFallStartTime = 0;
      }
      break;

    case FREE_FALL:
      if (a_mag > IMPACT_THRESHOLD) {
        Serial.print("🔴 IMPACT DETECTED: ");
        Serial.print(a_mag, 2);
        Serial.println(" g");

        logEvent(EVENT_IMPACT, a_mag);
        currentState = IMPACT;
      }
      break;

    case IMPACT:
      // Lock state until reset (black box behavior)
      break;
  }

  delay(5);
}