#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <Arduino.h>
#include "Config.h"

// Initialize display and LEDs
void initializeDisplay();
void initializeLEDs();

// Display screens
void displayWelcome();
void displayReadyState();
void displayAlert(String alertType);
void displayTowerData(TowerInfo tower);
void displayIncomingCallScreen(String number);

// Helper function for alert screens (ADDED THIS!)
void displayAlertScreen(String severity, String headline, String line1, 
                        String line2, String line3, uint16_t color, int ledPin);

// LED control
void blinkCallLEDs();

// Icon drawing functions
void drawFireIcon(int x, int y);
void drawFloodIcon(int x, int y);
void drawWeatherIcon(int x, int y);
void drawEarthquakeIcon(int x, int y);
void drawEmergencyIcon(int x, int y);
void drawWarningIcon(int x, int y);
void drawInfoIcon(int x, int y);
void drawCheckmarkIcon(int x, int y);
void drawPhoneIcon(int x, int y);

#endif