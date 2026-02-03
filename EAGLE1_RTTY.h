#pragma once

#include <RadioLib.h>

extern SX1278 fsk;
extern RTTYClient rtty;

void rttySetUp(void);
uint8_t gps_xor_checksum(char *string);
