/*================================================================================
 CONFIG.CPP - Global Variable Definitions
================================================================================
 This file contains the implementation of all global variables and structures
 defined in Config.h. These variables are shared across all modules.
================================================================================*/

#include "Config.h"

//=============================================================================
// GLOBAL OBJECTS
//=============================================================================

// TFT Display object - shared across all display functions
TFT_eSPI tft = TFT_eSPI();

//=============================================================================
// SYSTEM STATE
//=============================================================================

// Main system state structure
SystemState systemState = {
  .simReady = false,                  // SIM card status
  .networkRegistered = false,         // Network registration status
  .cbsEnabled = false,                // CBS monitoring enabled flag
  .currentAlert = "ready",            // Current alert being displayed
  .nasMonitoringActive = false,       // NAS monitoring active flag
  .lastStatusCheck = 0,               // Last time status was checked (ms)
  .lastNASCheck = 0                   // Last time NAS was checked (ms)
};

//=============================================================================
// TOWER INFORMATION
//=============================================================================

// Cell tower information structure
TowerInfo currentTower = {
  .cellID = "",                       // Cell ID (from eNodeB)
  .pci = "",                          // Physical Cell ID
  .band = "",                         // LTE Band (e.g., "20")
  .earfcn = "",                       // E-UTRA Absolute Radio Frequency Channel Number
  .rssi = 0,                          // Received Signal Strength Indicator (dBm)
  .rsrp = 0,                          // Reference Signal Received Power (dBm)
  .rsrq = 0,                          // Reference Signal Received Quality (dB)
  .sinr = 0,                          // Signal-to-Interference-plus-Noise Ratio (dB)
  .tac = "",                          // Tracking Area Code
  .operator_name = "",                // Network operator name
  .lastUpdate = 0                     // Last update timestamp (ms)
};

// Flag indicating if tower data is valid
bool towerDataValid = false;

//=============================================================================
// CBS MESSAGE
//=============================================================================

// Last received CBS message
CBSMessage lastCBSMessage = {
  .messageID = 0,                     // CBS message ID (4370-4399)
  .content = "",                      // Message content
  .timestamp = 0,                     // Reception timestamp (ms)
  .isNew = false                      // Flag for new unprocessed message
};

//=============================================================================
// NAS MESSAGE
//=============================================================================

// Last detected NAS message
NASMessage lastNASMessage = {
  .type = "",                         // NAS message type (TAU, ATTACH, etc.)
  .content = "",                      // Message description
  .timestamp = 0,                     // Detection timestamp (ms)
  .detected = false                   // Flag indicating message detected
};

//=============================================================================
// INCOMING CALL
//=============================================================================

// Current incoming call information
IncomingCall currentCall = {
  .callerNumber = "",                 // Caller phone number
  .timestamp = 0,                     // Call timestamp (ms)
  .isRinging = false                  // Flag indicating active ring
};

//=============================================================================
// ALERT HISTORY
//=============================================================================

// Alert history log array
AlertHistory alertLog[MAX_HISTORY];

// Current number of alerts in history
int historyCount = 0;

//=============================================================================
// HARDWARE INITIALIZATION FUNCTIONS
//=============================================================================

/**
 * Initialize LED pins
 * Sets up the three status LEDs (Red, Yellow, Green)
 * Initial state: Green ON, others OFF
 */
void initializeLEDs() {
  // Configure LED pins as outputs
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  
  // Set initial LED states
  digitalWrite(LED_RED, LOW);      // Red LED OFF
  digitalWrite(LED_YELLOW, LOW);   // Yellow LED OFF
  digitalWrite(LED_GREEN, HIGH);   // Green LED ON (system ready)
}

//=============================================================================
// END OF CONFIG.CPP
//=============================================================================