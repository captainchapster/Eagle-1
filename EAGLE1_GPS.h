#pragma once

#include <TinyGPS++.h>

extern TinyGPSPlus gps;

void smartDelay(unsigned long ms);
void updateGPS(void);
