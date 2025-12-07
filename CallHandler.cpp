#include "CallHandler.h"
#include "Config.h"
#include "ModemComm.h"
#include "DisplayManager.h"
#include "CBSHandler.h"

void handleIncomingRing() {
  currentCall.isRinging = true;
  currentCall.timestamp = millis();
  Serial.println("\n================================================================");
  Serial.println("         INCOMING CALL DETECTED!");
  Serial.println("================================================================");
  Serial.println("Status: RINGING...");
  Serial.println("Waiting for caller ID...");
}

void parseIncomingCall(String message) {
  Serial.println("\n================================================================");
  Serial.println("         INCOMING CALL INFORMATION");
  Serial.println("================================================================");
  
  int startQuote = message.indexOf("\"") + 1;
  int endQuote = message.indexOf("\"", startQuote);
  
  if(startQuote > 0 && endQuote > startQuote) {
    currentCall.callerNumber = message.substring(startQuote, endQuote);
    currentCall.timestamp = millis();
    
    Serial.println("Caller Number: " + currentCall.callerNumber);
    Serial.println("Time:          " + String(millis()/1000) + "s");
    Serial.println("Status:        RINGING");
    Serial.println("================================================================");
    Serial.println("\nCommands:");
    Serial.println("  answer - Answer the call");
    Serial.println("  reject - Reject the call");
    Serial.println("================================================================\n");
    
    displayIncomingCallScreen(currentCall.callerNumber);
    logAlertToHistory("CALL", currentCall.callerNumber);
    blinkCallLEDs();
  }
}

void handleCallEnd() {
  if(currentCall.isRinging) {
    Serial.println("\n[INFO] Call ended/missed");
    Serial.println("================================================================\n");
    currentCall.isRinging = false;
    displayReadyState();
  }
}

void answerCall() {
  Serial.println("\n[COMMAND] Answering call...");
  sendATCommandAndWait("ATA", 2000);
  currentCall.isRinging = false;
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_GREEN, HIGH);
  Serial.println("[OK] Call answered!\n");
}

void rejectCall() {
  Serial.println("\n[COMMAND] Rejecting call...");
  sendATCommandAndWait("ATH", 2000);
  currentCall.isRinging = false;
  digitalWrite(LED_YELLOW, LOW);
  Serial.println("[OK] Call rejected!\n");
  
  displayReadyState();
}

void simulateTestCall() {
  currentCall.callerNumber = "+447700900123";
  currentCall.isRinging = true;
  currentCall.timestamp = millis();
  parseIncomingCall("+CLIP: \"+447700900123\",145");
}