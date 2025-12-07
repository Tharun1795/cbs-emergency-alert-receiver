#ifndef CONFIG_H
#define CONFIG_H

#include <TFT_eSPI.h>

// Hardware Pin Definitions
#define LED_RED 19
#define LED_YELLOW 32
#define LED_GREEN 22

// Timing Constants
const unsigned long STATUS_INTERVAL = 60000;
const unsigned long NAS_CHECK_INTERVAL = 5000;

// History Configuration
#define MAX_HISTORY 20

// Global TFT Object
extern TFT_eSPI tft;

// System State Structure
struct SystemState {
  bool simReady;
  bool networkRegistered;
  bool cbsEnabled;
  String currentAlert;
  bool nasMonitoringActive;
  unsigned long lastStatusCheck;
  unsigned long lastNASCheck;
};

extern SystemState systemState;

// Tower Information Structure
struct TowerInfo {
  String cellID;
  String pci;
  String band;
  String earfcn;
  int rssi;
  int rsrp;
  int rsrq;
  int sinr;
  String tac;
  String operator_name;
  unsigned long lastUpdate;
};

extern TowerInfo currentTower;
extern bool towerDataValid;

// CBS Message Structure
struct CBSMessage {
  int messageID;
  String content;
  unsigned long timestamp;
  bool isNew;
};

extern CBSMessage lastCBSMessage;

// NAS Message Structure
struct NASMessage {
  String type;
  String content;
  unsigned long timestamp;
  bool detected;
};

extern NASMessage lastNASMessage;

// Incoming Call Structure
struct IncomingCall {
  String callerNumber;
  unsigned long timestamp;
  bool isRinging;
};

extern IncomingCall currentCall;

// Alert History Structure
struct AlertHistory {
  String alertType;
  String timestamp;
  String content;
};

extern AlertHistory alertLog[MAX_HISTORY];
extern int historyCount;

// Function to initialize LEDs
void initializeLEDs();

// Latency measurement configuration
#define ENABLE_LATENCY_LOGGING true
#define MAX_LATENCY_SAMPLES 50

// Timing variables
extern unsigned long cbsReceiveTime;
extern unsigned long uartReceiveTime;
extern unsigned long parseCompleteTime;
extern unsigned long displayCompleteTime;
extern unsigned long ledCompleteTime;

// Latency statistics structure
struct LatencyStats {
    unsigned long samples[MAX_LATENCY_SAMPLES];
    int count;
    unsigned long minLatency;
    unsigned long maxLatency;
    unsigned long avgLatency;
};

extern LatencyStats latencyStats;

// Function declarations
void recordLatency(unsigned long latency);
void printLatencyStats();

#endif