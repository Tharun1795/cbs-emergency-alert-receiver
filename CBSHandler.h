#ifndef CBS_HANDLER_H
#define CBS_HANDLER_H

#include <Arduino.h>

// Parse CBS message from modem
void parseCBSMessage(String message);

// Map message ID to alert type
String mapMessageIDToAlert(int messageID);

// Alert history management
void logAlertToHistory(String alertType, String content);
void showAlertHistory();

// Latency measurement functions
void recordLatency(unsigned long latency);
void printLatencyStats();

#endif