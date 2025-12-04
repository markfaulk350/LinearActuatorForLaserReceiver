// This code homes, goes up to x then steps down 4 steps (0.01in) at a time logging the current Beep status of OFF, FAST, SOLID, SLOW
// This code is used as a referece point for testing other code.
// This also tells us the range of each area of the sensor on the laser receiver

#include <AccelStepper.h>

// ───── Stepper Motor ─────
#define MF_PLUS_PIN 11        // for alarm clearing
#define PULSE_PIN 12
#define DIR_PIN   13
#define ALARM_PIN 14

#define PULSES_PER_REVOLUTION 400

// ───── POSITIONS (hard-coded in inches) ─────
const long POS_HOME     = 0;                                 // 0 inches
const long POS_MIDDLE   = 17L * PULSES_PER_REVOLUTION;       // 17 inches
const long POS_FAR      = 34L * PULSES_PER_REVOLUTION;       // 34 inches
const long ONE_INCH     = 1L  * PULSES_PER_REVOLUTION;       // 1 inch step

// ───── Motor Speed ─────
const long MAX_SPEED_INCHES_PER_SECOND = 6L;
const long MAX_ACCELERATION_PULSES_PER_SECOND = 60L;

const int PIEZO_PIN = 1;
const int SAMPLE_RATE_MS = 5;
const int WINDOW_SIZE = 250 / SAMPLE_RATE_MS;  // 50 samples

int buffer[WINDOW_SIZE];
int idx = 0;
unsigned long last = 0;


AccelStepper stepper(AccelStepper::DRIVER, PULSE_PIN, DIR_PIN);

void setup() {
  Serial.begin(2000000);
  analogReadResolution(12);

  Serial.println("=== RANGE TEST STARTING ===");

  pinMode(ALARM_PIN, INPUT_PULLUP);
  pinMode(MF_PLUS_PIN, OUTPUT);

  // Initialize stepper
  stepper.setMaxSpeed(MAX_SPEED_INCHES_PER_SECOND * PULSES_PER_REVOLUTION);
  stepper.setAcceleration(MAX_ACCELERATION_PULSES_PER_SECOND * PULSES_PER_REVOLUTION);
  stepper.setCurrentPosition(POS_HOME);  // Assume we start at home

 // This will reverse at 1 inch per second and Home automatically. You can comment this out and call it manually if you want using a voice command
  Serial.println("=== HOMING BY STALLING ===");
  homeByStall();

  delay(5000);

  // Move to 34 inches;
  Serial.println("=== MOVE TO 34 IN ===");
  safeMoveTo(25L * PULSES_PER_REVOLUTION);

  delay(5000);

  scanDown();

  Serial.println("=== FINISHED ===");

}

void safeMoveTo(long target) {
  target = constrain(target, POS_HOME, POS_FAR);  // Hard soft limits
  stepper.moveTo(target);
  while (stepper.distanceToGo() != 0) stepper.run();
}


void instantStop() {
  Serial.println("EMERGENCY STOP!");

  long currentPos = stepper.currentPosition();        // Save the exact position

  // Cancel motion instantly but preserve real position
  stepper.setCurrentPosition(currentPos);     // Keep the real position we just saved
  stepper.moveTo(currentPos);                 // Cancel any remaining travel
  stepper.setSpeed(0);                        // Stop generating pulses

  Serial.println("Motor stopped — position preserved for distance calc");


  long stepsFromCenter = currentPos - 6800;           // Subtract 6800 steps aka 17 inches
  float inchesFromCenter = (float)stepsFromCenter / 400.0;  // Convert to revolutions

  // ───── Print everything nicely ─────
  Serial.println("─────────────────────────────");
  Serial.print("Current Position (pulses): ");
  Serial.println(currentPos);

  Serial.print("Steps from Center (after -6800): ");
  Serial.println(stepsFromCenter);

  Serial.print("Distance from Center: ");
  Serial.print(inchesFromCenter, 3);
  Serial.println(" inches");
  Serial.println("─────────────────────────────");

  digitalWrite(LED_BUILTIN, HIGH); // Why is this here ??
}

void clearMotorAlarm() {
  digitalWrite(MF_PLUS_PIN, HIGH); delay(200);
  digitalWrite(MF_PLUS_PIN, LOW);  delay(200);
}

void homeByStall() {
  Serial.println("Re-homing by stall...");
  // Set speed and acceleration to low value like 1 inch per second
  // Use Negative value to go backwards
  stepper.setSpeed(-1 * PULSES_PER_REVOLUTION);
  while (digitalRead(ALARM_PIN) == HIGH) {
    stepper.runSpeed();
    delay(2);
  }
  clearMotorAlarm();
  stepper.stop();
  stepper.setCurrentPosition(0);
  // POS_HOME = 0; Why did grok do this?
  Serial.println("Homed — position = 0");
}

// Returns a String with the beep status
// String analyzePiezoData(float piezoData[50]) {
//   // Convert float voltages to binary 1 / 0 (same threshold you use)
//   int binary[50];
//   for (int i = 0; i < 50; i++) {
//     binary[i] = (piezoData[i] > 0.2f) ? 1 : 0;
//   }

//   // Find longest run of 1's
//   int longestOnes = 0;
//   int current = 0;
//   for (int i = 0; i < 50; i++) {
//     if (binary[i] == 1) {
//       current++;
//       if (current > longestOnes) longestOnes = current;
//     } else {
//       current = 0;
//     }
//   }

//   // Your exact rules
//   if (longestOnes == 0)                  return "100% OFF";
//   if (longestOnes == 50)                     return "100% SOLID (on-grade)";
//   if (longestOnes <= 9)                  return "OFF";
//   if (longestOnes <= 14)                 return "FAST (too high)";
//   if (longestOnes <= 21)                 return "GREY AREA";
//   if (longestOnes <= 26)                 return "SLOW (too low)";
//   return "SOLID (on-grade)";
// }

String analyzePiezoData(float piezoData[50]) {
  int binary[50];
  for (int i = 0; i < 50; i++) {
    binary[i] = (piezoData[i] > 0.2f) ? 1 : 0;
  }

  int longestOnes = 0;
  int current = 0;
  for (int i = 0; i < 50; i++) {
    if (binary[i] == 1) {
      current++;
      if (current > longestOnes) longestOnes = current;
    } else {
      current = 0;
    }
  }

  String base;
  if (longestOnes == 0)                  base = "OFF"; // 100% OFF
  else if (longestOnes == 50)            base = "SOLID"; // 100% SOLID (on-grade)
  else if (longestOnes <= 9)             base = "OFF"; // OFF
  else if (longestOnes <= 12)            base = "FAST"; // FAST (too high)
  else if (longestOnes <= 21)            base = "SLOW"; // GREY AREA
  else if (longestOnes <= 26)            base = "SLOW"; // SLOW (too low)
  else                                   base = "SOLID"; // SOLID (on-grade)

  // ← Append the count of longest run of 1's
  return base + " [" + String(longestOnes) + "]";
}

void scanDown() {
  // Get the current position
  // Wait 250ms
  // Look at the array and process
  // Determine state
  // Print findings
  // Move 4 down

 for (long i = 0; i <= POS_FAR; i += 4){
  long currentPosition = stepper.currentPosition();   // ← Starts at 13600 and will decrease by 4 every loop

  float piezoData[50];

  // Create a loop here for 250ms. Every 5ms for 50 loops grab the value of pin1 and save to array
  for (int i = 0; i < 50; i++) {
    int raw = analogRead(PIEZO_PIN);
    piezoData[i] = raw * 3.3f / 4095.0f;   // Convert to voltage
    delay(5);
  }

  // Read piezoData Array and decide what the status is
  String status = analyzePiezoData(piezoData);

  float in = (float)currentPosition / PULSES_PER_REVOLUTION;

  // Index: 0 Position: 13600
  // Serial.print("Index: ");
  // Serial.print(i);
  // Serial.print(" Position: ");
  // Serial.print(currentPosition);
  Serial.print(" Inches: ");
  Serial.print(in, 2);
  Serial.print( " Status: ");
  Serial.println(status);

  // move() and moveTo() are very different. Use move() for this
  stepper.move(-4);
  while (stepper.distanceToGo() != 0) stepper.run();
 }



}

void loop() {

}
