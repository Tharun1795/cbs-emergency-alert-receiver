#ifndef TEST_SUITE_H
#define TEST_SUITE_H

#include <Arduino.h>

// Main test suite runner
void runTestSuite();

// Individual test functions
bool testModuleCommunication();
bool testSIMErrorHandling();
bool testNetworkLoss();
bool testInvalidCommands();
bool testATCommandTimeout();
bool testDisplaySystem();
bool testMemoryManagement();
bool testCallEdgeCases();
bool testCBSParsing();
bool testSystemRecovery();

#endif