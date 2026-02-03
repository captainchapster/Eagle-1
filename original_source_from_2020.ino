#include <LoRa.h>   
#include <TinyGPS++.h>
#include <RadioLib.h>
#include "uCRC16XModemLib.h"

static const uint32_t GPSBaud = 9600;

TinyGPSPlus gps;
SX1278 fsk = new Module(3, 4, 2, 5);
RTTYClient rtty(&fsk);
uCRC16XModemLib crc;

#define clockF          8.00                    // Clock correction factor
                                                // Note this is a hack, appropriate fuses should
                                                // be burned using something like AVRDUDE if using
                                                // an external clock such as an 8MHz oscialltor.

#define SCK             13                      // GPIO5  -- SX1278's SCK
#define MISO            12                      // GPIO19 -- SX1278's MISnO
#define MOSI            11                      // GPIO27 -- SX1278's MOSI
#define SS              3                       // GPIO18 -- SX1278's CS
#define RST             2                       // GPIO14 -- SX1278's RESET
#define DI0             4                       // GPIO26 -- SX1278's IRQ(Interrupt Request)

#define enable_RTTY     1                       // Enable desired telemetry mode (both can be used simultaneously)
#define enable_LoRa     0                       // 0 -> off, 1-> on
#define enable_sleep    1                       // Sleep transmitter if GPS has no fix
#define sleepTime       500                     // milliseconds to sleep when GPS has no fix
#define nullIsland      0                       // Report null island location [0]no [1]yes

#define frequency_RTTY  434.0                   // MHz (use 915 MHz band for high baud rate)
#define carrierShift    425                     // Hz
#define baud            45                      // 45-200 anything greater fails for 433 MHz
#define stopBits        2                       // 1, 1.5, 2
#define idleTime        250                     // milliseconds

#define frequency_LoRa  434.25E6                // Hz
#define bandWidth       125.0E3                 // Hz
#define SF              12                      // 7-12
#define codingRate      4
#define txFrequency     4                       // Transmit LoRa every txFrequency'th RTTY message

#define checkSum        1                       // Use [0]off, [1]CRC-16 or [2]XOR for telemetry checksum

#define callSign        "call_sign_here"        // Payload callsign for RTTY

float V, Vr;
unsigned int counter = 0;
unsigned int payload_size;
String payload;
String rtty_payload;
String tme;
String xorCheckSum;
String crcCheckSum;
float latd;
float lngd;
float spd;
int alt;
int sats;
float fix;
unsigned int hdg;
unsigned int trueHdg;
float hdp;
int h, hNew;
int m, mNew;
int s, sNew;
String tme_h;
String tme_m;
String tme_s;

void setup() {
  
  Serial.begin(GPSBaud * clockF);
  
}

void loop() {

  h = gps.time.hour();
  m = gps.time.minute();
  s = gps.time.second();

  if (enable_sleep == 1)  {
      
    updateGPS();                      // Only report position while time stamp is new
      
  }

  Vr = analogRead(A0);                // Read board voltage and update values
  V = Vr * (5.0 / 1023);
  
  latd = gps.location.lat();
  lngd = gps.location.lng();
  sats = gps.satellites.value();
  fix = gps.date.age();
  hdp = gps.hdop.hdop();
  hdg = gps.course.deg();
  spd = gps.speed.kmph();
  alt = gps.altitude.meters();
  trueHdg = hdg % 360;
  tme_h = String(hNew);
  tme_m = String(mNew);
  tme_s = String(sNew);

  if (hNew < 10) {

    tme_h = "0" + String(hNew);
    
  }

  if (mNew < 10) {

    tme_m = "0" + String(mNew);
    
  }

  if (sNew < 10) {

    tme_s = "0" + String(sNew);
    
  }
  
  tme = String(tme_h) + ":" + String(tme_m) + ":" + String(tme_s);

  payload = String(counter) + "," + String(latd, 6) + "," + String(lngd, 6) + "," + String(sats) + "," + String(fix, 0) + "," + String(hdp, 2) + "," + String(alt, 0) + "," + String(spd, 0) + "," + String(trueHdg);
  rtty_payload = String(callSign) + "," + String(counter) + "," + String(tme) + "," + String(latd, 6) + "," + String(lngd, 6) + "," + String(alt) + "," + String(spd) + "," + String(trueHdg) + "," + String(sats);

  // UKHAS telemetry standard:
  // $$CALLSIGN,sentence_id,time,latitude,longitude,altitude,optional speed,optional bearing,optional internal temperature*CHECKSUM\n 

  payload_size = rtty_payload.length() + 1;
  char buff[payload_size];
  rtty_payload.toCharArray(buff, payload_size);

  if (checkSum == 0)  {

    rtty_payload = "$$" + String(rtty_payload) + "\n";                                  // no checksum
    
  }

  if (checkSum == 1)  {

    crcCheckSum = String(crc.calculate(buff, 0), HEX);

    while (crcCheckSum.length() < 4) {

      crcCheckSum = "0" + crcCheckSum;
    
    } 

    crcCheckSum.toUpperCase();
    rtty_payload = "$$" + String(rtty_payload) + "*" + String(crcCheckSum) + "\n";      // Use CRC-16
    
  }

  if (checkSum == 2)  {

    xorCheckSum = String(gps_xor_checksum(buff), HEX);

    while (xorCheckSum.length() < 2) {

      xorCheckSum = "0" + xorCheckSum;
    
    }

    xorCheckSum.toUpperCase();
    rtty_payload = "$$" + String(rtty_payload) + "*" + String(xorCheckSum) + "\n";      // Use XOR
    
  }

  float gonogo;
  gonogo = counter % txFrequency;

  // Setup LoRa if enabled
  if (enable_LoRa == 1 && gonogo == 0) {
    
    LoRaSetUp();

    // send packet
    LoRa.beginPacket();
    LoRa.print(payload);
    LoRa.endPacket();
    
  }

  // Setup RTTY if enabled
  if (enable_RTTY == 1) {
    
    rttySetUp();

    rtty.idle();
    smartDelay(idleTime);

    // send packet
    rtty.print(rtty_payload);
    
  }

  if (enable_RTTY == 1 || enable_LoRa == 1) {

    counter++;

  }                   
}

static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do
  {
    while (Serial.available())
      gps.encode(Serial.read());
  } while (millis() - start < (ms / clockF));
}

void LoRaSetUp()  {                                                                 // Have not got LoRa to work yet

  while (!Serial);
  LoRa.setPins(SS,RST,DI0);

  LoRa.setTxPower(17);
  LoRa.setFrequency(frequency_LoRa);
  LoRa.setSignalBandwidth(bandWidth);
  LoRa.setSpreadingFactor(SF);
  LoRa.setCodingRate4(codingRate);
  LoRa.setPreambleLength(8);
  LoRa.setSyncWord(0x34);

}

void rttySetUp()  {

  int state = fsk.beginFSK();

  state = rtty.begin(frequency_RTTY, carrierShift, baud * clockF, ASCII, stopBits);

}

void updateGPS()    {

  hNew = gps.time.hour();
  mNew = gps.time.minute();
  sNew = gps.time.second();

  if (nullIsland == 0)  {
    
    while (hNew == h && mNew == m && sNew == s) {

      smartDelay(sleepTime);                     // Enter sleep

      hNew = gps.time.hour();
      mNew = gps.time.minute();
      sNew = gps.time.second();

    }
    
  } else  {

    while (gps.location.lat() == 0 && gps.location.lng() == 0 || hNew == h && mNew == m && sNew == s) {

      smartDelay(sleepTime);                     // Enter sleep

      hNew = gps.time.hour();
      mNew = gps.time.minute();
      sNew = gps.time.second();

    }
  }
}

uint8_t gps_xor_checksum(char *string)
{
  size_t i;
  uint8_t XOR;
  uint8_t c;
 
  XOR = 0;
 
  // Calculate checksum ignoring the first two $s
  for (i = 0; i < strlen(string); i++)
  {
    c = string[i];
    XOR ^= c;
  }
 
  return XOR;
}
