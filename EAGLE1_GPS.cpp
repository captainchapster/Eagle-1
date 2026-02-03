#include "EAGLE1_GPS.h"
#include "EAGLE1_Config.h"

TinyGPSPlus gps;

static int h, m, s;
int hNew, mNew, sNew;

void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do {
    while (Serial.available())
      gps.encode(Serial.read());
  } while (millis() - start < (ms / clockF));
}

void updateGPS(void)
{
  h = gps.time.hour();
  m = gps.time.minute();
  s = gps.time.second();

  hNew = h;
  mNew = m;
  sNew = s;

  if (nullIsland == 0) {
    while (hNew == h && mNew == m && sNew == s) {
      smartDelay(sleepTime);
      hNew = gps.time.hour();
      mNew = gps.time.minute();
      sNew = gps.time.second();
    }
  } else {
    while ((gps.location.lat() == 0 && gps.location.lng() == 0) ||
           (hNew == h && mNew == m && sNew == s)) {
      smartDelay(sleepTime);
      hNew = gps.time.hour();
      mNew = gps.time.minute();
      sNew = gps.time.second();
    }
  }
}
