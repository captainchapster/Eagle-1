#include "EAGLE1_Config.h"
#include "EAGLE1_GPS.h"
#include "EAGLE1_RTTY.h"
#include "EAGLE1_LoRa.h"
#include "EAGLE1_Telemetry.h"

unsigned int counter = 0;

void setup()
{
  Serial.begin(GPSBaud * clockF);
}

void loop()
{
  if (enable_sleep)
    updateGPS();

  if (enable_RTTY) {
    rttySetUp();
    rtty.idle();
    smartDelay(idleTime);
    rtty.print(buildRTTYPayload(counter));
  }

  if (enable_LoRa && counter % txFrequency == 0) {
    LoRaSetUp();
    LoRa.beginPacket();
    LoRa.print(counter);
    LoRa.endPacket();
  }

  if (enable_RTTY || enable_LoRa)
    counter++;
}
