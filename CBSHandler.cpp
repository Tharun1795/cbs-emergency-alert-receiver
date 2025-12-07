#include "CBSHandler.h"
#include "Config.h"
#include "DisplayManager.h"

// Latency measurement variables
unsigned long cbsReceiveTime = 0;
unsigned long uartReceiveTime = 0;
unsigned long parseCompleteTime = 0;
unsigned long displayCompleteTime = 0;
unsigned long ledCompleteTime = 0;

// Latency statistics
LatencyStats latencyStats = {{0}, 0, 999999, 0, 0};

void parseCBSMessage(String rawMessage) {
    // Timestamp 1: UART received
    uartReceiveTime = millis();
    
    Serial.println("\n========================================");
    Serial.println("CBS MESSAGE LATENCY MEASUREMENT");
    Serial.println("========================================");
    Serial.print("[T1] UART Received: ");
    Serial.print(uartReceiveTime);
    Serial.println(" ms");
    
    // Validate message format
    if(rawMessage.indexOf("+CBM:") == -1) {
        Serial.println("ERROR: Invalid CBS format - missing +CBM:");
        return;
    }
    
    // Extract components
    int firstComma = rawMessage.indexOf(',');
    int secondComma = rawMessage.indexOf(',', firstComma + 1);
    
    if(firstComma == -1 || secondComma == -1) {
        Serial.println("ERROR: Invalid CBS format - missing commas");
        return;
    }
    
    // Parse message ID
    String msgIdStr = rawMessage.substring(firstComma + 1, secondComma);
    int messageID = msgIdStr.toInt();
    
    // Timestamp 2: Parsing complete
    parseCompleteTime = millis();
    Serial.print("[T2] Parsing Done: ");
    Serial.print(parseCompleteTime);
    Serial.print(" ms (Delta: ");
    Serial.print(parseCompleteTime - uartReceiveTime);
    Serial.println(" ms)");
    
    // Determine alert type
    String alertType = mapMessageIDToAlert(messageID);
    
    Serial.print("Message ID: ");
    Serial.println(messageID);
    Serial.print("Alert Type: ");
    Serial.println(alertType);
    
    // Extract message content
    int lastComma = rawMessage.lastIndexOf(',');
    String content = "";
    if(lastComma != -1) {
        content = rawMessage.substring(lastComma + 1);
        content.replace("\"", "");
        content.trim();
    }
    
    Serial.print("Content: ");
    Serial.println(content);
    
    // Timestamp 3: Display update starts
    Serial.print("[T3] Display Update Start: ");
    Serial.println(millis());
    
    // Update display (only one argument)
    displayAlert(alertType);
    
    // Timestamp 4: Display update complete
    displayCompleteTime = millis();
    Serial.print("[T4] Display Complete: ");
    Serial.print(displayCompleteTime);
    Serial.print(" ms (Delta: ");
    Serial.print(displayCompleteTime - parseCompleteTime);
    Serial.println(" ms)");
    
    // Timestamp 5: LED update complete
    ledCompleteTime = millis();
    Serial.print("[T5] LED Complete: ");
    Serial.print(ledCompleteTime);
    Serial.print(" ms (Delta: ");
    Serial.print(ledCompleteTime - displayCompleteTime);
    Serial.println(" ms)");
    
    // Calculate total latency
    unsigned long totalLatency = ledCompleteTime - uartReceiveTime;
    
    Serial.println("\n========================================");
    Serial.println("LATENCY BREAKDOWN SUMMARY");
    Serial.println("========================================");
    Serial.print("UART to Parse:    ");
    Serial.print(parseCompleteTime - uartReceiveTime);
    Serial.println(" ms");
    Serial.print("Parse to Display: ");
    Serial.print(displayCompleteTime - parseCompleteTime);
    Serial.println(" ms");
    Serial.print("Display to LED:   ");
    Serial.print(ledCompleteTime - displayCompleteTime);
    Serial.println(" ms");
    Serial.println("----------------------------------------");
    Serial.print("TOTAL LATENCY:    ");
    Serial.print(totalLatency);
    Serial.println(" ms");
    Serial.print("TOTAL LATENCY:    ");
    Serial.print(totalLatency / 1000.0, 3);
    Serial.println(" seconds");
    Serial.println("========================================\n");
    
    // Record this measurement
    recordLatency(totalLatency);
    
    // Alert history not implemented - comment out
    // addToAlertHistory(alertType, content);
}

String mapMessageIDToAlert(int messageID) {
  if(messageID >= 4370 && messageID <= 4379) return "fire";
  else if(messageID >= 4380 && messageID <= 4383) return "earthquake";
  else if(messageID >= 4384 && messageID <= 4395) return "flood";
  else if(messageID >= 4396 && messageID <= 4397) return "nuclear";
  else if(messageID >= 4398 && messageID <= 4399) return "test";
  else return "test";
}

void logAlertToHistory(String alertType, String content) {
  if(historyCount < MAX_HISTORY) {
    alertLog[historyCount].alertType = alertType;
    alertLog[historyCount].timestamp = String(millis()/1000) + "s";
    alertLog[historyCount].content = content;
    historyCount++;
  } else {
    for(int i = 0; i < MAX_HISTORY-1; i++) {
      alertLog[i] = alertLog[i+1];
    }
    alertLog[MAX_HISTORY-1].alertType = alertType;
    alertLog[MAX_HISTORY-1].timestamp = String(millis()/1000) + "s";
    alertLog[MAX_HISTORY-1].content = content;
  }
}

void showAlertHistory() {
  Serial.println("\n================================================================");
  Serial.println("         ALERT HISTORY");
  Serial.println("================================================================\n");
  
  if(historyCount == 0) {
    Serial.println("No alerts in history");
  } else {
    for(int i = 0; i < historyCount; i++) {
      Serial.print(i+1);
      Serial.print(". ");
      Serial.print(alertLog[i].alertType);
      Serial.print(" @ ");
      Serial.print(alertLog[i].timestamp);
      Serial.print(" - ");
      Serial.println(alertLog[i].content);
    }
  }
  
  Serial.println("================================================================\n");
}

void recordLatency(unsigned long latency) {
    if(latencyStats.count < MAX_LATENCY_SAMPLES) {
        latencyStats.samples[latencyStats.count] = latency;
        latencyStats.count++;
        
        if(latency < latencyStats.minLatency) latencyStats.minLatency = latency;
        if(latency > latencyStats.maxLatency) latencyStats.maxLatency = latency;
        
        unsigned long sum = 0;
        for(int i = 0; i < latencyStats.count; i++) {
            sum += latencyStats.samples[i];
        }
        latencyStats.avgLatency = sum / latencyStats.count;
    }
}

void printLatencyStats() {
    Serial.println("\n========================================");
    Serial.println("LATENCY STATISTICS REPORT");
    Serial.println("========================================");
    Serial.print("Samples Collected: ");
    Serial.println(latencyStats.count);
    
    if(latencyStats.count > 0) {
        Serial.print("Minimum Latency: ");
        Serial.print(latencyStats.minLatency);
        Serial.println(" ms");
        Serial.print("Maximum Latency: ");
        Serial.print(latencyStats.maxLatency);
        Serial.println(" ms");
        Serial.print("Average Latency: ");
        Serial.print(latencyStats.avgLatency);
        Serial.println(" ms");
        Serial.print("Average (seconds): ");
        Serial.print(latencyStats.avgLatency / 1000.0, 3);
        Serial.println(" s");
        
        Serial.println("\nLast 10 Measurements:");
        int start = (latencyStats.count > 10) ? latencyStats.count - 10 : 0;
        for(int i = start; i < latencyStats.count; i++) {
            Serial.print("  [");
            Serial.print(i + 1);
            Serial.print("] ");
            Serial.print(latencyStats.samples[i]);
            Serial.println(" ms");
        }
    } else {
        Serial.println("No latency data collected yet.");
        Serial.println("Run 'testcbs' to collect measurements.");
    }
    Serial.println("========================================\n");
}