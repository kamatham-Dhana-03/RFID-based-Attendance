#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define SS_PIN 10
#define RST_PIN 9

MFRC522 rfid(SS_PIN, RST_PIN);
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Registered students
struct Student {
  String uid;
  String name;
  bool present;
};

Student students[] = {
  {"01020304", "THANUSREE", false},
  {"11223344", "DHANA", false},
  {"55667788", "SNEHA", false},
  {"AABBCCDD", "PRIYA", false},
  {"C0FFEE99", "KIRAN", false}
};

const int totalStudents =
  sizeof(students) / sizeof(students[0]);

int attendanceCount = 0;

void setup() {
  Serial.begin(9600);

  // LCD setup
  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("RFID ATTENDANCE");
  lcd.setCursor(0, 1);
  lcd.print("System Starting");

  // RFID setup
  SPI.begin();
  rfid.PCD_Init();

  delay(2000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Scan Your Card");

  Serial.println("RFID Attendance System");
  Serial.println("System Ready");
}

void loop() {

  if (!rfid.PICC_IsNewCardPresent()) {
    return;
  }

  if (!rfid.PICC_ReadCardSerial()) {
    return;
  }

  // Read card UID
  String cardUID = "";

  for (byte i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] < 0x10) {
      cardUID += "0";
    }

    cardUID += String(rfid.uid.uidByte[i], HEX);
  }

  cardUID.toUpperCase();

  Serial.print("Card UID: ");
  Serial.println(cardUID);

  bool found = false;

  for (int i = 0; i < totalStudents; i++) {

    if (cardUID == students[i].uid) {

      found = true;

      lcd.clear();

      if (students[i].present == false) {

        students[i].present = true;
        attendanceCount++;

        lcd.setCursor(0, 0);
        lcd.print("Attendance Marked");

        lcd.setCursor(0, 1);
        lcd.print(students[i].name);

        Serial.print("Welcome, ");
        Serial.println(students[i].name);

        Serial.println("Attendance: PRESENT");
        Serial.print("Total Present: ");
        Serial.println(attendanceCount);

      } else {

        lcd.setCursor(0, 0);
        lcd.print("Already Marked");

        lcd.setCursor(0, 1);
        lcd.print(students[i].name);

        Serial.println("Attendance Already Marked");
      }

      break;
    }
  }

  if (!found) {

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Invalid Card");

    lcd.setCursor(0, 1);
    lcd.print("Access Denied");

    Serial.println("Unknown RFID Card");
  }

  delay(2000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Scan Your Card");

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}