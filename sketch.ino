#include <Keypad.h>

// Keypad configuration
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};

byte colPins[COLS] = { 21, 20, 19, 18 };
byte rowPins[ROWS] = { 28, 27, 26, 22 };

// Create Keypad object
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() {
  // Initialize Serial1 communication at 115200 baud rate
  Serial1.begin(115200);
  Serial1.println("Keypad Test");

  // Initialize keypad row pins with internal pull-ups
  for (byte i = 0; i < ROWS; i++) {
    pinMode(rowPins[i], INPUT_PULLUP);
  }

  // Initialize keypad column pins as inputs
  for (byte i = 0; i < COLS; i++) {
    pinMode(colPins[i], INPUT);
  }

  // Wait for Serial1 to be ready
  while (!Serial1) {
    ; // wait for serial port to connect. Needed for native USB
  }

  // Clear the serial buffer
  while (Serial1.available()) {
    Serial1.read();
  }

  // Give some time for the keypad to stabilize
  delay(500);
}

void loop() {
  // Read key press
  char key = keypad.getKey();
  if (key != NO_KEY) {
    Serial1.println(key);
  }

  // Print the states of the row pins
  Serial1.print("Row States: ");
  for (byte i = 0; i < ROWS; i++) {
    Serial1.print(digitalRead(rowPins[i]));
    Serial1.print(" ");
  }
  Serial1.println();

  // Print the states of the column pins
  Serial1.print("Column States: ");
  for (byte i = 0; i < COLS; i++) {
    Serial1.print(digitalRead(colPins[i]));
    Serial1.print(" ");
  }
  Serial1.println();

  // Delay for 500 ms to make it readable
  delay(500);
}