/*
 Wiring:
  - Pin 2  → Limit switch (other side to GND)
  - Pin 3  → Right eye LEDs (PWM)
  - Pin 6  → Left eye LEDs (PWM)
  - Pin 9  → Servo 1 signal wire
  - Pin 10 → Servo 2 signal wire
  - Vin    → 5V+ from power bank
  - GND    → GND from power bank + servo grounds + LED grounds
*/

#include <OneButton.h>
#include <VarSpeedServo.h>

// ── PIN DEFINITIONS ──────────────────────────────────────────
#define BUTTON_PIN     2
#define RIGHT_EYE_PIN  3
#define LEFT_EYE_PIN   6
#define SERVO1_PIN     9
#define SERVO2_PIN     10

// ── PWM RANGE ────────────────────────────────────────────────
// If servos rumble/twitch, raise PWM_LOW or lower PWM_HIGH
#define PWM_HIGH 2400
#define PWM_LOW   600

// ── SERVO POSITIONS ──────────────────────────────────────────
// Increase gap between OPEN and CLOSE if faceplate doesn't travel far enough
#define SERVO1_OPEN_POS    20
#define SERVO1_CLOSE_POS  160
#define SERVO2_OPEN_POS   160  // Mirrored from servo 1
#define SERVO2_CLOSE_POS   20

// ── SERVO SPEEDS (0=slow, 255=fast) ──────────────────────────
#define SERVO_OPEN_SPEED   255
#define SERVO_CLOSE_SPEED   60  // Slow = more torque for closing

// ── FACEPLATE STATE ───────────────────────────────────────────
#define FACEPLATE_CLOSED 0
#define FACEPLATE_OPEN   1
int facePlateCurMode = FACEPLATE_OPEN;

// ── LED EYE STATE ─────────────────────────────────────────────
#define LED_EYES_DIM_MODE      0
#define LED_EYES_BRIGHTEN_MODE 1
int ledEyesCurMode = LED_EYES_DIM_MODE;
int ledEyesCurPwm  = 0;
const int ledEyesIncrement = 15;

// ── OBJECTS ───────────────────────────────────────────────────
VarSpeedServo servo1;
VarSpeedServo servo2;
OneButton primaryButton = OneButton(BUTTON_PIN, true, true);

// ─────────────────────────────────────────────────────────────

// Non-blocking delay helper
void simDelay(long period) {
  long endTime = millis() + period;
  while (millis() <= endTime) {}
}

// ── LED EYE HELPERS ───────────────────────────────────────────
void setLedEyes(int pwmValue) {
  analogWrite(RIGHT_EYE_PIN, pwmValue);
  analogWrite(LEFT_EYE_PIN,  pwmValue);
  ledEyesCurPwm = pwmValue;
}

void ledEyesOn()  { setLedEyes(255); ledEyesCurMode = LED_EYES_DIM_MODE; }
void ledEyesOff() { setLedEyes(0);   ledEyesCurMode = LED_EYES_BRIGHTEN_MODE; }

void ledEyesBrighten() {
  ledEyesCurPwm = min(ledEyesCurPwm + ledEyesIncrement, 255);
}

void ledEyesDim() {
  ledEyesCurPwm = max(ledEyesCurPwm - ledEyesIncrement, 0);
}

// Double tap: toggle eyes on/off
void ledEyesOnOff() {
  if (facePlateCurMode == FACEPLATE_CLOSED) {
    if (ledEyesCurPwm > 0) ledEyesOff();
    else ledEyesOn();
  }
}

// Long press: dim or brighten eyes
void ledEyesFade() {
  if (ledEyesCurPwm >= 255) ledEyesCurMode = LED_EYES_DIM_MODE;
  else if (ledEyesCurPwm <= 0) ledEyesCurMode = LED_EYES_BRIGHTEN_MODE;

  if (ledEyesCurMode == LED_EYES_BRIGHTEN_MODE) ledEyesBrighten();
  else ledEyesDim();

  setLedEyes(ledEyesCurPwm);
  simDelay(200);
}

// ── MOVIE BLINK STARTUP EFFECT ────────────────────────────────
void movieBlink() {
  simDelay(300);
  int lowValue = 21;
  int delayInterval[] = { 210, 126, 84 };

  for (int i = 0; i <= lowValue; i++) { setLedEyes(i); simDelay(delayInterval[0] / lowValue); }
  setLedEyes(0); simDelay(delayInterval[0]);

  for (int i = 0; i <= lowValue; i++) { setLedEyes(i); simDelay(delayInterval[1] / lowValue); }
  setLedEyes(0); simDelay(delayInterval[1]);

  setLedEyes(lowValue); simDelay(delayInterval[2]);
  setLedEyes(0);        simDelay(delayInterval[2]);

  ledEyesOn();
}

// ── SERVO OPEN / CLOSE ────────────────────────────────────────
void facePlateOpen() {
  servo1.attach(SERVO1_PIN, PWM_LOW, PWM_HIGH);
  servo2.attach(SERVO2_PIN, PWM_LOW, PWM_HIGH);

  servo1.write(SERVO1_OPEN_POS, SERVO_OPEN_SPEED);
  servo2.write(SERVO2_OPEN_POS, SERVO_OPEN_SPEED);

  simDelay(2000); // Wait for servos to finish

  servo1.detach(); // Detach to prevent overheating/twitching
  servo2.detach();

  facePlateCurMode = FACEPLATE_OPEN;
}

void facePlateClose() {
  servo1.attach(SERVO1_PIN, PWM_LOW, PWM_HIGH);
  servo2.attach(SERVO2_PIN, PWM_LOW, PWM_HIGH);

  // Using microseconds directly for extra range beyond 0-180 degrees
  // Tune these values if faceplate doesn't close all the way:
  // Lower SERVO1 value = goes further (try 400, 300)
  // Raise SERVO2 value = goes further (try 2600, 2700)
  servo1.writeMicroseconds(1800);
  servo2.writeMicroseconds(700);

  simDelay(2000); // Wait for servos to finish

  servo1.detach();
  servo2.detach();

  facePlateCurMode = FACEPLATE_CLOSED;
}

// ── FACEPLATE FX (open/close with eye effects) ────────────────
void facePlateOpenFx() {
  ledEyesOff();
  simDelay(100); // Let PWM settle before servos move
  facePlateOpen();
}

void facePlateCloseFx() {
  facePlateClose();
  movieBlink(); // Eyes flicker on when closing
}

void facePlateFx() {
  if (facePlateCurMode == FACEPLATE_OPEN) facePlateCloseFx();
  else facePlateOpenFx();
}

// ── BUTTON HANDLERS ───────────────────────────────────────────
void handleSingleTap()   { facePlateFx(); }
void handleDoubleTap()   { ledEyesOnOff(); }
void handleLongPress()   { ledEyesFade(); }

// ─────────────────────────────────────────────────────────────
void setup() {
  pinMode(LEFT_EYE_PIN,  OUTPUT);
  pinMode(RIGHT_EYE_PIN, OUTPUT);

  // Attach button handlers
  primaryButton.attachClick(handleSingleTap);
  primaryButton.attachDoubleClick(handleDoubleTap);
  primaryButton.attachDuringLongPress(handleLongPress);

  // Startup: close faceplate then blink eyes on
  facePlateClose();
  movieBlink();
}

void loop() {
  primaryButton.tick();
}
