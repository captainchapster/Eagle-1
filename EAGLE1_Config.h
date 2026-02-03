#pragma once

#include <Arduino.h>

// ---------------- Clock ----------------
#define clockF          8.00

// ---------------- Pins -----------------
#define SCK             13
#define MISO            12
#define MOSI            11
#define SS              3
#define RST             2
#define DI0             4

// ---------------- Features -------------
#define enable_RTTY     1
#define enable_LoRa     0
#define enable_sleep    1
#define nullIsland      0

// ---------------- Timing ---------------
#define GPSBaud         9600
#define sleepTime       500
#define idleTime        250

// ---------------- RTTY -----------------
#define frequency_RTTY  434.0
#define carrierShift    425
#define baud            45
#define stopBits        2

// ---------------- LoRa -----------------
#define frequency_LoRa  434.25E6
#define bandWidth       125.0E3
#define SF              12
#define codingRate      4
#define txFrequency     4

// ---------------- Telemetry ------------
#define checkSum        1
#define callSign        "call_sign_here"
