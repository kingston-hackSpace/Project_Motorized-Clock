#include <Stepper.h>

const int stepsPerRevolution = 2048;

// IN1-IN3-IN2-IN4 sequence for proper stepping
Stepper myStepper = Stepper(stepsPerRevolution, 8, 10, 9, 11);

// --- Clock state ---
int currentHour   = 12;  // start at 12:00
int currentMinute = 0;

// Steps per minute ring movement (1 full rev = 60 minutes)
const float stepsPerMinute = stepsPerRevolution / 60.0;

// Accumulated step remainder (for sub-step precision)
float stepAccumulator = 0.0;

// Timing
unsigned long lastTickTime = 0;
const unsigned long TICK_INTERVAL = 60000; // 1 minute in milliseconds

// Button pins
const int BUTTON_PLUS  = 2;
const int BUTTON_MINUS = 3;

// Debounce
unsigned long lastDebounceplus  = 0;
unsigned long lastDebounceMinus = 0;
const unsigned long DEBOUNCE_DELAY = 50;

// --- Coil cutoff pins ---
const int IN1 = 8;
const int IN2 = 9;
const int IN3 = 10;
const int IN4 = 11;

void cutoffCoils() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

// --- Move the minute ring by N minutes ---
// Positive = forward, negative = backward
void moveMinutes(int minutes) {
  myStepper.setSpeed(10);

  stepAccumulator += stepsPerMinute * minutes;
  int stepsToMove = (int)stepAccumulator;
  stepAccumulator -= stepsToMove;

  if (stepsToMove != 0) {
    myStepper.step(stepsToMove);
  }

  cutoffCoils(); // cut power to coils after move
}

// --- Update clock state by N minutes ---
void advanceTime(int minutes) {
  currentMinute += minutes;

  // Handle overflow/underflow
  while (currentMinute >= 60) {
    currentMinute -= 60;
    currentHour++;
    if (currentHour > 12) currentHour = 1;
  }
  while (currentMinute < 0) {
    currentMinute += 60;
    currentHour--;
    if (currentHour < 1) currentHour = 12;
  }

  moveMinutes(minutes);
}

// --- Drive rings to a specific time from 12:00:00 ---
// Called once on startup after user sets the time
void driveToTime(int hour, int minute) {
  // Total minutes from 12 o'clock
  int totalMinutes = ((hour % 12) * 60) + minute;
  moveMinutes(totalMinutes);
}

void setup() {
  pinMode(BUTTON_PLUS,  INPUT_PULLUP);
  pinMode(BUTTON_MINUS, INPUT_PULLUP);

  Serial.begin(9600);
  Serial.println("Set the time using + and - buttons.");
  Serial.println("Hold button to fast-forward.");
  Serial.println("Press both buttons together to confirm.");

  // --- Time setting loop ---
  // Rings start at 12:00. User presses + or - to advance.
  // Press both buttons simultaneously to confirm and start the clock.
  bool timeSet = false;

  while (!timeSet) {
    bool plusPressed  = (digitalRead(BUTTON_PLUS)  == LOW);
    bool minusPressed = (digitalRead(BUTTON_MINUS) == LOW);

    // Both buttons together = confirm
    if (plusPressed && minusPressed) {
      delay(500); // debounce the confirm press
      timeSet = true;
      Serial.print("Time confirmed: ");
      Serial.print(currentHour);
      Serial.print(":");
      if (currentMinute < 10) Serial.print("0");
      Serial.println(currentMinute);
      break;
    }

    // Plus button — advance 1 minute
    if (plusPressed && (millis() - lastDebounceplus > DEBOUNCE_DELAY)) {
      lastDebounceplus = millis();
      advanceTime(1);
      Serial.print("Time: ");
      Serial.print(currentHour);
      Serial.print(":");
      if (currentMinute < 10) Serial.print("0");
      Serial.println(currentMinute);
      delay(200); // hold-to-fast-forward rate
    }

    // Minus button — go back 1 minute
    if (minusPressed && (millis() - lastDebounceMinus > DEBOUNCE_DELAY)) {
      lastDebounceMinus = millis();
      advanceTime(-1);
      Serial.print("Time: ");
      Serial.print(currentHour);
      Serial.print(":");
      if (currentMinute < 10) Serial.print("0");
      Serial.println(currentMinute);
      delay(200); // hold-to-fast-forward rate
    }
  }

  // Record when the clock started
  lastTickTime = millis();
}

void loop() {
  // --- Automatic tick every minute ---
  if (millis() - lastTickTime >= TICK_INTERVAL) {
    lastTickTime += TICK_INTERVAL; // keeps drift minimal vs reassigning millis()
    advanceTime(1);
  }

  // --- Button check during normal running (optional fine adjustment) ---
  bool plusPressed  = (digitalRead(BUTTON_PLUS)  == LOW);
  bool minusPressed = (digitalRead(BUTTON_MINUS) == LOW);

  if (plusPressed && (millis() - lastDebounceplus > DEBOUNCE_DELAY)) {
    lastDebounceplus = millis();
    advanceTime(1);
    delay(200);
  }

  if (minusPressed && (millis() - lastDebounceMinus > DEBOUNCE_DELAY)) {
    lastDebounceMinus = millis();
    advanceTime(-1);
    delay(200);
  }
}