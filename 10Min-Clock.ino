/*
10 Minutes Clock

DESCRIPTION:
  - One full motor revolution (2048 steps) represents 10 minutes.
  - The motor ticks every 1 minute, completing one full revolution after 10 ticks.

FEATURES:
  - Manual time setting via + and - buttons (with hold-to-fast-forward)
  - Confirm time by pressing both buttons simultaneously
  - Automatic advancement every 1 minute (TICK_INTERVAL)
  - Coil cutoff after each move to prevent motor overheating
  - LED_SEC flashes every second (visual heartbeat)
  - LED_CONF flashes on time confirmation
*/


#include <Stepper.h>

const int stepsPerRevolution = 2048;

// IN1-IN3-IN2-IN4 sequence for proper stepping
Stepper myStepper = Stepper(stepsPerRevolution, 8, 10, 9, 11);

// --- Clock state ---
int currentMinute = 0;

// Steps per minute ring movement (1 full rev = 10 minutes)
const float stepsPerMinute = stepsPerRevolution / 10.0; ///////////////////////////////////////////

// Accumulated step remainder (for sub-step precision)
float stepAccumulator = 0.0;

// Timing
unsigned long lastTickTime = 0;
unsigned long secondTimer = 0;
int counter = 0;

/////////// 5 seconds for testing, modify to 60000 to set the trigger every one minute:
const unsigned long TICK_INTERVAL = 5000; // 60000 = 1 minute in milliseconds ///////////////////////////////////////////
const unsigned long TICK_INTERVAL_SEC = 1000; // 1 second in milliseconds


// Button pins
const int BUTTON_PLUS  = 2;
const int BUTTON_MINUS = 3;

//LEDS
const int LED_SEC  = 13; ///////////////////////////////////////////
const int LED_CONF = 12; ///////////////////////////////////////////

// At the top of your file, add this variable:
unsigned long ledFlashTime = 0;
bool ledFlashing = false;


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
  while (currentMinute >= 10) {///////////////////////////////////////////
    currentMinute -= 10; ///////////////////////////////////////////
  }
  while (currentMinute < 0) {
    currentMinute += 10; ///////////////////////////////////////////
  }

  moveMinutes(minutes);

    // Resync timers after blocking motor call
  lastTickTime = millis();
  secondTimer  = millis();
}

void setup() {
  pinMode(BUTTON_PLUS,  INPUT_PULLUP);
  pinMode(BUTTON_MINUS, INPUT_PULLUP);

  pinMode(LED_SEC, OUTPUT); ///////////////////////////////////////////
  pinMode(LED_CONF, OUTPUT); ///////////////////////////////////////////

  digitalWrite(LED_SEC, LOW); ///////////////////////////////////////////
  digitalWrite(LED_CONF, LOW); ///////////////////////////////////////////

  Serial.begin(9600);
  Serial.println("Set the time using + and - buttons.");
  Serial.println("Hold button to fast-forward.");
  Serial.println("Press both buttons together to confirm.");

  bool timeSet = false;

  while (!timeSet) {
    bool plusPressed  = (digitalRead(BUTTON_PLUS)  == LOW);
    bool minusPressed = (digitalRead(BUTTON_MINUS) == LOW);

    // Both buttons together = confirm
    if (plusPressed && minusPressed) {
      delay(500); // debounce the confirm press
      timeSet = true;
      digitalWrite(LED_CONF, HIGH); ///////////////////////////////////////////
      Serial.print("Time confirmed: ");
      Serial.println(currentMinute);
      delay(50);
      digitalWrite(LED_CONF, LOW); ///////////////////////////////////////////
      break;
    }

    // Plus button — advance 1 minute
    if (plusPressed && (millis() - lastDebounceplus > DEBOUNCE_DELAY)) {
      lastDebounceplus = millis();
      advanceTime(1);
      Serial.print("Set minute to: ");
      Serial.println(currentMinute);
      delay(200); // hold-to-fast-forward rate
    }

    // Minus button — go back 1 minute
    if (minusPressed && (millis() - lastDebounceMinus > DEBOUNCE_DELAY)) {
      lastDebounceMinus = millis();
      advanceTime(-1);
      Serial.print("Set minute to: ");
      Serial.println(currentMinute);
      delay(200); // hold-to-fast-forward rate
    }
  }

  // Record when the clock started
  lastTickTime = millis();
  secondTimer = millis();
}

void loop() {

  // --- Automatic tick every minute ---
  if (millis() - lastTickTime >= TICK_INTERVAL) {
    lastTickTime += TICK_INTERVAL; // keeps drift minimal vs reassigning millis()
    advanceTime(1);
    // Serial.print("Current minute: ");
    // Serial.println(currentMinute);
  }

  if (millis() - secondTimer >= TICK_INTERVAL_SEC) { ///////////////////////////////////////////
    secondTimer += TICK_INTERVAL_SEC;
    counter++;
    if (counter >= 60){
      counter = 0;
    }
    Serial.print("second: ");
    Serial.println(counter);

    // Start the flash
    digitalWrite(LED_SEC, HIGH);
    ledFlashTime = millis();
    ledFlashing = true;
  }

      // Separately, turn off after 50ms
if (ledFlashing && millis() - ledFlashTime >= 200) {
    digitalWrite(LED_SEC, LOW);
    ledFlashing = false;
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
