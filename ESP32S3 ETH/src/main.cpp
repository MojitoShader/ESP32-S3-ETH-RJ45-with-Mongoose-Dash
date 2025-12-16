#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>

// --- W5500 Pins laut Board-Pinout ---
#define ETH_MOSI  11
#define ETH_MISO  12
#define ETH_SCK   13
#define ETH_CS    14
#define ETH_RST   9
#define ETH_INT   10   // optional

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 177);

EthernetServer server(80);

// Zeitstempel für doppelte Anfragen verhindern
unsigned long lastButtonPress = 0;
unsigned long lastMsgTime = 0;

void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("Starting setup...");

  // Reset W5500
  pinMode(ETH_RST, OUTPUT);
  digitalWrite(ETH_RST, LOW);
  delay(50);
  digitalWrite(ETH_RST, HIGH);
  delay(150);
  Serial.println("W5500 reset done");

  // SPI auf richtige Pins legen
  SPI.begin(ETH_SCK, ETH_MISO, ETH_MOSI, ETH_CS);
  Serial.println("SPI initialized");

  Ethernet.init(ETH_CS);
  Serial.println("Ethernet init done");

  // Statische IP mit Gateway und Subnet
  IPAddress gateway(192, 168, 1, 1);
  IPAddress subnet(255, 255, 255, 0);
  Ethernet.begin(mac, ip, gateway, gateway, subnet);
  Serial.println("Ethernet begin attempted");

  server.begin();
  Serial.println("Server begin done");

  Serial.println("Ethernet online");
  Serial.print("IP: ");
  Serial.println(Ethernet.localIP());
}

void loop() {
  EthernetClient client = server.available();
  if (!client) return;

  String request = "";
  bool currentLineIsBlank = false;

  while (client.connected()) {
    if (client.available()) {
      char c = client.read();
      request += c;

      // Header-Ende erreicht?
      if (c == '\n' && currentLineIsBlank) {

        // --- 1) Button-Auswertung: Nur in der ersten Zeile ---
        String firstLine = request.substring(0, request.indexOf('\n'));
        if (firstLine.indexOf("GET /?press=1") >= 0 && millis() - lastButtonPress > 1000) {
          Serial.println("Button wurde gedrueckt!");
          lastButtonPress = millis();
        }

        // --- 2) Textfeld-Auswertung: msg= aus der ersten Zeile ---
        int queryStart = firstLine.indexOf('?');
        if (queryStart >= 0) {
          int queryEnd = firstLine.indexOf(' ', queryStart);
          if (queryEnd < 0) queryEnd = firstLine.length();
          String query = firstLine.substring(queryStart + 1, queryEnd);
          
          int msgPos = query.indexOf("msg=");
          if (msgPos >= 0 && millis() - lastMsgTime > 1000) {
            int endPos = query.indexOf('&', msgPos);
            if (endPos < 0) endPos = query.length();
            String rawMsg = query.substring(msgPos + 4, endPos);

            // Primitive URL-Decode: '+' -> ' ', %20 usw.
            String decoded = "";
            for (int i = 0; i < (int)rawMsg.length(); i++) {
              char ch = rawMsg[i];
              if (ch == '+') {
                decoded += ' ';
              } else if (ch == '%' && i + 2 < (int)rawMsg.length()) {
                char h1 = rawMsg[i + 1];
                char h2 = rawMsg[i + 2];
                int v = 0;
                if (h1 >= '0' && h1 <= '9') v += (h1 - '0') << 4;
                else if (h1 >= 'A' && h1 <= 'F') v += (h1 - 'A' + 10) << 4;
                else if (h1 >= 'a' && h1 <= 'f') v += (h1 - 'a' + 10) << 4;
                if (h2 >= '0' && h2 <= '9') v += (h2 - '0');
                else if (h2 >= 'A' && h2 <= 'F') v += (h2 - 'A' + 10);
                else if (h2 >= 'a' && h2 <= 'f') v += (h2 - 'a' + 10);
                decoded += (char)v;
                i += 2; // zwei Hex-Zeichen überspringen
              } else {
                decoded += ch;
              }
            }

            Serial.print("Nachricht von Web: ");
            Serial.println(decoded);
            lastMsgTime = millis();
          }
        }

        // --- HTTP-Antwort (siehe Abschnitt 1) ---
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/html");
        client.println("Connection: close");
        client.println();
        client.println("<!DOCTYPE html>");
        client.println("<html><head><meta charset=\"UTF-8\"><title>ESP32-S3 ETH Test</title></head><body>");
        client.println("<h1>ESP32-S3 W5500 Test</h1>");
        client.println("<p>Button: schreibt eine feste Meldung auf Serial.</p>");
        client.println("<form method=\"GET\" action=\"/\">");
        client.println("<button type=\"submit\" name=\"press\" value=\"1\">Serial Button</button>");
        client.println("</form>");
        client.println("<hr>");
        client.println("<p>Text an ESP senden:</p>");
        client.println("<form method=\"GET\" action=\"/\">");
        client.println("<input type=\"text\" name=\"msg\" placeholder=\"Nachricht\" />");
        client.println("<button type=\"submit\">Senden</button>");
        client.println("</form>");
        client.println("</body></html>");
        break;
      }

      if (c == '\n') {
        currentLineIsBlank = true;
      } else if (c != '\r') {
        currentLineIsBlank = false;
      }
    }
  }

  delay(1);
  client.stop();
}
