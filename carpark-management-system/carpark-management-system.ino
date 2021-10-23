/*===================================================================

  This code is used to develop an automated Car Park Management system prototype using RFID.

  The system is equipped with following components.
  * IR sensors to check the availability of slots in the car park
  * RFID readers for access control
  * Servo Motors for the Car Park Barrier
  * LCD Display to show the availble and occupied slots.
  
  DEVELOPER: Pasindu Liyanage
  DATE: 2018.10.04
  =====================================================================*/


/*===================================================================

          /Sensor Intializing

  ===================================================================== */



#define slot1 2
#define slot2 3
#define slot3 4
#define slot4 5
#define slot5 6

/*===================================================================

          //LCD Display

  ===================================================================== */


#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 4);



/*===================================================================

          //Servo Gat

  ===================================================================== */


#include<Servo.h>

Servo Entgate;
Servo Closegate;

#define egate 7
#define cgate 8

/*===================================================================

          //RFID

  ===================================================================== */
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 53
#define RST_PIN 48

MFRC522 mfrc522(SS_PIN, RST_PIN);
/*===================================================================

          //Lights and Switches
  ===================================================================== */
#define exit_btn 9
#define green 16
#define blue 15
#define orange 14


/*===================================================================

          //Global Variables

  ===================================================================== */
int car_count = 0;
bool s1state, s2state, s3state, s4state, s5state = 0;
bool s1read, s2read, s3read, s4read, s5read = 0;
int occupied, availbl = 0;
const int gateOtime = 3000;

int clr_flag, prevclr_flag = 0;

String check = "";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();

  lcd.init();
  lcd.init();
  lcd.backlight();
  lcd.clear();

  initSensors();

  Entgate.attach(egate);
  Closegate.attach(cgate);

  Entgate.write(90);
  Closegate.write(90);


}



void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(blue, HIGH);
  readRFID();
  gateopen();
  checkSlots();
  occupied = car_count;
  availbl = 5 - occupied;
  displaycount();

}


void initSensors() {
  pinMode(slot1, INPUT);
  pinMode(slot2, INPUT);
  pinMode(slot3, INPUT);
  pinMode(slot4, INPUT);
  pinMode(slot5, INPUT);

  pinMode(green, OUTPUT);
  pinMode(blue, OUTPUT);
  pinMode(orange, OUTPUT);

}

void checkSlots() {

  s1read = !digitalRead(slot1);
  s2read = !digitalRead(slot2);
  s3read = !digitalRead(slot3);
  s4read = !digitalRead(slot4);
  s5read = !digitalRead(slot5);

  if (s1read == 1) {
    if (s1state == 0) {
      car_count++;
      s1state = 1;
    }
  }

  if (s2read == 1) {
    if (s2state == 0) {
      car_count++;
      s2state = 1;
    }
  }

  if (s3read == 1) {
    if (s3state == 0) {
      car_count++;
      s3state = 1;
    }
  }

  if (s4read == 1) {
    if (s4state == 0) {
      car_count++;
      s4state = 1;
    }
  }

  if (s5read == 1) {
    if (s5state == 0) {
      car_count++;
      s5state = 1;
    }
  }



  if (s1read == 0) {
    if (s1state == 1) {
      car_count--;
      s1state = 0;
    }
  }

  if (s2read == 0) {
    if (s2state == 1) {
      car_count--;
      s2state = 0;
    }
  }

  if (s3read == 0) {
    if (s3state == 1) {
      car_count--;
      s3state = 0;
    }
  }

  if (s4read == 0) {
    if (s4state == 1) {
      car_count--;
      s4state = 0;
    }
  }

  if (s5read == 0) {
    if (s5state == 1) {
      car_count--;
      s5state = 0;
    }
  }
}

void displaycount() {
  lcd.setCursor(0, 0);
  lcd.print("Smart Car Park");


  if (occupied == 5) {
    clr_flag = 1;
    lcd.setCursor(0, 1);
    lcd.print("Occupied");
    lcd.setCursor(12, 1);
    lcd.print(occupied);
    lcd.setCursor(-4, 2);
    lcd.print("Available");
    lcd.setCursor(8, 2);
    lcd.print(availbl);
    lcd.setCursor(-3, 3);
    lcd.print("Car Park Full");

  } else if (occupied == 4) {
    clr_flag = 2;
    lcd.setCursor(0, 1);
    lcd.print("Occupied");
    lcd.setCursor(12, 1);
    lcd.print(occupied);
    lcd.setCursor(-4, 2);
    lcd.print("Available");
    lcd.setCursor(8, 2);
    lcd.print(availbl);
    lcd.setCursor(-3, 3);
    lcd.print("Limited Acess");
  } else {
    clr_flag = 3;
    lcd.setCursor(0, 1);
    lcd.print("Occupied");
    lcd.setCursor(12, 1);
    lcd.print(occupied);
    lcd.setCursor(-4, 2);
    lcd.print("Available");
    lcd.setCursor(8, 2);
    lcd.print(availbl);
    lcd.setCursor(-4, 3);
    lcd.print("Slots Available!");
  }
  if (clr_flag != prevclr_flag) {
    lcd.clear();
  }
  prevclr_flag = clr_flag;

}

void readRFID() {
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  Serial.print("UID tag :");
  String content = "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  content.toUpperCase();
  check = content;
}

void gateopen() {
  if (check.substring(1) == "3D C5 0A C2" || check.substring(1) == "DA 84 C5 DB" || check.substring(1) == "85 37 5F 7B" || check.substring(1) == "FE 18 C6 DB" || check.substring(1) == "67 98 62 8B") {
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("SPV Detected");
    digitalWrite(blue, LOW);
    if (occupied < 5) {

      digitalWrite(green, HIGH);
      lcd.setCursor(-3, 2);
      lcd.print("Acess Granted");
      delay(1000);

      lcd.setCursor(-3, 3);
      lcd.print("Gate Opening!");

      Entgate.write(0);
      delay(gateOtime);
      Entgate.write(90);
      digitalWrite(green, LOW);

    } else {

      lcd.setCursor(-3, 2);
      lcd.print("Access Denied");
      for (int i = 0; i < 5; i++) {
        digitalWrite(orange, HIGH);
        delay(200);
        digitalWrite(orange, LOW);
        delay(200);
      }
    }
    lcd.clear();
  }

  if (check.substring(1) == "50 84 58 14" || check.substring(1) == "B9 CF 8C 01" || check.substring(1) == "29 A0 85 01" || check.substring(1) == "59 FD 88 01" || check.substring(1) == "19 99 86 01") {
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("HPV Detected");
    digitalWrite(blue, LOW);
    if (occupied < 4) {

      digitalWrite(green, HIGH);
      lcd.setCursor(-3, 2);
      lcd.print("Acess Granted");
      delay(1000);

      lcd.setCursor(-3, 3);
      lcd.print("Gate Opening!");

      Entgate.write(0);
      delay(gateOtime);
      Entgate.write(90);
      digitalWrite(green, LOW);

    } else {

      lcd.setCursor(-3, 2);
      lcd.print("Access Denied");
      for (int i = 0; i < 5; i++) {
        digitalWrite(orange, HIGH);
        delay(200);
        digitalWrite(orange, LOW);
        delay(200);
      }
    }
    lcd.clear();
  }

  if (digitalRead(exit_btn)  == 1) {
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("Smart Car Park");
    lcd.setCursor(-1, 2);
    lcd.print("Thank you!");
    lcd.setCursor(0, 3);
    lcd.print("Good Bye");

    Closegate.write(180);
    delay(gateOtime);
    Closegate.write(90);
    lcd.clear();
  }

  check = "";


}
