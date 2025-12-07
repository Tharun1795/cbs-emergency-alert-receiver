#ifndef MODEM_COMM_H
#define MODEM_COMM_H

#include <Arduino.h>

// Clear serial buffer
void clearSerialBuffer();

// Send AT command and wait for response (verbose)
String sendATCommandAndWait(String cmd, int timeout);

// Send AT command silently (no serial output)
String sendATCommandSilent(String cmd, int timeout);

// Initialize modem and CBS system
void initializeCBSSystem();

// Handle incoming data from modem
void handleModuleData();

#endif