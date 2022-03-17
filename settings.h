//**********************************************************************************
// Compiler stuff, change if needed
//**********************************************************************************

// Development mode. Uncomment (default) to enable for debugging and see debug info on the serial monitor
#define DEVMODE
// Leave this as you are using an ATMEGA328 chip. This is for the ADC functions
// If you are not using an ATMEGA328 chip, comment out or delete the line below
#define USE_ATMEGA328


//**********************************************************************************
// Set your Payload IDs
// Please refer to: https://github.com/projecthorus/horusdemodlib/blob/master/payload_id_list.txt
//
// If you do not have a payload ID, you can use 0 (=4FSKTEST) for V1 and 256 (=4FSKTEST-V2) for V2
//**********************************************************************************
#define PAYLOAD_ID_V1  0
#define PAYLOAD_ID_V2  256

//**********************************************************************************
// Choose between Horusbinary V1 or V2
//**********************************************************************************
#define USE_V1 true   // Set to false for no V1 transmissions
#define USE_V2 false  // Set to false for no V2 transmissions

/************************************************************************
* PIN NUMBERS for SX1278
*  
* Change if needed
************************************************************************************/
#define PIN_NSS   10 
#define PIN_DIO0  2
#define PIN_BUSY  -1  // Not used in this sketch for sx1278
#define PIN_RESET  9  
#define PIN_DIO1   3  


//**********************************************************************************
// RADIO SETUP
//**********************************************************************************
#define TX_FREQ         434.200
#define FSK4_BAUD       100    // recommended 100 baud
#define FSK4_SPACING    270    // NOTE: This results in a shift of 244 Hz due to the PLL Resolution of the SX127x which is 61Hz
#define TX_DELAY        3500   // Delay in ms between transmissions. Add about 7sec to this to get the true delay between to transmissions


/***********************************************************************************
* GPS SETTINGS
*  
* Change if needed
************************************************************************************/
// GPS Serial device
// We use SoftwareSerial so these pin numbers are basically free to choose
// Parameters for the GPS
// White: 7, green: 8
static const int Rx = 8, Tx = 7;
static const uint32_t GPSBaud = 9600;


/***********************************************************************************
* Other settings
*  
* Change if needed
************************************************************************************/
#define VCC_OFFSET 0.00  // Offset for error correction in Volts for the internal voltage. Ideally this should be 0.0 but usually is between -0.1 and +0.1 and is chip specific. 
