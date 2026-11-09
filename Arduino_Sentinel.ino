
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <string.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int buzzPin = A2;
const int redLED = 12;
const int greenLED = 11;
const int servoPin = 10;

Servo lock;

const int maxTries = 4;
int tries = 0;
const int lockoutTime = 15;
int countdown;
int breaches = 0;

const int lockedPos = 0;
const int unlockedPos = 90;
int delayTime = 3500;
int littleDelay = 25;

byte position = 0;
char enteredPIN[5];
char correctPIN[5] = "0000";   // Placeholder- change to your desired pin

// Number of rows and columns
const byte ROWS = 4;
const byte COLS = 4;

// Keypad layout
char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};

// Arduino pin connections
byte rowPins[ROWS] = { 9, 8, 7, 6 };
byte colPins[COLS] = { 5, 4, 3, 2 };

// Create keypad object
Keypad myKeypad = Keypad(
  makeKeymap(keys),
  rowPins,
  colPins,
  ROWS,
  COLS);

void playCorrectSound() {
  tone(buzzPin, 1319); 
  delay(130);
  tone(buzzPin, 1760); 
  delay(250);
  noTone(buzzPin);    
}

void playIncorrectSound() {
  tone(buzzPin, 147); 
  delay(180);
  noTone(buzzPin);    
  delay(80);
  
  tone(buzzPin, 147); 
  delay(300);
  noTone(buzzPin);
}

void lockout() {
  countdown = lockoutTime + (lockoutTime * breaches);
  digitalWrite(redLED, HIGH);
  for (countdown; countdown > 0; countdown--) {
    for (int freq = 500; freq <= 1500; freq += 10) {
    tone(buzzPin, freq);
    delay(5);
  }
    lcd.clear();
    lcd.home();
    lcd.print("SECURITY BREACH");
    lcd.setCursor(0, 1);
    lcd.print("RETURNS IN...");
    lcd.print(countdown);
    for (int freq = 1500; freq >= 500; freq -= 10) {
    tone(buzzPin, freq);
    delay(5);
  }
  }
  digitalWrite(redLED, LOW);
  noTone(buzzPin);
  tries = 0;
  breaches++;
}

void initialize() {
  position = 0;
  enteredPIN[0] = '\0';
  lcd.clear();
  lcd.home();
  lcd.print("Enter PIN");
  lcd.setCursor(0, 1);
  lock.write(lockedPos);
}

void setup() {
  Serial.begin(9600);
  Serial.println("Keypad Ready!");

  pinMode(buzzPin, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  
  lock.attach(servoPin);

  digitalWrite(redLED, LOW);
  digitalWrite(greenLED, LOW);

  lcd.init();
  lcd.backlight();

  initialize();
}

void loop() {
  char key = myKeypad.getKey();

  if (position == 4) {
    if (strcmp(enteredPIN, correctPIN) == 0) {
      lcd.clear();
      lcd.setCursor(0, 0);        
      lcd.print("Pin Correct!");
      digitalWrite(greenLED, HIGH);
      lock.write(unlockedPos);
      playCorrectSound();
      tries = 0;
      breaches = 0;
    }
    else {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Pin Incorrect!");
      digitalWrite(redLED, HIGH);
      playIncorrectSound();
      tries++;
      if (tries >= maxTries) {
      lockout();
    }
    }
    delay(delayTime);
    digitalWrite(redLED, LOW);
    digitalWrite(greenLED, LOW);
    initialize();
  }

  if (key) {
    if (key == '*') {
      initialize();
    }
    if (key >= '0' && key <= '9' && position < 4) {
    enteredPIN[position] = key;
    position++;
    enteredPIN[position] = '\0';
    lcd.print("*");
    }
  }
  delay(littleDelay);
}
