## Lizenz
Dieses Projekt steht unter der [Creative Commons Attribution-NonCommercial 4.0 International License](https://creativecommons.org/licenses/by-nc/4.0/).# ESP32-S3-ETH-RJ45-with-Mongoose-Dash

Dieses Projekt implementiert einen einfachen Webserver auf einem ESP32-S3 Mikrocontroller mit einem W5500 Ethernet-Shield. Der Server hostet eine Webseite mit einem Button und einem Textfeld, die Serial-Ausgaben auf dem ESP32 auslösen.

## Hardware

### Board: Integriertes ESP32-Board mit W5500 Ethernet und USB-C
- **Mikrocontroller:** ESP32-S3 (240 MHz, 8 MB Flash, 320 KB RAM)
- **Ethernet-Modul:** Integrierter W5500-Chip (SPI-basiert, RJ45-Anschluss für Ethernet)
- **Anschlüsse:** USB-C für Stromversorgung und Programmierung
- **Stromversorgung:** Über USB-C (5V)
- **Pinbelegung für W5500:**
  - MOSI: GPIO 11
  - MISO: GPIO 13
  - SCK: GPIO 12
  - CS: GPIO 14
  - RST: GPIO 9
  - INT: GPIO 10 (optional)

### Bilder
![Board](https://github.com/user-attachments/assets/3f4dc448-927f-484f-8d96-ed968148da22)
![Pinout](https://github.com/user-attachments/assets/8f5c0606-4138-4296-b900-de70891d49a0)

**Bezugsquelle:** [AliExpress](https://de.aliexpress.com/item/1005007945002026.html)

## Software / Code

### Übersicht
- **Framework:** Arduino (ESP32 Core)
- **Bibliotheken:**
  - Ethernet: Geforkte Version von [maxgerhardt/Ethernet](https://github.com/maxgerhardt/Ethernet) (ESP32-kompatibel)
  - SPI: Standard ESP32 SPI
- **Funktionalität:**
  - Ethernet-Initialisierung mit statischer IP (192.168.1.177)
  - Webserver auf Port 80
  - Webseite mit Button (triggert Serial "Button wurde gedrueckt!") und Textfeld (sendet Nachricht an Serial)
  - Anti-Duplikatschutz: Verhindert doppelte Serial-Ausgaben innerhalb von 1 Sekunde

### Code-Struktur
- `src/main.cpp`: Hauptcode mit Setup, Loop und HTTP-Handler
- `platformio.ini`: PlatformIO-Konfiguration (Board, Libs, Upload-Einstellungen)

### Setup-Prozess
1. Ethernet-Shield resetten
2. SPI initialisieren
3. Ethernet mit MAC, IP, Gateway und Subnet konfigurieren
4. Server starten
5. In der Loop: Auf Clients warten, Anfragen parsen, Serial-Ausgaben und HTTP-Response senden

## Bekannte Probleme und Fixes

### 1. Kompilierungsfehler: "EthernetServer abstrakt"
**Problem:** Die Standard-Arduino Ethernet-Lib (arduino-libraries/Ethernet) ist nicht ESP32-kompatibel, da `EthernetServer` von einer abstrakten `Server`-Klasse erbt.
**Fix:** Verwende die geforkte Lib `https://github.com/maxgerhardt/Ethernet.git` in `platformio.ini` unter `lib_deps`.

### 2. IP-Adresse 0.0.0.0
**Problem:** Ethernet-Initialisierung schlägt fehl, oft wegen falscher Pins oder fehlender Gateway/Subnet-Parameter.
**Fix:**
- Pins korrekt setzen (siehe Hardware-Abschnitt).
- `Ethernet.begin(mac, ip, gateway, dns, subnet)` verwenden statt nur `Ethernet.begin(mac, ip)`.
- Debug-Ausgaben hinzufügen, um Initialisierung zu überwachen.

### 3. Doppelte Serial-Nachrichten
**Problem:** Webbrowser senden manchmal mehrere HTTP-Anfragen (z. B. für Seite + Ressourcen), was zu doppelten Serial-Ausgaben führt.
**Fix:** Zeitstempel-Variablen (`lastButtonPress`, `lastMsgTime`) verwenden, um Aktionen innerhalb von 1 Sekunde zu ignorieren.

### 4. Serieller Monitor in VS Code funktioniert nicht
**Problem:** Port oder Baudrate falsch.
**Fix:** PlatformIO Serial Monitor verwenden, Port auf COM8 (oder korrekten Port) setzen, Baudrate 115200.

## Installation und Setup

1. **Voraussetzungen:**
   - PlatformIO (VS Code Extension)
   - ESP32-S3 DevKitC-1 Board
   - W5500 Ethernet-Shield

2. **Projekt klonen:**
   ```bash
   git clone https://github.com/MojitoShader/ESP32-S3-ETH-RJ45-with-Mongoose-Dash.git
   cd ESP32-S3-ETH-RJ45-with-Mongoose-Dash/ESP32S3 ETH
   ```

3. **Libs installieren:** PlatformIO lädt automatisch via `platformio.ini`.

4. **Build und Upload:**
   ```bash
   pio run --target upload
   ```

5. **Monitor starten:**
   ```bash
   pio device monitor
   ```

6. **Testen:** Öffne Browser zu `http://192.168.1.177`, drücke Button oder sende Text – Serial-Ausgaben erscheinen.

## Lizenz
[LICENSE](LICENSE) (füge entsprechende Lizenz hinzu, z. B. MIT).

