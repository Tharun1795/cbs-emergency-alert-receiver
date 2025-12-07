/*
================================================================================
Project: Smart Emergency Alert Device for Cell Broadcast Service
Hardware: ESP32 + SIM7600E-H + TFT ILI9341 Display + RGB LEDs
================================================================================

*/

#include "Config.h"
#include "ModemComm.h"
#include "NetworkManager.h"
#include "CBSHandler.h"
#include "CallHandler.h"
#include "DisplayManager.h"
#include "TestSuite.h"
#include "Commands.h"

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, 16, 17);
  delay(2000);
  
  // Initialize hardware
  initializeLEDs();
  initializeDisplay();
  
  // Show welcome screen
  displayWelcome();
  
  // Initialize CBS system
  initializeCBSSystem();
  
  // Show ready state
  displayReadyState();
  
  // Print welcome message
  printWelcomeMessage();
}

void loop() {
  // Handle user commands
  if(Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    command.toLowerCase();
    handleCommand(command);
  }
  
  // Handle modem data
  if(Serial2.available()) {
    handleModuleData();
  }
  
  // Periodic system status check
  if(millis() - systemState.lastStatusCheck > STATUS_INTERVAL) {
    checkSystemStatus();
    systemState.lastStatusCheck = millis();
  }
  
  // NAS monitoring (if active)
  if(systemState.nasMonitoringActive && 
     (millis() - systemState.lastNASCheck > NAS_CHECK_INTERVAL)) {
    checkNASMessages();
    systemState.lastNASCheck = millis();
  }
}