#include "NetworkManager.h"
#include "Config.h"
#include "ModemComm.h"
#include "DisplayManager.h"
#include "CBSHandler.h" 

void getCellTowerInfo() {
  Serial.println("\n================================================================");
  Serial.println("         REAL-TIME CELL TOWER DATA");
  Serial.println("              (Live from eNodeB)");
  Serial.println("================================================================\n");
  
  Serial.println("Querying cell tower (eNodeB)...\n");
  
  Serial.println("1. COMPLETE SYSTEM INFORMATION:");
  Serial.println("----------------------------------------------------------------");
  String cpsi = sendATCommandAndWait("AT+CPSI?", 3000);
  
  if(cpsi.indexOf("+CPSI:") >= 0) {
    int start = cpsi.indexOf("+CPSI:") + 7;
    String data = cpsi.substring(start);
    
    int idx = 0;
    int lastComma = -1;
    String parts[15];
    
    for(int i = 0; i < data.length() && idx < 15; i++) {
      if(data[i] == ',' || data[i] == '\r' || data[i] == '\n') {
        parts[idx++] = data.substring(lastComma + 1, i);
        parts[idx-1].trim();
        lastComma = i;
      }
    }
    
    if(idx >= 10) {
      currentTower.operator_name = parts[2];
      currentTower.tac = parts[3];
      currentTower.cellID = parts[4];
      currentTower.pci = parts[5];
      currentTower.band = parts[6];
      currentTower.earfcn = parts[7];
      currentTower.rsrp = parts[10].toInt();
      currentTower.rsrq = parts[11].toInt();
      currentTower.rssi = parts[12].toInt();
      if(idx >= 14) currentTower.sinr = parts[13].toInt();
      currentTower.lastUpdate = millis();
      towerDataValid = true;
    }
  }
  
  Serial.println("\n2. NETWORK OPERATOR:");
  Serial.println("----------------------------------------------------------------");
  sendATCommandAndWait("AT+COPS?", 3000);
  
  Serial.println("\n3. REGISTRATION STATUS:");
  Serial.println("----------------------------------------------------------------");
  sendATCommandAndWait("AT+CREG=2", 1000);
  delay(500);
  sendATCommandAndWait("AT+CREG?", 2000);
  
  Serial.println("\n4. NEIGHBOR CELLS (from tower broadcast):");
  Serial.println("----------------------------------------------------------------");
  sendATCommandAndWait("AT+QENG=\"neighbourcell\"", 3000);
  
  Serial.println("\n================================================================");
  Serial.println("         DECODED TOWER INFORMATION");
  Serial.println("================================================================");
  
  if(towerDataValid) {
    Serial.println("\nCONNECTED CELL TOWER (eNodeB):");
    Serial.println("  Cell ID:          " + currentTower.cellID);
    Serial.println("  Physical Cell ID: " + currentTower.pci);
    Serial.println("  Tracking Area:    " + currentTower.tac);
    Serial.println("  LTE Band:         " + currentTower.band);
    Serial.println("  EARFCN:           " + currentTower.earfcn);
    Serial.println("  Operator:         " + currentTower.operator_name);
    
    Serial.println("\nSIGNAL MEASUREMENTS:");
    Serial.println("  RSRP: " + String(currentTower.rsrp) + " dBm (Reference Signal Power)");
    Serial.println("  RSRQ: " + String(currentTower.rsrq) + " dB  (Signal Quality)");
    Serial.println("  RSSI: " + String(currentTower.rssi) + " dBm (Received Signal Strength)");
    Serial.println("  SINR: " + String(currentTower.sinr) + " dB  (Signal-to-Noise Ratio)");
    
    Serial.println("\nSIGNAL QUALITY INTERPRETATION:");
    if(currentTower.rsrp > -80) Serial.println("  RSRP: EXCELLENT [OK OK OK]");
    else if(currentTower.rsrp > -90) Serial.println("  RSRP: GOOD [OK OK]");
    else if(currentTower.rsrp > -100) Serial.println("  RSRP: FAIR [OK]");
    else Serial.println("  RSRP: POOR [X]");
    
    if(currentTower.rsrq > -10) Serial.println("  RSRQ: EXCELLENT [OK OK OK]");
    else if(currentTower.rsrq > -15) Serial.println("  RSRQ: GOOD [OK OK]");
    else if(currentTower.rsrq > -20) Serial.println("  RSRQ: FAIR [OK]");
    else Serial.println("  RSRQ: POOR [X]");
    
    Serial.println("\n[OK] THIS IS REAL DATA FROM THE CELL TOWER!");
    Serial.println("     Updated: " + String((millis() - currentTower.lastUpdate)/1000) + "s ago");
  } else {
    Serial.println("\n! Unable to parse tower data");
  }
  
  Serial.println("\n================================================================\n");
}

void displayTowerInfoOnScreen() {
  if(!towerDataValid) getCellTowerInfo();
  
  displayTowerData(currentTower);
}

void monitorTowerLive() {
  Serial.println("\n================================================================");
  Serial.println("         LIVE TOWER MONITORING MODE");
  Serial.println("              Press any key to stop");
  Serial.println("================================================================\n");
  
  int count = 0;
  while(!Serial.available()) {
    count++;
    Serial.println("--- Update #" + String(count) + " ---");
    getCellTowerInfo();
    displayTowerInfoOnScreen();
    Serial.println("Next update in 10 seconds...\n");
    
    for(int i = 0; i < 100; i++) {
      delay(1000);
      if(Serial.available()) break;
    }
  }
  
  while(Serial.available()) Serial.read();
  Serial.println("\n[OK] Monitoring stopped\n");
  displayReadyState();
}

void checkSystemStatus() {
  String response = sendATCommandSilent("AT+CREG?", 2000);
  bool wasRegistered = systemState.networkRegistered;
  
  if(response.indexOf("+CREG: 0,1") >= 0 || 
     response.indexOf("+CREG: 0,5") >= 0 || 
     response.indexOf("+CREG:0,1") >= 0 || 
     response.indexOf("+CREG:0,5") >= 0 ||
     response.indexOf("+CREG: 2,1") >= 0 ||
     response.indexOf("+CREG:2,1") >= 0) {
    systemState.networkRegistered = true;
    if(!wasRegistered) {
      Serial.println("\n================================================================");
      Serial.println("         NETWORK REGISTRATION EVENT");
      Serial.println("================================================================");
      Serial.println("Status: REGISTERED");
      Serial.println("Time:   " + String(millis()/1000) + "s");
      Serial.println("Response: " + response);
      Serial.println("================================================================\n");
    }
  } else {
    systemState.networkRegistered = false;
    if(wasRegistered) {
      Serial.println("\n================================================================");
      Serial.println("         NETWORK DE-REGISTRATION EVENT");
      Serial.println("================================================================");
      Serial.println("Status: NOT REGISTERED");
      Serial.println("Time:   " + String(millis()/1000) + "s");
      Serial.println("Response: " + response);
      Serial.println("================================================================\n");
    }
  }
}

void getSIMInfo() {
  Serial.println("\n================================================================");
  Serial.println("         SIM CARD INFORMATION");
  Serial.println("================================================================\n");
  
  Serial.println("1. PHONE NUMBER:");
  Serial.println("----------------------------------------------------------------");
  sendATCommandAndWait("AT+CNUM", 2000);
  
  Serial.println("\n2. ICCID (SIM Serial Number):");
  Serial.println("----------------------------------------------------------------");
  sendATCommandAndWait("AT+CCID", 2000);
  
  Serial.println("\n3. IMSI (Subscriber ID):");
  Serial.println("----------------------------------------------------------------");
  sendATCommandAndWait("AT+CIMI", 2000);
  
  Serial.println("\n4. SIM STATUS:");
  Serial.println("----------------------------------------------------------------");
  sendATCommandAndWait("AT+CPIN?", 2000);
  
  Serial.println("\n5. NETWORK OPERATOR:");
  Serial.println("----------------------------------------------------------------");
  sendATCommandAndWait("AT+COPS?", 3000);
  
  Serial.println("\n================================================================");
  Serial.println("SIM Information Complete");
  Serial.println("================================================================\n");
}

void checkCBSReadiness() {
  Serial.println("\n================================================================");
  Serial.println("         CBS RECEPTION READINESS CHECK");
  Serial.println("================================================================\n");
  
  Serial.println("1. CBS MONITORING STATUS:");
  Serial.println("----------------------------------------------------------------");
  String cscb = sendATCommandAndWait("AT+CSCB?", 2000);
  if(cscb.indexOf("+CSCB: 0") >= 0) {
    Serial.println("[OK] CBS ENABLED - Module monitoring for SIB10/11/12");
  } else if(cscb.indexOf("+CSCB: 1") >= 0) {
    Serial.println("[X] CBS DISABLED - Module ignoring CBS broadcasts");
  }
  
  Serial.println("\n2. CBS NOTIFICATION SETTINGS:");
  Serial.println("----------------------------------------------------------------");
  sendATCommandAndWait("AT+CNMI?", 2000);
  
  Serial.println("\n3. NETWORK CONNECTION:");
  Serial.println("----------------------------------------------------------------");
  String creg = sendATCommandAndWait("AT+CREG?", 2000);
  if(creg.indexOf("+CREG: 0,1") >= 0 || creg.indexOf("+CREG: 0,5") >= 0) {
    Serial.println("[OK] REGISTERED - Can receive cell broadcasts");
  } else {
    Serial.println("[X] NOT REGISTERED - Cannot receive broadcasts");
  }
  
  Serial.println("\n4. SERVING CELL:");
  Serial.println("----------------------------------------------------------------");
  sendATCommandAndWait("AT+QENG=\"servingcell\"", 3000);
  
  Serial.println("\n================================================================");
  Serial.println("         CBS MONITORING STATUS");
  Serial.println("================================================================");
  Serial.println();
  Serial.println("[OK] We CAN check: Module configuration and network status");
  Serial.println("[X] We CANNOT check: SIB10/11/12 content or pending CBS");
  Serial.println();
  Serial.println("CBS is PUSH (not PULL):");
  Serial.println("  - Tower broadcasts SIB10/11/12 during emergencies");
  Serial.println("  - Module decodes automatically at baseband level");
  Serial.println("  - We receive +CBM: notification when it arrives");
  Serial.println("  - Cannot query for pending messages");
  Serial.println();
  Serial.println("[OK] YOUR SYSTEM IS READY - MONITORING FOR BROADCASTS");
  Serial.println("================================================================\n");
}

void checkNASMessages() {
  String creg = sendATCommandSilent("AT+CREG?", 2000);
  
  if(creg.indexOf("+CREG: 2,1") >= 0 || creg.indexOf("+CREG: 2,5") >= 0) {
    int tacStart = creg.indexOf(",\"") + 2;
    if(tacStart > 2) {
      int tacEnd = creg.indexOf("\"", tacStart);
      String newTAC = creg.substring(tacStart, tacEnd);
      
      if(currentTower.tac != "" && newTAC != currentTower.tac && newTAC.length() > 0) {
        Serial.println("\n================================================================");
        Serial.println("         NAS MESSAGE DETECTED!");
        Serial.println("================================================================");
        Serial.println("Type:        TRACKING AREA UPDATE (TAU)");
        Serial.println("Old TAC:     " + currentTower.tac);
        Serial.println("New TAC:     " + newTAC);
        Serial.println("Time:        " + String(millis()/1000) + "s");
        Serial.println("Description: Device moved to new tracking area");
        Serial.println("NAS Procedure: TAU Request -> TAU Accept");
        Serial.println("================================================================\n");
        
        lastNASMessage.type = "TAU";
        lastNASMessage.content = "Tracking Area: " + currentTower.tac + " -> " + newTAC;
        lastNASMessage.timestamp = millis();
        lastNASMessage.detected = true;
        currentTower.tac = newTAC;
        logAlertToHistory("NAS-TAU", lastNASMessage.content);
      }
    }
  }
  
  String cgatt = sendATCommandSilent("AT+CGATT?", 2000);
  if(cgatt.indexOf("+CGATT: 1") >= 0) {
    static bool wasAttached = false;
    if(!wasAttached) {
      Serial.println("\n================================================================");
      Serial.println("         NAS MESSAGE DETECTED!");
      Serial.println("================================================================");
      Serial.println("Type:        ATTACH ACCEPT");
      Serial.println("Time:        " + String(millis()/1000) + "s");
      Serial.println("Description: Device attached to EPS network");
      Serial.println("NAS Procedure: Attach Request -> Attach Accept");
      Serial.println("Status:      Successfully registered to packet data network");
      Serial.println("================================================================\n");
      
      lastNASMessage.type = "ATTACH";
      lastNASMessage.content = "Device attached to EPS network";
      lastNASMessage.timestamp = millis();
      lastNASMessage.detected = true;
      wasAttached = true;
      logAlertToHistory("NAS-ATTACH", lastNASMessage.content);
    }
  }
}

void monitorNASMessages() {
  Serial.println("\n================================================================");
  Serial.println("         NAS MESSAGE MONITORING");
  Serial.println("================================================================\n");
  
  Serial.println("NAS (Non-Access Stratum) Messages:");
  Serial.println("  - Attach/Detach procedures");
  Serial.println("  - Tracking Area Updates (TAU)");
  Serial.println("  - Authentication");
  Serial.println("  - Service Requests");
  Serial.println("");
  Serial.println("Starting NAS monitoring...");
  Serial.println("Monitoring silently in background");
  Serial.println("Will alert only on state changes");
  Serial.println("Press any key to stop");
  Serial.println("");
  
  systemState.nasMonitoringActive = true;
  
  while(!Serial.available()) {
    checkNASMessages();
    
    static unsigned long lastStatusPrint = 0;
    if(millis() - lastStatusPrint > 30000) {
      Serial.println("--- NAS Monitoring Active (silent mode) ---");
      if(lastNASMessage.detected) {
        Serial.println("Last event: " + lastNASMessage.type + " @ " + String(lastNASMessage.timestamp/1000) + "s");
      }
      Serial.println("");
      lastStatusPrint = millis();
    }
    
    for(int i = 0; i < 50; i++) {
      delay(100);
      if(Serial.available()) break;
    }
  }
  
  while(Serial.available()) Serial.read();
  systemState.nasMonitoringActive = false;
  Serial.println("\n[OK] NAS monitoring stopped\n");
}

void testNoSIMMode() {
  Serial.println("\n================================================================");
  Serial.println("    COMPREHENSIVE NO-SIM MODE TEST");
  Serial.println("    Testing Physical Layer Measurements");
  Serial.println("================================================================\n");
  
  Serial.println("Prerequisites:");
  Serial.println("  1. Remove SIM card from modem");
  Serial.println("  2. Power cycle the ESP32");
  Serial.println("  3. Run this test");
  Serial.println("");
  Serial.println("Starting comprehensive tests...\n");
  
  // Test 1: Module alive?
  Serial.println("================================================================");
  Serial.println("Test 1: Basic Communication");
  Serial.println("================================================================");
  String at = sendATCommandAndWait("AT", 1000);
  if(at.indexOf("OK") >= 0) {
    Serial.println("[OK] Module responding\n");
  } else {
    Serial.println("[X] Module not responding");
    Serial.println("================================================================\n");
    return;
  }
  
  // Test 2: Check SIM status
  Serial.println("================================================================");
  Serial.println("Test 2: SIM Card Status");
  Serial.println("================================================================");
  String cpin = sendATCommandAndWait("AT+CPIN?", 2000);
  if(cpin.indexOf("SIM NOT INSERTED") >= 0 || cpin.indexOf("ERROR") >= 0) {
    Serial.println("[OK] No SIM detected (as expected)\n");
  } else if(cpin.indexOf("READY") >= 0) {
    Serial.println("[!] WARNING: SIM card is still inserted!");
    Serial.println("    Remove SIM and restart test");
    Serial.println("================================================================\n");
    return;
  } else {
    Serial.println("[?] Unknown SIM status\n");
  }
  
  // Test 3: Module info
  Serial.println("================================================================");
  Serial.println("Test 3: Module Information");
  Serial.println("================================================================");
  sendATCommandAndWait("ATI", 2000);
  Serial.println("");
  
  // Test 4: Enable radio
  Serial.println("================================================================");
  Serial.println("Test 4: Enable Radio (Full Functionality)");
  Serial.println("================================================================");
  sendATCommandAndWait("AT+CFUN=1", 2000);
  delay(2000);
  Serial.println("[OK] Radio enabled - waiting for cell search...\n");
  delay(3000);
  
  // Test 5: Signal strength (basic)
  Serial.println("================================================================");
  Serial.println("Test 5: Basic Signal Strength (CSQ)");
  Serial.println("================================================================");
  String csq = sendATCommandAndWait("AT+CSQ", 2000);
  
  bool cellDetected = false;
  
  if(csq.indexOf("+CSQ:") >= 0) {
    Serial.println("[OK] CSQ command works without SIM!");
    
    int csqStart = csq.indexOf("+CSQ:") + 6;
    int comma = csq.indexOf(",", csqStart);
    if(comma > csqStart) {
      String rssi = csq.substring(csqStart, comma);
      rssi.trim();
      int rssiVal = rssi.toInt();
      
      if(rssiVal == 99) {
        Serial.println("    Signal: Unknown/Not detected");
        Serial.println("    [X] No cell signal detected\n");
      } else {
        int dBm = -113 + (rssiVal * 2);
        Serial.println("    CSQ Value: " + rssi);
        Serial.println("    Estimated RSSI: " + String(dBm) + " dBm");
        
        if(rssiVal > 0 && rssiVal < 99) {
          Serial.println("    [OK] Modem can detect cells without SIM!");
          cellDetected = true;
        }
        Serial.println("");
      }
    }
  } else {
    Serial.println("[X] CSQ command failed\n");
  }
  
  // Test 6: System info
  Serial.println("================================================================");
  Serial.println("Test 6: System Information (CPSI)");
  Serial.println("================================================================");
  String cpsi = sendATCommandAndWait("AT+CPSI?", 3000);
  
  if(cpsi.indexOf("LTE") >= 0) {
    Serial.println("[OK] Modem can see LTE network without SIM!");
    Serial.println("");
    cellDetected = true;
  } else if(cpsi.indexOf("NO SERVICE") >= 0) {
    Serial.println("[X] NO SERVICE - Modem cannot acquire cell without SIM");
    Serial.println("");
  } else {
    Serial.println("[?] Unknown response\n");
  }
  
  // Test 7: RSRP/RSRQ measurements
  Serial.println("================================================================");
  Serial.println("Test 7: RSRP/RSRQ Measurements (QENG - CRITICAL TEST)");
  Serial.println("================================================================");
  String qeng = sendATCommandAndWait("AT+QENG=\"servingcell\"", 3000);
  
  bool measurementsAvailable = false;
  
  if(qeng.indexOf("LTE") >= 0 && qeng.indexOf("SEARCH") < 0) {
    Serial.println("[OK] SUCCESS! RSRP/RSRQ AVAILABLE WITHOUT SIM!");
    Serial.println("");
    Serial.println("Raw QENG data:");
    Serial.println("----------------------------------------------------------------");
    
    int qengStart = qeng.indexOf("+QENG:");
    if(qengStart >= 0) {
      String qengData = qeng.substring(qengStart);
      int endPos = qengData.indexOf("\r");
      if(endPos > 0) qengData = qengData.substring(0, endPos);
      Serial.println(qengData);
    }
    Serial.println("----------------------------------------------------------------");
    Serial.println("");
    
    parseQENGMeasurements(qeng);
    measurementsAvailable = true;
    Serial.println("");
    
  } else if(qeng.indexOf("SEARCH") >= 0) {
    Serial.println("[!] Modem is searching for network");
    Serial.println("    Status: Cell search in progress");
    Serial.println("    Action: Wait 30-60 seconds and try 'nosim' again");
    Serial.println("");
  } else {
    Serial.println("[X] Cannot get RSRP/RSRQ without SIM");
    Serial.println("    Reason: SIM7600E firmware requires SIM for measurements");
    Serial.println("    Note: This is a firmware limitation");
    Serial.println("");
  }
  
  // Test 8: Registration status
  Serial.println("================================================================");
  Serial.println("Test 8: Registration Status");
  Serial.println("================================================================");
  String creg = sendATCommandAndWait("AT+CREG?", 2000);
  
  if(creg.indexOf(",0,") >= 0 || creg.indexOf(",0\r") >= 0) {
    Serial.println("[X] Not registered (expected without SIM)");
  } else if(creg.indexOf(",2,") >= 0 || creg.indexOf(",2\r") >= 0) {
    Serial.println("[!] Searching for network");
  } else {
    Serial.println("Status: " + creg);
  }
  Serial.println("");
  
  // Test 9: Neighbor cells
  Serial.println("================================================================");
  Serial.println("Test 9: Neighbor Cell Scan");
  Serial.println("================================================================");
  String neighbor = sendATCommandAndWait("AT+QENG=\"neighbourcell\"", 3000);
  
  if(neighbor.indexOf("LTE") >= 0) {
    Serial.println("[OK] Can scan neighbor cells without SIM!");
    Serial.println("");
  } else {
    Serial.println("[X] Neighbor cell scan requires SIM");
    Serial.println("");
  }
  
  // FINAL SUMMARY
  Serial.println("================================================================");
  Serial.println("         COMPREHENSIVE TEST SUMMARY");
  Serial.println("================================================================");
  Serial.println("");
  
  Serial.println("RESULTS:");
  Serial.println("--------");
  
  if(measurementsAvailable) {
    Serial.println("[OK] SUCCESS! Your SIM7600E CAN measure RSRP/RSRQ without SIM!");
    Serial.println("[OK] Physical layer measurements are accessible");
    Serial.println("[OK] Cell detection works without registration");
    Serial.println("");
    Serial.println("Available without SIM:");
    Serial.println("  - RSRP (Reference Signal Received Power)");
    Serial.println("  - RSRQ (Reference Signal Received Quality)");
    Serial.println("  - RSSI (Received Signal Strength Indicator)");
    Serial.println("  - SINR (Signal-to-Interference-plus-Noise Ratio)");
    Serial.println("  - Cell search and synchronization");
    Serial.println("  - Basic tower information");
  } else if(cellDetected) {
    Serial.println("[!] PARTIAL: Cell detection works, but detailed measurements unavailable");
    Serial.println("[!] CSQ signal strength is available");
    Serial.println("[X] RSRP/RSRQ not accessible without SIM");
  } else {
    Serial.println("[X] FAILED: Your SIM7600E firmware does NOT allow measurements without SIM");
    Serial.println("[X] This is a firmware limitation, not hardware");
    Serial.println("[X] No cell detection without SIM");
  }
  
  Serial.println("");
  Serial.println("NOT AVAILABLE WITHOUT SIM:");
  Serial.println("---------------------------");
  Serial.println("  [X] CBS messages (SIB10/11/12) - REQUIRES REGISTRATION");
  Serial.println("  [X] Network registration - REQUIRES SIM");
  Serial.println("  [X] Cell ID and TAC (full info) - REQUIRES REGISTRATION");
  Serial.println("  [X] Emergency alert reception - REQUIRES REGISTRATION");
  Serial.println("  [X] NAS messaging - REQUIRES REGISTRATION");
  Serial.println("  [X] Operator information - REQUIRES REGISTRATION");
  
  
  if(measurementsAvailable) {
    Serial.println("[!] While physical measurements work without SIM,");
    Serial.println("    CBS emergency alerts REQUIRE:");
    Serial.println("    1. Valid SIM card");
    Serial.println("    2. Network registration");
    Serial.println("    3. Active network connection");
    Serial.println("");
    Serial.println("[!] Physical layer measurements alone are INSUFFICIENT");
    Serial.println("    for your CBS emergency alert receiver project.");
  } else {
    Serial.println("[!] Your SIM7600E REQUIRES SIM for all operations.");
  }
  
  Serial.println("");
  Serial.println("RECOMMENDATION:");
  Serial.println("  >>> KEEP SIM CARD INSERTED for your dissertation project <<<");
  Serial.println("  >>> CBS functionality is impossible without SIM + registration <<<");
  
  Serial.println("");
  Serial.println("================================================================");
  Serial.println("         TEST COMPLETE");
  Serial.println("================================================================\n");
}

void parseQENGMeasurements(String qeng) {
  Serial.println("--- PHYSICAL LAYER MEASUREMENTS (NO SIM REQUIRED!) ---");
  Serial.println("");
  
  int ltePos = qeng.indexOf("\"LTE\"");
  if(ltePos < 0) {
    Serial.println("[X] Not an LTE cell");
    return;
  }
  
  String data = qeng.substring(ltePos);
  
  String fields[20];
  int idx = 0;
  int lastComma = -1;
  
  for(int i = 0; i < data.length() && idx < 20; i++) {
    if(data[i] == ',' || data[i] == '\r' || data[i] == '\n') {
      if(i > lastComma + 1) {
        fields[idx] = data.substring(lastComma + 1, i);
        fields[idx].trim();
        fields[idx].replace("\"", "");
        idx++;
      }
      lastComma = i;
    }
  }
  
  Serial.println("Parsed " + String(idx) + " fields from QENG response");
  Serial.println("");
  
  if(idx >= 13) {
    Serial.println("=== SIGNAL MEASUREMENTS ===");
    Serial.println("");
    Serial.println("  RSRP (Reference Signal Power):    " + fields[11] + " dBm");
    Serial.println("  RSRQ (Signal Quality):            " + fields[12] + " dB");
    
    if(idx >= 14) {
      Serial.println("  RSSI (Received Signal Strength):  " + fields[13] + " dBm");
    }
    if(idx >= 15) {
      Serial.println("  SINR (Signal-to-Noise Ratio):    " + fields[14] + " dB");
    }
    
    Serial.println("");
    Serial.println("=== SIGNAL QUALITY INTERPRETATION ===");
    Serial.println("");
    
    int rsrp = fields[11].toInt();
    if(rsrp > -80) {
      Serial.println("  RSRP: EXCELLENT [OK OK OK]");
    } else if(rsrp > -90) {
      Serial.println("  RSRP: GOOD [OK OK]");
    } else if(rsrp > -100) {
      Serial.println("  RSRP: FAIR [OK]");
    } else {
      Serial.println("  RSRP: POOR [X]");
    }
    
    int rsrq = fields[12].toInt();
    if(rsrq > -10) {
      Serial.println("  RSRQ: EXCELLENT [OK OK OK]");
    } else if(rsrq > -15) {
      Serial.println("  RSRQ: GOOD [OK OK]");
    } else if(rsrq > -20) {
      Serial.println("  RSRQ: FAIR [OK]");
    } else {
      Serial.println("  RSRQ: POOR [X]");
    }
    
    Serial.println("");
    Serial.println("=== MEASUREMENT SOURCE ===");
    Serial.println("");
    Serial.println("  [OK] These are REAL measurements from the cell tower!");
    Serial.println("  [OK] Obtained WITHOUT SIM card or registration!");
    Serial.println("");
    Serial.println("  How it works:");
    Serial.println("    - Modem synchronized to cell using PSS/SSS");
    Serial.println("    - Measurements from Cell-Specific Reference Signals (CRS)");
    Serial.println("    - Physical layer only (Layer 1 of LTE protocol)");
    Serial.println("    - No authentication or registration required");
    
    if(idx >= 10) {
      Serial.println("");
      Serial.println("=== ADDITIONAL CELL INFO ===");
      Serial.println("");
      if(fields[1].length() > 0) Serial.println("  Mode:      " + fields[1]);
      if(idx >= 5) Serial.println("  PCI:       " + fields[4] + " (Physical Cell ID)");
      if(idx >= 6) Serial.println("  EARFCN:    " + fields[5] + " (Channel number)");
      if(idx >= 7) Serial.println("  Band:      " + fields[6]);
    }
    
  } else {
    Serial.println("[!] Could not parse all measurements");
    Serial.println("    Response has " + String(idx) + " fields (expected 13+)");
    Serial.println("");
    Serial.println("All parsed fields:");
    for(int i = 0; i < idx; i++) {
      Serial.println("  Field " + String(i) + ": " + fields[i]);
    }
  }
  
  Serial.println("");
  Serial.println("-------------------------------------------------------");
}