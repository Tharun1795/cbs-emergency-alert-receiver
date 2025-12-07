#ifndef CALL_HANDLER_H
#define CALL_HANDLER_H

#include <Arduino.h>

// Handle incoming ring
void handleIncomingRing();

// Parse incoming call details
void parseIncomingCall(String message);

// Handle call end
void handleCallEnd();

// Answer call
void answerCall();

// Reject call
void rejectCall();

// Simulate test call
void simulateTestCall();

#endif