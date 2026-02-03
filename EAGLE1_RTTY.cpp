#include "EAGLE1_RTTY.h"
#include "EAGLE1_Config.h"

SX1278 fsk = new Module(SS, DI0, RST, -1);
RTTYClient rtty(&fsk);

void rttySetUp(void)
{
  fsk.beginFSK();
  rtty.begin(frequency_RTTY, carrierShift, baud * clockF, ASCII, stopBits);
}

uint8_t gps_xor_checksum(char *string)
{
  uint8_t XOR = 0;
  for (size_t i = 0; i < strlen(string); i++)
    XOR ^= string[i];
  return XOR;
}
