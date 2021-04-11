// LoRa Simple Hello World Client with encrypted communications
// In order for this to compile you MUST uncomment the #define RH_ENABLE_ENCRYPTION_MODULE line
// at the bottom of RadioHead.h, AND you MUST have installed the Crypto directory from arduinolibs:
// http://rweather.github.io/arduinolibs/index.html
//  Philippe.Rochat'at'gmail.com
//  06.07.2017
#include <Arduino.h>
#include <RH_RF95.h>
#include <RHDatagram.h>
#include <RHEncryptedDriver.h>
#include <Speck.h>

/****************************************************/
/*  Defines       Nano Atmega ID 3                  */
/****************************************************/
#define MY_ADDRESS 3
#define SERVER_ADDRESS 2
#define TXPWR 13

RH_RF95 rf95;   // Instanciate a LoRa driver
Speck myCipher; // Instanciate a Speck block ciphering

RHEncryptedDriver myDriver(rf95, myCipher); // Instantiate the driver with those two
RHDatagram manager(myDriver, MY_ADDRESS);
//RHEncryptedDriver myDriver(rf95, myCipher); // Instantiate the driver with those two
//RHDatagram manager(rf95, MY_ADDRESS);


float frequency = 434.0;                                                                // Change the frequency here.
unsigned char encryptkey[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16}; // The very secret key
char HWMessage[] = "Hello World ! I'm happy if you can read me";
uint8_t HWMessageLen;

void setup()
{
  HWMessageLen = strlen(HWMessage);
  Serial.begin(115200);
  while (!Serial)
    ; // Wait for serial port to be available
  Serial.println("LoRa Simple_Encrypted Client");
  if (manager.init())
  {
    if (!rf95.setFrequency(frequency))
      Serial.println("Unable to set RF95 frequency");
    if (!rf95.setModemConfig(RH_RF95::BW41_7CR45SF9))
      Serial.println("Invalid setModemConfig() option");
    rf95.setTxPower(TXPWR);
    Serial.println("RF95 radio initialized.");
  }
  else
    Serial.println("RF95 radio initialization failed.");
  Serial.print("RF95 max message length = ");
  Serial.println(rf95.maxMessageLength());

  // if (!rf95.init())
  //   Serial.println("LoRa init failed");
  // // Setup ISM frequency
  // rf95.setFrequency(frequency);
  // // Setup Power,dBm
  // rf95.setTxPower(13);
  myCipher.setKey(encryptkey, sizeof(encryptkey));
  Serial.println("Waiting for radio to setup");
  delay(1000);
  Serial.println("Setup completed");
}

void loop()
{
  uint8_t data[HWMessageLen + 1] = {0};
  for (uint8_t i = 0; i <= HWMessageLen; i++)
    data[i] = (uint8_t)HWMessage[i];
  if (!manager.sendto((uint8_t *)&data, sizeof(data), SERVER_ADDRESS))
  {
    Serial.print("Transmit failed");
  rf95.waitPacketSent(250); // wait 100 mSec max for packet to be sent
  //myDriver.send(data, sizeof(data)); // Send out ID + Sensor data to LoRa gateway
  }
  else  {
    Serial.println("Timed out waiting for response");
    Serial.println();
  }  
  Serial.print("Sent: ");
  Serial.println((char *)&data);
  delay(9000);
}