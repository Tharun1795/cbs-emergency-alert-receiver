#ifndef COMMANDS_H
#define COMMANDS_H

#include <Arduino.h>

// Main command handler
void handleCommand(String cmd);

// System status commands
void showFullStatus();
void checkCBSStatus();
void testATCommands();

// Demo and utility commands
void autoCycleAlerts();
void runFullDemo();
void printWelcomeMessage();

#endif