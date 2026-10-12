# 🔑 RFID Kontrollü BLE Bluetooth Klavye ile Oturum Açma & Kilitleme

<p align="center">
  <a href="#donanimlar"><img src="https://img.shields.io/badge/Donan%C4%B1m-Deneyap%20Kart%201A-24292e?style=flat-square&logo=espressif" alt="Donanım" /></a>
  <a href="#proje-ozeti"><img src="https://img.shields.io/badge/Protokol-BLE%20HID%20Klavye-24292e?style=flat-square&logo=bluetooth" alt="Bluetooth" /></a>
  <a href="#pin-baglantilari"><img src="https://img.shields.io/badge/RFID-MFRC522%2013.56MHz-24292e?style=flat-square" alt="RFID" /></a>
  <a href="#calisma-mantigi"><img src="https://img.shields.io/badge/G%C3%BCvenlik-Donan%C4%B1m%20Tabanl%C4%B1%20Giri%C5%9F-24292e?style=flat-square" alt="Güvenlik" /></a>
  <a href="#lisans"><img src="https://img.shields.io/badge/Lisans-MIT-24292e?style=flat-square" alt="Lisans" /></a>
</p>

> **Fiziksel RFID kartları kablosuz kimlik doğrulayıcıya dönüştürün!** Deneyap Kart 1A'nın dahili Bluetooth Low Energy (BLE) donanımı kullanılarak bilgisayara **sanal bir kablosuz klavye (HID)** gibi bağlanılır. Yetkili giriş kartı okutulduğunda bilgisayarı uyandırıp şifreyi otomatik yazar; kilit kartı okutulduğunda ise `Windows + L` kısayoluyla tek dokunuşta ekranı kilitler.

---

<a id="icindekiler"></a>
## 📑 İçindekiler

1. [Proje Özeti ve Temel Özellikler](#proje-ozeti)
2. [Canlı Çalışma Demosu (Video)](#canli-demo)
3. [Çalışma Mantığı ve Kart Senaryoları](#calisma-mantigi)
4. [Kullanılan Donanımlar ve Modüller](#donanimlar)
5. [Pin Bağlantı Haritası (SPI)](#pin-baglantilari)
6. [Adım Adım Kurulum ve Yapılandırma](#kurulum)
7. [Kendi Kartınızı ve Şifrenizi Ekleme Rehberi](#ozellestirme)
8. [Karşılaşılabilecek Sorunlar ve Çözümleri](#sorunlar-ve-cozumleri)
9. [Geliştirici](#gelistirici)
10. [Lisans](#lisans)

---

<a id="proje-ozeti"></a>
## 🎯 1. Proje Özeti ve Temel Özellikler

Bu proje, günlük bilgisayar kullanımında güvenlik ve ergonomiyi birleştiren fiziksel bir 2FA (iki faktörlü) donanım anahtarı gibi çalışır:

- **Sıfır Kablo (Kablosuz BLE HID):** Bilgisayara herhangi bir USB kablosu bağlamadan, tıpkı piyasadaki orijinal Bluetooth klavyeler gibi `"Deneyap RFID Klavye"` adıyla eşleşir.
- **Otonom Oturum Açma:** Giriş kartı okutulduğunda `Enter ➔ Şifre ➔ Enter` sırasını 1 saniye içinde göndererek parola yazma zahmetini ortadan kaldırır.
- **Tek Dokunuşta Kilitleme (Gizlilik Kalkanı):** Masadan kalkarken uyku kartını okutarak anında `Windows + L` kombinasyonu ile bilgisayarı korumaya alır.
- **Yetkisiz Kart Koruması:** Tanımlı olmayan yabancı kartlar okutulduğunda klavye emülasyonu tetiklenmez ve Seri Portta `Yetkisiz Kart!` uyarısı basılır.

---

<a id="canli-demo"></a>
## 🎥 2. Canlı Çalışma Demosu

> 💡 **Video Demosu:** Kartın okuyucuya yaklaştırılmasıyla birlikte bilgisayarın anında uyanıp şifre girilerek açıldığı ve diğer kartla kilitlendiği çalışma anı:

<p align="center">
  <!-- Çektiğin videoyu GitHub README editörüne sürükleyip bırakınca oluşan linki buraya yapıştırabilirsin -->
  <video src="https://github.com/user-attachments/assets/ab198d41-5e2e-4e2a-962b-af1c2eaffcc9" controls="controls" width="75%" style="border-radius: 12px; box-shadow: 0 4px 25px rgba(0,0,0,0.25);" onerror="this.outerHTML='<div style=padding:20px;background:#1e293b;border-radius:10px;text-align:center;color:#38bdf8>🎥 Videoyu GitHub editörüne sürükle-bırak yaparak linkini buraya ekleyebilirsiniz.</div>';"></video>
</p>

<p align="center">
  <em>🔑 RFID Kart ile Temassız Bilgisayar Oturumu Açma ve Kilitleme Demosu</em>
</p>

---

<a id="calisma-mantigi"></a>
## 🧠 3. Çalışma Mantığı ve Kart Senaryoları

```text
        ┌────────────────────────────────┐
        │   RFID Kart Okutulur (RC522)   │
        └───────────────┬────────────────┘
                        │
                        ▼
             [ Kart UID Kontrolü ]
            /          │          \
           /           │           \
 [ Giriş Kartı ]  [ Uyku Kartı ]  [ Yetkisiz Kart ]
 (3B AA 43 0E)     (1A 3A D9 73)   (Yabancı UID)
       │               │                │
       ▼               ▼                ▼
 1. Enter (Uyan)   1. Win + L       İşlem Yapma
 2. Şifreyi Yaz       (Ekranı Kilitler) (Erişim Engellendi)
 3. Enter (Giriş)
```

---

<a id="donanimlar"></a>
## 🧰 4. Kullanılan Donanımlar ve Modüller

| Bileşen | Model / Açıklama | Görev |
| :--- | :--- | :--- |
| **Ana Kart** | Deneyap Kart 1A (ESP32) | BLE Bluetooth yığını ve SPI veri işleme |
| **RFID Okuyucu** | RC522 RFID Modülü (13.56 MHz) | Manyetik kart ve anahtarlıkların UID kodunu okuma |
| **RFID Kartlar** | 2x Mifare 1K Kart / Anahtarlık | Oturum Açma (Beyaz Kart) ve Kilitleme (Mavi Anahtarlık) |
| **Bağlantı** | Jumper Kablolar & Breadboard | SPI haberleşme ve 3.3V güç dağıtımı |

---

<a id="pin-baglantilari"></a>
## 🔌 5. Pin Bağlantı Haritası (SPI)

RC522 RFID modülü Deneyap Kart 1A ile donanımsal **SPI protokolü** üzerinden haberleşir:

| RC522 Pini | Deneyap Kart 1A Pini | Açıklama |
| :--- | :--- | :--- |
| **SDA (SS)** | **D10** | Chip Select / Köle Seçim Pini |
| **SCK** | **SCK** | SPI Seri Saat Sinyali |
| **MOSI** | **MOSI** | Master Out Slave In |
| **MISO** | **MISO** | Master In Slave Out |
| **RST** | **D0** | Modül Sıfırlama Pini |
| **3.3V** | **3.3V** | Modül Güç Beslemesi *(5V bağlamayın!)* |
| **GND** | **GND** | Toprak Hattı |

> ⚠️ **Kritik Voltaj Uyarısı:** RC522 modülü **3.3V** ile çalışır. 5V pinine bağlanması modülün RF çipini yakabilir.

---

<a id="kurulum"></a>
## 🚀 6. Adım Adım Kurulum ve Yapılandırma

### 1. Arduino Kütüphanelerini Kurun
Arduino IDE'de **Araçlar ➔ Kütüphaneleri Yönet** menüsünden şu iki kütüphaneyi aratıp kurun:
- `MFRC522` *(by GithubCommunity)*
- `ESP32-BLE-Keyboard` *(T-vK tarafından geliştirilen BLE HID kütüphanesi)*

### 2. Kodu Yükleyin
- `oturum_acma_ve_kapama.ino` dosyasını Arduino IDE ile açın.
- Kart olarak **Deneyap Kart 1A** seçin ve kodu karta yükleyin.

### 3. Bilgisayara Bluetooth ile Eşleştirin
1. Bilgisayarınızın **Ayarlar ➔ Bluetooth ve Cihazlar ➔ Cihaz Ekle** menüsünü açın.
2. Listede görünen `"Deneyap RFID Klavye"` cihazına tıklayıp eşleştirin.
3. Artık sistem klavye olarak hazır!

---

<a id="ozellestirme"></a>
## 🛠️ 7. Kendi Kartınızı ve Şifrenizi Ekleme Rehberi

Elinizdeki kartların UID kodları farklı olacağından, kartlarınızı sisteme tanıtmak için:

1. Kodu karta yükledikten sonra Arduino IDE **Seri Monitörünü (115200 baud)** açın.
2. Kartınızı okuyucuya yaklaştırın. Ekranda şöyle bir satır çıkacaktır:
   ```text
   Okunan Kart UID (HEX): 3BAA430E
   ```
3. Bu 4 baytlık kodu koddaki ilgili alana yazın:
   ```cpp
   byte loginCard[4] = {0x3B, 0xAA, 0x43, 0x0E}; // Giriş yapacak kartınız
   byte sleepCard[4] = {0x1A, 0x3A, 0xD9, 0x73}; // Kilitleyecek kartınız
   ```
4. Giriş şifrenizi `bleKeyboard.print("KendiSifreniz");` kısmına yazarak kodu yeniden yükleyin.

---

<a id="sorunlar-ve-cozumleri"></a>
## ⚠️ 8. Karşılaşılabilecek Sorunlar ve Çözümleri

### 1. Türkçe Karakterler Yanlış Yazılıyor
- **Neden:** BLE Keyboard varsayılan olarak US (Amerikan) Q klavye tuş düzenini kullanır. Türkçe harfler (`i, ş, ğ, ö, ç, ı`) farklı karakterlere dönüşebilir.
- **Çözüm:** Bilgisayar oturum açma şifrenizde standart ASCII karakterler (İngilizce harfler ve rakamlar) kullanmanız önerilir.

### 2. Bilgisayar Bluetooth ile Bağlanmıyor
- **Çözüm:** Bilgisayarınızın Bluetooth listesinden eski `"Deneyap RFID Klavye"` eşleşmesini kaldırıp (cihazı unutturup) yeniden eşleştirin.

---

<a id="gelistirici"></a>
## 👩‍💻 9. Geliştirici

**Kevser ÜSTÜN**  
🎓 Giresun Üniversitesi — Bilgisayar Mühendisliği  
🔗 [GitHub Profilim](https://github.com/kvsrustun)

---

<a id="lisans"></a>
## 📄 10. Lisans

Bu proje [MIT Lisansı](LICENSE) kapsamında açık kaynak olarak paylaşılmıştır.



