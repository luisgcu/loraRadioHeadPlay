//  LoRa simple server with encrypted communications
// In order for this to compile you MUST uncomment the #define RH_ENABLE_ENCRYPTION_MODULE line
// at the bottom of RadioHead.h, AND you MUST have installed the Crypto directory from arduinolibs:
// http://rweather.github.io/arduinolibs/index.html
//  Philippe.Rochat'at'gmail.com
//  06.07.2017

#include <RH_RF95.h>
#include <RHDatagram.h>
#include <RHEncryptedDriver.h>
#include <Speck.h>

/****************************************************/
/*  Defines        Motenino Mega   ID 2              */
/****************************************************/
#define MY_ADDRESS 2
#define SERVER_ADDRESS 3
#define TXPWR 13

RH_RF95 rf95(4, 2); // Instanciate a LoRa driver (4,2)Moteino Mega
Speck myCipher;     // Instanciate a Speck block ciphering
RHEncryptedDriver myDriver(rf95, myCipher); // Instantiate the driver with those two
//RHDatagram manager(rf95, MY_ADDRESS);
RHDatagram manager(myDriver, MY_ADDRESS);
float frequency = 434.0;                                                                // Change the frequency here.
unsigned char encryptkey[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16}; // The very secret key

void setup()
{
  Serial.begin(115200);
  while (!Serial)
    ; // Wait for serial port to be available
  Serial.println("LoRa Simple_Encrypted Server");
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
  myCipher.setKey(encryptkey, 16);
  delay(4000);
  Serial.println("Setup completed");
}

void loop()
{
  if (manager.available())
  {
    // Should be a message for us now
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (manager.recvfrom((uint8_t *)&buf, &len))
    {
      Serial.print("Received: ");
      Serial.println((char *)&buf);
    }
    else
    {
      Serial.println("recv failed");
    }
  }
}