#include "DisplayManager.h"
#include "Config.h"

void initializeDisplay() {
  tft.init();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);
}

void displayWelcome() {
  tft.fillScreen(TFT_BLACK);
  
  tft.setTextColor(TFT_RED, TFT_BLACK);
  tft.setTextSize(3);
  tft.setCursor(35, 40);
  tft.println("CBS");
  
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(15, 80);
  tft.println("EMERGENCY");
  tft.setCursor(30, 105);
  tft.println("ALERT");
  tft.setCursor(20, 130);
  tft.println("SYSTEM");
  
  tft.drawRect(5, 5, 230, 310, TFT_RED);
  tft.drawRect(6, 6, 228, 308, TFT_RED);
  
  tft.setTextSize(1);
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.setCursor(20, 180);
  tft.println("Cell Broadcast Service");
  tft.setCursor(35, 195);
  tft.println("UK Emergency Alerts");
  
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.setCursor(30, 220);
  tft.println("4G LTE SIM7600E-H");
  
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setCursor(50, 280);
  tft.println("READY!");
  
  delay(30000);
}

void displayReadyState() {
  systemState.currentAlert = "ready";
  
  tft.fillScreen(TFT_DARKGREEN);
  
  tft.fillRect(0, 0, 240, 30, TFT_GREEN);
  tft.setTextColor(TFT_BLACK, TFT_GREEN);
  tft.setTextSize(2);
  tft.setCursor(20, 7);
  tft.println("SYSTEM READY");
  
  drawCheckmarkIcon(95, 60);
  
  tft.setTextColor(TFT_WHITE, TFT_DARKGREEN);
  tft.setTextSize(2);
  tft.setCursor(10, 140);
  tft.println("CBS Monitoring");
  tft.setCursor(35, 165);
  tft.println("Active");
  
  tft.setTextSize(1);
  tft.setCursor(15, 200);
  tft.println("System operational");
  
  if(systemState.networkRegistered) {
    tft.setTextColor(TFT_GREEN, TFT_DARKGREEN);
    tft.setCursor(15, 230);
    tft.println("Network: READY");
  } else {
    tft.setTextColor(TFT_YELLOW, TFT_DARKGREEN);
    tft.setCursor(15, 230);
    tft.println("Check network");
  }
  
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_GREEN, HIGH);
}

void displayAlert(String alertType) {
  systemState.currentAlert = alertType;
  
  if(alertType == "ready") {
    displayReadyState();
  }
  else if(alertType == "fire") {
    displayAlertScreen("EMERGENCY", "FIRE ALERT", "Fire detected", 
                       "EVACUATE BUILDING", "IMMEDIATELY", TFT_RED, LED_RED);
    drawFireIcon(100, 40);
  }
  else if(alertType == "flood") {
    displayAlertScreen("EMERGENCY", "FLOOD WARNING", "SEVERE RISK", 
                       "EVACUATE TO", "HIGHER GROUND", TFT_RED, LED_RED);
    drawFloodIcon(100, 40);
  }
  else if(alertType == "weather") {
    displayAlertScreen("WARNING", "Severe Weather", "Heavy rain", 
                       "Stay indoors", "", TFT_YELLOW, LED_YELLOW);
    drawWeatherIcon(100, 40);
  }
  else if(alertType == "earthquake") {
    displayAlertScreen("EMERGENCY", "EARTHQUAKE", "TAKE COVER", 
                       "DROP COVER HOLD", "", TFT_RED, LED_RED);
    drawEarthquakeIcon(100, 40);
  }
  else if(alertType == "terrorism") {
    displayAlertScreen("EMERGENCY", "SECURITY ALERT", "Leave area", 
                       "immediately", "", TFT_RED, LED_RED);
    drawEmergencyIcon(100, 40);
  }
  else if(alertType == "chemical") {
    displayAlertScreen("EMERGENCY", "CHEMICAL HAZARD", "Stay indoors", 
                       "Close windows", "", TFT_RED, LED_RED);
    drawEmergencyIcon(100, 40);
  }
  else if(alertType == "nuclear") {
    displayAlertScreen("EMERGENCY", "NUCLEAR", "Seek shelter", 
                       "immediately", "", TFT_RED, LED_RED);
    drawEmergencyIcon(100, 40);
  }
  else if(alertType == "amber") {
    displayAlertScreen("AMBER ALERT", "Child Abduction", "Call 999", 
                       "if seen", "", TFT_ORANGE, LED_YELLOW);
    drawWarningIcon(100, 40);
  }
  else if(alertType == "safety") {
    displayAlertScreen("WARNING", "Public Safety", "Stay indoors", 
                       "until advised", "", TFT_YELLOW, LED_YELLOW);
    drawWarningIcon(100, 40);
  }
  else if(alertType == "test") {
    displayAlertScreen("INFORMATION", "Test Alert", "This is a test", 
                       "NO ACTION", "REQUIRED", TFT_BLUE, LED_GREEN);
    drawInfoIcon(100, 40);
  }
}

void displayAlertScreen(String severity, String headline, String line1, 
                        String line2, String line3, uint16_t color, int ledPin) {
  tft.fillScreen(TFT_BLACK);
  
  tft.fillRect(0, 0, 240, 30, color);
  tft.setTextColor(TFT_BLACK, color);
  tft.setTextSize(2);
  int16_t x = (240 - severity.length() * 12) / 2;
  tft.setCursor(x, 7);
  tft.println(severity);
  
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(10, 160);
  tft.println(headline);
  
  tft.setTextSize(1);
  tft.setCursor(10, 195);
  tft.println(line1);
  tft.setCursor(10, 210);
  tft.println(line2);
  tft.setCursor(10, 235);
  tft.println(line3);
  
  tft.drawRect(0, 0, 240, 320, color);
  tft.drawRect(1, 1, 238, 318, color);
  
  tft.fillCircle(220, 300, 8, color);
  
  digitalWrite(LED_RED, ledPin == LED_RED ? HIGH : LOW);
  digitalWrite(LED_YELLOW, ledPin == LED_YELLOW ? HIGH : LOW);
  digitalWrite(LED_GREEN, ledPin == LED_GREEN ? HIGH : LOW);
}

void displayTowerData(TowerInfo tower) {
  tft.fillScreen(TFT_BLACK);
  
  tft.fillRect(0, 0, 240, 30, TFT_BLUE);
  tft.setTextColor(TFT_WHITE, TFT_BLUE);
  tft.setTextSize(2);
  tft.setCursor(15, 7);
  tft.println("TOWER DATA");
  
  int tx = 120, ty = 50;
  tft.fillRect(tx-5, ty+20, 10, 30, TFT_CYAN);
  tft.fillTriangle(tx, ty, tx-15, ty+20, tx+15, ty+20, TFT_CYAN);
  tft.drawLine(tx-20, ty+25, tx-5, ty+15, TFT_YELLOW);
  tft.drawLine(tx+20, ty+25, tx+5, ty+15, TFT_YELLOW);
  
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.setTextSize(1);
  tft.setCursor(5, 100);
  tft.print("Cell ID: ");
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.println(tower.cellID);
  
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.setCursor(5, 115);
  tft.print("PCI: ");
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.println(tower.pci);
  
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.setCursor(5, 130);
  tft.print("Band: ");
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.println(tower.band);
  
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.setCursor(5, 145);
  tft.print("Channel: ");
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.println(tower.earfcn);
  
  tft.setCursor(5, 170);
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.println("SIGNAL STRENGTH:");
  
  int barWidth = map(tower.rsrp, -120, -60, 0, 200);
  if(barWidth < 0) barWidth = 0;
  if(barWidth > 200) barWidth = 200;
  
  uint16_t barColor = TFT_GREEN;
  if(tower.rsrp < -100) barColor = TFT_RED;
  else if(tower.rsrp < -90) barColor = TFT_ORANGE;
  
  tft.drawRect(5, 190, 200, 15, TFT_DARKGREY);
  tft.fillRect(6, 191, barWidth, 13, barColor);
  
  tft.setCursor(5, 210);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(1);
  tft.print("RSRP: ");
  tft.print(tower.rsrp);
  tft.println(" dBm");
  
  tft.setCursor(5, 225);
  tft.print("RSRQ: ");
  tft.print(tower.rsrq);
  tft.println(" dB");
  
  tft.setCursor(5, 240);
  tft.print("SINR: ");
  tft.print(tower.sinr);
  tft.println(" dB");
  
  tft.fillCircle(220, 300, 8, TFT_GREEN);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.setCursor(170, 295);
  tft.println("LIVE");
  
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.setCursor(5, 270);
  tft.println("Real-time from eNodeB");
  
  tft.drawRect(0, 0, 240, 320, TFT_BLUE);
  tft.drawRect(1, 1, 238, 318, TFT_BLUE);
}

void displayIncomingCallScreen(String number) {
  tft.fillScreen(TFT_BLACK);
  
  tft.fillRect(0, 0, 240, 30, TFT_BLUE);
  tft.setTextColor(TFT_WHITE, TFT_BLUE);
  tft.setTextSize(2);
  tft.setCursor(20, 7);
  tft.println("INCOMING CALL");
  
  drawPhoneIcon(120, 60);
  
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(10, 150);
  
  if(number.length() > 15) {
    tft.setTextSize(1);
    tft.setCursor(10, 155);
  }
  tft.println(number);
  
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.setTextSize(1);
  tft.setCursor(70, 200);
  tft.println("RINGING...");
  
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.setCursor(10, 240);
  tft.println("Serial commands:");
  tft.setCursor(10, 255);
  tft.println("'answer' or 'reject'");
  
  tft.fillCircle(220, 300, 8, TFT_GREEN);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.setCursor(155, 295);
  tft.println("CALLING");
  
  tft.drawRect(0, 0, 240, 320, TFT_BLUE);
  tft.drawRect(1, 1, 238, 318, TFT_BLUE);
}

void blinkCallLEDs() {
  for(int i = 0; i < 3; i++) {
    digitalWrite(LED_YELLOW, HIGH);
    delay(200);
    digitalWrite(LED_YELLOW, LOW);
    delay(200);
  }
  digitalWrite(LED_YELLOW, HIGH);
}

// Icon Drawing Functions
void drawFireIcon(int x, int y) {
  tft.fillTriangle(x, y+60, x-20, y+80, x+20, y+80, TFT_RED);
  tft.fillTriangle(x, y+50, x-15, y+75, x+15, y+75, TFT_ORANGE);
  tft.fillTriangle(x, y+40, x-10, y+70, x+10, y+70, TFT_YELLOW);
  tft.fillCircle(x, y+55, 8, TFT_YELLOW);
}

void drawFloodIcon(int x, int y) {
  tft.fillRect(x-15, y+50, 30, 20, TFT_BROWN);
  tft.fillTriangle(x, y+30, x-20, y+50, x+20, y+50, TFT_RED);
  
  for(int i = 0; i < 3; i++) {
    int wave_y = y+70 + i*8;
    for(int j = 0; j < 40; j+=8) {
      tft.fillCircle(x-20+j, wave_y, 4, TFT_BLUE);
    }
  }
}

void drawWeatherIcon(int x, int y) {
  tft.fillRoundRect(x-25, y+40, 50, 30, 10, TFT_DARKGREY);
  
  for(int i = 0; i < 5; i++) {
    tft.fillCircle(x-20+i*10, y+75, 3, TFT_BLUE);
  }
}

void drawEarthquakeIcon(int x, int y) {
  for(int i = 0; i < 5; i++) {
    int offset = (i % 2 == 0) ? 0 : 10;
    tft.drawLine(x-30+offset, y+50+i*8, x+30+offset, y+50+i*8, TFT_RED);
  }
  tft.fillTriangle(x-15, y+40, x, y+30, x+15, y+40, TFT_YELLOW);
}

void drawEmergencyIcon(int x, int y) {
  tft.fillCircle(x, y+55, 30, TFT_RED);
  tft.fillCircle(x, y+55, 25, TFT_YELLOW);
  tft.setTextColor(TFT_RED, TFT_YELLOW);
  tft.setTextSize(4);
  tft.setCursor(x-10, y+42);
  tft.println("!");
}

void drawWarningIcon(int x, int y) {
  tft.fillTriangle(x, y+30, x-30, y+80, x+30, y+80, TFT_YELLOW);
  tft.fillTriangle(x, y+35, x-25, y+75, x+25, y+75, TFT_ORANGE);
  tft.setTextColor(TFT_BLACK, TFT_ORANGE);
  tft.setTextSize(3);
  tft.setCursor(x-8, y+48);
  tft.println("!");
}

void drawInfoIcon(int x, int y) {
  tft.fillCircle(x, y+55, 30, TFT_BLUE);
  tft.fillCircle(x, y+55, 25, TFT_CYAN);
  tft.setTextColor(TFT_BLUE, TFT_CYAN);
  tft.setTextSize(4);
  tft.setCursor(x-10, y+42);
  tft.println("i");
}

void drawCheckmarkIcon(int x, int y) {
  tft.fillCircle(x, y+25, 30, TFT_GREEN);
  tft.fillCircle(x, y+25, 25, TFT_DARKGREEN);
  
  for(int i = 0; i < 3; i++) {
    tft.drawLine(x-15, y+25+i, x-5, y+35+i, TFT_WHITE);
    tft.drawLine(x-5, y+35+i, x+15, y+15+i, TFT_WHITE);
  }
}

void drawPhoneIcon(int x, int y) {
  tft.fillRoundRect(x-20, y+10, 15, 40, 5, TFT_GREEN);
  tft.fillRoundRect(x+5, y+10, 15, 40, 5, TFT_GREEN);
  tft.fillRect(x-10, y+25, 25, 10, TFT_GREEN);
  
  tft.drawCircle(x, y+30, 35, TFT_YELLOW);
  tft.drawCircle(x, y+30, 40, TFT_YELLOW);
  tft.drawCircle(x, y+30, 45, TFT_YELLOW);
}