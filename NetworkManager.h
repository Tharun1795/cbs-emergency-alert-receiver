#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include <Arduino.h>

// Get cell tower information
void getCellTowerInfo();

// Display tower info on TFT screen
void displayTowerInfoOnScreen();

// Live tower monitoring
void monitorTowerLive();

// Check system status (silent background check)
void checkSystemStatus();

// Get SIM card information
void getSIMInfo();

// Check CBS readiness
void checkCBSReadiness();

// NAS message monitoring
void checkNASMessages();
void monitorNASMessages();

// Test no-SIM mode
void testNoSIMMode();
void parseQENGMeasurements(String qeng);

#endif