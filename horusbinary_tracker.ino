/***********************************************************************************
 *  FIRST THING YOU NEED TO DO IS ADJUST THE SETTINGS IN Settings.h
 *  
 *  Have FUN!
 *  
 *  Roel Kroes - PD7R - March 2022
 ***********************************************************************************/

/*************************************************************************************************************
 * This sketch generates Horusbinary V1 and V2 packets for high altitude balloon tracking using an Arduino Uno 
 * or compatible and a SX127x chip.
 * 
 * The packages can be decoded and uploaded using a SSB receiver and software such as Horus Telemetry GUI and uploaded to the HabHub tracker
 * Horusbinary Gui: https://github.com/projecthorus/horus-gui
 * Habhub tracking page: https://tracker.habhub.org/
 * horusdemodlib: https://github.com/projecthorus/horusdemodlib/wiki
 * 
 * The basecode for this sketch was taken from the horusbinary project.
 * See:  https://github.com/projecthorus/horusbinary_radiolib
 * 
 * You need the following libraries to compile this sketch:
 *  https://github.com/jgromes/RadioLib (Radiolib)
 *  https://github.com/mikalhart/TinyGPSPlus (tinyGPS++)
 * 
 * Wiring example:
 *  GPS <> Arduino Nano
 *  ----------------------  
 *  VCC -> 3.3V
 *  GND -> GND 
 *  RX -> D7 
 *  TX -> D8 
 *  
 *  (Example for Arduino Nano)
 *  SX1278 <-> Arduino Nano
 *  ----------------------
 *  MISO   -> D12
 *  SCK    -> D13
 *  RST    -> Not connected
 *  GND    -> GND
 *  DI00   -> D2
 *  MOSI   -> D11
 *  NSS    -> D10 
 *  3V3    -> 3V3
 * 
 *
 * Other modules that can be used for FSK4:
 * (Untested, but work with RTTY to are likely to work here too)
 * - SX127x/RFM9x
 * - RF69
 * - SX1231
 * - CC1101
 * - SX126x
 * - nRF24
 * - Si443x/RFM2x
 * - SX128x
 * 
**************************************************************************************************************/

#include <RadioLib.h>
#include <SoftwareSerial.h>
#include "horus_l2.h"
#include "settings.h"


// Initialize the radio
SX1278 radio = new Module(PIN_NSS, PIN_DIO0, PIN_RESET, PIN_DIO1);

// Struct to hold GPS data
struct TGPS
{
  uint8_t Hours, Minutes, Seconds;
  float Longitude, Latitude;
  uint16_t Altitude;
  uint8_t Satellites;
  int8_t Temp;
  uint8_t Speed;
  int8_t Internal_VCC;
} UGPS;

// Horus Binary Structures & Variables

// Horus Binary Packet Structure - Version 1
struct HorusBinaryPacketV1
{
    uint8_t     PayloadID;    // Refer to  https://github.com/projecthorus/horusdemodlib/blob/master/payload_id_list.txt
    uint16_t	  Counter;      // Packet counter 
    uint8_t	    Hours;        // Hours Zulu
    uint8_t	    Minutes;      // Minutes
    uint8_t	    Seconds;      // Seconds
    float	      Latitude;     // Latitude in format xx.yyyy
    float	      Longitude;    // Longitude in format xx.yyyy
    uint16_t  	Altitude;     // Altitude in meters
    uint8_t     Speed;        // Speed in kmh
    uint8_t     Sats;         // Number of satellites visible
    int8_t      Temp;         // Twos Complement Temp value.
    uint8_t     BattVoltage;  // 0 = 0.5v, 255 = 5.0V, linear steps in-between.
    uint16_t    Checksum;     // CRC16-CCITT Checksum.
}  __attribute__ ((packed));

// Horus v2 Mode 1 (32-byte) Binary Packet
struct HorusBinaryPacketV2
{
    uint16_t    PayloadID;    // Refer to  https://github.com/projecthorus/horusdemodlib/blob/master/payload_id_list.txt 
    uint16_t	  Counter;      // Packet counter 
    uint8_t	    Hours;        // Hours Zulu
    uint8_t	    Minutes;      // Minutes
    uint8_t	    Seconds;      // Seconds
    float	      Latitude;     // Latitude in format xx.yyyy
    float	      Longitude;    // Longitude in format xx.yyyy
    uint16_t  	Altitude;     // Altitude in meters
    uint8_t     Speed;        // Speed in kmh
    uint8_t     Sats;         // Number of satellites visible 
    int8_t      Temp;         // Twos Complement Temp value.
    uint8_t     BattVoltage;  // 0 = 0.5v, 255 = 2.0V, linear steps in-between.
    // The following 9 bytes (up to the CRC) are user-customizable. The following just
    // provides an example of how they could be used.
    // Refer here for details: https://github.com/projecthorus/horusdemodlib/wiki/5-Customising-a-Horus-Binary-v2-Packet
    int16_t     dummy1;       // Interpreted as Ascent rate divided by 100 for the Payload ID: 4FSKTEST-V2 
    int16_t     dummy2;       // External temperature divided by 10 for the Payload ID: 4FSKTEST-V2 
    uint8_t     dummy3;       // External humidity for the Payload ID: 4FSKTEST-V2 
    uint16_t    dummy4;       // External pressure divided by 10 for the Payload ID:  4FSKTEST-V2  
    uint16_t    unused;       // 2 bytes which are not interpreted
    uint16_t    Checksum;     // CRC16-CCITT Checksum.

}  __attribute__ ((packed));

// Buffers, counters and globals
SoftwareSerial SerialGPS(Rx, Tx);
char rawbuffer [128];   // Buffer to temporarily store a raw binary packet.
char codedbuffer [128]; // Buffer to store an encoded binary packet
char debugbuffer[256]; // Buffer to store debug strings
uint16_t packet_count = 1;  // Packet counter


//*********************************************************************************************************************
// Generate a Horus Binary v1 packet, and populate it with data.
//*********************************************************************************************************************
int build_horus_binary_packet_v1(char *buffer)
{
  struct HorusBinaryPacketV1 BinaryPacket;

  BinaryPacket.PayloadID   = PAYLOAD_ID_V1; 
  BinaryPacket.Counter     = packet_count;
  BinaryPacket.Hours       = UGPS.Hours;
  BinaryPacket.Minutes     = UGPS.Minutes;
  BinaryPacket.Seconds     = UGPS.Seconds;
  BinaryPacket.Latitude    = UGPS.Latitude;
  BinaryPacket.Longitude   = UGPS.Longitude;
  BinaryPacket.Altitude    = UGPS.Altitude;
  BinaryPacket.Speed       = UGPS.Speed;
  BinaryPacket.BattVoltage = UGPS.Internal_VCC;
  BinaryPacket.Sats        = UGPS.Satellites;
  BinaryPacket.Temp        = UGPS.Temp;
  BinaryPacket.Checksum    = (uint16_t)crc16((unsigned char*)&BinaryPacket,sizeof(BinaryPacket)-2);

  memcpy(buffer, &BinaryPacket, sizeof(BinaryPacket));
  return sizeof(struct HorusBinaryPacketV1);
}

//*********************************************************************************************************************
// Generate a Horus Binary v2 packet, and populate it with data.
//*********************************************************************************************************************
int build_horus_binary_packet_v2(char *buffer)
{
  struct HorusBinaryPacketV2 BinaryPacketV2;

  BinaryPacketV2.PayloadID   = PAYLOAD_ID_V2; 
  BinaryPacketV2.Counter     = packet_count;
  BinaryPacketV2.Hours       = UGPS.Hours;
  BinaryPacketV2.Minutes     = UGPS.Minutes;
  BinaryPacketV2.Seconds     = UGPS.Seconds;
  BinaryPacketV2.Latitude    = UGPS.Latitude;
  BinaryPacketV2.Longitude   = UGPS.Longitude;
  BinaryPacketV2.Altitude    = UGPS.Altitude;
  BinaryPacketV2.Speed       = UGPS.Speed;
  BinaryPacketV2.BattVoltage = UGPS.Internal_VCC;
  BinaryPacketV2.Sats        = UGPS.Satellites;
  BinaryPacketV2.Temp        = UGPS.Temp;
  // Custom section. This is an example only, and the 9 bytes in this section can be used in other
  // ways. Refer here for details: https://github.com/projecthorus/horusdemodlib/wiki/5-Customising-a-Horus-Binary-v2-Packet
  BinaryPacketV2.dummy1      = 200;  // int16_t Interpreted as Ascent rate divided by 100 for 4FSKTEST-V2. This value would display as 2.0 on HabHub 
  BinaryPacketV2.dummy2      = -20;  // int16_t External temperature divided by 10 for 4FSKTEST-V2. This value would display as -2.0 on HabHub   
  BinaryPacketV2.dummy3      = 51;   // uint8_t External humidity for 4FSKTEST-V2. This value would display as 51 on HabHub  
  BinaryPacketV2.dummy4      = 21;   // uint16_t External pressure divided by 10 for 4FSKTEST-V2. This value would display as 2.1 on HabHub  
  BinaryPacketV2.unused      = 0;    // Two unused filler bytes
  BinaryPacketV2.Checksum    = (uint16_t)crc16((unsigned char*)&BinaryPacketV2,sizeof(BinaryPacketV2)-2);

  memcpy(buffer, &BinaryPacketV2, sizeof(BinaryPacketV2));
  return sizeof(struct HorusBinaryPacketV2);
}


//*********************************************************************************************************************
// Sketch setup and initialisation
//*********************************************************************************************************************
void setup() 
{
  
#if defined(DEVMODE) 
  // Initialize the Serial console
  Serial.begin(57600);
  // initialize SX1278 with default settings
  Serial.print(F("[SX1278] Initializing ... "));
#endif

  // Initialize the radio in FSK mode
  int state = radio.beginFSK();
  // when using one of the non-LoRa modules for FSK4
  // (RF69, CC1101, Si4432 etc.), use the basic begin() method
  // int state = radio.begin();

  // Only continue if rthere is no error state
  if(state == RADIOLIB_ERR_NONE) 
  {
#if defined(DEVMODE) 
    Serial.println(F("success!"));
#endif
  } else 
  {
#if defined(DEVMODE) 
    Serial.print(F("failed, code "));
    Serial.println(state);
#endif
    while(true);
  }


#if defined(DEVMODE) 
  Serial.print(F("[FSK4] Initializing ... "));
#endif

  // initialize FSK4 transmitter
  // NOTE: FSK4 frequency shift will be rounded
  //       to the nearest multiple of frequency step size.
  //       The exact value depends on the module:
  //         SX127x/RFM9x - 61 Hz
  //         RF69 - 61 Hz
  //         CC1101 - 397 Hz
  //         SX126x - 1 Hz
  //         nRF24 - 1000000 Hz
  //         Si443x/RFM2x - 156 Hz
  //         SX128x - 198 Hz
 state = fsk4_setup(&radio, TX_FREQ, FSK4_SPACING, FSK4_BAUD);

  if(state == RADIOLIB_ERR_NONE) 
  {
#if defined(DEVMODE) 
    Serial.println(F("success!"));
#endif
  } else 
  {
#if defined(DEVMODE) 
    Serial.print(F("failed, code "));
    Serial.println(state);
#endif
    while(true);
  }
}


//*******************************************************************************************************************
// Check GPS, send telemetry and loop
//*******************************************************************************************************************
void loop() 
{
  int pkt_len;
  int coded_len;
  
  // Initialise communication with the GPS
  // We need to do this here as having communication on all the time,
  // slows the processor and gives instability
  SerialGPS.begin(GPSBaud); 
  smartDelay(TX_DELAY);
  CheckGPS(); 
  SerialGPS.end();

if (USE_V1)
{
  // Start Horus Binary V1
#if defined(DEVMODE)
  Serial.println(F("Generating Horus Binary v1 Packet"));
#endif

  // Generate packet for V1
  pkt_len = build_horus_binary_packet_v1(rawbuffer);
  PrintHex(rawbuffer, pkt_len, debugbuffer);
#if defined(DEVMODE)
  Serial.print(F("Uncoded Length (bytes): "));
  Serial.println(pkt_len);
  Serial.print(F("Uncoded: ")); 
  Serial.println(debugbuffer);
#endif

  // Apply Encoding
  coded_len = horus_l2_encode_tx_packet((unsigned char*)codedbuffer,(unsigned char*)rawbuffer,pkt_len);
  PrintHex(codedbuffer, coded_len, debugbuffer);
#if defined(DEVMODE)  
  Serial.print(F("Encoded Length (bytes): "));
  Serial.println(coded_len);
  Serial.print(F("Coded: "));
  Serial.println(debugbuffer);
#endif

  // Transmit!
#if defined(DEVMODE)  
  Serial.println(F("Transmitting Horus Binary v1 Packet"));
#endif
  // send out idle condition for 1000 ms
  fsk4_idle(&radio);
  delay(1000);
  fsk4_preamble(&radio, 8);
  fsk4_write(&radio, codedbuffer, coded_len);
} // End of V1

if (USE_V2)
{
  // Start Horus Binary V2
#if defined(DEVMODE)  
  Serial.println(F("Generating Horus Binary v2 Packet"));
#endif

  // Generate packet
  pkt_len = build_horus_binary_packet_v2(rawbuffer);
  PrintHex(rawbuffer, pkt_len, debugbuffer);
#if defined(DEVMODE)  
  Serial.print(F("Uncoded Length (bytes): "));
  Serial.println(pkt_len);
  Serial.print("Uncoded: ");
  Serial.println(debugbuffer);
#endif


  // Apply Encoding
  coded_len = horus_l2_encode_tx_packet((unsigned char*)codedbuffer,(unsigned char*)rawbuffer,pkt_len);
  PrintHex(codedbuffer, coded_len, debugbuffer);
#if defined(DEVMODE)    
  Serial.print(F("Encoded Length (bytes): "));
  Serial.println(coded_len);
  Serial.print("Coded: ");
  Serial.println(debugbuffer);
#endif
  
  // Transmit!
#if defined(DEVMODE)
  Serial.println(F("Transmitting Horus Binary v2 Packet"));
#endif

  // send out idle condition for 1000 ms
  fsk4_idle(&radio);
  delay(1000);
  fsk4_preamble(&radio, 8);
  fsk4_write(&radio, codedbuffer, coded_len);
}  // end of V2

  packet_count++;

  // Do it all over again

}
