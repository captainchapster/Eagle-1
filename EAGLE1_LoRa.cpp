#include <LoRa.h>
#include "EAGLE1_Config.h"

void LoRaSetUp(void)
{
  LoRa.setPins(SS, RST, DI0);
  LoRa.setTxPower(17);
  LoRa.setFrequency(frequency_LoRa);
  LoRa.setSignalBandwidth(bandWidth);
  LoRa.setSpreadingFactor(SF);
  LoRa.setCodingRate4(codingRate);
  LoRa.setPreambleLength(8);
  LoRa.setSyncWord(0x34);
}
