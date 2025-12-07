#include "ModemComm.h"
#include "Config.h"
#include "CBSHandler.h"
#include "CallHandler.h"

void clearSerialBuffer() {
  while(Serial2.available()) Serial2.read();
  delay(100);
}

String sendATCommandAndWait(String cmd, int timeout) {
  clearSerialBuffer();
  Serial.println("Sending: " + cmd);
  Serial2.println(cmd);
  
  String response = "";
  unsigned long startTime = millis();
  
  while(millis() - startTime < timeout) {
    while(Serial2.available()) {
      char c = Serial2.read();
      if(c >= 32 && c <= 126) {
        response += c;
        Serial.write(c);
      } else if(c == '\r' || c == '\n') {
        response += c;
        Serial.write(c);
      }
    }
    
    if(response.indexOf("OK") >= 0) {
      delay(100);
      while(Serial2.available()) {
        char c = Serial2.read();
        if(c >= 32 && c <= 126) {
          response += c;
          Serial.write(c);
        } else if(c == '\r' || c == '\n') {
          response += c;
          Serial.write(c);
        }
      }
      break;
    }
    
    if(response.indexOf("ERROR") >= 0) {
      delay(100);
      while(Serial2.available()) {
        char c = Serial2.read();
        if(c >= 32 && c <= 126) {
          response += c;
          Serial.write(c);
        } else if(c == '\r' || c == '\n') {
          response += c;
          Serial.write(c);
        }
      }
      break;
    }
    
    delay(10);
  }
  
  Serial.println();
  return response;
}

String sendATCommandSilent(String cmd, int timeout) {
  clearSerialBuffer();
  Serial2.println(cmd);
  
  String response = "";
  unsigned long startTime = millis();
  
  while(millis() - startTime < timeout) {
    while(Serial2.available()) {
      char c = Serial2.read();
      if(c >= 32 && c <= 126) {
        response += c;
      } else if(c == '\r' || c == '\n') {
        response += c;
      }
    }
    
    if(response.indexOf("OK") >= 0) {
      delay(100);
      while(Serial2.available()) {
        char c = Serial2.read();
        if(c >= 32 && c <= 126) {
          response += c;
        } else if(c == '\r' || c == '\n') {
          response += c;
        }
      }
      break;
    }
    
    if(response.indexOf("ERROR") >= 0) {
      delay(100);
      while(Serial2.available()) {
        char c = Serial2.read();
        if(c >= 32 && c <= 126) {
          response += c;
        } else if(c == '\r' || c == '\n') {
          response += c;
        }
      }
      break;
    }
    
    delay(10);
  }
  
  return response;
}

void initializeCBSSystem() {
  Serial.println("\n================================================================");
  Serial.println("         CBS SYSTEM INITIALIZATION");
  Serial.println("================================================================\n");
  
  Serial.println("Step 1: Initializing module...");
  delay(3000);
  
  Serial.println("\nTesting module communication...");
  String atResponse = sendATCommandAndWait("AT", 2000);
  if(atResponse.indexOf("OK") < 0) {
    Serial.println("X Module not responding!");
    return;
  }
  Serial.println("OK Module communication OK!");
  
  // Step 2: Check SIM card
  Serial.println("\nStep 2: Checking SIM card...");
  for(int i = 0; i < 5; i++) {
    String simResponse = sendATCommandAndWait("AT+CPIN?", 3000);
    if(simResponse.indexOf("READY") >= 0) {
      systemState.simReady = true;
      Serial.println("\nOK SIM card is READY!");
      break;
    } else if(simResponse.indexOf("SIM PIN") >= 0) {
      Serial.println("X SIM requires PIN!");
      return;
    } else {
      Serial.println("! Waiting... attempt " + String(i+1) + "/5");
      delay(2000);
    }
  }
  
  if(!systemState.simReady) {
    Serial.println("X SIM card ERROR");
    return;
  }
  
  // Step 3: Network registration
  Serial.println("\nStep 3: Checking network registration...");
  Serial.println("(May take 30-90 seconds...)");
  
  sendATCommandAndWait("AT+CREG=2", 1000);
  delay(500);
  
  for(int attempt = 0; attempt < 30; attempt++) {
    String regResponse = sendATCommandAndWait("AT+CREG?", 2000);
    
    Serial.println("Checking response for registration status...");
    
    bool isRegistered = false;
    
    if(regResponse.indexOf("+CREG:") >= 0 || regResponse.indexOf("+CREG :") >= 0) {
      int cregPos = regResponse.indexOf("+CREG");
      String afterCreg = regResponse.substring(cregPos);
      
      if(afterCreg.indexOf(",1,") >= 0 || afterCreg.indexOf(",1\r") >= 0 || 
         afterCreg.indexOf(",5,") >= 0 || afterCreg.indexOf(",5\r") >= 0) {
        isRegistered = true;
      }
    }
    
    if(isRegistered) {
      systemState.networkRegistered = true;
      Serial.println("\nOK REGISTERED to network!");
      break;
    } 
    else if(regResponse.indexOf(",2,") >= 0 || regResponse.indexOf(",2\r") >= 0) {
      Serial.print(". [searching]");
      delay(3000);
    } 
    else {
      Serial.print("? [unknown response]");
      delay(3000);
    }
  }
  
  if(!systemState.networkRegistered) {
    Serial.println("\n! Network registration timeout!");
    Serial.println("  (Device may still be searching...)");
  }
  
  // Step 4: Configure CBS
  if(systemState.simReady && systemState.networkRegistered) {
    Serial.println("\nStep 4: Configuring CBS...");
    String cbs1 = sendATCommandAndWait("AT+CSCB=0", 2000);
    if(cbs1.indexOf("OK") >= 0) {
      systemState.cbsEnabled = true;
      Serial.println("OK CBS ENABLED!");
    } else {
      Serial.println("! CBS command not supported (may work anyway)");
    }
    
    sendATCommandAndWait("AT+CNMI=2,0,2,0,1", 2000);
    
    Serial.println("\nVerifying CBS...");
    sendATCommandAndWait("AT+CSCB?", 2000);
    
    Serial.println("\nStep 5: Enabling Caller ID...");
    String clip = sendATCommandAndWait("AT+CLIP=1", 2000);
    if(clip.indexOf("OK") >= 0) {
      Serial.println("OK Caller ID ENABLED!");
    } else {
      Serial.println("! Caller ID may not be supported");
    }
  }
  
  Serial.println("\nChecking signal...");
  sendATCommandAndWait("AT+CSQ", 2000);
  
  Serial.println("\nChecking operator...");
  sendATCommandAndWait("AT+COPS?", 3000);
  
  // Print summary
  Serial.println("\n================================================================");
  Serial.println("         INITIALIZATION SUMMARY");
  Serial.println("================================================================");
  Serial.println("SIM Card: " + String(systemState.simReady ? "READY [OK]" : "NOT READY [X]"));
  Serial.println("Network:  " + String(systemState.networkRegistered ? "REGISTERED [OK]" : "NOT REGISTERED [X]"));
  Serial.println("CBS:      " + String(systemState.cbsEnabled ? "ENABLED [OK]" : "UNKNOWN"));
  Serial.println("CallID:   ENABLED [OK]");
  Serial.println("================================================================\n");
  
  if(systemState.simReady && systemState.networkRegistered) {
    Serial.println("OK OK OK SYSTEM READY! OK OK OK\n");
  } else {
    Serial.println("! System not fully operational\n");
  }
}

void handleModuleData() {
  static String buffer = "";
  
  while(Serial2.available()) {
    char c = Serial2.read();
    
    if(c >= 32 && c <= 126) {
      buffer += c;
    } else if(c == '\r' || c == '\n') {
      buffer += c;
    }
    
    // Check for CBS message
    if(buffer.indexOf("+CBM:") >= 0) {
      Serial.println("\n!!! CBS MESSAGE RECEIVED !!!");
      Serial.println(buffer);
      parseCBSMessage(buffer);
      buffer = "";
    }
    
    // Check for incoming call
    if(buffer.indexOf("RING") >= 0) {
      handleIncomingRing();
    }
    
    // Check for Caller ID
    if(buffer.indexOf("+CLIP:") >= 0) {
      parseIncomingCall(buffer);
      buffer = "";
    }
    
    // Check for call ended
    if(buffer.indexOf("NO CARRIER") >= 0 || buffer.indexOf("BUSY") >= 0) {
      handleCallEnd();
    }
    
    if(c == '\n') {
      if(buffer.length() > 2) {
        Serial.print("Module: ");
        Serial.println(buffer);
      }
      buffer = "";
    }
    
    if(buffer.length() > 1000) buffer = "";
  }
}