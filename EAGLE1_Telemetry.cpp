#include "EAGLE1_Telemetry.h"
#include "EAGLE1_Config.h"
#include "EAGLE1_GPS.h"
#include "EAGLE1_RTTY.h"
#include "uCRC16XModemLib.h"

uCRC16XModemLib crc;

String buildRTTYPayload(unsigned int counter)
{
  String payload =
    String(callSign) + "," +
    String(counter) + "," +
    String(gps.time.hour()) + ":" +
    String(gps.time.minute()) + ":" +
    String(gps.time.second()) + "," +
    String(gps.location.lat(), 6) + "," +
    String(gps.location.lng(), 6) + "," +
    String(gps.altitude.meters()) + "," +
    String(gps.speed.kmph()) + "," +
    String(gps.course.deg()) + "," +
    String(gps.satellites.value());

  char buff[payload.length() + 1];
  payload.toCharArray(buff, sizeof(buff));

  if (checkSum == 1) {
    String crcSum = String(crc.calculate(buff, 0), HEX);
    crcSum.toUpperCase();
    return "$$" + payload + "*" + crcSum + "\n";
  }

  if (checkSum == 2) {
    String xorSum = String(gps_xor_checksum(buff), HEX);
    xorSum.toUpperCase();
    return "$$" + payload + "*" + xorSum + "\n";
  }

  return "$$" + payload + "\n";
}
