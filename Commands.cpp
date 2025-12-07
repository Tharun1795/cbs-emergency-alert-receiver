/*================================================================================
 COMMANDS.CPP - User Command Processing & Interface
================================================================================
 This file handles all user commands entered via Serial Monitor.
 It routes commands to appropriate modules and provides system interface.
================================================================================*/

#include "Commands.h"
#include "Config.h"
#include "ModemComm.h"
#include "NetworkManager.h"
#include "CBSHandler.h"
#include "CallHandler.h"
#include "DisplayManager.h"
#include "TestSuite.h"

//=============================================================================
// MAIN COMMAND HANDLER
//=============================================================================

void handleCommand(String cmd) {
  Serial.println("\n" + cmd);
  
  // Alert simulation commands
  if(cmd == "fire" || cmd == "flood" || cmd == "weather" || 
     cmd == "earthquake" || cmd == "terrorism" || cmd == "chemical" || 
     cmd == "nuclear" || cmd == "amber" || cmd == "safety" || 
     cmd == "test" || cmd == "ready") {
    displayAlert(cmd);
    logAlertToHistory(cmd, "Simulation");
  }
  // System status commands
  else if(cmd == "status") {
    showFullStatus();
  }
  else if(cmd == "cbsstatus") {
    checkCBSStatus();
  }
  else if(cmd == "cbscheck") {
    checkCBSReadiness();
  }
  else if(cmd == "signal") {
    sendATCommandAndWait("AT+CSQ", 2000);
    Serial.println("Scale: 0-9=Poor, 10-14=OK, 15-19=Good, 20-31=Excellent\n");
  }
  else if(cmd == "network") {
    sendATCommandAndWait("AT+COPS?", 3000);
    sendATCommandAndWait("AT+CREG?", 2000);
  }
  // Tower data commands
  else if(cmd == "tower") {
    getCellTowerInfo();
  }
  else if(cmd == "towershow") {
    displayTowerInfoOnScreen();
  }
  else if(cmd == "towerlive") {
    monitorTowerLive();
  }
  else if(cmd == "neighbor") {
    Serial.println("\n================================================================");
    Serial.println("         NEIGHBOR CELLS (from eNodeB SIB4/5)");
    Serial.println("================================================================\n");
    sendATCommandAndWait("AT+QENG=\"neighbourcell\"", 3000);
    Serial.println("================================================================\n");
  }
  // SIM card commands
  else if(cmd == "siminfo") {
    getSIMInfo();
  }
  // NAS monitoring
  else if(cmd == "nas") {
    monitorNASMessages();
  }
  // Call handling commands
  else if(cmd == "answer") {
    if(currentCall.isRinging) {
      answerCall();
    } else {
      Serial.println("No incoming call");
    }
  }
  else if(cmd == "reject" || cmd == "hangup") {
    if(currentCall.isRinging) {
      rejectCall();
    } else {
      Serial.println("No active call");
    }
  }
  else if(cmd == "calltest") {
    simulateTestCall();
  }
  // Testing commands
  else if(cmd == "nosim") {
    testNoSIMMode();
  }
  else if(cmd == "testsuite" || cmd == "tests") {
    runTestSuite();
  }
  else if(cmd == "test at") {
    testATCommands();
  }
  // Utility commands
  else if(cmd == "history") {
    showAlertHistory();
  }
  else if(cmd == "reinit") {
    initializeCBSSystem();
  }
  else if(cmd == "auto") {
    autoCycleAlerts();
  }
  else if(cmd == "demo") {
    runFullDemo();
  }
  else if(cmd == "help") {
    printWelcomeMessage();
  }
  // AT command passthrough
  else if(cmd.startsWith("at")) {
    cmd.toUpperCase();
    sendATCommandAndWait(cmd, 3000);
  }
  // Latency measurement test
    else if(cmd == "testcbs") {
        Serial.println("\nTESTING CBS LATENCY MEASUREMENT\n");
        
        String testMsg = "+CBM: 50,4370,0,1,88,1,\"TEST FIRE ALERT - LATENCY MEASUREMENT\"";
        
        Serial.println("Simulating CBS message...");
        Serial.println("Raw: " + testMsg);
        
        parseCBSMessage(testMsg);
    }
    
    // Show latency statistics
    else if(cmd == "latstats") {
        printLatencyStats();
    }
    
    // Export latency data as CSV
    else if(cmd == "latcsv") {
        Serial.println("\nLATENCY DATA (CSV FORMAT)\n");
        Serial.println("Sample,Latency_ms,Latency_s");
        for(int i = 0; i < latencyStats.count; i++) {
            Serial.print(i + 1);
            Serial.print(",");
            Serial.print(latencyStats.samples[i]);
            Serial.print(",");
            Serial.println(latencyStats.samples[i] / 1000.0, 3);
        }
        Serial.println("\nCopy above data to Excel for graphing\n");
    }
    
    // Reset latency statistics
    else if(cmd == "latreset") {
        latencyStats.count = 0;
        latencyStats.minLatency = 999999;
        latencyStats.maxLatency = 0;
        latencyStats.avgLatency = 0;
        Serial.println("Latency statistics reset");
    }
    
    // Run multiple latency tests
    else if(cmd == "lattest10") {
        Serial.println("\nRunning 10 latency tests...\n");
        for(int i = 0; i < 10; i++) {
            Serial.print("Test ");
            Serial.print(i + 1);
            Serial.println("/10");
            
            String testMsg = "+CBM: 50,4370,0,1,88,1,\"TEST ALERT " + String(i+1) + "\"";
            parseCBSMessage(testMsg);
            
            delay(500);
        }
        Serial.println("\n10 tests complete!");
        Serial.println("Run 'latstats' to see results\n");
    }
  // Unknown command
  else {
    Serial.println("Unknown command. Type 'help'");
  }
  
  Serial.println("\n>");
}

//=============================================================================
// SYSTEM STATUS COMMANDS
//=============================================================================

void showFullStatus() {
  Serial.println("\n================================================================");
  Serial.println("         COMPLETE SYSTEM STATUS");
  Serial.println("================================================================\n");
  
  Serial.println("1. MODULE:");
  sendATCommandAndWait("ATI", 2000);
  
  Serial.println("\n2. SIM:");
  sendATCommandAndWait("AT+CPIN?", 3000);
  
  Serial.println("\n3. REGISTRATION:");
  sendATCommandAndWait("AT+CREG?", 2000);
  
  Serial.println("\n4. SIGNAL:");
  sendATCommandAndWait("AT+CSQ", 2000);
  
  Serial.println("\n5. OPERATOR:");
  sendATCommandAndWait("AT+COPS?", 3000);
  
  Serial.println("\n6. CBS:");
  sendATCommandAndWait("AT+CSCB?", 2000);
  
  Serial.println("================================================================\n");
}

void checkCBSStatus() {
  Serial.println("\n================================================================");
  Serial.println("         CBS STATUS");
  Serial.println("================================================================\n");
  
  sendATCommandAndWait("AT+CSCB?", 2000);
  
  Serial.println("\nSystem:");
  Serial.println("  SIM:     " + String(systemState.simReady ? "READY [OK]" : "NOT READY [X]"));
  Serial.println("  Network: " + String(systemState.networkRegistered ? "REGISTERED [OK]" : "NOT REGISTERED [X]"));
  Serial.println("  CBS:     " + String(systemState.cbsEnabled ? "ENABLED [OK]" : "UNKNOWN"));
  Serial.println("================================================================\n");
}

void testATCommands() {
  Serial.println("\n================================================================");
  Serial.println("         AT COMMAND TEST");
  Serial.println("================================================================\n");
  
  Serial.println("Test 1: Basic AT");
  sendATCommandAndWait("AT", 1000);
  
  Serial.println("\nTest 2: Module Info");
  sendATCommandAndWait("ATI", 2000);
  
  Serial.println("\nTest 3: Signal");
  sendATCommandAndWait("AT+CSQ", 2000);
  
  Serial.println("\nTest 4: SIM");
  sendATCommandAndWait("AT+CPIN?", 3000);
  
  Serial.println("\nTest 5: Registration");
  sendATCommandAndWait("AT+CREG?", 2000);
  
  Serial.println("================================================================\n");
}

//=============================================================================
// DEMO AND UTILITY COMMANDS
//=============================================================================

void autoCycleAlerts() {
  Serial.println("\nAuto-cycling alerts (3s each)...\n");
  
  String alerts[] = {"ready", "fire", "flood", "weather", "safety", 
                     "test", "earthquake", "terrorism", "chemical", 
                     "amber", "nuclear"};
  
  for(int i = 0; i < 11; i++) {
    Serial.println("[" + String(i+1) + "/11] " + alerts[i]);
    displayAlert(alerts[i]);
    delay(3000);
  }
  
  Serial.println("\nComplete!\n");
}

void runFullDemo() {
  Serial.println("\n================================================================");
  Serial.println("         FULL DEMONSTRATION");
  Serial.println("================================================================\n");
  
  Serial.println("Phase 1: System Check");
  sendATCommandAndWait("AT", 1000);
  sendATCommandAndWait("AT+CSQ", 2000);
  
  Serial.println("\nPhase 2: Tower Info");
  getCellTowerInfo();
  displayTowerInfoOnScreen();
  delay(5000);
  
  Serial.println("\nPhase 3: Alerts");
  String demos[] = {"ready", "test", "weather", "flood", "fire", "earthquake"};
  for(int i = 0; i < 6; i++) {
    Serial.println("Showing: " + demos[i]);
    displayAlert(demos[i]);
    delay(3000);
  }
  
  Serial.println("================================================================\n");
}

void printWelcomeMessage() {
  Serial.println("\n================================================================");
  Serial.println("      CBS EMERGENCY ALERT SYSTEM");
  Serial.println("================================================================");
  Serial.println("\nCOMMANDS:");
  
  Serial.println("\nSYSTEM STATUS:");
  Serial.println("  status     - Complete system status");
  Serial.println("  cbsstatus  - CBS configuration");
  Serial.println("  cbscheck   - CBS readiness check");
  Serial.println("  signal     - Signal quality");
  Serial.println("  network    - Network info");
  Serial.println("  test at    - AT command test");
  Serial.println("  reinit     - Reinitialize system");
  
  Serial.println("\nCELL TOWER DATA:");
  Serial.println("  tower      - Get tower information");
  Serial.println("  towershow  - Display tower on screen");
  Serial.println("  towerlive  - Live monitoring (10s)");
  Serial.println("  neighbor   - Neighbor cells");
  
  Serial.println("\nNAS MESSAGE MONITORING:");
  Serial.println("  nas        - Monitor NAS messages");
  Serial.println("               (Attach, TAU, etc.)");
  
  Serial.println("\nCALL DETECTION:");
  Serial.println("  answer     - Answer incoming call");
  Serial.println("  reject     - Reject incoming call");
  Serial.println("  calltest   - Simulate call (testing)");
  
  Serial.println("\nSIM CARD:");
  Serial.println("  siminfo    - SIM card details");
  
  Serial.println("\nTESTING:");
  Serial.println("  nosim      - Test no-SIM measurements");
  Serial.println("  testsuite  - Run comprehensive test suite");
  Serial.println("               (Tests failure cases)");
  
  Serial.println("\nALERT SIMULATION:");
  Serial.println("  fire, flood, weather, earthquake,");
  Serial.println("  terrorism, chemical, nuclear,");
  Serial.println("  amber, safety, test, ready");
  
  Serial.println("\nDEMONSTRATION:");
  Serial.println("  demo       - Full system demo");
  Serial.println("  auto       - Auto-cycle alerts");
  Serial.println("  history    - Alert log");
  
  Serial.println("\nAT COMMANDS:");
  Serial.println("  AT+...     - Any AT command");
  
  Serial.println("\n================================================================");
  Serial.println("CURRENT STATUS:");
  Serial.println("  SIM:     " + String(systemState.simReady ? "READY [OK]" : "NOT READY [X]"));
  Serial.println("  Network: " + String(systemState.networkRegistered ? "REGISTERED [OK]" : "NOT REGISTERED [X]"));
  Serial.println("  CBS:     " + String(systemState.cbsEnabled ? "ENABLED [OK]" : "UNKNOWN"));
  Serial.println("  CallID:  ENABLED [OK]");
  
  Serial.println("\nLATENCY MEASUREMENT:");
  Serial.println("  testcbs       - Test CBS with latency measurement");
  Serial.println("  latstats      - Show latency statistics");
  Serial.println("  latcsv        - Export latency data (CSV)");
  Serial.println("  latreset      - Reset latency statistics");
  Serial.println("  lattest10     - Run 10 automatic tests");
    
  if(systemState.simReady && systemState.networkRegistered) {
    Serial.println("\n[OK] SYSTEM READY");
    Serial.println("  Type 'demo' for full demonstration");
    Serial.println("  Type 'tower' for real cell tower data");
    Serial.println("  Type 'nas' for NAS message monitoring");
    Serial.println("  Type 'calltest' to test call detection");
    Serial.println("  Type 'testsuite' for validation tests");
  } else {
    Serial.println("\n! Type 'status' to check system");
  }
  
  Serial.println("================================================================\n>");
}

//=============================================================================
// END OF COMMANDS.CPP
//=============================================================================
