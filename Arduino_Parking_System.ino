#include <Servo.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>

const int SERVO_PIN = 4;
const int ENTRANCE_IRSENSOR_PIN = 2;

const int EXIT_TRIGGER_PIN = 5;
const int EXIT_ECHO_PIN = 12;

const byte RS = 10;
const byte EN = 11;
const byte D4 = 9;
const byte D5 = 8;
const byte D6 = 7;
const byte D7 = 6;

LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);

Servo gateServo;
int closedGateAngle = map(0, 0, 210, 25, 235);
int openedGateAngle = map(90, 0, 210, 25, 235);

const int MAX_CARS = 5;
int carsInside = 0;

unsigned long lastGateOpenTime = 0;
const unsigned long gateOpenTime = 3000;

unsigned long lastEntryTime = 0;
unsigned long lastExitTime = 0;
const unsigned long sensorCooldown = 1500;

bool gateOpen = false;
bool lastIRState = LOW;
bool exitDetected = false;
bool processingExit = false;

unsigned long lcdInitStartTime = 0;
bool lcdInitDisplayed = false;

const int EEPROM_CARS_ADDR = 0;

void setup() {
  Serial.begin(9600);

  pinMode(ENTRANCE_IRSENSOR_PIN, INPUT);
  pinMode(EXIT_TRIGGER_PIN, OUTPUT);
  pinMode(EXIT_ECHO_PIN, INPUT);

  carsInside = EEPROM.read(EEPROM_CARS_ADDR);
  if (carsInside > MAX_CARS || carsInside < 0) {
    carsInside = 0;
    EEPROM.write(EEPROM_CARS_ADDR, 0);
  }

  gateServo.attach(SERVO_PIN);
  closeGate();

  lcd.begin(16, 2);
  displayWelcomeMessage();
  lcdInitStartTime = millis();

  Serial.print("Parking System Started. Cars inside: ");
  Serial.println(carsInside);
}

void loop() {
  handleEntry();
  handleExit();
  manageGate();
  manageLCDInit();
}

void handleEntry() {
  bool currentIRState = digitalRead(ENTRANCE_IRSENSOR_PIN);
  static unsigned long parkingFullMessageTime = 0;
  static bool showingFullMessage = false;

  if (currentIRState == HIGH && lastIRState == LOW) {
    if (millis() - lastEntryTime > sensorCooldown && !gateOpen) {
      Serial.print("IR Sensor triggered. Cars inside: ");
      Serial.println(carsInside);
      
      if (carsInside < MAX_CARS) {
        carsInside++;
        EEPROM.write(EEPROM_CARS_ADDR, carsInside);
        openGate("ENTERING");
        displayParkingStatus();
        Serial.println("Car entered");
        showingFullMessage = false;
      } else {
        lcd.clear();
        lcd.print("PARKING FULL");
        lcd.setCursor(0, 1);
        lcd.print("Entry blocked!");
        Serial.println("Parking FULL - Entry blocked");
        
        parkingFullMessageTime = millis();
        showingFullMessage = true;
      }
      lastEntryTime = millis();
    }
  }

  if (showingFullMessage && millis() - parkingFullMessageTime >= 2000) {
    displayParkingStatus();
    showingFullMessage = false;
  }

  lastIRState = currentIRState;
}

void handleExit() {
  if (gateOpen && !processingExit) return;
  
  long distance = readUltrasonicDistance();
  
  static unsigned long lastPrintTime = 0;
  if (millis() - lastPrintTime > 1000) {
    Serial.print("Ultrasonic distance: ");
    Serial.println(distance);
    lastPrintTime = millis();
  }

  if (distance > 0 && distance < 7) {
    if (!exitDetected && millis() - lastExitTime > sensorCooldown && !gateOpen) {
      if (carsInside > 0) {
        processingExit = true;
        carsInside--;
        EEPROM.write(EEPROM_CARS_ADDR, carsInside);
        openGate("EXITING");
        displayParkingStatus();
        Serial.println("Car exiting");
      } 
      exitDetected = true;
      lastExitTime = millis();
    }
  } else {
    exitDetected = false;
  }
}

long readUltrasonicDistance() {
  digitalWrite(EXIT_TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(EXIT_TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(EXIT_TRIGGER_PIN, LOW);

  long duration = pulseIn(EXIT_ECHO_PIN, HIGH, 25000);
  if (duration == 0) return -1;

  return duration * 0.034 / 2;
}

void openGate(const char* reason) {
  gateServo.write(openedGateAngle);
  gateOpen = true;
  lastGateOpenTime = millis();

  if (lcdInitDisplayed) {
    lcd.clear();
    lcd.print("GATE: OPEN");
    lcd.setCursor(0, 1);
    lcd.print(reason);
  }

  Serial.print("Gate opened for: ");
  Serial.println(reason);
}

void closeGate() {
  gateServo.write(closedGateAngle);
  gateOpen = false;
  processingExit = false;

  if (lcdInitDisplayed) {
    displayParkingStatus();
  }

  Serial.println("Gate closed");
}

void manageGate() {
  if (gateOpen && millis() - lastGateOpenTime > gateOpenTime) {
    closeGate();
  }
}

void manageLCDInit() {
  if (!lcdInitDisplayed && millis() - lcdInitStartTime >= 2000) {
    displayParkingStatus();
    lcdInitDisplayed = true;
  }
}

void displayWelcomeMessage() {
  lcd.clear();
  lcd.print("Parking System");
  lcd.setCursor(0, 1);
  lcd.print("Cars: ");
  lcd.print(carsInside);
  lcd.print("/");
  lcd.print(MAX_CARS);
}

void displayParkingStatus() {
  lcd.clear();
  lcd.print("Cars: ");
  lcd.print(carsInside);
  lcd.print("/");
  lcd.print(MAX_CARS);

  lcd.setCursor(0, 1);
  if (carsInside >= MAX_CARS) {
    lcd.print("PARKING FULL");
  } else {
    lcd.print("Free: ");
    lcd.print(MAX_CARS - carsInside);
  }
}