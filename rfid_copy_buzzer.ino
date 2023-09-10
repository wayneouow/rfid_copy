#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#define I2C_Addr 0x27    
LiquidCrystal_I2C lcd(I2C_Addr,16,2); // set LCD address to 0x27 as 16 chars with 2 line display

#define RST_PIN   9     // Configurable, see typical pin layout above
#define SS_PIN    10    // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance

#define NEW_UID {0xDE, 0xAD, 0xBE, 0xEF}

MFRC522::MIFARE_Key key;

void setup() {
  lcd.init();                      // initialize the lcd 
  lcd.backlight();
  Serial.begin(115200);  // Initialize serial communications with the PC
  while (!Serial);     // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  SPI.begin();         // Init SPI bus
  mfrc522.PCD_Init();  // Init MFRC522 card
  
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
 
 
  pinMode(7,OUTPUT);
}

void loop() {

  Serial.println("Please place the old card...");
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("RFID UID Copyer");
  lcd.setCursor(0,1);
  lcd.print("Place old card..");
  
  while( !mfrc522.PICC_IsNewCardPresent()){}

  if( !mfrc522.PICC_ReadCardSerial()){
    
    Serial.println("Read UID Error!!");
    lcd.setCursor(0,1);
    lcd.print("Read UID Error!!");
    delay(1000);
    return;    
  }

  byte newUid[] = NEW_UID;
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("UID:");
  Serial.print(F("Card UID:"));

  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    newUid[i]=mfrc522.uid.uidByte[i];
    lcd.print(newUid[i] < 0x10 ? "0" : "");
    lcd.print(newUid[i],HEX);
  } 

  Serial.println();
  Serial.println("Please remove old card!");
  Serial.println();
  lcd.setCursor(0,1);

  lcd.print("Remove old card!");
  digitalWrite(7,HIGH);
  delay(200);
  digitalWrite(7,LOW);
 
  WaitCardLeft();

  delay(1000);
  
  digitalWrite(7,HIGH);
  delay(1000);
  digitalWrite(7,LOW);
  
  Serial.println("Now place the new  card!");
  lcd.setCursor(0,1);
  lcd.print("Now the new card");
  
  while(!mfrc522.PICC_IsNewCardPresent()){
    delay(50);
  }

  lcd.setCursor(0,1);
  lcd.print("New card Detect!");

  while(!mfrc522.PICC_ReadCardSerial()){
    delay(50);
  }

  //delay(1000);

  if ( mfrc522.MIFARE_SetUid(newUid, (byte)4, true) ) {
    Serial.println(F("Wrote new UID to card."));
    lcd.setCursor(0,1);
    lcd.print(" Copy Success!  ");
    
    digitalWrite(7,HIGH);
    delay(150);
    digitalWrite(7,LOW);
    delay(100);
    digitalWrite(7,HIGH);
    delay(150);
    digitalWrite(7,LOW);
  }

  else{
    Serial.println("Wrote new UID failed!!");
    lcd.setCursor(0,1);
    lcd.print("Wrote UID failed");
  }

  delay(2000);

  WaitCardLeft();
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();  // Stop encryption on PCD
  delay(1000);       

 }

void WaitCardLeft(){
  byte count=0;
  while(count<5){
      if( ! mfrc522.PICC_IsNewCardPresent())
        count++;
      else
        count=0;
      delay(50);
  }

}
