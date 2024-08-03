#include <pitches.h>

#define BUZZER_PIN 2    // Define the buzzer pin
#define JOY_X_PIN 26    // Define the joystick X-axis pin
#define JOY_Y_PIN 27    // Define the joystick Y-axis pin (not used)
#define JOY_BTN_PIN 16  // Define the joystick button pin

bool toneOn = false;
unsigned long debounceDelay = 50;
unsigned long lastDebounceTime = 0;
bool lastButtonState = HIGH;
bool buttonState = HIGH;

void setup() {
  pinMode(BUZZER_PIN, OUTPUT);  // Set the buzzer pin as an output
  pinMode(JOY_X_PIN, INPUT);    // Set the joystick X-axis pin as an input
  pinMode(JOY_Y_PIN, INPUT);    // Set the joystick Y-axis pin as an input
  pinMode(JOY_BTN_PIN, INPUT_PULLUP); // Set the joystick button pin as an input with pull-up resistor
  Serial1.begin(115200);         // Initialize Serial1 communication for debugging
}

void loop() {
  int joyX = analogRead(JOY_X_PIN); // Read the X-axis value from the joystick
  int joyY = analogRead(JOY_Y_PIN); // Read the Y-axis value from the joystick
  int buttonRead = digitalRead(JOY_BTN_PIN); // Read the joystick button state

  // Check for button press with debouncing
  if (buttonRead != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (buttonRead != buttonState) {
      buttonState = buttonRead;
      if (buttonState == LOW) {
        toneOn = !toneOn; // Toggle the tone state
      }
    }
  }

  lastButtonState = buttonRead;

  if (toneOn) {
    int frequency = map(joyX, 0, 1023, 31, 4186); // Map joystick X-axis value to a frequency range
    tone(BUZZER_PIN, frequency); // Play the tone at the mapped frequency
  } else {
    noTone(BUZZER_PIN); // Stop the tone
  }

  // Print the joystick values for debugging
  Serial1.print("Joystick X: ");
  Serial1.print(joyX);
  Serial1.print(" Y: ");
  Serial1.print(joyY);
  Serial1.print(" Button: ");
  Serial1.print(buttonState);
  Serial1.print(" ToneOn: ");
  Serial1.println(toneOn);

  delay(100); // Small delay for stability
}
