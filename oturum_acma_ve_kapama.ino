#include <MFRC522.h>
#include <SPI.h>
#include <BleKeyboard.h>

#define SDA D10 
#define RST D0 
MFRC522 rfid(SDA, RST); 

BleKeyboard bleKeyboard("Deneyap RFID Klavye", "Deneyap", 100);

byte loginCard[4] = {0x3B, 0xAA, 0x43, 0x0E};
byte sleepCard[4] = {0x1A, 0x3A, 0xD9, 0x73};


void setup() {
  Serial.begin(115200); 
  Serial.println("RFID Okuyucu Baslatiliyor...");
  
  SPI.begin();
  rfid.PCD_Init();
  
  Serial.println("Bluetooth Klavye Baslatiliyor...");
  bleKeyboard.begin(); 

  Serial.println("Lutfen bir kart okutun...");
}

void loop() {
  if (bleKeyboard.isConnected()) {
    if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
      delay(50);
      return;
    }

    Serial.print("Okunan Kart UID (HEX): ");
    for (byte i = 0; i < rfid.uid.size; i++) {
      if(rfid.uid.uidByte[i] < 0x10) Serial.print("0"); 
      Serial.print(rfid.uid.uidByte[i], HEX);
    }
    Serial.println();

    if (memcmp(rfid.uid.uidByte, loginCard, rfid.uid.size) == 0) {
      Serial.println("Giris Karti Algilandi (Uyandirma & Sifre).");
      bleKeyboard.press(KEY_RETURN); 
      bleKeyboard.releaseAll();
      delay(1000); 
      bleKeyboard.print("Şifre"); // Bilgisayarın şifresini giriniz.
      bleKeyboard.press(KEY_RETURN);
      bleKeyboard.releaseAll();
    }
    else if (memcmp(rfid.uid.uidByte, sleepCard, rfid.uid.size) == 0) {
      Serial.println("Uyku Karti Algilandi. Bilgisayar uyku moduna aliniyor...");

      bleKeyboard.press(KEY_LEFT_GUI);
      bleKeyboard.press('l');
      bleKeyboard.releaseAll();
      delay(300); 
     
      
    } else {
      Serial.println("Yetkisiz Kart!");
    }

    rfid.PICC_HaltA();    
    rfid.PCD_StopCrypto1(); 
    delay(1400);
  } else {
    Serial.println("Bluetooth baglantisi bekleniyor...");
    delay(1000);
  }
}