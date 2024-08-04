#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <pitches.h>
#include "alpha.h"
#include <LedControl.h>


#define DATA_PIN 28
#define CS_PIN 27
#define CLK_PIN 26

#define BUZZER_PIN 2 
#define JOY_X_PIN 21 
#define JOY_Y_PIN 22 
#define JOY_BTN_PIN 16

bool toneOn = false; 
unsigned long debounceDelay = 50;
unsigned long lastDebounceTime = 0;
bool lastButtonState = HIGH; 
bool buttonState = HIGH;

// 16 columns and 2 rows
LiquidCrystal_I2C lcd(0x27, 16, 2);

LedControl lc = LedControl(DATA_PIN, CLK_PIN, CS_PIN, 4);

unsigned long bufferLong [14] = {0};
const unsigned char scrollText[] PROGMEM = {"     SOUNDTOY      "};


void setup() {
  // Set the pin mode
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(JOY_X_PIN, INPUT);
  pinMode(JOY_Y_PIN, INPUT);
  pinMode(JOY_BTN_PIN, INPUT_PULLUP);

  // Initialize Serial1
  Serial1.begin(115200);

  // Init LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Move joystick");
  lcd.setCursor(0, 1);
  lcd.print("Click to toggle");

  // Init Matrix
  lc.shutdown(0, false);
  lc.setIntensity(0, 8);
  lc.clearDisplay(0);

}

void loop() {
  int joyX = analogRead(JOY_X_PIN);
  int joyY = analogRead(JOY_Y_PIN);
  int buttonRead = digitalRead(JOY_BTN_PIN);

  // Debounce button press
  if (buttonRead != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (buttonRead != buttonState) {
      buttonState = buttonRead;
      if (buttonState == LOW) {
        toneOn = !toneOn;
      }
    }
  }
  lastButtonState = buttonRead;

  // Play or stop the tone based on the button press
  if (toneOn) {
    int frequency = map(joyX, 0, 1023, 31, 4186); // Map joystick X value to frequency
    tone(BUZZER_PIN, frequency);

    // Update the LCD with the current frequency
    lcd.setCursor(0, 1);
    lcd.print("Freq: ");
    lcd.print(frequency);
    lcd.print(" Hz   ");
  } else {
    noTone(BUZZER_PIN);

    // Update the LCD to show the tone is off
    lcd.setCursor(0, 1);
    lcd.print("Tone: OFF       "); // Clear previous characters
  }

  // debuggin
  Serial1.print("Joystick X: ");
  Serial1.print(joyX);
  Serial1.print(" Y: ");
  Serial1.print(joyY);
  Serial1.print(" Button: ");
  Serial1.print(buttonState);
  Serial1.print(" ToneOn: ");
  Serial1.println(toneOn);

  delay(100); // Short delay for stability

  scrollMessage(scrollText);
}

void scrollFont() {
  for (int counter = 0x20; counter < 0x80; counter++) {
    loadBufferLong(counter);
    delay(500);
  }
}

// Scroll Message
void scrollMessage(const unsigned char * messageString) {
    int counter = 0;
    int myChar = 0;
    do {
      // read back a char
      myChar =  pgm_read_byte_near(messageString + counter);
      if (myChar != 0) {
        loadBufferLong(myChar);
      }
      counter++;
    }
    while (myChar != 0);
}
// Load character into scroll buffer
void loadBufferLong(int ascii) {
  if (ascii >= 0x20 && ascii <= 0x7f) {
    for (int a = 0; a < 7; a++) {               // Loop 7 times for a 5x7 font
      unsigned long c = pgm_read_byte_near(font5x7 + ((ascii - 0x20) * 8) + (6-a));     // Index into character table to get row data
      unsigned long x = bufferLong [a * 2];   // Load current scroll buffer
      x = x | c;                              // OR the new character onto end of current
      bufferLong [a * 2] = x;                 // Store in buffer
    }
    byte count = pgm_read_byte_near(font5x7 + ((ascii - 0x20) * 8) + 7);    // Index into character table for kerning data
    for (byte x = 0; x < count; x++) {
      rotateBufferLong();
      printBufferLong();
      delay(40);
    }
  }
}
// Rotate the buffer
void rotateBufferLong() {
  for (int a = 0; a < 7; a++) {               // Loop 7 times for a 5x7 font
    unsigned long x = bufferLong [a * 2];   // Get low buffer entry
    byte b = bitRead(x, 31);                // Copy high order bit that gets lost in rotation
    x = x << 1;                             // Rotate left one bit
    bufferLong [a * 2] = x;                 // Store new low buffer
    x = bufferLong [a * 2 + 1];             // Get high buffer entry
    x = x << 1;                             // Rotate left one bit
    bitWrite(x, 0, b);                      // Store saved bit
    bufferLong [a * 2 + 1] = x;             // Store new high buffer
  }
}
// Display Buffer on LED matrix
void printBufferLong() {
  for (int a = 0; a < 7; a++) {             // Loop 7 times for a 5x7 font
    unsigned long x = bufferLong [a * 2 + 1]; // Get high buffer entry
    byte y = x;                             // Mask off first character
    lc.setRow(3, a, y);                     // Send row to relevent MAX7219 chip
    x = bufferLong [a * 2];                 // Get low buffer entry
    y = (x >> 8);                          // Mask off second character
    lc.setRow(2, a, y);                     // Send row to relevent MAX7219 chip
    y = (x >> 16);                          // Mask off third character
    lc.setRow(1, a, y);                     // Send row to relevent MAX7219 chip
    y = (x >> 24);                           // Mask off forth character
    lc.setRow(0, a, y);                     // Send row to relevent MAX7219 chip
  }
}

