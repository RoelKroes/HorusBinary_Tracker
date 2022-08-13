#include <TinyGPS++.h>
#include <SoftwareSerial.h>

// The TinyGPS++ object
TinyGPSPlus gps;

/*********************************************************************************************************************************/
void CheckGPS()
{    
  processGPSData();
  printGPSData();
}


/*********************************************************************************************************************************/
// This custom version of delay() ensures that the gps object is being "fed".
static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (SerialGPS.available())
      gps.encode(SerialGPS.read());
  } while (millis() - start < ms);
}

/*********************************************************************************************************************************/
static void processGPSData()
{
  byte DesiredMode;
  
  // Number of Satellites
  if (gps.satellites.isValid())
    UGPS.Satellites = gps.satellites.value();
  else
    UGPS.Satellites = 0;

 // Time
 if (gps.time.isValid())
 {
    UGPS.Hours = gps.time.hour();
    UGPS.Minutes = gps.time.minute();
    UGPS.Seconds = gps.time.second();
 }
 else
 {
    UGPS.Hours = 0;
    UGPS.Minutes = 0;
    UGPS.Seconds = 0;
 }

 // Position
 if (gps.location.isValid())
 {
    UGPS.Longitude = gps.location.lng();
    UGPS.Latitude = gps.location.lat();
 }
 else
 {
   UGPS.Longitude = 0;
   UGPS.Latitude = 0;
 }

 // Altitude
 if (gps.altitude.isValid())
    UGPS.Altitude = gps.altitude.meters();
 else
    UGPS.Altitude = 5;    

 if (UGPS.Altitude < 0)
   UGPS.Altitude = 5;    

 if (gps.speed.isValid())
 {
    UGPS.Speed = uint8_t(gps.speed.kmph());
 }

 // Chip temperature
 UGPS.Temp = ReadTemp() + tempOffset;  
 // Chip VCC
 UGPS.Internal_VCC = ReadVCC(); 
}

/*********************************************************************************************************************************/
void printGPSData()
{
#if defined(DEVMODE)
  Serial.println();
  Serial.println(F("-------GPS DATA-----------"));
  Serial.print(F("         Time: ")); Serial.print(UGPS.Hours); Serial.print(":"); Serial.print(UGPS.Minutes); Serial.print(":"); Serial.println(UGPS.Seconds);
  Serial.print(F("     Latitude: ")); Serial.println(UGPS.Latitude, 6);
  Serial.print(F("    Longitude: ")); Serial.println(UGPS.Longitude, 6);
  Serial.print(F(" Altitude (m): ")); Serial.println(UGPS.Altitude);
  Serial.print(F("   Satellites: ")); Serial.println(UGPS.Satellites);
  Serial.println(F("-------------------------"));
  Serial.println();
#endif
}


/*********************************************************************************************************************************/
void SendUBX(unsigned char *Message, int Length)
{  
  int i;
  
  for (i=0; i<Length; i++)
  {
    SerialGPS.write(Message[i]);
  }
}
