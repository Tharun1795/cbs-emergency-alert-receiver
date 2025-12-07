# cbs-emergency-alert-receiver
CBS Emergency Alert Receiver - MSc Dissertation Project

# CBS Emergency Alert Receiver

MSc Dissertation Project

## Project Overview

4G LTE Cell Broadcast Service (CBS) emergency alert receiver

## Key Results

- **Latency:** 2.29s (54% faster than 5s UK requirement)
- **Reliability:** 98.2% (110/112 messages over 24 hours)
- **Accuracy:** 100% classification across all UK alert types
- **Cost:** £80 development cost
- **Target Users:** 2.7M elderly UK citizens without smartphones

## Hardware

- ESP32-WROOM-32 microcontroller
- SIM7600E-H 4G LTE modem
- ILI9341 2.8" TFT display
- RGB LEDs for status indication

## Performance Comparison

This 4G optimised system (2.29s latency) outperforms baseline 5G implementations (5.0s) by 54%, demonstrating that system-level optimisation is more impactful than network generation for CBS delivery.

## Project Structure

Main Program

CBS_System.ino - Main Arduino sketch, setup, and loop functions

CBS Message Handling Module

CBSHandler.cpp - CBS message reception, PDU parsing, GSM-7 decoding implementation
CBSHandler.h - CBS handler interface and message structure definitions

Call Management Module

CallHandler.cpp - Phone call handling and voice communication implementation
CallHandler.h - Call handler interface and call state definitions

Network Communication Module

NetworkManager.cpp - 4G network registration, signal monitoring, connectivity management
NetworkManager.h - Network manager interface and network status structures

Modem Communication Module

ModemComm.cpp - Low-level UART communication, AT command handling implementation
ModemComm.h - Modem communication interface and AT command definitions

Display Management Module

DisplayManager.cpp - TFT display control, screen rendering, alert visualization
DisplayManager.h - Display manager interface and screen layout definitions

Command Interface Module

Commands.cpp - Serial command processing, debugging commands, test functions
Commands.h - Command interface and command handler definitions

Configuration Module

Config.cpp - System configuration, settings storage, parameter management
Config.h - Configuration interface, constants, and GPIO pin definitions

Testing Module

TestSuite.cpp - Unit tests, integration tests, performance validation
TestSuite.h - Test suite interface and test case definitions

Library Configuration

User_Setup.h - TFT_eSPI library configuration (display pins, driver selection)

## Features

- CBS message reception and parsing
- GSM-7 character decoding
- Colour-coded alert display (Red/Orange/Yellow/Blue/Green)
- Priority-based alert queue
- Network status indicators
- Performance monitoring (latency tracking)
- Message deduplication

## Technical Specifications

- **Network:** 4G LTE Cat 4 (UK bands B1/B3/B7/B8/B20)
- **Protocol:** 3GPP TS 23.041 CBS standard
- **Alert Types:** UK Emergency Alerts (Message IDs 4370-4382)
- **Processing:** ESP32 dual-core 240 MHz
- **Display:** 320x240 RGB565 TFT, 40 MHz SPI
- **Total Code:** 3,200+ lines across 17 files

## Author

Tharun  
MSc Electrical and Electronics Engineering  
Coventry University  
