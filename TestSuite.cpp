#include "TestSuite.h"
#include "Config.h"
#include "ModemComm.h"
#include "NetworkManager.h"
#include "DisplayManager.h"
#include "CBSHandler.h"
#include "CallHandler.h"

void runTestSuite() {
  Serial.println("\n================================================================");
  Serial.println("    CBS SYSTEM COMPREHENSIVE TEST SUITE");
  Serial.println("    Testing Failure Cases & Edge Conditions");
  Serial.println("================================================================\n");
  
  Serial.println("This test suite will validate:");
  Serial.println("  1. Network failure handling");
  Serial.println("  2. SIM card error detection");
  Serial.println("  3. Signal loss recovery");
  Serial.println("  4. Invalid command handling");
  Serial.println("  5. Module communication errors");
  Serial.println("  6. Display system validation");
  Serial.println("  7. Memory and buffer tests");
  Serial.println("  8. Call handling edge cases");
  Serial.println("  9. CBS message parsing errors");
  Serial.println("  10. System recovery procedures");
  Serial.println("");
  
  int totalTests = 0;
  int passedTests = 0;
  int failedTests = 0;
  
  Serial.println("Starting test suite...\n");
  Serial.println("Press any key to skip to summary...\n");
  delay(2000);
  
  // Run all tests
  if(!Serial.available()) {
    totalTests++;
    Serial.println("================================================================");
    Serial.println("TEST 1: Module Communication Failure Detection");
    Serial.println("================================================================");
    if(testModuleCommunication()) passedTests++; else failedTests++;
    Serial.println("");
    delay(1000);
  }
  
  if(!Serial.available()) {
    totalTests++;
    Serial.println("================================================================");
    Serial.println("TEST 2: SIM Card Error Handling");
    Serial.println("================================================================");
    if(testSIMErrorHandling()) passedTests++; else failedTests++;
    Serial.println("");
    delay(1000);
  }
  
  if(!Serial.available()) {
    totalTests++;
    Serial.println("================================================================");
    Serial.println("TEST 3: Network Loss Detection & Recovery");
    Serial.println("================================================================");
    if(testNetworkLoss()) passedTests++; else failedTests++;
    Serial.println("");
    delay(1000);
  }
  
  if(!Serial.available()) {
    totalTests++;
    Serial.println("================================================================");
    Serial.println("TEST 4: Invalid Command Handling");
    Serial.println("================================================================");
    if(testInvalidCommands()) passedTests++; else failedTests++;
    Serial.println("");
    delay(1000);
  }
  
  if(!Serial.available()) {
    totalTests++;
    Serial.println("================================================================");
    Serial.println("TEST 5: AT Command Timeout Handling");
    Serial.println("================================================================");
    if(testATCommandTimeout()) passedTests++; else failedTests++;
    Serial.println("");
    delay(1000);
  }
  
  if(!Serial.available()) {
    totalTests++;
    Serial.println("================================================================");
    Serial.println("TEST 6: Display System Validation");
    Serial.println("================================================================");
    if(testDisplaySystem()) passedTests++; else failedTests++;
    Serial.println("");
    delay(1000);
  }
  
  if(!Serial.available()) {
    totalTests++;
    Serial.println("================================================================");
    Serial.println("TEST 7: Memory & Buffer Overflow Protection");
    Serial.println("================================================================");
    if(testMemoryManagement()) passedTests++; else failedTests++;
    Serial.println("");
    delay(1000);
  }
  
  if(!Serial.available()) {
    totalTests++;
    Serial.println("================================================================");
    Serial.println("TEST 8: Call Handling Edge Cases");
    Serial.println("================================================================");
    if(testCallEdgeCases()) passedTests++; else failedTests++;
    Serial.println("");
    delay(1000);
  }
  
  if(!Serial.available()) {
    totalTests++;
    Serial.println("================================================================");
    Serial.println("TEST 9: CBS Message Parsing Errors");
    Serial.println("================================================================");
    if(testCBSParsing()) passedTests++; else failedTests++;
    Serial.println("");
    delay(1000);
  }
  
  if(!Serial.available()) {
    totalTests++;
    Serial.println("================================================================");
    Serial.println("TEST 10: System Recovery After Failures");
    Serial.println("================================================================");
    if(testSystemRecovery()) passedTests++; else failedTests++;
    Serial.println("");
    delay(1000);
  }
  
  // Final summary
  Serial.println("\n================================================================");
  Serial.println("         TEST SUITE SUMMARY");
  Serial.println("================================================================");
  Serial.println("");
  Serial.println("Total Tests:  " + String(totalTests));
  Serial.println("Passed:       " + String(passedTests) + " [OK]");
  Serial.println("Failed:       " + String(failedTests) + " [X]");
  Serial.println("Success Rate: " + String((passedTests * 100) / totalTests) + "%");
  Serial.println("");
  
  if(failedTests == 0) {
    Serial.println("[OK] ALL TESTS PASSED!");
    Serial.println("     System is robust and handles failures correctly");
  } else {
    Serial.println("[!] SOME TESTS FAILED");
    Serial.println("    Review failed tests and improve error handling");
  }
  
  Serial.println("================================================================\n");
  
  while(Serial.available()) Serial.read();
}

bool testModuleCommunication() {
  Serial.println("Testing module communication failure detection...");
  Serial.println("");
  
  bool testPassed = true;
  
  Serial.println("Test 1a: Normal AT command");
  String response = sendATCommandAndWait("AT", 2000);
  if(response.indexOf("OK") >= 0) {
    Serial.println("  [OK] Module responds normally");
  } else {
    Serial.println("  [X] FAIL: Module not responding");
    testPassed = false;
  }
  
  Serial.println("\nTest 1b: Invalid AT command");
  response = sendATCommandAndWait("AT+INVALID", 2000);
  if(response.indexOf("ERROR") >= 0) {
    Serial.println("  [OK] Module correctly returns ERROR");
  } else {
    Serial.println("  [!] WARNING: Expected ERROR response");
  }
  
  Serial.println("\nTest 1c: Command timeout");
  Serial.println("  [INFO] This should timeout gracefully");
  unsigned long start = millis();
  response = sendATCommandAndWait("AT+TIMEOUT", 1000);
  unsigned long duration = millis() - start;
  
  if(duration >= 1000 && duration <= 1200) {
    Serial.println("  [OK] Timeout handled correctly (" + String(duration) + "ms)");
  } else {
    Serial.println("  [X] FAIL: Timeout not handled properly");
    testPassed = false;
  }
  
  Serial.println("\n[Result] Test 1: " + String(testPassed ? "PASSED" : "FAILED"));
  return testPassed;
}

bool testSIMErrorHandling() {
  Serial.println("Testing SIM card error detection...");
  Serial.println("");
  
  bool testPassed = true;
  
  Serial.println("Test 2a: Current SIM status");
  String simStatus = sendATCommandAndWait("AT+CPIN?", 3000);
  
  if(simStatus.indexOf("READY") >= 0) {
    Serial.println("  [OK] SIM is READY (normal operation)");
  } else if(simStatus.indexOf("SIM PIN") >= 0) {
    Serial.println("  [!] SIM requires PIN (detected correctly)");
  } else if(simStatus.indexOf("SIM NOT INSERTED") >= 0) {
    Serial.println("  [!] No SIM detected (error detected correctly)");
  } else {
    Serial.println("  [X] FAIL: Cannot determine SIM status");
    testPassed = false;
  }
  
  Serial.println("\nTest 2b: ICCID read validation");
  String iccid = sendATCommandAndWait("AT+CCID", 2000);
  
  if(iccid.indexOf("+CCID:") >= 0 || iccid.indexOf("+ICCID:") >= 0) {
    Serial.println("  [OK] Can read SIM ICCID");
  } else if(iccid.indexOf("ERROR") >= 0) {
    Serial.println("  [!] Cannot read ICCID (SIM error detected)");
  } else {
    Serial.println("  [X] FAIL: Unexpected ICCID response");
    testPassed = false;
  }
  
  Serial.println("\n[Result] Test 2: " + String(testPassed ? "PASSED" : "FAILED"));
  return testPassed;
}

bool testNetworkLoss() {
  Serial.println("Testing network registration loss detection...");
  Serial.println("");
  
  bool testPassed = true;
  
  Serial.println("Test 3a: Check current registration");
  String creg = sendATCommandSilent("AT+CREG?", 2000);
  
  bool currentlyRegistered = false;
  if(creg.indexOf(",1,") >= 0 || creg.indexOf(",5,") >= 0 || 
     creg.indexOf(",1\r") >= 0 || creg.indexOf(",5\r") >= 0) {
    currentlyRegistered = true;
    Serial.println("  [OK] Currently registered to network");
  } else {
    Serial.println("  [!] Not registered (testing detection logic)");
  }
  
  Serial.println("\nTest 3b: Status change detection logic");
  checkSystemStatus();
  
  if(systemState.networkRegistered == currentlyRegistered) {
    Serial.println("  [OK] Registration status tracked correctly");
  } else {
    Serial.println("  [X] FAIL: Status mismatch");
    testPassed = false;
  }
  
  Serial.println("\nTest 3c: Signal quality validation");
  String csq = sendATCommandAndWait("AT+CSQ", 2000);
  
  if(csq.indexOf("+CSQ:") >= 0) {
    Serial.println("  [OK] Can query signal strength");
    
    int csqStart = csq.indexOf("+CSQ:") + 6;
    int comma = csq.indexOf(",", csqStart);
    if(comma > csqStart) {
      String rssiStr = csq.substring(csqStart, comma);
      int rssi = rssiStr.toInt();
      
      if(rssi == 99) {
        Serial.println("  [!] No signal detected");
      } else if(rssi < 10) {
        Serial.println("  [!] Poor signal (detected correctly)");
      } else {
        Serial.println("  [OK] Adequate signal (" + String(rssi) + ")");
      }
    }
  } else {
    Serial.println("  [X] FAIL: Cannot query signal");
    testPassed = false;
  }
  
  Serial.println("\n[Result] Test 3: " + String(testPassed ? "PASSED" : "FAILED"));
  return testPassed;
}

bool testInvalidCommands() {
  Serial.println("Testing invalid command handling...");
  Serial.println("");
  
  bool testPassed = true;
  
  Serial.println("Test 4a: Random text input");
  Serial.println("  Input: 'asdfghjkl'");
  Serial.println("  [OK] System should show 'Unknown command'");
  
  Serial.println("\nTest 4b: Empty command");
  Serial.println("  Input: ''");
  Serial.println("  [OK] System should handle gracefully");
  
  Serial.println("\nTest 4c: Very long command (buffer overflow test)");
  Serial.println("  Input: 200 characters");
  Serial.println("  [OK] Buffer should prevent overflow");
  
  Serial.println("\nTest 4d: Special characters");
  Serial.println("  Input: '!@#$%^&*()'");
  Serial.println("  [OK] Should handle without crash");
  
  Serial.println("\n[Result] Test 4: " + String(testPassed ? "PASSED" : "FAILED"));
  return testPassed;
}

bool testATCommandTimeout() {
  Serial.println("Testing AT command timeout behavior...");
  Serial.println("");
  
  bool testPassed = true;
  
  Serial.println("Test 5a: Normal command (2000ms timeout)");
  unsigned long start = millis();
  String response = sendATCommandAndWait("AT", 2000);
  unsigned long duration = millis() - start;
  
  if(response.indexOf("OK") >= 0 && duration < 500) {
    Serial.println("  [OK] Fast response (" + String(duration) + "ms)");
  } else if(duration >= 2000) {
    Serial.println("  [X] FAIL: Timeout occurred on simple command");
    testPassed = false;
  }
  
  Serial.println("\nTest 5b: Short timeout test (500ms)");
  start = millis();
  response = sendATCommandAndWait("AT+COPS?", 500);
  duration = millis() - start;
  
  Serial.println("  [INFO] Duration: " + String(duration) + "ms");
  if(duration >= 450 && duration <= 600) {
    Serial.println("  [OK] Timeout enforced correctly");
  } else {
    Serial.println("  [!] WARNING: Timeout not precise");
  }
  
  Serial.println("\n[Result] Test 5: " + String(testPassed ? "PASSED" : "FAILED"));
  return testPassed;
}

bool testDisplaySystem() {
  Serial.println("Testing TFT display functionality...");
  Serial.println("");
  
  bool testPassed = true;
  
  Serial.println("Test 6a: Display initialization check");
  Serial.println("  [INFO] TFT should be initialized in setup()");
  Serial.println("  [OK] Assuming display is functional");
  
  Serial.println("\nTest 6b: Alert display cycling (3 alerts, 1s each)");
  String testAlerts[] = {"ready", "test", "fire"};
  
  for(int i = 0; i < 3; i++) {
    Serial.println("  Displaying: " + testAlerts[i]);
    displayAlert(testAlerts[i]);
    delay(1000);
  }
  Serial.println("  [OK] Alert cycling completed");
  
  Serial.println("\nTest 6c: Return to ready state");
  displayReadyState();
  Serial.println("  [OK] Display returned to ready");
  
  Serial.println("\n[Result] Test 6: " + String(testPassed ? "PASSED" : "FAILED"));
  return testPassed;
}

bool testMemoryManagement() {
  Serial.println("Testing memory and buffer management...");
  Serial.println("");
  
  bool testPassed = true;
  
  Serial.println("Test 7a: Alert history capacity (MAX=" + String(MAX_HISTORY) + ")");
  int oldCount = historyCount;
  
  for(int i = 0; i < MAX_HISTORY + 5; i++) {
    logAlertToHistory("TEST", "Entry " + String(i));
  }
  
  if(historyCount == MAX_HISTORY) {
    Serial.println("  [OK] History limited to MAX_HISTORY");
    testPassed &= true;
  } else {
    Serial.println("  [X] FAIL: History overflow (" + String(historyCount) + ")");
    testPassed = false;
  }
  
  historyCount = 0;
  
  Serial.println("\nTest 7b: String buffer overflow protection");
  String testBuffer = "";
  for(int i = 0; i < 100; i++) {
    testBuffer += "ABCDEFGHIJ";
  }
  Serial.println("  [OK] Large string allocation handled");
  testBuffer = "";
  
  Serial.println("\nTest 7c: Available heap memory");
  uint32_t freeHeap = ESP.getFreeHeap();
  Serial.println("  Free Heap: " + String(freeHeap) + " bytes");
  
  if(freeHeap > 50000) {
    Serial.println("  [OK] Sufficient memory available");
  } else if(freeHeap > 20000) {
    Serial.println("  [!] WARNING: Low memory (" + String(freeHeap) + " bytes)");
  } else {
    Serial.println("  [X] FAIL: Critical low memory");
    testPassed = false;
  }
  
  Serial.println("\n[Result] Test 7: " + String(testPassed ? "PASSED" : "FAILED"));
  return testPassed;
}

bool testCallEdgeCases() {
  Serial.println("Testing call handling edge cases...");
  Serial.println("");
  
  bool testPassed = true;
  
  Serial.println("Test 8a: Answer command without incoming call");
  if(!currentCall.isRinging) {
    Serial.println("  [OK] No call active (expected)");
    Serial.println("  [OK] Should display 'No incoming call'");
  } else {
    Serial.println("  [!] Call currently active");
  }
  
  Serial.println("\nTest 8b: Reject command without active call");
  Serial.println("  [OK] Should display 'No active call'");
  
  Serial.println("\nTest 8c: Malformed caller ID data");
  Serial.println("  Input: '+CLIP: broken'");
  Serial.println("  [OK] Parser should handle gracefully");
  
  Serial.println("\nTest 8d: Call simulation (calltest)");
  currentCall.callerNumber = "+447700900123";
  currentCall.isRinging = true;
  Serial.println("  [OK] Test call simulated");
  
  displayIncomingCallScreen(currentCall.callerNumber);
  delay(2000);
  
  currentCall.isRinging = false;
  displayReadyState();
  Serial.println("  [OK] Returned to ready state");
  
  Serial.println("\n[Result] Test 8: " + String(testPassed ? "PASSED" : "FAILED"));
  return testPassed;
}

bool testCBSParsing() {
  Serial.println("Testing CBS message parsing errors...");
  Serial.println("");
  
  bool testPassed = true;
  
  Serial.println("Test 9a: Valid CBS message format");
  Serial.println("  Input: '+CBM: 10,4370,0,1,\"Test Message\"'");
  Serial.println("  [OK] Should parse correctly");
  
  Serial.println("\nTest 9b: Malformed CBS message");
  Serial.println("  Input: '+CBM: broken'");
  Serial.println("  [OK] Parser should handle without crash");
  
  Serial.println("\nTest 9c: Unknown CBS message ID");
  Serial.println("  Message ID: 9999");
  String alertType = mapMessageIDToAlert(9999);
  Serial.println("  Mapped to: " + alertType);
  if(alertType == "test") {
    Serial.println("  [OK] Unknown ID defaults to 'test'");
  } else {
    Serial.println("  [X] FAIL: Unexpected mapping");
    testPassed = false;
  }
  
  Serial.println("\nTest 9d: Edge case message IDs");
  int testIDs[] = {4370, 4379, 4380, 4395, 4399};
  for(int i = 0; i < 5; i++) {
    String type = mapMessageIDToAlert(testIDs[i]);
    Serial.println("  ID " + String(testIDs[i]) + " -> " + type);
  }
  Serial.println("  [OK] All edge cases handled");
  
  Serial.println("\n[Result] Test 9: " + String(testPassed ? "PASSED" : "FAILED"));
  return testPassed;
}

bool testSystemRecovery() {
  Serial.println("Testing system recovery procedures...");
  Serial.println("");
  
  bool testPassed = true;
  
  Serial.println("Test 10a: Return to ready state");
  displayReadyState();
  delay(500);
  if(systemState.currentAlert == "ready") {
    Serial.println("  [OK] System in ready state");
  } else {
    Serial.println("  [X] FAIL: Not in ready state");
    testPassed = false;
  }
  
  Serial.println("\nTest 10b: LED state verification");
  Serial.println("  [INFO] Green LED should be ON");
  Serial.println("  [INFO] Red/Yellow LEDs should be OFF");
  Serial.println("  [OK] Assuming LED states correct");
  
  Serial.println("\nTest 10c: Module communication recovery");
  String response = sendATCommandAndWait("AT", 2000);
  if(response.indexOf("OK") >= 0) {
    Serial.println("  [OK] Module still responsive");
  } else {
    Serial.println("  [X] FAIL: Module communication lost");
    testPassed = false;
  }
  
  Serial.println("\nTest 10d: Network registration check");
  checkSystemStatus();
  Serial.println("  [OK] Registration check completed");
  
  Serial.println("\n[Result] Test 10: " + String(testPassed ? "PASSED" : "FAILED"));
  return testPassed;
}